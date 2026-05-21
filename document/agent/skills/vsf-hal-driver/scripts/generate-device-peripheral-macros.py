#!/usr/bin/env python3
"""
Generate device.h peripheral macro block from a YAML instance map.

Usage:
    generate-device-peripheral-macros.py <input.yaml>
    generate-device-peripheral-macros.py --in-place <device.h> <input.yaml>

Exit codes:
    0 = success
    1 = file not found or unreadable
    2 = YAML parse error
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


def generate_macros(yaml_path: Path) -> str:
    with yaml_path.open(encoding="utf-8") as f:
        data = yaml.safe_load(f)

    if not isinstance(data, dict) or "peripherals" not in data:
        raise ValueError("YAML must contain a 'peripherals' key")

    peripherals = data["peripherals"]
    if not isinstance(peripherals, dict):
        raise ValueError("'peripherals' must be a mapping")

    lines: list[str] = []

    for name, cfg in peripherals.items():
        if not isinstance(cfg, dict):
            continue

        upper = name.upper()
        if name == "gpio":
            port_count = int(cfg.get("port_count", 0))
            pin_count = int(cfg.get("pin_count", 32))
            lines.extend([
                "// GPIO",
                f"#define VSF_HW_GPIO_PORT_COUNT                  {port_count}",
                f"#define VSF_HW_GPIO_PIN_COUNT                   {pin_count}",
                "",
            ])
        elif "instances" in cfg:
            instances = cfg["instances"]
            if not isinstance(instances, list):
                continue

            indices = []
            for inst in instances:
                if isinstance(inst, dict):
                    indices.append(int(inst.get("index", 0)))

            if not indices:
                continue

            contiguous = indices == list(range(indices[0], indices[-1] + 1))
            lines.append(f"// {upper}")
            lines.append(f"#define VSF_HW_{upper}_COUNT                    {len(indices)}")

            if not (contiguous and indices[0] == 0):
                mask = sum(1 << idx for idx in indices)
                lines.append(f"#define VSF_HW_{upper}_MASK                     0x{mask:02X}")

            for inst in instances:
                if not isinstance(inst, dict):
                    continue
                idx = int(inst["index"])
                lines.append(f"#define VSF_HW_{upper}{idx}_IRQN                    {inst['irqn']}")
                lines.append(f"#define VSF_HW_{upper}{idx}_IRQHandler              {inst['irq_handler']}")
                lines.append(f"#define VSF_HW_{upper}{idx}_REG                     {inst['reg']}")
                if "rst_bit" in inst and inst["rst_bit"]:
                    lines.append(f"#define VSF_HW_{upper}{idx}_RST_BIT                 {inst['rst_bit']}")
                if "clk_bit" in inst and inst["clk_bit"]:
                    lines.append(f"#define VSF_HW_{upper}{idx}_CLK_BIT                 {inst['clk_bit']}")
            lines.append("")
        elif "count" in cfg:
            count = int(cfg["count"])
            lines.extend([f"// {upper}", f"#define VSF_HW_{upper}_COUNT                     {count}", ""])

    return "\n".join(lines)


def replace_zone(content: str, zone_name: str, replacement: str) -> str:
    begin = f"// {zone_name}\n"
    end = f"// {zone_name} end\n"
    start_pos = content.find(begin)
    end_pos = content.find(end, start_pos)
    if start_pos == -1 or end_pos == -1:
        return content
    return content[:start_pos] + replacement.rstrip("\n") + "\n" + content[end_pos + len(end):]


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate device.h peripheral macro block from YAML.")
    parser.add_argument("--in-place", metavar="DEVICE_H", help="Replace // peripheral defines zone in the given device.h")
    parser.add_argument("yaml", help="Path to YAML peripheral instance map")
    args = parser.parse_args()

    yaml_path = Path(args.yaml)
    if not yaml_path.is_file():
        print(f"Error: file not found: {yaml_path}", file=sys.stderr)
        return 1

    try:
        output = generate_macros(yaml_path)
    except (yaml.YAMLError, ValueError) as err:
        print(f"Error: {err}", file=sys.stderr)
        return 2

    if args.in_place:
        device_path = Path(args.in_place)
        if not device_path.is_file():
            print(f"Error: file not found: {device_path}", file=sys.stderr)
            return 1
        content = device_path.read_text(encoding="utf-8")
        new_content = replace_zone(content, "peripheral defines", output)
        if new_content == content:
            print("Warning: // peripheral defines zone not found; file unchanged", file=sys.stderr)
        device_path.write_text(new_content, encoding="utf-8")
        print(f"Updated {device_path}")
    else:
        print(output)

    return 0


if __name__ == "__main__":
    sys.exit(main())
