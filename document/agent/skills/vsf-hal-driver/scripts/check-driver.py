#!/usr/bin/env python3
"""
Unified entry point for VSF HAL driver checks.

Usage:
    check-driver.py --periph usart --side header uart.h     # structure mode
    check-driver.py --quality uart.c                         # quality mode

Exit codes:
    0 = pass
    1 = error
    2 = warning only
"""

import argparse
import subprocess
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent.resolve()


def main() -> int:
    parser = argparse.ArgumentParser(description="Unified VSF HAL driver checker.")
    parser.add_argument("--periph", help="Peripheral name (for structure mode)")
    parser.add_argument("--side", choices=["header", "source"], help="Which side to check")
    parser.add_argument("--quality", action="store_true", help="Quality mode (anti-pattern detection)")
    parser.add_argument("file", nargs="+", help="Path to driver file(s)")
    args = parser.parse_args()

    if args.quality:
        cmd = [sys.executable, str(SCRIPT_DIR / "check-driver-quality.py")] + args.file
    else:
        if not args.periph or not args.side:
            parser.error("--periph and --side are required for structure mode")
        cmd = [sys.executable, str(SCRIPT_DIR / "check-driver-structure.py"),
               "--periph", args.periph, "--side", args.side] + args.file

    result = subprocess.run(cmd)
    return result.returncode


if __name__ == "__main__":
    sys.exit(main())
