#!/usr/bin/env python3
"""Deterministic check: I2C source implementation completeness.
Usage: check-i2c-source.py <i2c.c>
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
    if has(r"VSF_HAL_USE_I2C\s*==\s*ENABLED"):
        say("OK", "VSF_HAL_USE_I2C guard")
    else:
        say("FAIL", "missing VSF_HAL_USE_I2C == ENABLED guard")

    # ── HW struct ──
    if has(r"typedef\s+struct\s+\w*.*_i2c_t"):
        say("OK", "HW i2c struct defined")
    elif has(r"implement\(vsf_\w+_i2c_t\)"):
        say("OK", "IPCore-based struct (implement pattern)")
    else:
        say("FAIL", "missing i2c struct or IPCore implement pattern")

    # ── Essential API implementations ──
    apis = [
        "i2c_init", "i2c_fini",
        "i2c_enable", "i2c_disable",
        "i2c_irq_enable", "i2c_irq_disable",
    ]
    for api in apis:
        if has(re.escape(api)):
            say("OK", f"implements _{api}")
        else:
            say("FAIL", f"missing _{api}")

    # Master/Slave APIs (at least one set should exist)
    master_apis = ["i2c_master_request", "i2c_master_xfer", "i2c_master_fifo_transfer"]
    slave_apis = ["i2c_slave_set_address", "i2c_slave_get_address", "i2c_slave_xfer", "i2c_slave_request"]

    master_found = any(has(re.escape(a)) for a in master_apis)
    slave_found = any(has(re.escape(a)) for a in slave_apis)

    if master_found:
        say("OK", "master API present")
    else:
        say("WARN", "no master API found")

    if slave_found:
        say("OK", "slave API present")
    else:
        say("WARN", "no slave API found")

    if not master_found and not slave_found:
        say("FAIL", "neither master nor slave API found")

    # ── Instance instantiation ──
    if has(r"VSF_I2C_CFG_IMP_LV0"):
        say("OK", "VSF_I2C_CFG_IMP_LV0 defined")
    else:
        say("FAIL", "missing VSF_I2C_CFG_IMP_LV0 instance instantiation")

    if has(r"i2c_template\.inc"):
        say("OK", "i2c_template.inc included")
    else:
        say("FAIL", r'missing #include "...i2c_template.inc"')

    # ── Prefix config ──
    for pref in ("VSF_I2C_CFG_IMP_PREFIX", "VSF_I2C_CFG_IMP_UPCASE_PREFIX"):
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
        print(f"Usage: {sys.argv[0]} <i2c.c>")
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
