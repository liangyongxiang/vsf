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


# ---------------------------------------------------------------- function extraction


_KEYWORDS_BEFORE_BRACE = frozenset({
    "struct", "union", "enum", "typedef", "for", "while", "if",
    "switch", "do",
})


def extract_functions(text: str) -> list[dict]:
    """Extract top-level C function definitions from source text.

    Scans line-by-line looking for function signatures that end with ')'
    and are followed by '{'.  Uses brace counting to find the matching '}'.
    Filters out struct/enum/union definitions, initialisers, and control-flow
    blocks.

    Returns a list of dicts with keys:
        - name: function name
        - body: full body text (including braces)
        - start_line: 1-based line number of the function signature start
        - end_line:   1-based line number of the closing brace
        - lines:      list of body lines (including the opening-brace line)
    """
    lines = text.splitlines()
    funcs: list[dict] = []

    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        # Skip preprocessor, comments, empty lines
        if (
            not stripped
            or stripped.startswith("#")
            or stripped.startswith("//")
            or stripped.startswith("/*")
            or stripped.endswith("*/")
        ):
            i += 1
            continue

        # Heuristic: a function signature must contain '(' within the first
        # few lines.  If the current line has no '(' and neither do the next
        # two lines, this is not a function start.
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

        # Accumulate signature lines until we find )
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
                # Declaration, not definition
                break
            if "{" in s and not found_close_paren:
                # Not a function (struct, if, etc.)
                break
            j += 1

        full_sig = "\n".join(sig_lines)

        # Must have both ) and { in the accumulated signature
        if not (found_close_paren and "{" in full_sig):
            i += 1
            continue

        # Skip declarations
        if ";" in full_sig:
            i = j + 1
            continue

        # Skip struct/enum/union and control-flow blocks by first word
        first_line = lines[i].strip()
        skip_prefixes = _KEYWORDS_BEFORE_BRACE
        if any(first_line.startswith(p) for p in skip_prefixes):
            i = j + 1
            continue

        # Skip initialisers (e.g. struct foo bar = { ... }; )
        if "= {" in full_sig:
            i = j + 1
            continue

        # Extract function name
        sig_flat = full_sig.replace("\n", " ")
        if "VSF_MCONNECT" in sig_flat:
            m_name = re.search(r'VSF_MCONNECT\s*\([^)]*,\s*(_\w+)\)', sig_flat)
            func_name = m_name.group(1) if m_name else "unknown"
        else:
            m_name = re.search(r'(\w+)\s*\(', sig_flat)
            func_name = m_name.group(1) if m_name else "unknown"

        # Extract body with brace counting
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
            funcs.append(
                {
                    "name": func_name,
                    "body": "\n".join(body_lines),
                    "start_line": i + 1,
                    "end_line": k,
                    "lines": body_lines,
                }
            )
            i = k
        else:
            i = j + 1

    return funcs
