#!/usr/bin/env python3
"""Deterministic check: PWM source implementation completeness.
Usage: check-pwm-source.py <pwm.c>
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
    if has(r"VSF_HAL_USE_PWM\s*==\s*ENABLED"):
        say("OK", "VSF_HAL_USE_PWM guard")
    else:
        say("FAIL", "missing VSF_HAL_USE_PWM == ENABLED guard")

    # ── HW struct ──
    if has(r"typedef\s+struct\s+\w*.*_pwm_t"):
        say("OK", "HW pwm struct defined")
    elif has(r"implement\(vsf_\w+_pwm_t\)"):
        say("OK", "IPCore-based struct (implement pattern)")
    else:
        say("FAIL", "missing pwm struct or IPCore implement pattern")

    # ── Essential API implementations ──
    core_apis = [
        "pwm_init", "pwm_fini",
        "pwm_enable", "pwm_disable",
    ]
    for api in core_apis:
        if has(re.escape(api)):
            say("OK", f"implements _{api}")
        else:
            say("FAIL", f"missing _{api}")

    # IRQ APIs (PWM may not use interrupts)
    for api in ("pwm_irq_enable", "pwm_irq_disable"):
        if has(re.escape(api)):
            say("OK", f"implements _{api}")
        else:
            say("WARN", f"missing _{api} (OK if PWM has no IRQ)")

    # PWM config/set APIs
    pwm_apis = ["pwm_set", "pwm_config_channel", "pwm_set_duty", "pwm_set_period"]
    pwm_found = any(has(re.escape(a)) for a in pwm_apis)
    if pwm_found:
        say("OK", "PWM set/config API present")
    else:
        say("FAIL", "no PWM set/config API found")

    # ── Instance instantiation ──
    if has(r"VSF_PWM_CFG_IMP_LV0"):
        say("OK", "VSF_PWM_CFG_IMP_LV0 defined")
    else:
        say("FAIL", "missing VSF_PWM_CFG_IMP_LV0 instance instantiation")

    if has(r"pwm_template\.inc"):
        say("OK", "pwm_template.inc included")
    else:
        say("FAIL", r'missing #include "...pwm_template.inc"')

    # ── Prefix config ──
    for pref in ("VSF_PWM_CFG_IMP_PREFIX", "VSF_PWM_CFG_IMP_UPCASE_PREFIX"):
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
        print(f"Usage: {sys.argv[0]} <pwm.c>")
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
