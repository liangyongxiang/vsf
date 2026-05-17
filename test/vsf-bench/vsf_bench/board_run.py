"""vsf-board-run — build → flash → run test script(s) → return results."""

import argparse
import importlib.util
import inspect
import json
import re
import sys
from collections.abc import Callable
from datetime import datetime
from pathlib import Path

from vsf_bench.hardware_map import load as load_hardware_map, validate_runners
from vsf_bench.runners.cmake_runner import CMakeRunner
from vsf_bench.runners.registry import get_runner_class
from vsf_bench.instruments.serial_instrument import SerialInstrument
from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument


class SerialAlreadyCompleted:
    """Serial wrapper for multi-script runs.

    The orchestrator has already consumed the real serial completion event.
    This wrapper makes expect() return immediately so scripts can be reused
    without changing their signatures.
    """

    def __init__(self, real_serial: SerialInstrument):
        self._real = real_serial

    def expect(self, pattern: str, timeout: float | None = None) -> str:
        return pattern

    def close(self) -> None:
        pass  # orchestrator owns lifecycle

    def __getattr__(self, name: str):
        return getattr(self._real, name)


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


def _load_module(path: str | Path):
    p = Path(path).resolve()
    spec = importlib.util.spec_from_file_location("scenario_probe", p)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def _gather_scenarios(script_paths: list[str]) -> set[str] | None:
    """Collect declared scenarios across scripts.

    Returns the union of each script's SCENARIOS list, or None if any
    script omits the declaration (caller should treat as run-all).
    """
    all_scenarios: set[str] = set()
    for path in script_paths:
        mod = _load_module(path)
        scenarios = getattr(mod, "SCENARIOS", None)
        if scenarios is None:
            return None
        all_scenarios.update(scenarios)
    return all_scenarios


def _gateway_dialog(serial: SerialInstrument, scenarios: set[str] | None) -> None:
    """Respond to firmware's scenario gateway dialog.

    Pre-condition: firmware has just booted; serial buffer may already
    contain GATEWAY:HELLO. If the firmware doesn't support the protocol
    (no HELLO within timeout), this is a no-op.

    With scenarios=None, every READY? gets a GO (legacy/run-all).
    """
    try:
        serial.expect(r"GATEWAY:HELLO", timeout=2.0)
    except TimeoutError:
        print("[gateway] no GATEWAY:HELLO from firmware — skipping gateway dialog")
        return

    serial.send("GATEWAY:HELLO\r\n")

    while True:
        try:
            line = serial.expect(r"SCENARIO:\w+:READY\?|GATEWAY:DONE", timeout=2.0)
        except TimeoutError:
            print("[gateway] no scenario marker after HELLO — firmware likely fell back to run-all")
            return
        if "GATEWAY:DONE" in line:
            return
        m = re.search(r"SCENARIO:(\w+):READY\?", line)
        if not m:
            continue
        name = m.group(1)
        if scenarios is None or name in scenarios:
            serial.send(f"SCENARIO:{name}:GO\r\n")
        else:
            serial.send(f"SCENARIO:{name}:SKIP\r\n")


def _call_run(run_fn: Callable[..., None], ser: SerialInstrument, la: LogicAnalyzerInstrument | None, project_root: Path) -> None:
    """Call run_fn with project_root as first positional arg. la is passed only if the script accepts it."""
    sig = inspect.signature(run_fn)
    if "la" in sig.parameters:
        run_fn(project_root, ser, la=la)
    else:
        run_fn(project_root, ser)


def _derive_run_name(script_paths: list[str]) -> str:
    if len(script_paths) == 1:
        return Path(script_paths[0]).stem
    stems = [Path(p).stem for p in script_paths]
    prefix = stems[0]
    for s in stems[1:]:
        while not s.startswith(prefix) and prefix:
            prefix = prefix[:-1]
    return prefix.rstrip("_-") if prefix else "multi"


def parse_args():
    parser = argparse.ArgumentParser(prog="vsf-board-run")
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
        "hardware_map",
        help="Path to hardware-map.yml",
    )
    parser.add_argument(
        "test_scripts",
        nargs="*",
        default=None,
        help="One or more test scripts with run(serial[, la]) function",
    )
    return parser.parse_args()


def main():
    args = parse_args()
    project_root = args.project_root.resolve()

    board = load_hardware_map(args.hardware_map)
    validate_runners(board)

    # Build
    print(f"[vsf-board-run] Building ({board.build.source_dir})...")
    cmake = CMakeRunner(board.build, project_root)
    build_dir = cmake.build()
    print(f"[vsf-board-run] Build complete: {build_dir}")

    # Flash
    runner_cfg = board.runners[board.active_runner]
    runner_cls = get_runner_class(runner_cfg.type)
    if runner_cls is None:
        print(f"[vsf-board-run] Unknown runner type: {runner_cfg.type}", file=sys.stderr)
        sys.exit(1)

    runner = runner_cls(runner_cfg)

    # Test scripts are optional — build+flash only when omitted
    if not args.test_scripts:
        print(f"[vsf-board-run] Flashing via {board.active_runner}...")
        runner.flash(build_dir)
        print("[vsf-board-run] Flash complete")
        print("[vsf-board-run] No test script provided — done.")
        return

    # Log directory
    if args.log_dir:
        run_dir = args.log_dir.resolve()
    else:
        timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
        run_name = _derive_run_name(args.test_scripts)
        run_dir = Path(f"logs/{timestamp}-{run_name}")
    run_dir.mkdir(parents=True, exist_ok=True)
    log_path = run_dir / "vsf-board-run.jsonl"

    # Create LA instrument if configured
    la: LogicAnalyzerInstrument | None = None
    if board.logic_analyzer:
        la_cfg = board.logic_analyzer
        capture_path = run_dir / f"{run_name}-capture.dsl"
        la = LogicAnalyzerInstrument(
            cli_path=project_root / la_cfg.cli,
            device=la_cfg.device,
            samplerate=la_cfg.samplerate,
            channels=la_cfg.channels,
            capture_path=capture_path,
        )
        la.start(la_cfg.capture_duration)
        print(f"[vsf-board-run] LA capture started ({la_cfg.capture_duration}s → {capture_path})")

    # Open serial BEFORE flashing so no firmware output is missed after reset.
    ser = SerialInstrument(board.serial, board.baud, audit_log=log_path)
    ser.open()

    print(f"[vsf-board-run] Flashing via {board.active_runner}...")
    runner.flash(build_dir)
    print("[vsf-board-run] Flash complete")

    # Scenario gateway: tell firmware which scenarios this run cares about.
    # If a script omits SCENARIOS, all scenarios get GO (legacy behavior).
    scenarios = _gather_scenarios(args.test_scripts)
    if scenarios is not None:
        print(f"[vsf-board-run] Scenarios: {sorted(scenarios)}")
    _gateway_dialog(ser, scenarios)

    # Single script: let the script manage serial/la lifecycle (backward compatible)
    # Multiple scripts: orchestrator waits for completion, then runs each script
    multi_mode = len(args.test_scripts) > 1

    if multi_mode:
        # Wait for firmware to complete ALL tests
        print("[vsf-board-run] Waiting for firmware test completion...")
        ser.expect("All test cases completed", timeout=120.0)
        print("[vsf-board-run] Firmware tests completed")

        # Wait for LA capture to finish
        if la is not None:
            la.wait()
            print("[vsf-board-run] LA capture done")

        # Run each script with a serial wrapper that skips expect()
        wrapped_ser = SerialAlreadyCompleted(ser)
        overall_pass = True
        for script_path in args.test_scripts:
            run_fn = load_test_script(script_path)
            print(f"\n[vsf-board-run] Running test script: {script_path}")
            try:
                _call_run(run_fn, wrapped_ser, la, project_root)
                print(f"[vsf-board-run] PASS: {script_path}")
            except (TimeoutError, AssertionError, RuntimeError) as e:
                print(f"[vsf-board-run] FAIL: {script_path}: {e}")
                overall_pass = False

        ser.close()

        with open(log_path, "a") as f:
            verdict = "pass" if overall_pass else "fail"
            f.write(json.dumps({"verdict": verdict}) + "\n")
        print(f"\n[vsf-board-run] {'PASS' if overall_pass else 'FAIL'}")
        if not overall_pass:
            sys.exit(1)
        return

    # Single-script mode (backward compatible)
    script_path = args.test_scripts[0]
    run_fn = load_test_script(script_path)
    print(f"[vsf-board-run] Running test script: {script_path}")

    try:
        _call_run(run_fn, ser, la, project_root)
        print("\n[vsf-board-run] PASS")
        with open(log_path, "a") as f:
            f.write(json.dumps({"verdict": "pass"}) + "\n")
    except (TimeoutError, AssertionError, RuntimeError) as e:
        print(f"\n[vsf-board-run] FAIL: {e}")
        with open(log_path, "a") as f:
            f.write(json.dumps({"verdict": "fail", "error": str(e)}) + "\n")
        sys.exit(1)
    finally:
        ser.close()
        if la is not None:
            la.wait()


if __name__ == "__main__":
    main()
