"""vsf-bench — unified build / flash / test pipeline.

Composes `pipeline.build_phase`, `pipeline.flash_phase`, and
`pipeline.run_test_phase` based on the requested flags. Standalone
scripts (`vsf-bench-build`, `vsf-bench-flash`, `vsf-bench-test`) call
the same underlying functions, so behavior stays consistent.

LA capture supports two modes (see `--la-mode`):
  * `shared` (default): a single LA capture for the entire run; uses
    --decode-start / --decode-end so each suite's decode only scans its
    own window.
  * `per-suite`: one LA capture per suite; small files decode fast.
"""

import argparse
import sys
from pathlib import Path

from vsf_bench import pipeline


def parse_args():
    parser = argparse.ArgumentParser(prog="vsf-bench")
    parser.add_argument("--project-root", type=Path, default=Path.cwd())
    parser.add_argument("--log-dir", type=Path, default=None)
    parser.add_argument("--build", action="store_true")
    parser.add_argument("--flash", action="store_true")
    parser.add_argument("--test", action="store_true")
    parser.add_argument("--all", action="store_true")
    parser.add_argument("--suite", action="append", default=None)
    parser.add_argument("--case", action="append", default=None)
    parser.add_argument("--case-index", action="append", type=int, default=None)
    parser.add_argument("--script", type=Path, default=None)
    parser.add_argument(
        "--la-mode",
        choices=["per-suite", "shared"],
        default="shared",
        help="LA capture lifetime",
    )
    parser.add_argument("hardware_map")
    return parser.parse_args()


def main():
    args = parse_args()
    project_root = args.project_root.resolve()

    do_build = args.build or args.all
    do_flash = args.flash or args.all
    do_test = args.test or args.all

    if not (do_build or do_flash or do_test):
        print("[vsf-bench] Error: at least one of --build, --flash, --test, --all is required")
        sys.exit(2)

    hardware_map_path = project_root / args.hardware_map
    try:
        board = pipeline.load_board(hardware_map_path)
    except Exception as e:
        print(f"[vsf-bench] Config error: {e}", file=sys.stderr)
        sys.exit(2)

    build_dir = project_root / board.build.build_dir

    if do_build:
        try:
            build_dir = pipeline.build_phase(board, project_root)
        except Exception as e:
            print(f"[vsf-bench] Build failed: {e}", file=sys.stderr)
            sys.exit(1)

    if do_flash:
        if not build_dir.exists():
            print(f"[vsf-bench] Build directory missing: {build_dir}", file=sys.stderr)
            print(f"[vsf-bench] Run with --build first.", file=sys.stderr)
            sys.exit(2)
        try:
            pipeline.flash_phase(board, build_dir)
        except Exception as e:
            print(f"[vsf-bench] Flash failed: {e}", file=sys.stderr)
            sys.exit(1)

    if not do_test:
        return

    case_specs: list[str] = []
    if args.case:
        case_specs.extend(args.case)
    if args.case_index:
        case_specs.extend(str(i) for i in args.case_index)

    try:
        overall_pass = pipeline.run_test_phase(
            board=board,
            project_root=project_root,
            suite_names=args.suite,
            script_override=args.script,
            case_specs=case_specs,
            la_mode=args.la_mode,
            log_dir=args.log_dir,
        )
    except Exception as e:
        print(f"[vsf-bench] Test phase error: {e}", file=sys.stderr)
        sys.exit(1)

    if not overall_pass:
        sys.exit(1)


if __name__ == "__main__":
    main()
