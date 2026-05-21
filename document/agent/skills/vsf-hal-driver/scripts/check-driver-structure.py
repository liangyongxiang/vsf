#!/usr/bin/env python3
"""
Data-driven structural check for VSF HAL driver files.
Reads peripheral-specific rules from scripts/check-specs/<periph>.yml.

Usage:
    check-driver-structure.py --periph usart --side header uart.h
    check-driver-structure.py --periph gpio --side source gpio.c

Exit: 0=pass, 1=errors, 2=warnings
"""

import argparse
import re
import sys
from pathlib import Path

try:
    import yaml  # type: ignore[reportMissingModuleSource]
except ImportError:
    print("Error: pyyaml required. Install with: pip install pyyaml", file=sys.stderr)
    sys.exit(1)


def load_spec(periph: str, script_dir: Path) -> dict:
    spec_dir = script_dir / "check-specs"
    spec_file = spec_dir / f"{periph}.yml"
    if not spec_file.is_file():
        # Fallback: scan all YAML files for matching periph or api_prefix
        for f in spec_dir.glob("*.yml"):
            data = yaml.safe_load(f.read_text(encoding="utf-8"))
            if data.get("periph") == periph or data.get("api_prefix") == periph:
                return data
        raise FileNotFoundError(f"No spec found for peripheral '{periph}'")
    return yaml.safe_load(spec_file.read_text(encoding="utf-8"))


def check_header(path: str, spec: dict) -> tuple[int, int]:
    errors = 0
    warnings = 0
    text = Path(path).read_text(encoding="utf-8")

    def has(pattern: str) -> bool:
        return bool(re.search(pattern, text, re.MULTILINE))

    def say(kind: str, msg: str):
        nonlocal errors, warnings
        if kind == "OK":
            print(f"  OK: {msg}")
        elif kind == "FAIL":
            print(f"  FAIL: {msg}")
            errors += 1
        else:
            print(f"  WARN: {msg}")
            warnings += 1

    is_ipcore = bool(re.search(r'#include\s+.*IPCore/', text))
    is_ipcore_impl = '/IPCore/' in path.replace('\\', '/')

    if is_ipcore:
        print("  INFO: chip header using IPCore — type/enum checks delegated")
    if is_ipcore_impl:
        print("  INFO: IPCore implementation — checks not applicable")

    # ── Guard ──
    guard = spec.get("guard", "")
    if guard:
        if has(rf"{re.escape(guard)}\s*==\s*ENABLED"):
            say("OK", f"{guard} guard")
        else:
            say("FAIL", f"missing {guard} == ENABLED guard")

    # ── Template include ──
    tpl = spec.get("template_include", "")
    if tpl:
        if has(rf'#include.*{re.escape(tpl)}'):
            say("OK", f"{tpl} included")
        elif is_ipcore:
            say("OK", "types provided by IPCore header include")
        else:
            say("WARN", f"missing #include {tpl} (OK if types from driver.h)")

    # ── irq_mask_t ──
    irq_mask = spec.get("irq_mask_t", "")
    if irq_mask:
        if has(re.escape(irq_mask)):
            say("OK", f"{irq_mask} defined")
        elif is_ipcore:
            pass
        else:
            say("WARN", f"{irq_mask} not found (OK if from driver.h)")

    # ── IRQ bits ──
    if not is_ipcore:
        for bit in spec.get("required_irq_bits", []):
            macro = spec.get("irq_prefix", f"VSF_{spec.get('api_prefix', spec['periph']).upper()}_IRQ_MASK_{bit}")
            if has(re.escape(macro)):
                say("OK", f"{macro}")
            else:
                say("FAIL", f"{macro} not found")
        for bit in spec.get("warn_irq_bits", []):
            macro = spec.get("irq_prefix", f"VSF_{spec.get('api_prefix', spec['periph']).upper()}_IRQ_MASK_{bit}")
            if has(re.escape(macro)):
                say("OK", f"{macro}")
            else:
                say("WARN", f"{macro} not found")

    # ── Mode bits ──
    if not is_ipcore:
        for bit in spec.get("required_modes", []):
            if has(re.escape(bit)):
                say("OK", f"{bit}")
            else:
                say("FAIL", f"mandatory mode bit {bit} not found")
        for bit in spec.get("warn_modes", []):
            if has(re.escape(bit)):
                say("OK", f"{bit}")
            else:
                say("WARN", f"{bit} not found")

    # ── isr_t ──
    isr = spec.get("isr_t", "")
    if isr:
        if has(re.escape(isr)):
            say("OK", f"{isr} defined")
        elif not is_ipcore:
            say("WARN", f"{isr} not found")

    return errors, warnings


def check_source(path: str, spec: dict) -> tuple[int, int]:
    errors = 0
    warnings = 0
    text = Path(path).read_text(encoding="utf-8")
    src = spec.get("source", {})

    def has(pattern: str) -> bool:
        return bool(re.search(pattern, text, re.MULTILINE))

    def say(kind: str, msg: str):
        nonlocal errors, warnings
        if kind == "OK":
            print(f"  OK: {msg}")
        elif kind == "FAIL":
            print(f"  FAIL: {msg}")
            errors += 1
        else:
            print(f"  WARN: {msg}")
            warnings += 1

    # ── Guard ──
    guard = spec.get("guard", "")
    if guard:
        if has(rf"{re.escape(guard)}\s*==\s*ENABLED"):
            say("OK", f"{guard} guard")
        else:
            say("FAIL", f"missing {guard} == ENABLED guard")

    # ── HW struct ──
    struct_pat = src.get("struct_pattern", "")
    if struct_pat:
        if has(struct_pat):
            say("OK", f"HW {spec['periph']} struct defined")
        elif has(rf"implement\(vsf_\w+_{re.escape(spec['periph'])}_t\)"):
            say("OK", "IPCore-based struct (implement pattern)")
        else:
            say("FAIL", f"missing {spec['periph']} struct or IPCore implement pattern")

    # ── Required APIs ──
    for api in src.get("required_apis", []):
        if has(re.escape(api)):
            say("OK", f"implements _{api}")
        else:
            say("FAIL", f"missing _{api}")

    # ── Lifecycle APIs (template may provide defaults) ──
    for api in src.get("lifecycle_apis", []):
        if has(re.escape(api)):
            say("OK", f"implements _{api}")
        else:
            say("WARN", f"missing _{api} (OK if template default used)")

    # ── Optional API groups ──
    for group in src.get("optional_api_groups", []):
        group_name = group["name"]
        apis = group["apis"]
        found = [a for a in apis if has(re.escape(a))]
        if found:
            say("OK", f"{group_name} API present ({', '.join(found)})")
        else:
            level = group.get("level", "WARN")
            say(level, f"no {group_name} API found")

    # ── Instance instantiation ──
    imp_lv0 = src.get("imp_lv0", "")
    if imp_lv0:
        if has(re.escape(imp_lv0)):
            say("OK", f"{imp_lv0} defined")
        else:
            say("FAIL", f"missing {imp_lv0} instance instantiation")

    # ── Template include ──
    tpl_inc = src.get("template_inc", "")
    if tpl_inc:
        if has(re.escape(tpl_inc)):
            say("OK", f"{tpl_inc} included")
        else:
            say("FAIL", f'missing #include "...{tpl_inc}"')

    # ── Prefix config ──
    for pref in src.get("prefix_macros", []):
        if has(re.escape(pref)):
            say("OK", f"{pref} defined")
        else:
            say("FAIL", f"missing {pref}")

    # ── IRQHandler ──
    count = len(re.findall(r"_IRQHandler", text))
    if count:
        say("OK", f"{count} IRQHandler(s) defined")
    else:
        say("WARN", "no IRQHandler found (OK if IPCore dispatch used)")

    # ── Unimplemented API convention ──
    api_prefix = spec.get("api_prefix", spec["periph"])
    stub_re = re.compile(
        rf"(?:vsf_err_t|fsm_rt_t|void|uint\w+_t|int\w+_t)\s+\w*{re.escape(api_prefix)}_\w+\s*\([^)]*\)\s*\{{[^}}]*?return\s+VSF_ERR_NONE;[^}}]*?\}}",
        re.DOTALL,
    )
    for stub in stub_re.findall(text):
        if "VSF_HAL_ASSERT(0)" not in stub:
            func_name = re.search(rf"\w*{re.escape(api_prefix)}_\w+", stub)
            if func_name:
                say("WARN", f"stub {func_name.group()} returns VSF_ERR_NONE without VSF_HAL_ASSERT(0)")

    # ── Peripheral-specific checks ──
    if spec.get("periph") == "gpio":
        is_gpio_driver = "/gpio/" in str(path).replace("\\", "/") or Path(path).name.startswith("gpio.")
        if not is_gpio_driver:
            for pat in [r"gpio_set_function\b", r"io_bank0_hw\s*->", r"pads_bank0_hw\s*->"]:
                if has(pat):
                    say("WARN", f"pinmux register access in non-GPIO file: {pat}")

    return errors, warnings


def main() -> int:
    parser = argparse.ArgumentParser(description="Data-driven VSF HAL driver structural check.")
    parser.add_argument("--periph", required=True, help="Peripheral name (e.g. usart, gpio, spi)")
    parser.add_argument("--side", required=True, choices=["header", "source"], help="Which side to check")
    parser.add_argument("file", help="Path to the driver file")
    args = parser.parse_args()

    script_dir = Path(__file__).parent.resolve()

    try:
        spec = load_spec(args.periph, script_dir)
    except FileNotFoundError as err:
        print(f"FAIL: {err}", file=sys.stderr)
        sys.exit(1)
    except yaml.YAMLError as err:
        print(f"FAIL: YAML parse error: {err}", file=sys.stderr)
        sys.exit(1)

    path = args.file
    if not Path(path).is_file():
        print(f"FAIL: file not found: {path}")
        sys.exit(1)

    print(f"=== Checking {path} ({args.periph} {args.side}) ===")

    if args.side == "header":
        errors, warnings = check_header(path, spec)
    else:
        errors, warnings = check_source(path, spec)

    print()
    if errors:
        print(f"FAIL: {errors} essential check(s) failed")
        sys.exit(1)
    elif warnings:
        print(f"PASS: all essential checks passed ({warnings} warnings)")
        sys.exit(2)
    else:
        print("PASS: all checks passed")
        sys.exit(0)


if __name__ == "__main__":
    sys.exit(main())
