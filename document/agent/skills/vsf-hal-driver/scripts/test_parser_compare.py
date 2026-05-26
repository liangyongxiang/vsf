#!/usr/bin/env python3
"""
Compare old hand-rolled parser (checker_base) vs new tree-sitter parser (_c_parser)
across all real driver files.

Usage:
    python3 test_parser_compare.py
"""

import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent.resolve()
sys.path.insert(0, str(SCRIPT_DIR))

from checker_base import preprocess as old_preprocess, extract_functions as old_extract
from _c_parser   import preprocess as new_preprocess, extract_functions as new_extract


def find_driver_files() -> list[Path]:
    driver_root = SCRIPT_DIR.parents[5] / "vsf" / "source" / "hal" / "driver" / "RaspberryPi" / "RP2040"
    if not driver_root.exists():
        raise FileNotFoundError(f"driver root not found: {driver_root}\n  SCRIPT_DIR={SCRIPT_DIR}")
    files: list[Path] = []
    for p in sorted(driver_root.rglob("*.c")):
        if "template" not in str(p):
            files.append(p)
    return files


def compare_scanlines(path: Path) -> tuple[int, int, list[str]]:
    text = path.read_text(encoding="utf-8")
    old = old_preprocess(text)
    new = new_preprocess(text)

    diffs: list[str] = []
    if len(old) != len(new):
        diffs.append(f"  line count: old={len(old)} new={len(new)}")

    for i in range(min(len(old), len(new))):
        o, n = old[i], new[i]
        issues = []
        if o.in_comment != n.in_comment:
            issues.append(f"in_comment old={o.in_comment} new={n.in_comment}")
        if o.in_imp_lv0 != n.in_imp_lv0:
            issues.append(f"in_imp_lv0 old={o.in_imp_lv0} new={n.in_imp_lv0}")
        if o.suppress != n.suppress:
            issues.append(f"suppress old={o.suppress} new={n.suppress}")
        if issues:
            diffs.append(f"  L{o.lineno}: {', '.join(issues)}")
            if len(diffs) > 30:
                diffs.append("  ... (truncated)")
                break

    return len(old), len(new), diffs


def compare_functions(path: Path) -> tuple[int, int, list[str]]:
    text = path.read_text(encoding="utf-8")
    old = old_extract(text)
    new = new_extract(text)

    diffs: list[str] = []
    old_names = {f["name"]: f for f in old}
    new_names = {f["name"]: f for f in new}

    only_old = set(old_names) - set(new_names)
    only_new = set(new_names) - set(old_names)
    common = set(old_names) & set(new_names)

    for name in sorted(only_old):
        f = old_names[name]
        diffs.append(f"  ONLY OLD: {name} L{f['start_line']}-L{f['end_line']}")
    for name in sorted(only_new):
        f = new_names[name]
        diffs.append(f"  ONLY NEW: {name} L{f['start_line']}-L{f['end_line']}")

    for name in sorted(common):
        o, n = old_names[name], new_names[name]
        if o["start_line"] != n["start_line"]:
            diffs.append(f"  {name}: start_line old={o['start_line']} new={n['start_line']}")
        if o["end_line"] != n["end_line"]:
            diffs.append(f"  {name}: end_line old={o['end_line']} new={n['end_line']}")

    return len(old), len(new), diffs


def main():
    files = find_driver_files()
    if not files:
        print("ERROR: no driver files found")
        return 1

    print(f"Comparing parsers across {len(files)} driver files...\n")

    total_old_funcs = 0
    total_new_funcs = 0
    issues = 0

    for f in files:
        rel = str(f.relative_to(f.parents[3])) if len(f.parents) >= 3 else str(f)
        sl_old, sl_new, sl_diffs = compare_scanlines(f)
        fn_old, fn_new, fn_diffs = compare_functions(f)

        total_old_funcs += fn_old
        total_new_funcs += fn_new

        if sl_diffs or fn_diffs:
            issues += 1
            print(f"--- {rel} ---")
            print(f"  ScanLines: old={sl_old} new={sl_new}")
            if sl_diffs:
                for d in sl_diffs:
                    print(d)
            print(f"  Functions: old={fn_old} new={fn_new}")
            if fn_diffs:
                for d in fn_diffs:
                    print(d)
            print()

    print(f"{'='*60}")
    print(f"Files: {len(files)} checked, {issues} with differences")
    print(f"Functions: {total_old_funcs} (old) vs {total_new_funcs} (new)")

    if issues == 0:
        print("RESULT: 100% match — tree-sitter parser is a drop-in replacement.")
        return 0
    else:
        print(f"RESULT: {issues} files have differences — review before replacing.")
        return 1


if __name__ == "__main__":
    sys.exit(main())
