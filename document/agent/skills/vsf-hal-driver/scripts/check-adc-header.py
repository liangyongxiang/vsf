#!/usr/bin/env python3
"""Deterministic check: ADC header completeness.
Usage: check-adc-header.py <adc.h>
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

    is_ipcore = bool(re.search(r'#include\s+.*IPCore/', text))
    is_ipcore_impl = '/IPCore/' in path.replace('\\', '/')

    if is_ipcore:
        print("  INFO: chip header using IPCore")
    if is_ipcore_impl:
        print("  INFO: IPCore implementation")

    # ── Guard ──
    if has(r"VSF_HAL_USE_ADC\s*==\s*ENABLED"):
        say("OK", "VSF_HAL_USE_ADC guard")
    else:
        say("FAIL", "missing VSF_HAL_USE_ADC == ENABLED guard")

    # ── Template include ──
    if has(r'#include.*vsf_template_adc\.h'):
        say("OK", "vsf_template_adc.h included")
    elif is_ipcore:
        say("OK", "types provided by IPCore header include")
    else:
        say("WARN", "missing #include vsf_template_adc.h (OK if types from driver.h)")

    # ── irq_mask_t ──
    if has(r"vsf_adc_irq_mask_t"):
        say("OK", "vsf_adc_irq_mask_t defined")
    elif is_ipcore:
        pass
    else:
        say("WARN", "vsf_adc_irq_mask_t not found (OK if from driver.h)")

    # ── IRQ bits ──
    if not is_ipcore:
        for bit in ("CPL",):
            macro = f"VSF_ADC_IRQ_MASK_{bit}"
            if has(re.escape(macro)):
                say("OK", f"{macro}")
            else:
                say("WARN", f"{macro} not found")

    # ── vsf_adc_isr_t ──
    if has(r"vsf_adc_isr_t"):
        say("OK", "vsf_adc_isr_t defined")
    elif not is_ipcore:
        say("WARN", "vsf_adc_isr_t not found")

    return errors, warnings


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <adc.h>")
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
