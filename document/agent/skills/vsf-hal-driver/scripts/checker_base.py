#!/usr/bin/env python3
"""Shared infrastructure for VSF HAL driver checkers.

C parsing is delegated to _c_parser (tree-sitter based).  If tree-sitter is
not available the module falls back to the hand-rolled parser with a warning.
"""

from __future__ import annotations

import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


# ---------------------------------------------------------------- C parser backend

_PREPROCESS_IMPL = None
_EXTRACT_FUNCTIONS_IMPL = None


def _init_parser():
    global _PREPROCESS_IMPL, _EXTRACT_FUNCTIONS_IMPL
    if _PREPROCESS_IMPL is not None:
        return

    try:
        from _c_parser import preprocess as _pp, extract_functions as _ef
        _PREPROCESS_IMPL = _pp
        _EXTRACT_FUNCTIONS_IMPL = _ef
    except ImportError:
        print("checker_base: tree-sitter not available, using hand-rolled C parser"
              " (pip install tree-sitter tree-sitter-c)",
              file=sys.stderr)
        _PREPROCESS_IMPL = _preprocess_handrolled
        _EXTRACT_FUNCTIONS_IMPL = _extract_functions_handrolled


def preprocess(text: str) -> list[ScanLine]:
    _init_parser()
    return _PREPROCESS_IMPL(text)


def extract_functions(text: str) -> list[dict]:
    _init_parser()
    return _EXTRACT_FUNCTIONS_IMPL(text)


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


# ---------------------------------------------------------------- fallback: hand-rolled C parser
# Kept as fallback when tree-sitter is not installed.  Entirely replaced by
# _c_parser.py when tree-sitter + tree-sitter-c are available.


def _preprocess_handrolled(text: str) -> list[ScanLine]:
    lines: list[ScanLine] = []
    in_imp_lv0 = False
    in_comment = False
    macro_continues = False

    for idx, raw in enumerate(text.splitlines(), start=1):
        stripped = raw.strip()

        if in_comment:
            line_in_comment = True
            if "*/" in raw:
                in_comment = False
        else:
            line_in_comment = False
            if "/*" in raw:
                start = raw.find("/*")
                if "*/" in raw[start:]:
                    line_in_comment = True
                else:
                    in_comment = True

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


_KEYWORDS_BEFORE_BRACE = frozenset({
    "struct", "union", "enum", "typedef", "for", "while", "if",
    "switch", "do",
})


def _extract_functions_handrolled(text: str) -> list[dict]:
    lines = text.splitlines()
    funcs: list[dict] = []

    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        if (
            not stripped
            or stripped.startswith("#")
            or stripped.startswith("//")
            or stripped.startswith("/*")
            or stripped.endswith("*/")
        ):
            i += 1
            continue

        has_paren = "(" in stripped
        if not has_paren:
            next_has_paren = any(
                "(" in lines[k].strip()
                for k in range(i + 1, min(i + 3, len(lines)))
                if not lines[k].strip().startswith("#")
            )
            if not next_has_paren:
                i += 1
                continue

        sig_lines: list[str] = []
        found_open_paren = False
        found_close_paren = False
        j = i

        while j < len(lines):
            sig_lines.append(lines[j])
            s = lines[j].strip()
            if "(" in s and not s.startswith("#"):
                found_open_paren = True
            if found_open_paren and ")" in s:
                found_close_paren = True
            if found_close_paren and "{" in s:
                break
            if ";" in s and found_open_paren:
                break
            if "{" in s and not found_close_paren:
                break
            j += 1

        full_sig = "\n".join(sig_lines)

        if not (found_close_paren and "{" in full_sig):
            i += 1
            continue

        if ";" in full_sig:
            i = j + 1
            continue

        first_line = lines[i].strip()
        if any(first_line.startswith(p) for p in _KEYWORDS_BEFORE_BRACE):
            i = j + 1
            continue

        if "= {" in full_sig:
            i = j + 1
            continue

        sig_flat = full_sig.replace("\n", " ")
        if "VSF_MCONNECT" in sig_flat:
            m_name = re.search(r'VSF_MCONNECT\s*\([^)]*,\s*(_\w+)\)', sig_flat)
            func_name = m_name.group(1) if m_name else "unknown"
        else:
            m_name = re.search(r'(\w+)\s*\(', sig_flat)
            func_name = m_name.group(1) if m_name else "unknown"

        body_lines: list[str] = []
        brace_depth = 0
        k = j
        while k < len(lines):
            body_lines.append(lines[k])
            brace_depth += lines[k].count("{")
            brace_depth -= lines[k].count("}")
            k += 1
            if brace_depth == 0:
                break

        if brace_depth == 0:
            funcs.append({
                "name": func_name,
                "body": "\n".join(body_lines),
                "start_line": i + 1,
                "end_line": k,
                "lines": body_lines,
            })
            i = k
        else:
            i = j + 1

    return funcs
