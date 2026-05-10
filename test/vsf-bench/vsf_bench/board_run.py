"""board-run — build → flash → run test script → return results."""

import argparse
import importlib.util
import json
import sys
from datetime import datetime
from pathlib import Path

from vsf_bench.hardware_map import load as load_hardware_map, validate_runners
from vsf_bench.runners.cmake_runner import CMakeRunner
from vsf_bench.runners.registry import get_runner_class
from vsf_bench.instruments.serial_instrument import SerialInstrument


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


def parse_args():
    parser = argparse.ArgumentParser(prog="board-run")
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
        help="Optional test script with run(serial) function",
    )
    return parser.parse_args()


def main():
    args = parse_args()

    board = load_hardware_map(args.hardware_map)
    validate_runners(board)

    # Build
    print(f"[board-run] Building ({board.build.source_dir})...")
    cmake = CMakeRunner(board.build, args.project_root.resolve())
    build_dir = cmake.build()
    print(f"[board-run] Build complete: {build_dir}")

    # Flash
    runner_cfg = board.runners[board.active_runner]
    runner_cls = get_runner_class(runner_cfg.type)
    if runner_cls is None:
        print(f"[board-run] Unknown runner type: {runner_cfg.type}", file=sys.stderr)
        sys.exit(1)

    runner = runner_cls(runner_cfg)
    print(f"[board-run] Flashing via {board.active_runner}...")
    runner.flash(build_dir)
    print("[board-run] Flash complete")

    # Test script is optional — build+flash only when omitted
    if args.test_script is None:
        print("[board-run] No test script provided — done.")
        return

    # Open serial before running test script
    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    log_path = Path(f"logs/{timestamp}-board-run.jsonl")
    log_path.parent.mkdir(parents=True, exist_ok=True)

    ser = SerialInstrument(board.serial, board.baud, audit_log=log_path)
    ser.open()

    run_fn = load_test_script(args.test_script)
    print(f"[board-run] Running test script: {args.test_script}")

    try:
        run_fn(ser)
        print("\n[board-run] PASS")
        with open(log_path, "a") as f:
            f.write(json.dumps({"verdict": "pass"}) + "\n")
    except (TimeoutError, AssertionError) as e:
        print(f"\n[board-run] FAIL: {e}")
        with open(log_path, "a") as f:
            f.write(json.dumps({"verdict": "fail", "error": str(e)}) + "\n")
        sys.exit(1)
    finally:
        ser.close()


if __name__ == "__main__":
    main()
