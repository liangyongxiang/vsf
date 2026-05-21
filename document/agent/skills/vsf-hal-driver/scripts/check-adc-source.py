#!/usr/bin/env python3
"""Deterministic check: ADC source implementation completeness.
Usage: check-adc-source.py <adc.c>
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
    if has(r"VSF_HAL_USE_ADC\s*==\s*ENABLED"):
        say("OK", "VSF_HAL_USE_ADC guard")
    else:
        say("FAIL", "missing VSF_HAL_USE_ADC == ENABLED guard")

    # ── HW struct ──
    if has(r"typedef\s+struct\s+\w*.*_adc_t"):
        say("OK", "HW adc struct defined")
    elif has(r"implement\(vsf_\w+_adc_t\)"):
        say("OK", "IPCore-based struct (implement pattern)")
    else:
        say("FAIL", "missing adc struct or IPCore implement pattern")

    # ── Essential API implementations ──
    apis = [
        "adc_init", "adc_fini",
        "adc_enable", "adc_disable",
        "adc_irq_enable", "adc_irq_disable",
    ]
    for api in apis:
        if has(re.escape(api)):
            say("OK", f"implements _{api}")
        else:
            say("FAIL", f"missing _{api}")

    # ADC conversion/read APIs
    adc_apis = ["adc_channel_request_once", "adc_channel_request", "adc_start", "adc_stop", "adc_read", "adc_calibrate"]
    adc_found = any(has(re.escape(a)) for a in adc_apis)
    if adc_found:
        say("OK", "conversion/read API present")
    else:
        say("FAIL", "no conversion/read API found")

    # ── Instance instantiation ──
    if has(r"VSF_ADC_CFG_IMP_LV0"):
        say("OK", "VSF_ADC_CFG_IMP_LV0 defined")
    else:
        say("FAIL", "missing VSF_ADC_CFG_IMP_LV0 instance instantiation")

    if has(r"adc_template\.inc"):
        say("OK", "adc_template.inc included")
    else:
        say("FAIL", r'missing #include "...adc_template.inc"')

    # ── Prefix config ──
    for pref in ("VSF_ADC_CFG_IMP_PREFIX", "VSF_ADC_CFG_IMP_UPCASE_PREFIX"):
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
        print(f"Usage: {sys.argv[0]} <adc.c>")
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
