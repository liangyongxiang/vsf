#!/usr/bin/env python3
"""Shared infrastructure for VSF HAL driver checkers."""

from __future__ import annotations

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
    severity: str = "error"  # "error" or "warn"

    def render(self) -> str:
        ref = f"  (see REFERENCE.md: {self.reference})" if self.reference else ""
        sev_tag = "WARN" if self.severity == "warn" else "FAIL"
        return f"{self.file}:{self.line}: [{self.rule_id}] {sev_tag}: {self.message}{ref}"


# ---------------------------------------------------------------- scan line context

@dataclass
class ScanLine:
    lineno: int
    text: str
    in_imp_lv0: bool       # inside a #define ..._IMP_LV0 multi-line macro
    in_comment: bool       # inside a /* */ block comment
    suppress: set[str]     # rule ids suppressed for this line


_SUPPRESS_RE = re.compile(r"//\s*quality:\s*allow-([a-z][a-z0-9-]*)")


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
