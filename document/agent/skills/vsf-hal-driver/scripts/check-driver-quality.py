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
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


@dataclass
class Finding:
    file: Path
    line: int
    rule_id: str
    message: str
    reference: str | None = None

    def render(self) -> str:
        ref = f"  (see REFERENCE.md: {self.reference})" if self.reference else ""
        return f"{self.file}:{self.line}: [{self.rule_id}] {self.message}{ref}"


# A "scannable line" carries enough context for the rules.
@dataclass
class ScanLine:
    lineno: int
    text: str
    in_imp_lv0: bool       # inside a #define ..._IMP_LV0 multi-line macro
    in_comment: bool       # inside a /* ... */ block comment
    suppress: set[str]     # rule ids suppressed for this line


_SUPPRESS_RE = re.compile(r"//\s*quality:\s*allow-([a-z][a-z0-9-]*)")


def preprocess(text: str) -> list[ScanLine]:
    """Walk the file and tag each line with context relevant to rules.

    The two important contexts are: (a) inside a multi-line `_IMP_LV0`
    macro definition — that is precisely where per-instance literals are
    *meant* to be — and (b) inside a `/* */` block comment, where any
    identifier is documentation, not code.
    """
    lines: list[ScanLine] = []
    in_imp_lv0 = False
    in_comment = False
    macro_continues = False

    for idx, raw in enumerate(text.splitlines(), start=1):
        stripped = raw.strip()

        # Block-comment tracking (rough: doesn't handle /* */ on the same line
        # opening a new one, but the cases we care about don't do that).
        if in_comment:
            line_in_comment = True
            if "*/" in raw:
                in_comment = False
        else:
            line_in_comment = False
            if "/*" in raw and "*/" not in raw[raw.find("/*"):]:
                in_comment = True

        # IMP_LV0 macro definition tracking.
        if not macro_continues and re.search(r"#\s*define\s+\w*_IMP_LV0\b", raw):
            in_imp_lv0 = True
        if in_imp_lv0:
            macro_continues = raw.rstrip().endswith("\\")
            line_in_imp_lv0 = True
            if not macro_continues:
                in_imp_lv0 = False
        else:
            line_in_imp_lv0 = False

        suppress = set(_SUPPRESS_RE.findall(raw))
        lines.append(ScanLine(idx, raw, line_in_imp_lv0, line_in_comment, suppress))

    return lines


# ---------------------------------------------------------------- rule helpers


def _emit(lines: Iterable[ScanLine], rule_id: str, message: str,
          predicate, reference: str | None = None) -> list[Finding]:
    """Yield a finding for every line where `predicate(line)` is truthy and the
    rule is not inline-suppressed."""
    out: list[Finding] = []
    for sl in lines:
        if sl.in_comment or rule_id in sl.suppress:
            continue
        if predicate(sl):
            out.append(Finding(Path(""), sl.lineno, rule_id, message, reference))
    return out


# ---------------------------------------------------------------- rules


_INSTANCE_NAME_RE = re.compile(r"\bVSF_HW_[A-Z]+\d+_[A-Z_]+\b")


def check_hardcoded_instance_name(lines: list[ScanLine]) -> list[Finding]:
    """Driver .c references a specific instance by name (e.g. VSF_HW_USART0_REG)
    instead of expanding it through VSF_MCONNECT in IMP_LV0."""
    def predicate(sl: ScanLine) -> bool:
        if sl.in_imp_lv0:
            return False
        return bool(_INSTANCE_NAME_RE.search(sl.text))
    return _emit(lines, "hardcoded-instance-name",
                 "per-instance literal (VSF_HW_<P><N>_*) outside IMP_LV0 — "
                 "should expand via VSF_MCONNECT(..., __IDX, ...)",
                 predicate, reference="Per-instance parameterization in device.h")


_INDEX_BRANCH_IF_RE = re.compile(r"\bif\s*\(\s*\w*(?:idx|index|inst|instance)\w*\s*==\s*\d")
_INDEX_BRANCH_SWITCH_RE = re.compile(r"\bswitch\s*\(\s*\w*(?:idx|index|inst|instance)\w*\s*\)")


def check_instance_index_branch(lines: list[ScanLine]) -> list[Finding]:
    """Driver dispatches behavior on instance index (e.g. `if (idx == 0)`).
    The per-instance differences should be parameterized in device.h, not
    branched in the driver."""
    def predicate(sl: ScanLine) -> bool:
        if sl.in_imp_lv0:
            return False
        return bool(_INDEX_BRANCH_IF_RE.search(sl.text)
                    or _INDEX_BRANCH_SWITCH_RE.search(sl.text))
    return _emit(lines, "instance-index-branch",
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
    return _emit(lines, "hardcoded-irq",
                 "literal IRQ name (e.g. UART0_IRQn) — should come from "
                 "VSF_HW_<P><N>_IRQN macro and reach the driver via IMP_LV0",
                 predicate, reference="Per-instance parameterization in device.h")


_RESET_NAME_RE = re.compile(r"\b(?:RESET|RST)_[A-Z]+\d+\b")


def check_hardcoded_reset(lines: list[ScanLine]) -> list[Finding]:
    def predicate(sl: ScanLine) -> bool:
        if sl.in_imp_lv0:
            return False
        return bool(_RESET_NAME_RE.search(sl.text))
    return _emit(lines, "hardcoded-reset",
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
    return _emit(lines, "hardcoded-clock",
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
    return _emit(lines, "hardcoded-address",
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
    return _emit(lines, "missing-vsf-mconnect",
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
    return _emit(lines, "pinmux-in-driver",
                 "pinmux call in peripheral driver — move to board file",
                 predicate, reference="Board wiring")


RULES = [
    check_hardcoded_instance_name,
    check_instance_index_branch,
    check_hardcoded_irq,
    check_hardcoded_reset,
    check_hardcoded_clock,
    check_hardcoded_address,
    check_missing_vsf_mconnect,
    check_pinmux_in_driver,
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
        return 2

    paths: list[Path] = []
    for raw in argv[1:]:
        p = Path(raw)
        if not p.is_file():
            print(f"error: not a file: {raw}", file=sys.stderr)
            return 2
        paths.append(p)

    all_findings: list[Finding] = []
    for p in paths:
        all_findings.extend(check_file(p))

    for f in all_findings:
        print(f.render())

    if all_findings:
        print(f"\nFAIL: {len(all_findings)} finding(s) in {len(paths)} file(s)")
        return 1
    print(f"PASS: {len(paths)} file(s) clean")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
