"""vsf-bench-build — standalone build entry point.

Invokes cmake on the source/build directories declared in hardware-map.yml.
Does not flash or test.

Exit codes:
  0  build complete
  1  cmake error
  2  config / argument error
"""

import argparse
import sys
from pathlib import Path

from vsf_bench import pipeline


def main():
    parser = argparse.ArgumentParser(prog="vsf-bench-build")
    parser.add_argument("--project-root", type=Path, default=Path.cwd())
    parser.add_argument("--source-dir", type=str, default=None)
    parser.add_argument("--build-dir", type=str, default=None)
    parser.add_argument("hardware_map")
    args = parser.parse_args()

    project_root = args.project_root.resolve()
    hardware_map_path = project_root / args.hardware_map

    try:
        board = pipeline.load_board(hardware_map_path)
    except Exception as e:
        print(f"[vsf-bench-build] Config error: {e}", file=sys.stderr)
        sys.exit(2)

    if args.source_dir:
        board.build.source_dir = args.source_dir
        if not args.build_dir:
            board.build.build_dir = str(Path(args.source_dir) / "build")
    if args.build_dir:
        board.build.build_dir = args.build_dir

    try:
        pipeline.build_phase(board)
    except Exception as e:
        print(f"[vsf-bench-build] Build failed: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
