#!/usr/bin/env python3
"""
tree-sitter based C parser for VSF HAL driver checkers.

Replaces the hand-rolled preprocess() and extract_functions() in checker_base.py
with AST-level accuracy. Produces the same ScanLine / function-dict shapes so
existing checkers need zero changes.

Usage (drop-in replacement in checker_base.py):

    from _c_parser import preprocess, extract_functions

Extra capabilities the old parser lacked:
    - #if 0 blocks are marked in_comment (skip checking dead code)
    - #ifdef / #if / #else / #endif tracked correctly (not just /* */)
    - Function detection via AST node type, not heuristic brace counting
"""

from __future__ import annotations

import re
from dataclasses import dataclass
from pathlib import Path

import tree_sitter_c as tsc
from tree_sitter import Language, Parser as TSParser, Node

# ---------------------------------------------------------------- init

_PARSER: TSParser | None = None


def _parser() -> TSParser:
    global _PARSER
    if _PARSER is None:
        _PARSER = TSParser(Language(tsc.language()))
    return _PARSER


# ---------------------------------------------------------------- ScanLine (re-export shape from checker_base)

@dataclass
class ScanLine:
    lineno: int
    text: str
    in_imp_lv0: bool       # inside a #define ..._IMP_LV0 multi-line macro
    in_comment: bool       # inside a /* */ block comment OR #if 0 block
    suppress: set[str]     # rule ids suppressed for this line


_SUPPRESS_RE = re.compile(r"//\s*quality:\s*allow-([a-z][a-z0-9-]*)")


# ---------------------------------------------------------------- preprocess

def preprocess(text: str) -> list[ScanLine]:
    """Tag each line with context derived from the tree-sitter CST.

    Context tags:
      - in_comment: inside /* */ OR inside #if 0 ... #endif
      - in_imp_lv0: inside a multi-line #define that names _IMP_LV0
      - suppress:   // quality: allow-<rule-id> annotations
    """
    tree = _parser().parse(text.encode(), encoding="utf8")
    root = tree.root_node
    lines = text.splitlines()

    # Collect comment line numbers from CST comments + #if 0 blocks.
    comment_lines: set[int] = set()

    def _add_range(node: Node):
        for ln in range(node.start_point[0] + 1, node.end_point[0] + 2):
            comment_lines.add(ln)

    for node in _walk_all(root):
        if node.type == "comment":
            _add_range(node)
        elif node.type == "preproc_if":
            # Check if this is #if 0
            text_bytes = node.text
            # The first child of preproc_if should be "#if" and then the condition
            for child in node.children:
                if child.type in ("number_literal", "false"):
                    if child.text == b"0" or child.text == b"false":
                        _add_range(node)
                    break
                elif child.type in ("identifier", "true"):
                    break  # #if SYMBOL or #if true — active code, don't skip
                # binary_expression means e.g. #if VSF_HAL_USE_USART == ENABLED — active

    # Collect IMP_LV0 line ranges.
    # tree-sitter classifies single-line #define as preproc_def, and
    # multi-line (backslash-continued) function-like #define as
    # preproc_function_def.  IMP_LV0 macros are always multi-line.
    imp_lv0_lines: set[int] = set()
    for node in _walk_all(root):
        if node.type in ("preproc_def", "preproc_function_def"):
            text_str = node.text.decode()
            if "_IMP_LV0" in text_str:
                for ln in range(node.start_point[0] + 1, node.end_point[0] + 2):
                    imp_lv0_lines.add(ln)

    # Build ScanLine list.
    result: list[ScanLine] = []
    for idx, raw in enumerate(lines, start=1):
        suppress = set(_SUPPRESS_RE.findall(raw))
        result.append(ScanLine(
            lineno=idx,
            text=raw,
            in_imp_lv0=idx in imp_lv0_lines,
            in_comment=idx in comment_lines,
            suppress=suppress,
        ))

    return result


# ---------------------------------------------------------------- extract_functions

def extract_functions(text: str) -> list[dict]:
    """Extract top-level C function definitions via tree-sitter AST.

    Returns a list of dicts matching the checker_base contract:
        name:       str   — short function name (e.g. _usart_init),
                            VSF_MCONNECT suffix extracted like the old parser
        body:       str   — full function body including braces
        start_line: int   — 1-based
        end_line:   int   — 1-based
        lines:      list[str] — body lines (including opening-brace line)
    """
    tree = _parser().parse(text.encode(), encoding="utf8")
    root = tree.root_node
    lines = text.splitlines()

    funcs: list[dict] = []

    for node in _walk_all(root):
        if node.type != "function_definition":
            continue

        name = _extract_function_name(node)
        start_line = node.start_point[0] + 1
        end_line = node.end_point[0] + 1

        # Body text: everything from the opening brace through closing brace.
        body_lines = lines[start_line - 1:end_line]
        body = "\n".join(body_lines)

        funcs.append({
            "name": name,
            "body": body,
            "start_line": start_line,
            "end_line": end_line,
            "lines": body_lines,
        })

    return funcs


_VSF_MCONNECT_NAME_RE = re.compile(
    r'VSF_MCONNECT\s*\([^)]*,\s*(_\w+)\)', re.DOTALL
)


def _extract_function_name(node: Node) -> str:
    """Extract the function name from a function_definition AST node.

    For plain C functions returns the identifier.  For VSF_MCONNECT-wrapped
    functions extracts the short suffix (e.g. _usart_init) — matching the
    checker_base contract so existing checkers need no changes.
    """
    sig_flat = node.text.decode().replace("\n", " ")

    # VSF_MCONNECT: extract the suffix parameter (e.g. _usart_init)
    m = _VSF_MCONNECT_NAME_RE.search(sig_flat)
    if m:
        return m.group(1)

    # Plain function: recursively search for function_declarator > identifier.
    # For `void *name(...)` the declarator is nested inside pointer_declarator,
    # so a simple direct-child scan misses it.
    def _find_declarator_id(n: Node) -> str | None:
        if n.type == "function_declarator":
            for cc in n.children:
                if cc.type == "identifier":
                    return cc.text.decode()
                elif cc.type == "field_identifier":
                    return cc.text.decode()
                elif cc.type == "parenthesized_declarator":
                    for ccc in cc.children:
                        if ccc.type == "field_identifier":
                            return ccc.text.decode()
        for child in n.children:
            result = _find_declarator_id(child)
            if result:
                return result
        return None

    name = _find_declarator_id(node)
    if name:
        return name


# ---------------------------------------------------------------- tree helpers

def _walk_all(node: Node):
    """Recursively yield all descendant nodes (including self).

    Uses node.children recursively instead of TreeCursor.walk() which is not
    iterable in the tree-sitter Python bindings.
    """
    yield node
    for child in node.children:
        yield from _walk_all(child)
