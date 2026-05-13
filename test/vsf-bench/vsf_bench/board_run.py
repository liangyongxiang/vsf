"""vsf-board-run — build → flash → run test script → return results."""

import argparse
import importlib.util
import inspect
import json
import sys
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
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    if not hasattr(mod, "run"):
        raise AttributeError(f"Test script {p} must define a run(serial) function")
    return mod.run


def _call_run(run_fn, ser: SerialInstrument, la: LogicAnalyzerInstrument | None) -> None:
    """Call run_fn, passing la only if the script's signature accepts it."""
    sig = inspect.signature(run_fn)
    if "la" in sig.parameters:
        run_fn(ser, la=la)
    else:
        run_fn(ser)


def parse_args():
    parser = argparse.ArgumentParser(prog="vsf-board-run")
    parser.add_argument(
        "--project-root",
        type=Path,
        default=Path.cwd(),
        help="Project root directory (default: cwd)",
    )
    parser.add_argument(
        "hardware_map",
        help="Path to hardware-map.yml",
    )
    parser.add_argument(
        "test_script",
        nargs="?",
        default=None,
        help="Optional test script with run(serial[, la]) function",
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
    print(f"[vsf-board-run] Flashing via {board.active_runner}...")
    runner.flash(build_dir)
    print("[vsf-board-run] Flash complete")

    # Test script is optional — build+flash only when omitted
    if args.test_script is None:
        print("[vsf-board-run] No test script provided — done.")
        return

    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    log_path = Path(f"logs/{timestamp}-vsf-board-run.jsonl")
    log_path.parent.mkdir(parents=True, exist_ok=True)

    # Create LA instrument if configured
    la: LogicAnalyzerInstrument | None = None
    if board.logic_analyzer:
        la_cfg = board.logic_analyzer
        capture_path = Path(f"logs/{timestamp}-capture.dsl")
        la = LogicAnalyzerInstrument(
            cli_path=project_root / la_cfg.cli,
            device=la_cfg.device,
            samplerate=la_cfg.samplerate,
            channels=la_cfg.channels,
            capture_path=capture_path,
        )
        la.start(la_cfg.capture_duration)
        print(f"[vsf-board-run] LA capture started ({la_cfg.capture_duration}s → {capture_path})")

    ser = SerialInstrument(board.serial, board.baud, audit_log=log_path)
    ser.open()

    run_fn = load_test_script(args.test_script)
    print(f"[vsf-board-run] Running test script: {args.test_script}")

    try:
        _call_run(run_fn, ser, la)
        print("\n[vsf-board-run] PASS")
        with open(log_path, "a") as f:
            f.write(json.dumps({"verdict": "pass"}) + "\n")
    except (TimeoutError, AssertionError) as e:
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
