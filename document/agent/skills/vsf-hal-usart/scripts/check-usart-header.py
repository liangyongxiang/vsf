#!/usr/bin/env python3
"""
Deterministic check: USART header completeness.
Usage: check-usart-header.py <uart.h>
Exit: 0=pass, 1=errors, 2=warnings-only
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
    if has(r"VSF_HAL_USE_USART\s*==\s*ENABLED"):
        say("OK", "VSF_HAL_USE_USART guard")
    else:
        say("FAIL", "missing VSF_HAL_USE_USART == ENABLED guard")

    # ── Template include ──
    if has(r'#include.*vsf_template_usart\.h'):
        say("OK", "vsf_template_usart.h included")
    else:
        say("FAIL", "missing #include vsf_template_usart.h")

    # ── irq_mask_t ──
    if has(r"vsf_usart_irq_mask_t"):
        say("OK", "vsf_usart_irq_mask_t defined")
    else:
        say("FAIL", "vsf_usart_irq_mask_t not found")

    # ── IRQ mandatory bits ──
    for bit in ("TX", "RX"):
        macro = f"VSF_USART_IRQ_MASK_{bit}"
        if has(re.escape(macro)):
            say("OK", f"{macro}")
        else:
            say("FAIL", f"{macro} not found")

    # ── IRQ DMA bits (warn only) ──
    for bit in ("TX_CPL", "RX_CPL"):
        macro = f"VSF_USART_IRQ_MASK_{bit}"
        if has(re.escape(macro)):
            say("OK", f"{macro}")
        else:
            say("WARN", f"{macro} not found (add if using DMA/fifo2req)")

    # ── Non-mandatory IRQ bits: enum present → #define required ──
    for bit in ("RX_TIMEOUT", "CTS", "FRAME_ERR", "BREAK_ERR",
                "PARITY_ERR", "RX_OVERFLOW_ERR", "RX_IDLE"):
        macro = f"VSF_USART_IRQ_MASK_{bit}"
        in_enum = has(rf"^\s+{re.escape(macro)}\s*=")
        if in_enum:
            if has_define(macro):
                say("OK", f"{macro} in enum + #define present")
            else:
                say("WARN", f"{macro} in enum but missing #define (VSF treats as unsupported)")

    # ── Mandatory mode bits ──
    mandatory_modes = [
        "VSF_USART_NO_PARITY", "VSF_USART_EVEN_PARITY", "VSF_USART_ODD_PARITY",
        "VSF_USART_1_STOPBIT", "VSF_USART_8_BIT_LENGTH",
        "VSF_USART_NO_HWCONTROL",
        "VSF_USART_TX_ENABLE", "VSF_USART_RX_ENABLE",
        "VSF_USART_TX_FIFO_THRESHOLD_EMPTY",
        "VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY",
    ]
    for bit in mandatory_modes:
        if has(re.escape(bit)):
            say("OK", f"{bit}")
        else:
            say("FAIL", f"mandatory mode bit {bit} not found")

    # ── Placeholder mode bits ──
    placeholders = [
        "VSF_USART_9_BIT_LENGTH",
        "VSF_USART_1_5_STOPBIT", "VSF_USART_0_5_STOPBIT",
        "VSF_USART_10_BIT_LENGTH",
        "VSF_USART_SYNC_CLOCK_ENABLE", "VSF_USART_SYNC_CLOCK_DISABLE",
        "VSF_USART_HALF_DUPLEX_ENABLE", "VSF_USART_HALF_DUPLEX_DISABLE",
    ]
    for bit in placeholders:
        if has(re.escape(bit)):
            say("OK", f"placeholder {bit}")
        else:
            say("FAIL", f"missing placeholder {bit} (build compat)")

    # ── vsf_usart_isr_t ──
    if has(r"vsf_usart_isr_t"):
        say("OK", "vsf_usart_isr_t defined")

    # ── CTRL #define ──
    for ctrl in ("SEND_BREAK", "SET_BREAK", "CLEAR_BREAK"):
        macro = f"VSF_USART_CTRL_{ctrl}"
        if has(re.escape(macro)):
            if has_define(macro):
                say("OK", f"{macro} with #define")
            else:
                say("WARN", f"{macro} missing #define")

    return errors, warnings


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <uart.h>")
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
