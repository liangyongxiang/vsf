"""board-run — build → flash → run test script → return results."""

import importlib.util
import sys
from datetime import datetime
from pathlib import Path

from vsf_bench.hardware_map import load as load_hardware_map
from vsf_bench.runners.cmake_runner import CMakeRunner
from vsf_bench.runners.swd_runner import SWDRunner
from vsf_bench.runners.uf2_runner import UF2Runner
from vsf_bench.instruments.serial_instrument import SerialInstrument

RUNNER_MAP = {
    "openocd": SWDRunner,
    "uf2": UF2Runner,
}


def load_test_script(path: str | Path):
    """Load a test script and return its run() function."""
    p = Path(path).resolve()
    spec = importlib.util.spec_from_file_location("test_script", p)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod.run


def main():
    if len(sys.argv) < 3:
        print(f"Usage: board-run <hardware-map.yml> <test_script.py>", file=sys.stderr)
        sys.exit(1)

    hwmap_path = sys.argv[1]
    test_script_path = sys.argv[2]
    project_root = Path(hwmap_path).resolve().parents[2]  # board/pico/hardware-map.yml → repo root

    board = load_hardware_map(hwmap_path)

    # Build
    print(f"[board-run] Building ({board.build.source_dir})...")
    cmake = CMakeRunner(board.build, project_root)
    build_dir = cmake.build()
    print(f"[board-run] Build complete: {build_dir}")

    # Open serial before flash so we capture boot output
    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    log_path = Path(f"logs/{timestamp}-board-run.jsonl")
    log_path.parent.mkdir(parents=True, exist_ok=True)

    ser = SerialInstrument(board.serial, board.baud, audit_log=log_path)
    ser.open()

    # Flash
    runner_cfg = board.runners[board.active_runner]
    runner_cls = RUNNER_MAP.get(runner_cfg.type)
    if runner_cls is None:
        print(f"[board-run] Unknown runner type: {runner_cfg.type}", file=sys.stderr)
        ser.close()
        sys.exit(1)

    runner = runner_cls(runner_cfg)
    print(f"[board-run] Flashing via {board.active_runner}...")
    runner.flash(build_dir)
    print("[board-run] Flash complete")

    # Run test script
    run_fn = load_test_script(test_script_path)
    print(f"[board-run] Running test script: {test_script_path}")

    try:
        run_fn(ser)
        print("\n[board-run] PASS")
    except (TimeoutError, AssertionError) as e:
        print(f"\n[board-run] FAIL: {e}")
        sys.exit(1)
    finally:
        ser.close()


if __name__ == "__main__":
    main()
