#!/usr/bin/env python3
"""Generate C test parameter header from YAML configuration.

Usage:
    python gen_test_params.py <input.yml> <output.h>

The input YAML defines test scenarios; the output is a C header with
static const arrays and macros consumed by the firmware.

YAML structure:
    marker:        # ignored by this script (host-side only)
    <scenario_key>:
        name: <c_identifier_stem>     # required: drives generated names
        common:                        # optional: scenario-level fixed params → #define <NAME>_COMMON_<KEY>
            <key>: <scalar|list>
        cases:                         # required: per-case varying params → struct array
            - {<field>: <value>, ..., host: {...}}

Each case dict becomes a C struct initializer; the `host` key is reserved
for host-only data and is excluded from C generation.

Generated identifiers, given `name: foo`:
    struct: vsf_test_usart_foo_case_t
    array:  __foo_cases
    count:  FOO_CASE_COUNT
    common: FOO_COMMON_<KEY>
"""

import argparse
import sys
from pathlib import Path

try:
    import yaml
except ImportError as e:
    print("Error: PyYAML is required. Install with: pip install pyyaml", file=sys.stderr)
    raise SystemExit(1) from e


def _format_value(value) -> str:
    """Format a YAML value as a C literal/expression."""
    if isinstance(value, bool):
        return "true" if value else "false"
    if isinstance(value, list):
        return "(" + " | ".join(str(v) for v in value) + ")"
    if isinstance(value, str):
        return value
    return str(value)


def _format_case(case: dict) -> str:
    """Format one case dict as a C designated initializer.

    Skips the reserved `host` key (host-side only data).
    """
    parts = []
    for key, value in case.items():
        if key == "host":
            continue
        parts.append(f".{key} = {_format_value(value)}")
    return "{ " + ", ".join(parts) + " }"


def _emit_scenario(lines: list[str], scenario_key: str, sc: dict) -> None:
    name = sc.get("name")
    if not name:
        return
    cases = sc.get("cases")
    if not cases:
        return

    upper = name.upper()
    struct_type = f"vsf_test_usart_{name}_case_t"
    array_name = f"__{name}_cases"
    count_macro = f"{upper}_CASE_COUNT"

    lines.append(f"/* === {scenario_key} ({name}) === */")
    lines.append("")

    # common params → #define <NAME>_COMMON_<KEY>
    common = sc.get("common") or {}
    if common:
        for key, value in common.items():
            macro = f"{upper}_COMMON_{key.upper()}"
            lines.append(f"#define {macro}  {_format_value(value)}")
        lines.append("")

    # cases → struct array
    lines.append(f"static const {struct_type} {array_name}[] = {{")
    for case in cases:
        lines.append(f"    {_format_case(case)},")
    lines.append("};")
    lines.append(f"#define {count_macro}  (sizeof({array_name}) / sizeof({array_name}[0]))")
    lines.append("")


def generate_header(yml_path: Path, out_path: Path) -> None:
    with open(yml_path) as f:
        params = yaml.safe_load(f) or {}

    lines = [
        "/* Auto-generated from test_params.yml — do not edit manually */",
        "#ifndef __TEST_PARAMS_GENERATED_H__",
        "#define __TEST_PARAMS_GENERATED_H__",
        "",
        '#include "test/vsf_test/usart/test_usart.h"',
        "",
    ]

    # Global firmware params from marker section (still needed by scenario code)
    marker = params.get("marker") or {}
    if "delay_ms" in marker:
        lines.append(f"#define MARKER_DELAY_MS  {marker['delay_ms']}")

    for scenario_key, sc in params.items():
        if scenario_key == "marker":
            continue
        if not isinstance(sc, dict):
            continue
        # Per-scenario payload + drain (currently shared values across scenarios,
        # but emit per-scenario to keep gen script generic).
        if "payload" in sc or "payload_drain_ms" in sc:
            payload = sc.get("payload", "Hello VSF\r\n")
            drain_ms = sc.get("payload_drain_ms", 500)
            c_payload = (
                payload.replace('\\', '\\\\')
                       .replace('"', '\\"')
                       .replace('\r', '\\r')
                       .replace('\n', '\\n')
            )
            name = sc.get("name", scenario_key).upper()
            lines.append(f'#define {name}_PAYLOAD          "{c_payload}"')
            lines.append(f"#define {name}_PAYLOAD_DRAIN_MS {drain_ms}")
    lines.append("")

    for scenario_key, sc in params.items():
        if scenario_key == "marker":
            continue
        if not isinstance(sc, dict):
            continue
        _emit_scenario(lines, scenario_key, sc)

    lines.extend([
        "#endif /* __TEST_PARAMS_GENERATED_H__ */",
        "",
    ])

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(lines))
    print(f"Generated: {out_path}")


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate C header from test params YAML")
    parser.add_argument("yml", type=Path, help="Input YAML file")
    parser.add_argument("out", type=Path, help="Output C header file")
    args = parser.parse_args()

    if not args.yml.exists():
        print(f"Error: {args.yml} not found", file=sys.stderr)
        return 1

    generate_header(args.yml, args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
