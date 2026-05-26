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
    EXIT_WARNING,
    Finding,
    ScanLine,
    preprocess,
    emit,
    extract_functions,
    load_pattern_rules,
    check_pattern_rules,
)

_SCRIPT_DIR = Path(__file__).parent.resolve()
_PATTERN_RULES = load_pattern_rules(_SCRIPT_DIR / "quality-rules.yml")


# ---------------------------------------------------------------- pattern rules (Python: special logic)

_LITERAL_ADDR_RE = re.compile(r"\b0x([0-9A-Fa-f]{8,})\b")


def _looks_like_mask(hex_digits: str) -> bool:
    s = hex_digits.upper()
    chars = set(s)
    if chars.issubset(set("F0")):
        return True
    nonzero = chars - {"0"}
    return len(nonzero) == 1


def check_hardcoded_address(lines: list[ScanLine]) -> list[Finding]:
    def predicate(sl: ScanLine) -> bool:
        if sl.in_imp_lv0:
            return False
        if sl.text.lstrip().startswith("#"):
            return False
        m = _LITERAL_ADDR_RE.search(sl.text)
        if not m:
            return False
        if _looks_like_mask(m.group(1)):
            return False
        return True
    return emit(lines, "hardcoded-address",
                 "literal hex address in driver body — wire it through device.h instead",
                 predicate, reference="Per-instance parameterization in device.h")


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


_SPIN_WAIT_RE = re.compile(r"\bwhile\s*\([^;{]*\)\s*;")
_SPIN_WAIT_KEYWORDS = frozenset({
    "spin-wait", "spinwait", "busy-wait", "busywait",
    "wait", "poll", "polling",
    "abort", "reset", "ready", "done", "complete", "completion",
    "busy", "idle",
    "cycle", "us", "μs", "microsecond",
    "delay", "timeout",
})


def check_spin_wait_comment(lines: list[ScanLine]) -> list[Finding]:
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


_CHIP_CONSTANT_SUFFIXES_EXTENDED = frozenset({
    "SIZE", "SECTOR_SIZE", "PAGE_SIZE", "BLOCK_SIZE",
    "XIP_BASE", "SECTOR_NUM", "CHANNEL_NUM", "CHANNEL_COUNT",
    "PER_INSTANCE",
})


def _extract_chip_prefix(path: Path) -> str | None:
    parts = list(path.parts)
    for i, p in enumerate(parts):
        if p.lower() == 'driver':
            if i + 4 < len(parts):
                return parts[i + 2].upper()
            break
    return None


def check_chip_prefixed_define(lines: list[ScanLine], path: Path) -> list[Finding]:
    if path.name.lower() == "device.h":
        return []
    if path.name.lower().startswith("vsf_board") or path.name.lower() == "board.c":
        return []

    chip_prefix = _extract_chip_prefix(path)
    if not chip_prefix:
        return []

    findings: list[Finding] = []
    _define_re = re.compile(r'#\s*define\s+([A-Z][A-Z0-9]*)_([A-Z][A-Z0-9_]*)')

    for sl in lines:
        if sl.in_comment:
            continue
        m = _define_re.search(sl.text)
        if not m:
            continue
        prefix = m.group(1)
        rest = m.group(2)

        if prefix.startswith("VSF") or prefix.startswith("__VSF"):
            continue

        if prefix == chip_prefix:
            if any(s in rest for s in _CHIP_CONSTANT_SUFFIXES_EXTENDED):
                findings.append(Finding(
                    path, sl.lineno, "chip-prefixed-define",
                    f"chip-prefixed constant '#define {prefix}_{rest}' in driver file — "
                    f"move to device.h as VSF_HW_<PERIPH>_...",
                    reference="Convention 13: No magic numbers",
                ))

    return findings


PATTERN_RULES = [
    check_hardcoded_address,
    check_macro_backslash_align,
    check_spin_wait_comment,
]


# ---------------------------------------------------------------- function-level checks


def _line_in_func(func: dict, needle: str) -> int:
    """Return 0-based line index of first occurrence of *needle* in function body,
    or -1 if not found."""
    for i, line in enumerate(func["lines"]):
        if needle in line:
            return i
    return -1


def _func_has_any(func: dict, *needles: str) -> bool:
    """Return True if any needle appears anywhere in the function body."""
    body = func["body"]
    return any(n in body for n in needles)


# Patterns that exempt an init() from clock/reset requirements.
# Example:  // no clock gate: RP2040 timer is always-on
_CLOCK_EXEMPT_RE = re.compile(
    r"//\s*no[-\s]clock[-\s]?gate\b|/\*\s*no[-\s]clock[-\s]?gate\b",
    re.IGNORECASE,
)
_RESET_EXEMPT_RE = re.compile(
    r"//\s*no[-\s]reset\b|/\*\s*no[-\s]reset\b",
    re.IGNORECASE,
)


def _func_has_exemption(func: dict, exempt_re: re.Pattern) -> bool:
    """Return True if the function body contains an exemption comment."""
    for line in func["lines"]:
        if exempt_re.search(line):
            return True
    return False


def check_nvic_priority_order(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """In init(), NVIC_SetPriority must come before NVIC_EnableIRQ."""
    errors: list[Finding] = []
    for func in funcs:
        if "_init" not in func["name"]:
            continue
        setprio = _line_in_func(func, "NVIC_SetPriority")
        enable = _line_in_func(func, "NVIC_EnableIRQ")
        if enable < 0:
            continue                    # no NVIC enable — nothing to check
        if setprio < 0:
            errors.append(Finding(
                path, func["start_line"] + enable, "nvic-priority-order",
                f"{func['name']} calls NVIC_EnableIRQ without preceding NVIC_SetPriority",
                reference="IRQ enable in init()",
            ))
        elif setprio >= enable:
            errors.append(Finding(
                path, func["start_line"] + enable, "nvic-priority-order",
                f"NVIC_EnableIRQ appears before NVIC_SetPriority in {func['name']}",
                reference="IRQ enable in init()",
            ))
    return errors, []


def check_init_has_reset(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """init() that uses NVIC should also deassert the peripheral reset."""
    warnings: list[Finding] = []
    reset_markers = ("reset_hw", "resets_hw", "RST_BIT", "rst_bit", "reset &=", "reset |=")
    for func in funcs:
        if "_init" not in func["name"]:
            continue
        if not _func_has_any(func, "NVIC_EnableIRQ"):
            continue                    # not a real init — skip
        if _func_has_exemption(func, _RESET_EXEMPT_RE):
            continue
        if not _func_has_any(func, *reset_markers):
            warnings.append(Finding(
                path, func["start_line"], "init-has-reset",
                f"{func['name']} has NVIC_EnableIRQ but no reset deassert — "
                f"add reset_hw->reset &= ~rst_bit",
                reference="Clock and reset",
                severity="warn",
            ))
    return [], warnings


def check_init_has_clock(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """init() that uses NVIC should also enable the peripheral clock gate."""
    warnings: list[Finding] = []
    clock_markers = ("clk_bit", "CLK_BIT", "clock_hw", "clock_get_hz",
                     "AHBENR", "APBENR", "AHB1ENR", "APB1ENR",
                     "vsf_hw_peripheral_enable", "peripheral_enable")
    for func in funcs:
        if "_init" not in func["name"]:
            continue
        if not _func_has_any(func, "NVIC_EnableIRQ"):
            continue
        if _func_has_exemption(func, _CLOCK_EXEMPT_RE):
            continue
        if not _func_has_any(func, *clock_markers):
            warnings.append(Finding(
                path, func["start_line"], "init-has-clock",
                f"{func['name']} has NVIC_EnableIRQ but no clock gate enable — "
                f"add clock enable before register access",
                reference="Clock and reset",
                severity="warn",
            ))
    return [], warnings


def check_fini_nvic_order(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """fini() must call NVIC_DisableIRQ before clearing peripheral IRQ enable bits."""
    errors: list[Finding] = []
    for func in funcs:
        if "_fini" not in func["name"]:
            continue
        disable = _line_in_func(func, "NVIC_DisableIRQ")
        if disable < 0:
            continue
        # Look for peripheral-level IRQ clear (e.g. reg->IER &= ~mask)
        peri_clear = -1
        for i, line in enumerate(func["lines"]):
            if re.search(r'reg->\w+\s*&=\s*~', line):
                peri_clear = i
                break
        if peri_clear >= 0 and disable > peri_clear:
            errors.append(Finding(
                path, func["start_line"] + disable, "fini-nvic-order",
                f"NVIC_DisableIRQ appears after peripheral IRQ clear in {func['name']} — "
                f"disable NVIC first to prevent racing IRQ pends",
                reference="IRQ disable in fini()",
            ))
    return errors, []


def check_irq_disable_nvic_leak(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """irq_disable() must only clear peripheral-level IRQ bits; never call NVIC_DisableIRQ."""
    errors: list[Finding] = []
    for func in funcs:
        if "_irq_disable" not in func["name"]:
            continue
        for i, line in enumerate(func["lines"]):
            if "NVIC_DisableIRQ" in line:
                errors.append(Finding(
                    path, func["start_line"] + i, "irq-disable-nvic-leak",
                    f"{func['name']} calls NVIC_DisableIRQ — "
                    f"peripheral irq_disable must only clear reg->IER bits; "
                    f"NVIC_DisableIRQ belongs in fini()",
                    reference="NVIC and peripheral IRQ separation",
                ))
                break
    return errors, []


def check_init_null_isr(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """init() with NVIC_EnableIRQ should handle cfg_ptr->isr.handler_fn == NULL
    by ensuring interrupts are disabled (NVIC_DisableIRQ or clearing peripheral
    IRQ enable bits)."""
    warnings: list[Finding] = []
    for func in funcs:
        if "_init" not in func["name"]:
            continue
        if not _func_has_any(func, "NVIC_EnableIRQ"):
            continue
        # Match either explicit == NULL or != NULL branch
        has_null_check = bool(re.search(
            r'handler_fn\s*[!=]=\s*NULL|NULL\s*[!=]=\s*handler_fn',
            func["body"]
        ))
        if not has_null_check:
            warnings.append(Finding(
                path, func["start_line"], "init-null-isr-no-disable",
                f"{func['name']} enables NVIC unconditionally — "
                f"add a handler_fn != NULL guard; when NULL call NVIC_DisableIRQ(irqn)",
                reference="Init without ISR handler",
                severity="warn",
            ))
    return [], warnings


def check_silent_freq_default(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """cfg_ptr->clock_hz (or freq) being 0 must return VSF_ERR_INVALID_PARAMETER;
    silently substituting a default hides the misconfiguration."""
    errors: list[Finding] = []
    _freq_default_re = re.compile(
        r'\bif\s*\(\s*(?:freq|clock_hz)\s*==\s*0\s*\)\s*\{?\s*\b(?:freq|clock_hz)\s*=',
        re.IGNORECASE | re.DOTALL,
    )
    for func in funcs:
        m = _freq_default_re.search(func["body"])
        if m:
            # Find the line number of the match
            line_offset = func["body"][:m.start()].count("\n")
            errors.append(Finding(
                path, func["start_line"] + line_offset, "silent-freq-default",
                f"{func['name']} silently substitutes default frequency for 0 — "
                f"return VSF_ERR_INVALID_PARAMETER instead",
                reference="Invalid frequency",
            ))
    return errors, []


def check_mode_bits_translation(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """Detect mode bits translated via if/else instead of direct register mapping.

    When VSF mode bits naturally align with hardware register fields, the enum
    should be reimplemented in the chip-specific .h so init() can extract them
    with shifts/masks instead of branching. See convention 8.
    """
    warnings: list[Finding] = []
    # Match if (something_mode_something & something_MODE_something) { ... }
    # with register manipulation inside the block.
    _mode_branch_re = re.compile(
        r'if\s*\([^)]*mode[^)]*&[^)]*MODE_[^)]*\)\s*\{[^{}]*?\w+\s*\|?=\s*[^;]+;[^{}]*?\}',
        re.IGNORECASE | re.DOTALL,
    )
    for func in funcs:
        if "_init" not in func["name"]:
            continue
        m = _mode_branch_re.search(func["body"])
        if m:
            line_offset = func["body"][:m.start()].count("\n")
            warnings.append(Finding(
                path, func["start_line"] + line_offset, "mode-bits-translation",
                f"{func['name']}: mode bits appear to be translated via if/else — "
                f"consider reimplementing the enum to encode register bits directly "
                f"(convention 8: Mode/config bits map hardware registers)",
                reference="Conventions",
                severity="warn",
            ))
    return [], warnings


FUNC_RULES = [
    check_nvic_priority_order,
    check_init_has_reset,
    check_init_has_clock,
    check_fini_nvic_order,
    check_irq_disable_nvic_leak,
    check_init_null_isr,
    check_silent_freq_default,
    check_mode_bits_translation,
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
                    "hardcoded-reset", "hardcoded-clock",
                    "chip-prefixed-define"}
    if name.startswith("vsf_board") or name == "board.c":
        skipped |= {"pinmux-in-driver", "chip-prefixed-define"}
    # driver.c contains chip-level system initialization (PLL, global resets)
    # that does not use per-instance IMP_LV0 parameterization.
    if name == "driver.c":
        skipped |= {"hardcoded-reset"}
    # A gpio driver legitimately touches GPIO function selectors; the
    # pinmux-in-driver rule targets non-GPIO drivers. Detect by path.
    if "/gpio/" in str(path).replace("\\", "/") or path.name.startswith("gpio."):
        skipped |= {"pinmux-in-driver"}
    return skipped


def check_file(path: Path) -> tuple[list[Finding], list[Finding]]:
    text = path.read_text()
    lines = preprocess(text)
    skip = filename_skip_rules(path)
    error_findings: list[Finding] = []
    warn_findings: list[Finding] = []

    # YAML pattern rules (bulk of the checks)
    for f in check_pattern_rules(lines, _PATTERN_RULES, path):
        if f.rule_id in skip:
            continue
        error_findings.append(f)

    # Python pattern rules (rules with special logic)
    for rule in PATTERN_RULES:
        for f in rule(lines):
            if f.rule_id in skip:
                continue
            f.file = path
            if f.severity == "warn":
                warn_findings.append(f)
            else:
                error_findings.append(f)

    # Path-dependent check for chip-prefixed constants
    for f in check_chip_prefixed_define(lines, path):
        if f.rule_id in skip:
            continue
        if f.severity == "warn":
            warn_findings.append(f)
        else:
            error_findings.append(f)

    funcs = extract_functions(text)
    for checker in FUNC_RULES:
        errs, warns = checker(funcs, path)
        error_findings.extend(errs)
        warn_findings.extend(warns)

    return error_findings, warn_findings


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

    all_errors: list[Finding] = []
    all_warns: list[Finding] = []
    for p in paths:
        errs, warns = check_file(p)
        all_errors.extend(errs)
        all_warns.extend(warns)

    for f in all_errors + all_warns:
        print(f.render())

    if all_errors:
        print(f"\nFAIL: {len(all_errors)} error(s), {len(all_warns)} warning(s)")
        return EXIT_ERROR
    elif all_warns:
        print(f"\nPASS: {len(all_warns)} warning(s) (review and proceed)")
        return EXIT_WARNING
    print(f"PASS: {len(paths)} file(s) clean")
    return EXIT_PASS


if __name__ == "__main__":
    sys.exit(main(sys.argv))
