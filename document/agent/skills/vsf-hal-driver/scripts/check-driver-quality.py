#!/usr/bin/env python3
"""
Deterministic check: VSF HAL driver anti-pattern detector.

Scans one or more driver .c / .h files for code that branches on hardware
instance, hardcodes per-instance register/IRQ/clock/reset values, or otherwise
violates the parameterize-everything-in-device.h convention.

Each rule fires independently. Lines containing `// quality: allow-<rule-id>`
suppress that specific rule for that line.

Usage:
    check-driver-quality.py <file> [<file> ...]

Exit codes:
    0 = clean
    1 = at least one finding
    2 = script error (bad arguments, missing file)
"""

from __future__ import annotations

import re
import sys
from pathlib import Path
from typing import Iterable

from checker_base import (
    EXIT_PASS,
    EXIT_ERROR,
    Finding,
    ScanLine,
    preprocess,
    emit,
)


# ---------------------------------------------------------------- rules


_INSTANCE_NAME_RE = re.compile(r"\bVSF_HW_[A-Z]+\d+_[A-Z_]+\b")


def check_hardcoded_instance_name(lines: list[ScanLine]) -> list[Finding]:
    """Driver .c references a specific instance by name (e.g. VSF_HW_USART0_REG)
    instead of expanding it through VSF_MCONNECT in IMP_LV0."""
    def predicate(sl: ScanLine) -> bool:
        if sl.in_imp_lv0:
            return False
        return bool(_INSTANCE_NAME_RE.search(sl.text))
    return emit(lines, "hardcoded-instance-name",
                 "per-instance literal (VSF_HW_<P><N>_*) outside IMP_LV0 — "
                 "should expand via VSF_MCONNECT(..., __IDX, ...)",
                 predicate, reference="Per-instance parameterization in device.h")


_INDEX_BRANCH_IF_RE = re.compile(r"\bif\s*\(\s*\w*(?:idx|index|inst|instance)\w*\s*==\s*\d")
_INDEX_BRANCH_SWITCH_RE = re.compile(r"\bswitch\s*\(\s*\w*(?:idx|index|inst|instance)\w*\s*\)")
# Pointer equality against a per-instance SDK handle (e.g. `reg == spi0_hw`).
# Names of the form `<periph><digit>_hw` are the RP2040/pico SDK convention
# for instance pointers (spi0_hw, spi1_hw, uart0_hw, etc.). Bare `adc_hw`,
# `dma_hw`, `resets_hw` are single-instance so they don't trigger.
_INDEX_BRANCH_PTR_RE = re.compile(r"==\s*&?[a-z]+\d+_hw\b|\b[a-z]+\d+_hw\s*==\s*&?\w")


def check_instance_index_branch(lines: list[ScanLine]) -> list[Finding]:
    """Driver dispatches behavior on instance index (e.g. `if (idx == 0)` or
    `reg == spi0_hw`).  The per-instance differences should be parameterized
    in device.h, not branched in the driver."""
    def predicate(sl: ScanLine) -> bool:
        if sl.in_imp_lv0:
            return False
        return bool(_INDEX_BRANCH_IF_RE.search(sl.text)
                    or _INDEX_BRANCH_SWITCH_RE.search(sl.text)
                    or _INDEX_BRANCH_PTR_RE.search(sl.text))
    return emit(lines, "instance-index-branch",
                 "branching on instance index — parameterize the difference "
                 "in device.h as a per-instance macro instead",
                 predicate, reference="Per-instance parameterization in device.h")


# Match bare IRQ names like UART0_IRQn / UART0_IRQHandler.
# Skip when preceded by `VSF_HW_`, `VSF_MCONNECT` token-paste, `__IDX` macro arg
# expansion, or part of `_RST_BIT` style suffixes.
_IRQ_NAME_RE = re.compile(
    r"(?<![\w])"                  # not part of a longer identifier
    r"(?<!VSF_HW_)"               # not VSF_HW_-prefixed
    r"([A-Z][A-Z0-9_]*\d+)_(?:IRQn|IRQHandler)\b"
)


def check_hardcoded_irq(lines: list[ScanLine]) -> list[Finding]:
    def predicate(sl: ScanLine) -> bool:
        if sl.in_imp_lv0:
            return False
        m = _IRQ_NAME_RE.search(sl.text)
        if not m:
            return False
        # Skip multi-channel offset patterns where a single-instance peripheral
        # exposes one IRQ per channel and the driver accesses them by base + idx
        # (e.g. `TIMER_IRQ_0_IRQn + timer_idx`). The driver author cannot
        # parameterize the vendor's IRQ naming and the arithmetic is itself the
        # right form.
        tail = sl.text[m.end():].lstrip()
        if tail.startswith("+"):
            return False
        return True
    return emit(lines, "hardcoded-irq",
                 "literal IRQ name (e.g. UART0_IRQn) — should come from "
                 "VSF_HW_<P><N>_IRQN macro and reach the driver via IMP_LV0",
                 predicate, reference="Per-instance parameterization in device.h")


_RESET_NAME_RE = re.compile(
    # Long form (e.g. RP2040 SDK): RESETS_RESET_<PERIPH>_<FIELD>
    r"\bRESETS_RESET_[A-Z][A-Z0-9_]*_(?:LSB|MSB|BITS|MASK|RESET|ACCESS)\b"
    # Short form with trailing digit (e.g. RESET_UART0, RST_UART0)
    r"|\b(?:RESET|RST)_[A-Z]+\d+\b"
    # Short form without digit — known RP2040 enum entries for single-instance
    # peripherals (e.g. RESET_RTC, RESET_ADC). Add more chip vocabularies here
    # as new ports show up.
    r"|\b(?:RESET|RST)_(?:ADC|BUSCTRL|DMA|JTAG|PWM|RTC|SYSCFG|SYSINFO|TBMAN|TIMER|USBCTRL|PLL_SYS|PLL_USB|IO_BANK\d|IO_QSPI|PADS_BANK\d|PADS_QSPI)\b"
)


def check_hardcoded_reset(lines: list[ScanLine]) -> list[Finding]:
    def predicate(sl: ScanLine) -> bool:
        if sl.in_imp_lv0:
            return False
        return bool(_RESET_NAME_RE.search(sl.text))
    return emit(lines, "hardcoded-reset",
                 "literal reset bit / register — pass it through as a "
                 "per-instance macro (VSF_HW_<P><N>_RST_BIT)",
                 predicate, reference="Per-instance parameterization in device.h")


_CLOCK_NAME_RE = re.compile(
    r"\b(?:CLK|CLOCK)_[A-Z]+\d+\b"
    r"|\bRCC_[A-Z0-9_]*EN\b"
)


def check_hardcoded_clock(lines: list[ScanLine]) -> list[Finding]:
    def predicate(sl: ScanLine) -> bool:
        if sl.in_imp_lv0:
            return False
        return bool(_CLOCK_NAME_RE.search(sl.text))
    return emit(lines, "hardcoded-clock",
                 "literal clock gate / bit — parameterize as a per-instance "
                 "macro (VSF_HW_<P><N>_CLK_BIT or similar)",
                 predicate, reference="Per-instance parameterization in device.h")


# Eight or more hex digits, not anchored to a typedef cast that's clearly used
# as a defensive constant pattern (e.g. masks).
_LITERAL_ADDR_RE = re.compile(r"\b0x([0-9A-Fa-f]{8,})\b")


def _looks_like_mask(hex_digits: str) -> bool:
    """0xFFFFFFFF, 0x00FFFFFF, 0xFF00FF00 etc. are masks, not addresses.
    Heuristic: only F and 0 (case-insensitive), or only one unique non-zero
    nibble."""
    s = hex_digits.upper()
    chars = set(s)
    if chars.issubset(set("F0")):
        return True
    nonzero = chars - {"0"}
    if len(nonzero) == 1:
        return True
    return False


def check_hardcoded_address(lines: list[ScanLine]) -> list[Finding]:
    """Hex literals with 8+ digits in a .c body strongly suggest a base
    address baked into the driver. False positives (e.g. mask constants like
    0xFFFFFFFF) are filtered out by shape; remaining false positives can be
    silenced with `// quality: allow-hardcoded-address`."""
    def predicate(sl: ScanLine) -> bool:
        if sl.in_imp_lv0:
            return False
        # Skip preprocessor lines (defines / includes) — addresses in those
        # are typically deliberate config.
        if sl.text.lstrip().startswith("#"):
            return False
        m = _LITERAL_ADDR_RE.search(sl.text)
        if not m:
            return False
        if _looks_like_mask(m.group(1)):
            return False
        return True
    return emit(lines, "hardcoded-address",
                 "literal hex address in driver body — wire it through "
                 "device.h instead",
                 predicate, reference="Per-instance parameterization in device.h")


# Function definitions/prototypes using a plain vsf_hw_<periph>_<api>( signature
# instead of VSF_MCONNECT(VSF_..._CFG_IMP_PREFIX, _<api>). This catches drivers
# that haven't been template-migrated yet.
_PLAIN_PREFIX_DEF_RE = re.compile(
    r"\b(?:vsf_err_t|fsm_rt_t|void|uint\w+_t|int\w+_t|bool|"
    r"vsf_\w+)\s+vsf_hw_[a-z]+_[a-z_]+\s*\("
)


def check_missing_vsf_mconnect(lines: list[ScanLine]) -> list[Finding]:
    def predicate(sl: ScanLine) -> bool:
        if sl.in_imp_lv0:
            return False
        # Function pointer typedefs and IRQ handler dispatch lines are not
        # what we want to flag — the rule targets API entry points whose name
        # should be macro-built via VSF_MCONNECT.
        if "VSF_MCONNECT" in sl.text:
            return False
        return bool(_PLAIN_PREFIX_DEF_RE.search(sl.text))
    return emit(lines, "missing-vsf-mconnect",
                 "function definition uses hardcoded `vsf_hw_<periph>_` "
                 "prefix — wrap with VSF_MCONNECT(VSF_..._CFG_IMP_PREFIX, _<api>)",
                 predicate, reference="Macro prefix convention")


_PINMUX_API_RE = re.compile(
    r"\bgpio_set_function\b"
    r"|\bio_bank0_hw\s*->"
    r"|\bsio_hw\s*->\s*gpio_oe\b"
    r"|\bvsf_gpio_port_config_pins\s*\("
)


def check_pinmux_in_driver(lines: list[ScanLine]) -> list[Finding]:
    """Pinmux belongs in the board file. Driver .c files should never touch
    GPIO function selectors or IO banks. The board file passes pins fully
    configured before init()."""
    def predicate(sl: ScanLine) -> bool:
        # We don't skip in_imp_lv0 here — pinmux in IMP_LV0 would be just as wrong.
        return bool(_PINMUX_API_RE.search(sl.text))
    return emit(lines, "pinmux-in-driver",
                 "pinmux call in peripheral driver — move to board file",
                 predicate, reference="Board wiring")


# Multi-line #define macro continuation backslash must align to column 81
# (content length 80 + '\' at column 81).
_BACKSLASH_TARGET_COL = 81


def check_macro_backslash_align(lines: list[ScanLine]) -> list[Finding]:
    def predicate(sl: ScanLine) -> bool:
        stripped = sl.text.rstrip()
        if not stripped.rstrip().endswith("\\"):
            return False
        content = stripped.rstrip().rstrip("\\").rstrip()
        content_len = len(content)
        backslash_col = len(stripped)
        if content_len > _BACKSLASH_TARGET_COL - 1:
            return True
        return backslash_col != _BACKSLASH_TARGET_COL
    return emit(lines, "macro-backslash-align",
                 "multi-line #define continuation backslash not at column 81 — "
                 "run fix-macro-align.py to auto-fix",
                 predicate, reference="Macro formatting convention")


def check_spin_wait_comment(lines: list[ScanLine]) -> list[Finding]:
    """Any `while (cond);` empty-loop polling a hardware register must have an
    explanatory comment preceding it (see REFERENCE.md: spin-wait convention)."""
    _SPIN_WAIT_RE = re.compile(r"\bwhile\s*\([^;{]*\)\s*;")
    _SPIN_WAIT_KEYWORDS = frozenset({
        "spin-wait", "spinwait", "busy-wait", "busywait",
        "wait", "poll", "polling",
        "abort", "reset", "ready", "done", "complete", "completion",
        "busy", "idle",
        "cycle", "us", "μs", "microsecond",
        "delay", "timeout",
    })

    def _has_explanation(idx: int) -> bool:
        for j in range(max(0, idx - 3), idx):
            sl = lines[j]
            txt = sl.text
            comment_part = ""
            if sl.in_comment:
                comment_part = txt.strip().lstrip("*").strip()
            elif "//" in txt:
                comment_part = txt.split("//", 1)[1]
            elif "/*" in txt:
                start = txt.find("/*")
                end = txt.find("*/", start)
                if end == -1:
                    comment_part = txt[start:]
                else:
                    comment_part = txt[start:end + 2]
            if comment_part:
                lower = comment_part.lower()
                if any(kw in lower for kw in _SPIN_WAIT_KEYWORDS):
                    return True
        return False

    findings: list[Finding] = []
    for i, sl in enumerate(lines):
        if sl.in_comment:
            continue
        m = _SPIN_WAIT_RE.search(sl.text)
        if not m:
            continue
        inner = m.group(0).split("(", 1)[1].rsplit(")", 1)[0].strip()
        if inner in ("1", "0", "true", "false", "TRUE", "FALSE", "ENABLED", "DISABLED"):
            continue
        if not _has_explanation(i):
            findings.append(Finding(
                Path(""), sl.lineno, "spin-wait-no-comment",
                "bare spin-wait loop without explanatory comment — "
                "add a comment explaining why and expected duration (< X us)",
                reference="Spin-wait on hardware state"))
    return findings


RULES = [
    check_hardcoded_instance_name,
    check_instance_index_branch,
    check_hardcoded_irq,
    check_hardcoded_reset,
    check_hardcoded_clock,
    check_hardcoded_address,
    check_missing_vsf_mconnect,
    check_pinmux_in_driver,
    check_macro_backslash_align,
    check_spin_wait_comment,
]


# ---------------------------------------------------------------- per-file driver


# Some files are not driver implementations and should be skipped to avoid
# noise: device.h is the *destination* of per-instance literals; a board
# file is the *destination* of pinmux. The script accepts these but applies
# rules narrowly.
def filename_skip_rules(path: Path) -> set[str]:
    name = path.name.lower()
    skipped: set[str] = set()
    if name == "device.h":
        skipped |= {"hardcoded-instance-name", "hardcoded-irq",
                    "hardcoded-reset", "hardcoded-clock"}
    if name.startswith("vsf_board") or name == "board.c":
        skipped |= {"pinmux-in-driver"}
    # A gpio driver legitimately touches GPIO function selectors; the
    # pinmux-in-driver rule targets non-GPIO drivers. Detect by path.
    if "/gpio/" in str(path).replace("\\", "/") or path.name.startswith("gpio."):
        skipped |= {"pinmux-in-driver"}
    return skipped


def check_file(path: Path) -> list[Finding]:
    text = path.read_text()
    lines = preprocess(text)
    skip = filename_skip_rules(path)
    findings: list[Finding] = []
    for rule in RULES:
        for f in rule(lines):
            if f.rule_id in skip:
                continue
            f.file = path
            findings.append(f)
    return findings


# ---------------------------------------------------------------- main


def main(argv: list[str]) -> int:
    if len(argv) < 2:
        print(f"Usage: {argv[0]} <file> [<file> ...]", file=sys.stderr)
        return EXIT_ERROR

    paths: list[Path] = []
    for raw in argv[1:]:
        p = Path(raw)
        if not p.is_file():
            print(f"error: not a file: {raw}", file=sys.stderr)
            return EXIT_ERROR
        paths.append(p)

    all_findings: list[Finding] = []
    for p in paths:
        all_findings.extend(check_file(p))

    for f in all_findings:
        print(f.render())

    if all_findings:
        print(f"\nFAIL: {len(all_findings)} finding(s) in {len(paths)} file(s)")
        return EXIT_ERROR
    print(f"PASS: {len(paths)} file(s) clean")
    return EXIT_PASS


if __name__ == "__main__":
    sys.exit(main(sys.argv))
