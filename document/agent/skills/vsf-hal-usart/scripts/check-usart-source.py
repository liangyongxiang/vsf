#!/usr/bin/env python3
"""
Deterministic check: USART source implementation completeness.
Supports both direct-reg and IPCore-based drivers.
Usage: check-usart-source.py <uart.c>
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

    is_ipcore = has(r"#define\s+__VSF_HAL_.*_CLASS_INHERIT__")
    if is_ipcore:
        print("  INFO: IPCore-based driver detected")

    # ── Guard ──
    if has(r"VSF_HAL_USE_USART\s*==\s*ENABLED"):
        say("OK", "VSF_HAL_USE_USART guard")
    else:
        say("FAIL", "missing VSF_HAL_USE_USART == ENABLED guard")

    # ── HW struct ──
    if has(r"typedef\s+struct\s+\w*.*_usart_t"):
        say("OK", "HW usart struct defined")
    elif has(r"implement\(vsf_\w+_usart_t\)"):
        say("OK", "IPCore-based struct (implement pattern)")
    else:
        say("FAIL", "missing usart struct or IPCore implement pattern")

    # ── Essential API implementations ──
    apis = [
        "usart_init", "usart_fini",
        "usart_enable", "usart_disable",
        "usart_irq_enable", "usart_irq_disable",
        "usart_status",
        "usart_rxfifo_get_data_count", "usart_rxfifo_read",
        "usart_txfifo_get_free_count", "usart_txfifo_write",
    ]
    for api in apis:
        if has(re.escape(api)):
            say("OK", f"implements _{api}")
        else:
            say("FAIL", f"missing _{api}")

    # ── Instance instantiation ──
    if has(r"VSF_USART_CFG_IMP_LV0"):
        say("OK", "VSF_USART_CFG_IMP_LV0 defined")
    else:
        say("FAIL", "missing VSF_USART_CFG_IMP_LV0 instance instantiation")

    if has(r"usart_template\.inc"):
        say("OK", "usart_template.inc included")
    else:
        say("FAIL", r'missing #include "...usart_template.inc"')

    # ── Prefix config ──
    for pref in ("VSF_USART_CFG_IMP_PREFIX", "VSF_USART_CFG_IMP_UPCASE_PREFIX"):
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

    # ── Optional DMA APIs ──
    dma_apis = ["request_rx", "request_tx", "cancel_rx", "cancel_tx",
                "get_rx_count", "get_tx_count"]
    for api in dma_apis:
        if not has(re.escape(f"_usart_{api}")):
            say("WARN", f"missing _usart_{api} (OK if fifo2req template used)")

    return errors, warnings


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <uart.c>")
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
