#!/usr/bin/env python3
"""Shared infrastructure for VSF HAL driver checkers."""

import re
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


# ---------------------------------------------------------------- exit codes
EXIT_PASS = 0
EXIT_ERROR = 1
EXIT_WARNING = 2


# ---------------------------------------------------------------- findings

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


# ---------------------------------------------------------------- scan line context

@dataclass
class ScanLine:
    lineno: int
    text: str
    in_imp_lv0: bool       # inside a #define ..._IMP_LV0 multi-line macro
    in_comment: bool       # inside a /* */ block comment
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
            if "/*" in raw:
                start = raw.find("/*")
                if "*/" in raw[start:]:
                    # Single-line block comment — mark this line only
                    line_in_comment = True
                else:
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


# ---------------------------------------------------------------- quality rule helpers


def emit(lines: Iterable[ScanLine], rule_id: str, message: str,
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


# ---------------------------------------------------------------- structured check helpers


class ResultAccumulator:
    """Collects OK/FAIL/WARN messages and tracks counts for structured checks."""

    def __init__(self):
        self.errors = 0
        self.warnings = 0

    def say(self, kind: str, msg: str):
        if kind == "OK":
            print(f"  OK: {msg}")
        elif kind == "FAIL":
            print(f"  FAIL: {msg}")
            self.errors += 1
        else:
            print(f"  WARN: {msg}")
            self.warnings += 1

    def finalize(self, label: str = "") -> int:
        print()
        if self.errors:
            print(f"FAIL: {self.errors} essential check(s) failed")
            return EXIT_ERROR
        elif self.warnings:
            print(f"PASS: all essential checks passed ({self.warnings} warnings)")
            return EXIT_WARNING
        else:
            print("PASS: all checks passed")
            return EXIT_PASS
