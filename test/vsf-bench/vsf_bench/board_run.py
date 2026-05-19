"""vsf-bench — build → flash → run tests.

LA capture supports two modes (see `--la-mode`):
  * `per-scene` (default): one LA capture per scene; small files decode fast.
  * `shared`: a single LA capture for the entire run; uses --decode-start /
    --decode-end so each scene's decode only scans its own window.
"""

import argparse
import importlib.util
import inspect
import json
import sys
import time
from datetime import datetime
from pathlib import Path

from vsf_bench.hardware_map import load as load_hardware_map, validate_runners
from vsf_bench.runners.cmake_runner import CMakeRunner
from vsf_bench.runners.registry import get_runner_class
from vsf_bench.instruments.serial_instrument import SerialInstrument
from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument


def _load_script_module(path: Path):
    if not path.exists():
        raise FileNotFoundError(f"Test script not found: {path}")
    spec = importlib.util.spec_from_file_location("test_script", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Cannot create module spec for {path}")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    if not hasattr(mod, "run"):
        raise AttributeError(f"Test script {path} must define run(project_root, serial)")
    return mod


def _discover_scenes(project_root: Path) -> dict[str, Path]:
    scenes: dict[str, Path] = {}
    base = project_root / "vsf.demo" / "vsf" / "test" / "vsf_test"
    if not base.exists():
        return scenes
    for peripheral_dir in base.iterdir():
        scenario_dir = peripheral_dir / "scenario"
        if not scenario_dir.is_dir():
            continue
        for f in scenario_dir.glob("vsf_test_*.py"):
            stem = f.stem
            if stem.startswith("vsf_test_"):
                scene_name = stem[len("vsf_test_"):]
                scenes[scene_name] = f
    return scenes


def parse_args():
    parser = argparse.ArgumentParser(prog="vsf-bench")
    parser.add_argument("--project-root", type=Path, default=Path.cwd())
    parser.add_argument("--log-dir", type=Path, default=None)
    parser.add_argument("--build", action="store_true")
    parser.add_argument("--flash", action="store_true")
    parser.add_argument("--test", action="store_true")
    parser.add_argument("--all", action="store_true")
    parser.add_argument("--scene", action="append", default=None)
    parser.add_argument("--case", action="append", default=None)
    parser.add_argument("--case-index", action="append", type=int, default=None)
    parser.add_argument("--script", type=Path, default=None)
    parser.add_argument(
        "--la-mode",
        choices=["per-scene", "shared"],
        default="shared",
        help="LA capture lifetime: shared (default, one capture, time-windowed decode) or per-scene (per-scene capture, slower but tighter window)",
    )
    parser.add_argument("hardware_map")
    return parser.parse_args()


def _query_firmware_scenes(ser: SerialInstrument) -> set[str]:
    ser.send("vsf-test scene --list\r\n")
    time.sleep(0.3)
    output = ser.read_all(timeout=2.0)
    scenes: set[str] = set()
    for line in output.splitlines():
        line = line.strip()
        if line and line[0].isdigit():
            parts = line.split(None, 1)
            if len(parts) == 2:
                scenes.add(parts[1])
    return scenes


def _script_needs_la(script_path: Path | None, mod=None) -> bool:
    """Heuristic + module check: does the script use the LA at all?"""
    if mod is not None and hasattr(mod, "decode"):
        return True
    if script_path is None:
        return False
    try:
        source = script_path.read_text()
        return any(
            "la." in line
            for line in source.splitlines()
            if "def run(" not in line and "def decode(" not in line
        )
    except Exception:
        return False


def _build_run_cmd(scene: str, case: str | None) -> str:
    if case:
        return f"vsf-test run {scene}.{case}\r\n"
    return f"vsf-test run {scene}\r\n"


def _drain_repl(ser: SerialInstrument) -> None:
    """Clear stale REPL output between scenes.

    With host-side DONE marker writes removed from rx_* scenarios, the REPL is
    quiet shortly after each scene's test summary; a short drain is enough.
    """
    ser.read_all(timeout=0.3)
    ser.send("\r\n")
    time.sleep(0.2)
    ser.read_all(timeout=0.3)


def _run_script_phase1(
    scene_name: str,
    case: str | None,
    script_module,
    project_root: Path,
    ser: SerialInstrument,
) -> bool:
    """Send trigger, run script.run(). Returns True on PASS."""
    case_tag = f".{case}" if case else ""
    cmd = _build_run_cmd(scene_name, case)
    _drain_repl(ser)
    ser.send(cmd)
    print(f"[vsf-bench] Triggered: {cmd.strip()}")

    try:
        ser.expect(r"Scene not found:", timeout=5.0)
        print(f"[vsf-bench] FAIL: {scene_name}{case_tag}: Scene not found in firmware")
        return False
    except TimeoutError:
        pass

    try:
        if script_module is not None:
            run_fn = script_module.run
            sig = inspect.signature(run_fn)
            if "la" in sig.parameters:
                run_fn(project_root, ser, None)
            else:
                run_fn(project_root, ser)
        else:
            ser.expect_test_summary(scene_name, timeout=180.0)
        print(f"[vsf-bench] PASS phase1: {scene_name}{case_tag}")
        return True
    except (TimeoutError, AssertionError, RuntimeError, KeyError, AttributeError) as e:
        print(f"[vsf-bench] FAIL: {scene_name}{case_tag}: {e}")
        return False


def _call_decode(mod, project_root: Path, la: LogicAnalyzerInstrument,
                 start_ns: int | None, end_ns: int | None) -> None:
    """Invoke a script's decode() with whichever signature it accepts."""
    sig = inspect.signature(mod.decode)
    params = sig.parameters
    kwargs = {}
    if "decode_start_ns" in params:
        kwargs["decode_start_ns"] = start_ns
    if "decode_end_ns" in params:
        kwargs["decode_end_ns"] = end_ns
    mod.decode(project_root, la, **kwargs)


def _new_la(la_cfg, cli_path: Path, capture_path: Path) -> LogicAnalyzerInstrument:
    return LogicAnalyzerInstrument(
        cli_path=cli_path,
        device=la_cfg.device,
        samplerate=la_cfg.samplerate,
        channels=la_cfg.channels,
        capture_path=capture_path,
    )


def main():
    args = parse_args()
    project_root = args.project_root.resolve()

    do_build = args.build or args.all
    do_flash = args.flash or args.all
    do_test = args.test or args.all

    if not (do_build or do_flash or do_test):
        print("[vsf-bench] Error: at least one of --build, --flash, --test, --all is required")
        sys.exit(1)

    hardware_map_path = project_root / args.hardware_map
    board = load_hardware_map(str(hardware_map_path))
    validate_runners(board)

    build_dir = None
    if do_build or do_flash or do_test:
        print(f"[vsf-bench] Building ({board.build.source_dir})...")
        cmake = CMakeRunner(board.build, project_root)
        build_dir = cmake.build()
        print(f"[vsf-bench] Build complete: {build_dir}")

    if do_flash and not do_test:
        runner_cfg = board.runners[board.active_runner]
        runner_cls = get_runner_class(runner_cfg.type)
        if runner_cls is None:
            print(f"[vsf-bench] Unknown runner type: {runner_cfg.type}", file=sys.stderr)
            sys.exit(1)
        runner = runner_cls(runner_cfg)
        print(f"[vsf-bench] Flashing via {board.active_runner}...")
        runner.flash(build_dir)
        print("[vsf-bench] Flash complete")
        return

    if not do_test:
        return

    discovered = _discover_scenes(project_root)

    if args.scene:
        if args.script and len(args.scene) > 1:
            print("[vsf-bench] Error: --script can only be used with a single --scene")
            sys.exit(1)
        ordered_scenes = []
        for name in args.scene:
            if args.script:
                ordered_scenes.append((name, args.script.resolve()))
            elif name in discovered:
                ordered_scenes.append((name, discovered[name]))
            else:
                print(f"[vsf-bench] Scene not found: {name}. Discovered: {sorted(discovered.keys())}")
                sys.exit(1)
    else:
        ordered_scenes = [(name, discovered[name]) for name in sorted(discovered.keys())]

    if not ordered_scenes:
        print("[vsf-bench] No scenes discovered")
        sys.exit(1)

    print(f"[vsf-bench] Scenes: {[s for s, _ in ordered_scenes]}")
    print(f"[vsf-bench] LA mode: {args.la_mode}")

    case_specs: list[str] = []
    if args.case:
        case_specs.extend(args.case)
    if args.case_index:
        case_specs.extend(str(i) for i in args.case_index)
    if case_specs and len(ordered_scenes) > 1:
        print("[vsf-bench] Error: --case/--case-index requires exactly one --scene")
        sys.exit(1)

    if args.log_dir:
        run_dir = args.log_dir.resolve()
    else:
        timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
        tag = ordered_scenes[0][0] if len(ordered_scenes) == 1 else "vsf_test"
        run_dir = Path(f"logs/{timestamp}-{tag}")
    run_dir.mkdir(parents=True, exist_ok=True)
    log_path = run_dir / "vsf-bench.jsonl"

    ser = SerialInstrument(board.serial, board.baud, audit_log=log_path)
    ser.open()

    runner_cfg = board.runners[board.active_runner]
    runner_cls = get_runner_class(runner_cfg.type)
    if runner_cls is None:
        print(f"[vsf-bench] Unknown runner type: {runner_cfg.type}", file=sys.stderr)
        sys.exit(1)
    runner = runner_cls(runner_cfg)
    print(f"[vsf-bench] Flashing via {board.active_runner}...")
    runner.flash(build_dir)
    print("[vsf-bench] Flash complete")

    la_cfg = board.logic_analyzer
    cli_path = project_root / la_cfg.cli if la_cfg else None

    try:
        ser.expect("VSF Test Ready", timeout=10.0)
    except TimeoutError:
        print("[vsf-bench] Warning: REPL banner not seen, continuing anyway")

    if not args.scene:
        fw_scenes = _query_firmware_scenes(ser)
        if fw_scenes:
            skipped = [s for s, _ in ordered_scenes if s not in fw_scenes]
            ordered_scenes = [(s, p) for s, p in ordered_scenes if s in fw_scenes]
            if skipped:
                print(f"[vsf-bench] Skipped (not in firmware): {skipped}")
        print(f"[vsf-bench] Effective scenes: {[s for s, _ in ordered_scenes]}")

    # Pre-load script modules.
    loaded: list[tuple[str, Path | None, object | None, bool]] = []
    for scene_name, script_path in ordered_scenes:
        if script_path is None:
            loaded.append((scene_name, None, None, False))
            continue
        try:
            mod = _load_script_module(script_path)
        except (FileNotFoundError, AttributeError, RuntimeError) as e:
            print(f"[vsf-bench] Failed to load {script_path}: {e}")
            ser.close()
            sys.exit(1)
        loaded.append((scene_name, script_path, mod, _script_needs_la(script_path, mod)))

    any_needs_la = any(needs for _, _, _, needs in loaded) and la_cfg is not None and cli_path is not None

    overall_pass = True

    if args.la_mode == "shared" and any_needs_la:
        # ----- Shared LA: single capture spanning the LA-needing block -----
        # We start LA right before the first scene that needs it and stop right
        # after the last, leaving non-LA scenes outside the capture entirely.
        # This keeps the LA file small (and well under dsview-cli's sample
        # buffer cap — at 10MS/s the cap is ~120s of capture).
        #
        # `la_start_t` is recorded BEFORE the 3s warmup so it sits close to the
        # dsview-cli capture file's t=0 (dsview takes ~0.5–1s to actually start
        # sampling). Each scene's window is then padded by SHARED_WINDOW_PAD_NS
        # on both sides so dsview-cli's --decode-start/--decode-end cover the
        # full scene regardless of any residual la_start_t ↔ capture_t0 skew.
        # Scenes are >5s apart and use distinct marker prefixes, so overlapping
        # padded windows are harmless.
        SHARED_WINDOW_PAD_NS = 3_000_000_000

        la_indices = [i for i, (_, _, _, n) in enumerate(loaded) if n]
        first_la_idx = la_indices[0]
        last_la_idx = la_indices[-1]

        shared_capture = run_dir / "shared-capture.dsl"
        shared_la: LogicAnalyzerInstrument | None = None
        la_start_t: float | None = None

        # Phase 1: trigger + script.run() per scene. LA only runs over the
        # contiguous block [first_la_idx, last_la_idx].
        scene_windows: list[tuple[str, object | None, int, int]] = []
        for i, (scene_name, _script_path, mod, _needs) in enumerate(loaded):
            if i == first_la_idx:
                shared_la = _new_la(la_cfg, cli_path, shared_capture)
                la_start_t = time.monotonic()
                shared_la.start(300.0)
                time.sleep(3.0)
                print(f"[vsf-bench] Shared LA started -> {shared_capture}")

            cases_to_run = case_specs if case_specs else [None]
            for case in cases_to_run:
                t_start = int((time.monotonic() - la_start_t) * 1e9) if la_start_t is not None else 0
                ok = _run_script_phase1(scene_name, case, mod, project_root, ser)
                t_end = int((time.monotonic() - la_start_t) * 1e9) if la_start_t is not None else 0
                if not ok:
                    overall_pass = False
                if mod is not None and hasattr(mod, "decode") and shared_la is not None:
                    scene_windows.append((scene_name, mod, t_start, t_end))

            if i == last_la_idx and shared_la is not None:
                print("[vsf-bench] Stopping shared LA...")
                shared_la.stop()
                try:
                    shared_la.wait(timeout=30.0)
                except (TimeoutError, RuntimeError) as e:
                    print(f"[vsf-bench] LA wait warning: {e}")

        for scene_name, mod, t_start, t_end in scene_windows:
            decode_start = max(0, t_start - SHARED_WINDOW_PAD_NS)
            decode_end = t_end + SHARED_WINDOW_PAD_NS
            print(f"\n[vsf-bench] Decoding (shared): {scene_name}  window=[{decode_start/1e9:.2f}s,{decode_end/1e9:.2f}s]")
            try:
                _call_decode(mod, project_root, shared_la, decode_start, decode_end)
                print(f"[vsf-bench] PASS decode: {scene_name}")
            except (AssertionError, RuntimeError, KeyError, AttributeError, FileNotFoundError) as e:
                print(f"[vsf-bench] FAIL decode: {scene_name}: {e}")
                overall_pass = False
    else:
        # ----- Per-scene LA (or no LA at all) -----
        for scene_name, _script_path, mod, needs_la in loaded:
            cases_to_run = case_specs if case_specs else [None]
            for case in cases_to_run:
                case_tag = f".{case}" if case else ""
                print(f"\n[vsf-bench] Scene: {scene_name}{case_tag}")

                scene_la: LogicAnalyzerInstrument | None = None
                if needs_la and la_cfg is not None and cli_path is not None:
                    label = f"{scene_name}{('_' + case) if case else ''}"
                    capture_path = run_dir / f"{label}-capture.dsl"
                    scene_la = _new_la(la_cfg, cli_path, capture_path)
                    scene_la.start(180.0)
                    time.sleep(3.0)

                ok = _run_script_phase1(scene_name, case, mod, project_root, ser)
                if not ok:
                    overall_pass = False

                if scene_la is not None:
                    scene_la.stop()
                    try:
                        scene_la.wait(timeout=15.0)
                    except (TimeoutError, RuntimeError) as e:
                        print(f"[vsf-bench] LA wait warning: {e}")

                if ok and mod is not None and hasattr(mod, "decode") and scene_la is not None:
                    try:
                        _call_decode(mod, project_root, scene_la, None, None)
                        print(f"[vsf-bench] PASS decode: {scene_name}{case_tag}")
                    except (AssertionError, RuntimeError, KeyError, AttributeError, FileNotFoundError) as e:
                        print(f"[vsf-bench] FAIL decode: {scene_name}{case_tag}: {e}")
                        overall_pass = False

    ser.close()

    with open(log_path, "a") as f:
        verdict = "pass" if overall_pass else "fail"
        f.write(json.dumps({"verdict": verdict}) + "\n")
    print(f"\n[vsf-bench] {'PASS' if overall_pass else 'FAIL'}")
    if not overall_pass:
        sys.exit(1)


if __name__ == "__main__":
    main()
