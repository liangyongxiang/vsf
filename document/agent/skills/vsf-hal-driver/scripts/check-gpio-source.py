#!/usr/bin/env python3
"""Deterministic check: GPIO source implementation completeness.
Usage: check-gpio-source.py <gpio.c>
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

    is_gpio_driver = "/gpio/" in str(path).replace("\\", "/") or path.name.startswith("gpio.")

    # ── Guard ──
    if has(r"VSF_HAL_USE_GPIO\s*==\s*ENABLED"):
        say("OK", "VSF_HAL_USE_GPIO guard")
    else:
        say("FAIL", "missing VSF_HAL_USE_GPIO == ENABLED guard")

    # ── HW struct ──
    if has(r"typedef\s+struct\s+\w*.*_gpio_t"):
        say("OK", "HW gpio struct defined")
    else:
        say("FAIL", "missing gpio struct")

    # ── Essential API implementations ──
    # Core pin operations (must be implemented)
    core_apis = [
        "gpio_set", "gpio_clear", "gpio_read",
        "gpio_port_config_pins",
    ]
    for api in core_apis:
        if has(re.escape(api)):
            say("OK", f"implements _{api}")
        else:
            say("FAIL", f"missing _{api}")

    # Lifecycle APIs (template may provide defaults)
    lifecycle_apis = [
        "gpio_init", "gpio_fini",
        "gpio_enable", "gpio_disable",
        "gpio_irq_enable", "gpio_irq_disable",
    ]
    for api in lifecycle_apis:
        if has(re.escape(api)):
            say("OK", f"implements _{api}")
        else:
            say("WARN", f"missing _{api} (OK if template default used)")

    # ── Instance instantiation ──
    if has(r"VSF_GPIO_CFG_IMP_LV0"):
        say("OK", "VSF_GPIO_CFG_IMP_LV0 defined")
    else:
        say("FAIL", "missing VSF_GPIO_CFG_IMP_LV0 instance instantiation")

    if has(r"gpio_template\.inc"):
        say("OK", "gpio_template.inc included")
    else:
        say("FAIL", r'missing #include "...gpio_template.inc"')

    # ── Prefix config ──
    for pref in ("VSF_GPIO_CFG_IMP_PREFIX", "VSF_GPIO_CFG_IMP_UPCASE_PREFIX"):
        if has(re.escape(pref)):
            say("OK", f"{pref} defined")
        else:
            say("FAIL", f"missing {pref}")

    # ── IRQHandler ──
    count = len(re.findall(r"_IRQHandler", text))
    if count:
        say("OK", f"{count} IRQHandler(s) defined")
    else:
        say("WARN", "no IRQHandler found")

    # ── Pinmux boundary check ──
    if is_gpio_driver:
        # GPIO driver legitimately touches IO banks
        pass
    else:
        # Non-GPIO file containing pinmux calls — flag it
        pinmux_patterns = [r"gpio_set_function\b", r"io_bank0_hw\s*->", r"pads_bank0_hw\s*->"]
        for pat in pinmux_patterns:
            if has(pat):
                say("WARN", f"pinmux register access in non-GPIO file: {pat}")

    # ── Unimplemented API convention ──
    # Look for functions that return VSF_ERR_NONE without an assert
    stub_candidates = re.findall(
        r"(?:vsf_err_t|fsm_rt_t|void|uint\w+_t|int\w+_t)\s+\w+gpio_\w+\s*\([^)]*\)\s*\{[^}]*?return\s+VSF_ERR_NONE;[^}]*?\}",
        text, re.DOTALL
    )
    for stub in stub_candidates:
        if "VSF_HAL_ASSERT(0)" not in stub:
            func_name = re.search(r"\w+gpio_\w+", stub)
            if func_name:
                say("WARN", f"stub {func_name.group()} returns VSF_ERR_NONE without VSF_HAL_ASSERT(0)")

    return errors, warnings


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <gpio.c>")
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
