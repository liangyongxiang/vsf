#!/usr/bin/env python3
"""
Migrate static variables from scenario .c files into a central union.

Run: python3 migrate_vars.py --apply
"""

import re
import sys
from pathlib import Path
from dataclasses import dataclass, field

ROOT = Path("/home/yongxiang/work/vsf-work/vsf.demo.pico/vsf.demo/vsf/test/vsf_test")


@dataclass
class StaticVar:
    decl_line: str
    name: str
    raw_name: str


@dataclass
class StaticFunc:
    decl_lines: list[str]
    body_lines: list[str]
    name: str


@dataclass
class Scenario:
    periph: str
    name: str
    c_path: Path
    h_path: Path
    static_vars: list[StaticVar] = field(default_factory=list)
    static_funcs: list[StaticFunc] = field(default_factory=list)
    has_if_guard: bool = False
    enable_macro: str = ""


def discover_scenarios() -> list[Scenario]:
    scenarios = []
    for periph_dir in sorted(ROOT.iterdir()):
        if not periph_dir.is_dir():
            continue
        periph = periph_dir.name
        suite_dir = periph_dir / "suite"
        if not suite_dir.exists():
            continue
        for c_file in sorted(suite_dir.glob("*.c")):
            stem = c_file.stem
            prefix = f"vsf_test_{periph}_"
            if not stem.startswith(prefix):
                continue
            scenario_short = stem[len(prefix):]
            scenario_name = f"{periph}_{scenario_short}"
            h_file = suite_dir / f"{stem}.h"
            if not h_file.exists():
                continue
            enable_macro = f"VSF_TEST_{scenario_name.upper()}_ENABLE"
            scenarios.append(Scenario(
                periph=periph,
                name=scenario_name,
                c_path=c_file,
                h_path=h_file,
                enable_macro=enable_macro,
            ))
    return scenarios


def parse_c_file(content: str) -> tuple[list[StaticVar], list[StaticFunc], bool]:
    lines = content.splitlines()
    vars = []
    funcs = []
    has_if_guard = False

    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        m = re.match(r'#if\s+(VSF_TEST_\w+_ENABLE)\s+==\s+ENABLED', stripped)
        if m:
            has_if_guard = True

        if not stripped.startswith('static '):
            i += 1
            continue

        if stripped.endswith(';'):
            var_matches = list(re.finditer(r'\b(__\w+)\s*(?:\[|;|=)', stripped))
            for vm in var_matches:
                raw_name = vm.group(1)
                name = raw_name.lstrip('_')
                vars.append(StaticVar(
                    decl_line=line,
                    name=name,
                    raw_name=raw_name,
                ))
            i += 1
            continue

        decl_lines = [line]
        j = i + 1
        while j < len(lines):
            decl_lines.append(lines[j])
            if ';' in lines[j] or '{' in lines[j]:
                break
            j += 1

        decl_text = '\n'.join(decl_lines)

        if '{' in decl_text:
            before_brace = decl_text.split('{')[0]
            if '(' in before_brace:
                func_name_match = re.search(r'static\s+\w+\s*\*?\s*(\w+)\s*\(', decl_text)
                func_name = func_name_match.group(1) if func_name_match else "unknown"

                body_lines = decl_lines[:]
                brace_depth = 0
                for l in decl_lines:
                    brace_depth += l.count('{')
                    brace_depth -= l.count('}')

                k = j + 1
                while k < len(lines) and brace_depth > 0:
                    body_lines.append(lines[k])
                    brace_depth += lines[k].count('{')
                    brace_depth -= lines[k].count('}')
                    k += 1

                funcs.append(StaticFunc(
                    decl_lines=decl_lines,
                    body_lines=body_lines,
                    name=func_name,
                ))
                i = k
                continue

        var_matches = list(re.finditer(r'\b(__\w+)\s*(?:\[|;|=)', decl_text))
        for vm in var_matches:
            raw_name = vm.group(1)
            name = raw_name.lstrip('_')
            pos = vm.start()
            line_idx = decl_text[:pos].count('\n')
            var_decl = decl_lines[line_idx]
            vars.append(StaticVar(
                decl_line=var_decl,
                name=name,
                raw_name=raw_name,
            ))

        i = j + 1

    return vars, funcs, has_if_guard


def modify_h_file(scenario: Scenario) -> None:
    if not scenario.static_vars:
        return
    content = scenario.h_path.read_text()
    lines = content.splitlines()

    struct_lines = [f"#if {scenario.enable_macro} == ENABLED"]
    struct_lines.append("typedef struct {")
    for v in scenario.static_vars:
        field_decl = v.decl_line.strip()
        field_decl = re.sub(r'^static\s+', '', field_decl)
        field_decl = field_decl.replace(v.raw_name, v.name)
        struct_lines.append(f"    {field_decl}")
    struct_lines.append(f"}} vsf_test_{scenario.name}_var_t;")
    struct_lines.append(f"#endif")
    struct_def = '\n'.join(struct_lines)

    for i in range(len(lines) - 1, -1, -1):
        if lines[i].strip().startswith('#endif'):
            lines.insert(i, '')
            lines.insert(i, struct_def)
            break

    scenario.h_path.write_text('\n'.join(lines) + '\n')
    print(f"  modified {scenario.h_path.relative_to(ROOT)}")


def modify_c_file(scenario: Scenario) -> None:
    if not scenario.static_vars:
        return
    content = scenario.c_path.read_text()
    lines = content.splitlines()

    # 1. Add #include "vsf_test_suites.h"
    if 'vsf_test_suites.h' not in content:
        last_include_idx = -1
        for i, line in enumerate(lines):
            if line.strip().startswith('#include'):
                last_include_idx = i
        if last_include_idx >= 0:
            lines.insert(last_include_idx + 1, '#include "vsf_test_suites.h"')

    # 2. Rebuild content to apply include change
    content = '\n'.join(lines)
    lines = content.splitlines()

    # 3. Find static var lines to delete
    var_indices = []
    for i, line in enumerate(lines):
        stripped = line.strip()
        if not stripped.startswith('static '):
            continue
        for v in scenario.static_vars:
            if v.raw_name in stripped and stripped.endswith(';'):
                var_indices.append(i)
                break

    # Delete var lines (reverse order)
    for idx in sorted(set(var_indices), reverse=True):
        del lines[idx]
        # Remove trailing empty line if present
        if idx < len(lines) and lines[idx].strip() == '':
            del lines[idx]

    # 4. Find #if line and move it up after LOCAL VARIABLES
    if_line_idx = None
    for i, line in enumerate(lines):
        if re.match(rf'#if\s+{re.escape(scenario.enable_macro)}\s+==\s+ENABLED', line.strip()):
            if_line_idx = i
            break

    if if_line_idx is not None:
        if_line = lines[if_line_idx]
        del lines[if_line_idx]
        # Remove surrounding empty lines
        while if_line_idx < len(lines) and lines[if_line_idx].strip() == '':
            del lines[if_line_idx]

        # Find LOCAL VARIABLES section
        local_vars_idx = None
        for i, line in enumerate(lines):
            if 'LOCAL VARIABLES' in line:
                local_vars_idx = i
                break

        if local_vars_idx is not None:
            insert_pos = local_vars_idx + 1
            while insert_pos < len(lines) and lines[insert_pos].strip() == '':
                insert_pos += 1
            lines.insert(insert_pos, '')
            lines.insert(insert_pos, if_line)
            lines.insert(insert_pos, '')

    # 5. Replace variable references
    content = '\n'.join(lines)
    for v in scenario.static_vars:
        old = v.raw_name
        new = f"vsf_test_suites.{scenario.name}.{v.name}"
        content = re.sub(rf'\b{re.escape(old)}\b', new, content)

    scenario.c_path.write_text(content)
    print(f"  modified {scenario.c_path.relative_to(ROOT)}")


def modify_vsf_test_suites_h(scenarios: list[Scenario]) -> None:
    path = ROOT / 'vsf_test_suites.h'
    content = path.read_text()
    lines = content.splitlines()

    union_lines = ['']
    union_lines.append('/*============================ SCENARIO STATE UNION ==========================*/')
    union_lines.append('')
    union_lines.append('typedef union {')
    for sc in scenarios:
        if sc.static_vars:
            union_lines.append(f'#if {sc.enable_macro} == ENABLED')
            union_lines.append(f'    vsf_test_{sc.name}_var_t {sc.name};')
            union_lines.append(f'#endif')
    union_lines.append('} vsf_test_suites_t;')
    union_lines.append('')
    union_lines.append('extern vsf_test_suites_t vsf_test_suites;')
    union_def = '\n'.join(union_lines)

    for i in range(len(lines) - 1, -1, -1):
        if lines[i].strip() == '#endif // __VSF_TEST_SUITES_H__':
            lines.insert(i, union_def)
            break

    path.write_text('\n'.join(lines) + '\n')
    print(f"  modified {path.relative_to(ROOT)}")


def modify_vsf_test_suites_c(scenarios: list[Scenario]) -> None:
    path = ROOT / 'vsf_test_suites.c'
    content = path.read_text()
    lines = content.splitlines()

    instance_lines = ['']
    instance_lines.append('/*============================ SCENARIO STATE INSTANCE ========================*/')
    instance_lines.append('')
    instance_lines.append('vsf_test_suites_t vsf_test_suites;')
    instance_def = '\n'.join(instance_lines)

    last_include_idx = -1
    for i, line in enumerate(lines):
        if line.strip().startswith('#include'):
            last_include_idx = i

    if last_include_idx >= 0:
        lines.insert(last_include_idx + 1, instance_def)

    path.write_text('\n'.join(lines) + '\n')
    print(f"  modified {path.relative_to(ROOT)}")


def main() -> int:
    apply = '--apply' in sys.argv

    scenarios = discover_scenarios()
    for sc in scenarios:
        content = sc.c_path.read_text()
        sc.static_vars, sc.static_funcs, sc.has_if_guard = parse_c_file(content)

    total_vars = sum(len(s.static_vars) for s in scenarios)
    with_vars = sum(1 for s in scenarios if s.static_vars)
    print(f"Scenarios: {len(scenarios)}")
    print(f"With static vars: {with_vars}")
    print(f"Total static vars: {total_vars}")

    if not apply:
        print("\nDry run. Pass --apply to modify files.")
        return 0

    print("\nApplying changes...")
    for sc in scenarios:
        modify_h_file(sc)
        modify_c_file(sc)
    modify_vsf_test_suites_h(scenarios)
    modify_vsf_test_suites_c(scenarios)
    print("\nDone.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
