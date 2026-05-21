#!/usr/bin/env python3
"""Deterministic check: SPI header completeness.
Usage: check-spi-header.py <spi.h>
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
        print("  INFO: chip header using IPCore — type/enum checks delegated")
    if is_ipcore_impl:
        print("  INFO: IPCore implementation — checks not applicable")

    # ── Guard ──
    if has(r"VSF_HAL_USE_SPI\s*==\s*ENABLED"):
        say("OK", "VSF_HAL_USE_SPI guard")
    else:
        say("FAIL", "missing VSF_HAL_USE_SPI == ENABLED guard")

    # ── Template include ──
    if has(r'#include.*vsf_template_spi\.h'):
        say("OK", "vsf_template_spi.h included")
    elif is_ipcore:
        say("OK", "types provided by IPCore header include")
    else:
        say("WARN", "missing #include vsf_template_spi.h (OK if types from driver.h)")

    # ── irq_mask_t ──
    if has(r"vsf_spi_irq_mask_t"):
        say("OK", "vsf_spi_irq_mask_t defined")
    elif is_ipcore:
        pass
    else:
        say("WARN", "vsf_spi_irq_mask_t not found (OK if from driver.h)")

    # ── IRQ bits ──
    if not is_ipcore:
        for bit in ("TX", "RX"):
            macro = f"VSF_SPI_IRQ_MASK_{bit}"
            if has(re.escape(macro)):
                say("OK", f"{macro}")
            else:
                say("WARN", f"{macro} not found")

    # ── Mode bits ──
    if not is_ipcore:
        mandatory_modes = [
            "VSF_SPI_MODE_0", "VSF_SPI_MODE_1", "VSF_SPI_MODE_2", "VSF_SPI_MODE_3",
            "VSF_SPI_DATASIZE_8", "VSF_SPI_DATASIZE_16",
            "VSF_SPI_MSB_FIRST", "VSF_SPI_LSB_FIRST",
        ]
        for bit in mandatory_modes:
            if has(re.escape(bit)):
                say("OK", f"{bit}")
            else:
                say("WARN", f"mode bit {bit} not found (OK if from driver.h)")

    # ── vsf_spi_isr_t ──
    if has(r"vsf_spi_isr_t"):
        say("OK", "vsf_spi_isr_t defined")
    elif not is_ipcore:
        say("WARN", "vsf_spi_isr_t not found")

    return errors, warnings


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <spi.h>")
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
