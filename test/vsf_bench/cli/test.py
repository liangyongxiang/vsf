"""vsf-bench-test — standalone test entry point.

Runs the test phase against firmware that is already flashed and running.
Does not build or flash. This is the fastest iteration loop when only
the host-side test scripts are changing.

Exit codes:
  0  all tests passed
  1  one or more failures
  2  config / argument error
"""

import argparse
import sys
from pathlib import Path

from vsf_bench import pipeline


def parse_args():
    parser = argparse.ArgumentParser(prog="vsf-bench-test")
    parser.add_argument("--project-root", type=Path, default=Path.cwd())
    parser.add_argument("--log-dir", type=Path, default=None)
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
    parser.add_argument(
        "--random",
        action="store_true",
        help="Shuffle case execution order within each suite",
    )
    parser.add_argument(
        "--seed",
        type=int,
        default=None,
        help="Fixed shuffle seed for reproducibility (implies --random)",
    )
    parser.add_argument("hardware_map")
    return parser.parse_args()


def main():
    args = parse_args()
    project_root = args.project_root.resolve()
    hardware_map_path = project_root / args.hardware_map

    try:
        board = pipeline.load_board(hardware_map_path)
    except Exception as e:
        print(f"[vsf-bench-test] Config error: {e}", file=sys.stderr)
        sys.exit(2)

    case_specs: list[str] = []
    if args.case:
        case_specs.extend(args.case)
    if args.case_index:
        case_specs.extend(str(i) for i in args.case_index)

    # --case is mutually exclusive with --random: --case already specifies an
    # explicit order, so shuffling on top makes no sense.
    if (args.random or args.seed is not None) and case_specs:
        print("[vsf-bench-test] Config error: --case/--case-index cannot combine with --random/--seed",
              file=sys.stderr)
        sys.exit(2)

    shuffle_seed = None
    if args.random or args.seed is not None:
        import os
        shuffle_seed = (args.seed
                        if args.seed is not None
                        else int.from_bytes(os.urandom(4), "little") or 1)

    try:
        overall_pass = pipeline.run_test_phase(
            board=board,
            project_root=project_root,
            suite_names=args.suite,
            script_override=args.script,
            case_specs=case_specs,
            la_mode=args.la_mode,
            log_dir=args.log_dir,
            shuffle_seed=shuffle_seed,
        )
    except (ValueError, KeyError) as e:
        print(f"[vsf-bench-test] Config error: {e}", file=sys.stderr)
        sys.exit(2)
    except Exception as e:
        print(f"[vsf-bench-test] Test phase error: {e}", file=sys.stderr)
        sys.exit(1)

    if not overall_pass:
        sys.exit(1)


if __name__ == "__main__":
    main()
