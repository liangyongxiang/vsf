"""vsf-bench-flash — standalone flash entry point.

Flashes the artifact produced by a prior build to the board via the
active runner declared in hardware-map.yml.

Exit codes:
  0  flash complete
  1  runner error (OpenOCD failure, missing UF2 mount, …)
  2  config / argument error
"""

import argparse
import sys
from pathlib import Path

from vsf_bench import pipeline


def main():
    parser = argparse.ArgumentParser(prog="vsf-bench-flash")
    parser.add_argument("hardware_map")
    args = parser.parse_args()

    hardware_map_path = Path(args.hardware_map)

    try:
        board = pipeline.load_board(hardware_map_path)
    except Exception as e:
        print(f"[vsf-bench-flash] Config error: {e}", file=sys.stderr)
        sys.exit(2)

    build_dir = Path(board.build.build_dir)
    if not build_dir.exists():
        print(f"[vsf-bench-flash] Build directory missing: {build_dir}", file=sys.stderr)
        print(f"[vsf-bench-flash] Run vsf-bench-build first.", file=sys.stderr)
        sys.exit(2)

    try:
        pipeline.flash_phase(board, build_dir)
    except Exception as e:
        print(f"[vsf-bench-flash] Flash failed: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
