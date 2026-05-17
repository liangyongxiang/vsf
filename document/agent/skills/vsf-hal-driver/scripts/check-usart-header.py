#!/usr/bin/env python3
"""
Deterministic check: USART header completeness.
Supports both direct-reg and IPCore-based drivers.
Usage: check-usart-header.py <uart.h>
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

    # Three roles:
    #   chip-direct: chip header defining types/enums directly
    #   chip-ipcore: chip header using IPCore (includes IPCore/ header)
    #   ipcore:      IPCore implementation itself (in IPCore/ directory)
    is_ipcore = bool(re.search(r'#include\s+.*IPCore/', text))
    is_ipcore_impl = '/IPCore/' in path.replace('\\', '/')

    if is_ipcore:
        print("  INFO: chip header using IPCore — type/enum checks delegated")
    if is_ipcore_impl:
        print("  INFO: IPCore implementation — DMA/CTRL checks not applicable")

    # ── Guard ──
    if has(r"VSF_HAL_USE_USART\s*==\s*ENABLED"):
        say("OK", "VSF_HAL_USE_USART guard")
    else:
        say("FAIL", "missing VSF_HAL_USE_USART == ENABLED guard")

    # ── Template/IPCore include ──
    if has(r'#include.*vsf_template_usart\.h'):
        say("OK", "vsf_template_usart.h included")
    elif is_ipcore:
        say("OK", "types provided by IPCore header include")
    else:
        say("FAIL", "missing #include vsf_template_usart.h")

    # ── irq_mask_t (from chip or IPCore header) ──
    if has(r"vsf_usart_irq_mask_t"):
        say("OK", "vsf_usart_irq_mask_t defined")
    elif is_ipcore:
        pass  # defined in IPCore header, not in chip file
    else:
        say("FAIL", "vsf_usart_irq_mask_t not found")

    # ── IRQ mandatory bits ──
    if is_ipcore:
        pass  # defined in IPCore header
    else:
        for bit in ("TX", "RX"):
            macro = f"VSF_USART_IRQ_MASK_{bit}"
            if has(re.escape(macro)):
                say("OK", f"{macro}")
            else:
                say("FAIL", f"{macro} not found")

    # ── IRQ DMA bits (skip for IPCore implementation) ──
    if not is_ipcore_impl:
        for bit in ("TX_CPL", "RX_CPL"):
            macro = f"VSF_USART_IRQ_MASK_{bit}"
            if has(re.escape(macro)):
                say("OK", f"{macro}")
            else:
                say("WARN", f"{macro} not found (add if using DMA/fifo2req)")

    # ── Non-mandatory IRQ bits: #define present? ──
    #   For IPCore, the IPCore header provides these #define aliases.
    if not is_ipcore:
        for bit in ("RX_TIMEOUT", "CTS", "FRAME_ERR", "BREAK_ERR",
                    "PARITY_ERR", "RX_OVERFLOW_ERR", "RX_IDLE"):
            macro = f"VSF_USART_IRQ_MASK_{bit}"
            if has_define(macro):
                say("OK", f"{macro} #define present")
            else:
                say("WARN", f"{macro} #define not found (VSF treats as unsupported)")

    # ── Mode bits: skip for IPCore (provided by IPCore header) ──
    if not is_ipcore:
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

    # ── vsf_usart_isr_t (check only if not IPCore) ──
    if has(r"vsf_usart_isr_t"):
        say("OK", "vsf_usart_isr_t defined")
    elif not is_ipcore:
        say("WARN", "vsf_usart_isr_t not found")

    # ── CTRL #define (skip for IPCore — template provides defaults) ──
    if not (is_ipcore or is_ipcore_impl):
        for ctrl in ("SEND_BREAK", "SET_BREAK", "CLEAR_BREAK"):
            macro = f"VSF_USART_CTRL_{ctrl}"
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
