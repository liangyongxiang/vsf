"""vsf-bench — build → flash → run tests."""

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


def load_test_script(path: str | Path):
    """Load a test script and return its run() function."""
    p = Path(path).resolve()
    if not p.exists():
        raise FileNotFoundError(f"Test script not found: {p}")
    spec = importlib.util.spec_from_file_location("test_script", p)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Cannot create module spec for {p}")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    if not hasattr(mod, "run"):
        raise AttributeError(f"Test script {p} must define a run(serial) function")
    return mod.run


def _discover_scenes(project_root: Path) -> dict[str, Path]:
    """Scan vsf.demo/vsf/test/vsf_test/*/scenario/ for default scripts.

    Returns: {scene_name: script_path}
    """
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
    parser.add_argument(
        "--project-root",
        type=Path,
        default=Path.cwd(),
        help="Project root directory (default: cwd)",
    )
    parser.add_argument(
        "--log-dir",
        type=Path,
        default=None,
        help="Explicit log directory. If omitted, auto-generated under logs/",
    )
    parser.add_argument(
        "--build", action="store_true", help="Build firmware"
    )
    parser.add_argument(
        "--flash", action="store_true", help="Flash firmware"
    )
    parser.add_argument(
        "--test", action="store_true", help="Run tests"
    )
    parser.add_argument(
        "--all", action="store_true", help="Build + flash + test all scenes"
    )
    parser.add_argument(
        "--scene", action="append", default=None, help="Scene name to run (repeatable)"
    )
    parser.add_argument(
        "--case", action="append", default=None, help="Case parameter value (repeatable)"
    )
    parser.add_argument(
        "--case-index", action="append", type=int, default=None, help="Case index (repeatable)"
    )
    parser.add_argument(
        "--script", type=Path, default=None, help="Override default script for the scene"
    )
    parser.add_argument(
        "hardware_map",
        help="Path to hardware-map.yml",
    )
    return parser.parse_args()


def _query_firmware_scenes(ser: SerialInstrument) -> set[str]:
    """Ask the firmware for its scene list via the REPL."""
    ser.send("vsf-test scene --list\r\n")
    time.sleep(0.3)
    output = ser.read_all(timeout=2.0)
    scenes: set[str] = set()
    for line in output.splitlines():
        line = line.strip()
        # Format: "  N scene_name"
        if line and line[0].isdigit():
            parts = line.split(None, 1)
            if len(parts) == 2:
                scenes.add(parts[1])
    return scenes


def _script_needs_la(script_path: Path | None) -> bool:
    """Heuristic: does the script actually call methods on the la object?"""
    if script_path is None:
        return False
    try:
        source = script_path.read_text()
        # Exclude the function signature line which always contains 'la:'
        return any("la." in line for line in source.splitlines() if "def run(" not in line)
    except Exception:
        return False


def _build_run_cmd(scene: str, case: str | None) -> str:
    if case:
        return f"vsf-test run {scene}.{case}\r\n"
    return f"vsf-test run {scene}\r\n"


def _run_scene(
    scene_name: str,
    script_path: Path | None,
    case_specs: list[str],
    project_root: Path,
    ser: SerialInstrument,
    la_cfg,
    run_dir: Path,
    cli_path: Path | None,
) -> bool:
    """Run a single scene end-to-end. Returns True on PASS."""
    cases_to_run = case_specs if case_specs else [None]
    ok = True
    for case in cases_to_run:
        case_tag = f".{case}" if case else ""
        print(f"\n[vsf-bench] Scene: {scene_name}{case_tag}")

        scene_la: LogicAnalyzerInstrument | None = None
        needs_la = _script_needs_la(script_path)
        if la_cfg is not None and cli_path is not None and needs_la:
            label = f"{scene_name}{('_' + case) if case else ''}"
            capture_path = run_dir / f"{label}-capture.dsl"
            scene_la = LogicAnalyzerInstrument(
                cli_path=cli_path,
                device=la_cfg.device,
                samplerate=la_cfg.samplerate,
                channels=la_cfg.channels,
                capture_path=capture_path,
            )
            scene_la.start(180.0)
            # dsview-cli needs ~3s to initialize the device before SIGTERM works reliably.
            time.sleep(3.0)

        cmd = _build_run_cmd(scene_name, case)
        ser.send(cmd)
        print(f"[vsf-bench] Triggered: {cmd.strip()}")

        # Detect firmware "Scene not found" immediately.
        try:
            ser.expect(r"Scene not found:", timeout=5.0)
            print(f"[vsf-bench] FAIL: {scene_name}{case_tag}: Scene not found in firmware")
            return False
        except TimeoutError:
            pass  # Normal path – firmware is running the scene.

        try:
            if script_path is not None:
                run_fn = load_test_script(script_path)
                sig = inspect.signature(run_fn)
                if "la" in sig.parameters:
                    run_fn(project_root, ser, scene_la)
                else:
                    run_fn(project_root, ser)
            else:
                ser.expect_test_summary(scene_name, timeout=180.0)
            print(f"[vsf-bench] PASS: {scene_name}{case_tag}")
        except (TimeoutError, AssertionError, RuntimeError, KeyError, AttributeError) as e:
            print(f"[vsf-bench] FAIL: {scene_name}{case_tag}: {e}")
            ok = False
        finally:
            if scene_la is not None:
                scene_la.stop()
                try:
                    scene_la.wait(timeout=10.0)
                except (TimeoutError, RuntimeError) as cleanup_err:
                    print(f"[vsf-bench] LA cleanup warning: {cleanup_err}")

    return ok


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

    # ----- Test mode -----
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
        if len(ordered_scenes) == 1:
            tag = ordered_scenes[0][0]
        else:
            tag = "vsf_test"
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

    # Wait for the REPL banner so the first `vsf-test run …` is not eaten by the boot stream.
    try:
        ser.expect("VSF Test Ready", timeout=10.0)
    except TimeoutError:
        print("[vsf-bench] Warning: REPL banner not seen, continuing anyway")

    # When running all discovered scenes, intersect with firmware scene list
    # so disabled scenes don't cause spurious failures.
    if not args.scene:
        fw_scenes = _query_firmware_scenes(ser)
        if fw_scenes:
            skipped = [s for s, _ in ordered_scenes if s not in fw_scenes]
            ordered_scenes = [(s, p) for s, p in ordered_scenes if s in fw_scenes]
            if skipped:
                print(f"[vsf-bench] Skipped (not in firmware): {skipped}")
        print(f"[vsf-bench] Effective scenes: {[s for s, _ in ordered_scenes]}")

    overall_pass = True
    for scene_name, script_path in ordered_scenes:
        ok = _run_scene(
            scene_name=scene_name,
            script_path=script_path,
            case_specs=case_specs,
            project_root=project_root,
            ser=ser,
            la_cfg=la_cfg,
            run_dir=run_dir,
            cli_path=cli_path,
        )
        if not ok:
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
