#!/usr/bin/env python3
"""Deterministic check: GPIO header completeness.
Usage: check-gpio-header.py <gpio.h>
Exit: 0=pass, 1=errors, 2=warnings
"""

import re
import sys
from pathlib import Path


def check_header(path: str) -> tuple[int, int]:
    errors = 0
    warnings = 0
    text = Path(path).read_text()

    def has(pattern: str) -> bool:
        return bool(re.search(pattern, text, re.MULTILINE))

    def has_define(macro: str) -> bool:
        return bool(re.search(rf'^\s*#\s*define\s+{re.escape(macro)}\b', text, re.MULTILINE))

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
    if has(r"VSF_HAL_USE_GPIO\s*==\s*ENABLED"):
        say("OK", "VSF_HAL_USE_GPIO guard")
    else:
        say("FAIL", "missing VSF_HAL_USE_GPIO == ENABLED guard")

    # ── Template include ──
    if has(r'#include.*vsf_template_gpio\.h'):
        say("OK", "vsf_template_gpio.h included")
    else:
        say("WARN", "missing #include vsf_template_gpio.h (OK if types from driver.h)")

    # ── Mode bits (GPIO doesn't always have a redefined mode enum) ──
    for bit in ("VSF_GPIO_INPUT", "VSF_GPIO_OUTPUT_PUSH_PULL", "VSF_GPIO_OUTPUT_OPEN_DRAIN",
                "VSF_GPIO_AF", "VSF_GPIO_ANALOG", "VSF_GPIO_EXTI"):
        if has(re.escape(bit)):
            say("OK", f"{bit}")
        else:
            say("WARN", f"{bit} not found")

    for bit in ("VSF_GPIO_PULL_UP", "VSF_GPIO_PULL_DOWN", "VSF_GPIO_NO_PULL_UP_DOWN"):
        if has(re.escape(bit)):
            say("OK", f"{bit}")
        else:
            say("WARN", f"{bit} not found")

    # ── Capability macros ──
    for cap in ("VSF_GPIO_CAP_IRQ", "VSF_GPIO_CAP_IO_MODE"):
        if has_define(cap):
            say("OK", f"{cap} #define present")
        else:
            say("WARN", f"{cap} #define not found")

    # ── vsf_gpio_isr_t or exti handler typedef ──
    if has(r"vsf_gpio_isr_t|vsf_gpio_exti_isr_handler_t"):
        say("OK", "ISR handler type defined")
    else:
        say("WARN", "ISR handler type not found")

    return errors, warnings


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <gpio.h>")
        sys.exit(1)
    path = sys.argv[1]
    if not Path(path).is_file():
        print(f"FAIL: file not found: {path}")
        sys.exit(1)
    print(f"=== Checking {path} ===")
    errors, warnings = check_header(path)
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
