#!/usr/bin/env python3
"""
Functional equivalence test: run check-driver-quality.py with both parsers
and compare findings across all RP2040 driver files.
"""
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent.resolve()
sys.path.insert(0, str(SCRIPT_DIR))

# Patch: override checker_base's preprocess/extract_functions with tree-sitter versions
import checker_base
from _c_parser import preprocess as ts_preprocess, extract_functions as ts_extract
import importlib.util

# Import check-driver-quality.py (filename has hyphens)
def _import_quality():
    spec = importlib.util.spec_from_file_location(
        "check_driver_quality",
        SCRIPT_DIR / "check-driver-quality.py",
    )
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod

quality_mod = _import_quality()
quality_check_file = quality_mod.check_file

def main():
    driver_root = SCRIPT_DIR.parents[5] / "vsf" / "source" / "hal" / "driver" / "RaspberryPi" / "RP2040"
    if not driver_root.exists():
        print(f"ERROR: {driver_root} not found")
        return 1

    files = sorted(p for p in driver_root.rglob("*.c") if "template" not in str(p))

    # 1. Run with old parser
    print("=== Running with OLD parser ===")
    old_findings = {}
    for fpath in files:
        errs, warns = quality_check_file(fpath)
        old_findings[fpath.name] = (errs, warns)
        print(f"  {fpath.name}: {len(errs)} errors, {len(warns)} warnings")

    # 2. Swap in tree-sitter parser
    checker_base.preprocess = ts_preprocess
    checker_base.extract_functions = ts_extract

    print("\n=== Running with TREE-SITTER parser ===")
    new_findings = {}
    for fpath in files:
        errs, warns = quality_check_file(fpath)
        new_findings[fpath.name] = (errs, warns)
        print(f"  {fpath.name}: {len(errs)} errors, {len(warns)} warnings")

    # 3. Compare
    print("\n=== Comparison ===")
    all_ok = True
    for name in sorted(old_findings):
        old_e, old_w = old_findings[name]
        new_e, new_w = new_findings[name]

        if old_e != new_e:
            # tree-sitter may find MORE issues (better detection), that's OK
            if new_e > old_e:
                print(f"  {name}: +{new_e - old_e} NEW errors (expected: better comment/IMP_LV0 coverage)")
            else:
                print(f"  {name}: {old_e} -> {new_e} errors (FEWER — investigate)")
                all_ok = False
        if old_w != new_w:
            if new_w > old_w:
                print(f"  {name}: +{new_w - old_w} NEW warnings (expected)")
            else:
                print(f"  {name}: {old_w} -> {new_w} warnings (FEWER — investigate)")
                all_ok = False

    if all_ok:
        print("\nFUNCTIONAL EQUIVALENCE: tree-sitter matches or exceeds old parser on all files.")
        return 0
    else:
        print("\nREGRESSION: some files have fewer findings with tree-sitter.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
