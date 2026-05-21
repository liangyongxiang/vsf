#!/usr/bin/env python3
"""Deterministic check: SPI source implementation completeness.
Usage: check-spi-source.py <spi.c>
Exit: 0=pass, 1=errors, 2=warnings
"""

import re
import sys
from pathlib import Path


def check_source(path: str) -> tuple[int, int]:
    errors = 0
    warnings = 0
    text = Path(path).read_text()

    def has(pattern: str) -> bool:
        return bool(re.search(pattern, text, re.MULTILINE))

    def say(kind: str, msg: str):
        nonlocal errors, warnings
        if kind == "OK":
            print(f"  OK: {msg}")
        elif kind == "FAIL":
            print(f"  FAIL: {msg}")
            errors += 1
        else:
            print(f"  WARN: {msg}")
            warnings += 1

    # ── Guard ──
    if has(r"VSF_HAL_USE_SPI\s*==\s*ENABLED"):
        say("OK", "VSF_HAL_USE_SPI guard")
    else:
        say("FAIL", "missing VSF_HAL_USE_SPI == ENABLED guard")

    # ── HW struct ──
    if has(r"typedef\s+struct\s+\w*.*_spi_t"):
        say("OK", "HW spi struct defined")
    elif has(r"implement\(vsf_\w+_spi_t\)"):
        say("OK", "IPCore-based struct (implement pattern)")
    else:
        say("FAIL", "missing spi struct or IPCore implement pattern")

    # ── Essential API implementations ──
    apis = [
        "spi_init", "spi_fini",
        "spi_enable", "spi_disable",
        "spi_irq_enable", "spi_irq_disable",
    ]
    for api in apis:
        if has(re.escape(api)):
            say("OK", f"implements _{api}")
        else:
            say("FAIL", f"missing _{api}")

    # Transfer APIs (at least one)
    xfer_apis = ["spi_fifo_transfer", "spi_request_transfer", "spi_master_xfer", "spi_slave_xfer"]
    xfer_found = any(has(re.escape(a)) for a in xfer_apis)
    if xfer_found:
        say("OK", "transfer API present")
    else:
        say("FAIL", "no transfer API found")

    # Status/capability
    for api in ("spi_status", "spi_capability"):
        if has(re.escape(api)):
            say("OK", f"_{api} present")
        else:
            say("WARN", f"_{api} not found")

    # ── Instance instantiation ──
    if has(r"VSF_SPI_CFG_IMP_LV0"):
        say("OK", "VSF_SPI_CFG_IMP_LV0 defined")
    else:
        say("FAIL", "missing VSF_SPI_CFG_IMP_LV0 instance instantiation")

    if has(r"spi_template\.inc"):
        say("OK", "spi_template.inc included")
    else:
        say("FAIL", r'missing #include "...spi_template.inc"')

    # ── Prefix config ──
    for pref in ("VSF_SPI_CFG_IMP_PREFIX", "VSF_SPI_CFG_IMP_UPCASE_PREFIX"):
        if has(re.escape(pref)):
            say("OK", f"{pref} defined")
        else:
            say("FAIL", f"missing {pref}")

    # ── IRQHandler ──
    count = len(re.findall(r"_IRQHandler", text))
    if count:
        say("OK", f"{count} IRQHandler(s) defined")
    else:
        say("WARN", "no IRQHandler found (OK if IPCore dispatch used)")

    return errors, warnings


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <spi.c>")
        sys.exit(1)
    path = sys.argv[1]
    if not Path(path).is_file():
        print(f"FAIL: file not found: {path}")
        sys.exit(1)
    print(f"=== Checking {path} ===")
    errors, warnings = check_source(path)
    print()
    if errors:
        print(f"FAIL: {errors} essential check(s) failed")
        sys.exit(1)
    elif warnings:
        print(f"PASS: all essential checks passed ({warnings} warnings)")
        sys.exit(2)
    else:
        print("PASS: all checks passed")
        sys.exit(0)


if __name__ == "__main__":
    main()
