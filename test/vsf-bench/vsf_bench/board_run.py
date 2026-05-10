"""board-run — build → flash → run test script → return results."""

import importlib.util
import subprocess
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


def find_project_root(hwmap_path: str) -> Path:
    """Find the git repository root containing hardware-map.yml."""
    hwmap = Path(hwmap_path).resolve()
    try:
        result = subprocess.run(
            ["git", "rev-parse", "--show-toplevel"],
            capture_output=True, text=True, check=True,
            cwd=str(hwmap.parent),
        )
        return Path(result.stdout.strip())
    except (subprocess.CalledProcessError, FileNotFoundError):
        # Fallback: walk up to find CONTEXT.md or .git
        for parent in hwmap.parents:
            if (parent / ".git").exists() or (parent / "CONTEXT.md").exists():
                return parent
        raise RuntimeError(f"Cannot find project root from {hwmap_path}")


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


def main():
    if len(sys.argv) < 3:
        print(f"Usage: board-run <hardware-map.yml> <test_script.py>", file=sys.stderr)
        sys.exit(1)

    hwmap_path = sys.argv[1]
    test_script_path = sys.argv[2]
    project_root = find_project_root(hwmap_path)

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
        with open(log_path, "a") as f:
            import json
            f.write(json.dumps({"verdict": "pass"}) + "\n")
    except (TimeoutError, AssertionError) as e:
        print(f"\n[board-run] FAIL: {e}")
        with open(log_path, "a") as f:
            import json
            f.write(json.dumps({"verdict": "fail", "error": str(e)}) + "\n")
        sys.exit(1)
    finally:
        ser.close()


if __name__ == "__main__":
    main()
