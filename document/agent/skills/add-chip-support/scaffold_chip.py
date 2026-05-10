#!/usr/bin/env python3
"""
Generate VSF HAL driver skeleton for a new chip.

Reads a YAML config describing the chip's hardware and scaffolds the driver
directory from vsf/source/hal/driver/template/.
"""

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, TypeAlias, cast

try:
    import yaml  # type: ignore[reportMissingModuleSource]
except ImportError:
    print("Error: pyyaml required. Install with: pip install pyyaml", file=sys.stderr)
    sys.exit(1)


RawMap: TypeAlias = dict[str, Any]

REQUIRED_TOP = ("vendor", "series", "device", "cpu", "arch_pri_num", "arch_pri_bit")


def api_prefix(name: str) -> str:
    return "usart" if name in ("uart", "usart") else name


def api_upper(name: str) -> str:
    return api_prefix(name).upper()


def text(value: Any, default: str = "") -> str:
    return default if value is None else str(value)


@dataclass(frozen=True)
class Instance:
    index: int
    irqn: str
    irq_handler: str
    reg: str

    @classmethod
    def from_raw(cls, raw: Any) -> "Instance":
        return cls(
            index=int(raw["index"]),
            irqn=text(raw["irqn"]),
            irq_handler=text(raw["irq_handler"]),
            reg=text(raw["reg"]),
        )


@dataclass(frozen=True)
class Peripheral:
    name: str
    raw: RawMap | int | None

    @property
    def data(self) -> RawMap:
        return self.raw if isinstance(self.raw, dict) else {}

    @property
    def is_usb(self) -> bool:
        return self.name.startswith("usb")

    @property
    def api_prefix(self) -> str:
        return api_prefix(self.name)

    @property
    def api_upper(self) -> str:
        return api_upper(self.name)

    @property
    def ipcore(self) -> str:
        return text(self.data.get("ipcore"))

    @property
    def usb_mode(self) -> str:
        return text(self.data.get("mode"), "nonip")

    @property
    def count(self) -> int | None:
        if isinstance(self.raw, int):
            return self.raw
        if "count" in self.data and "instances" not in self.data:
            return int(self.data["count"])
        return None

    @property
    def instances(self) -> list[Instance]:
        return [Instance.from_raw(item) for item in self.data.get("instances", []) if isinstance(item, dict)]

    def validate(self) -> list[str]:
        errors: list[str] = []
        if self.is_usb:
            if self.usb_mode not in ("nonip", "ip"):
                errors.append("USB peripheral requires mode: nonip or ip")
            return errors

        if "instances" in self.data:
            for inst in self.data.get("instances", []):
                if not isinstance(inst, dict):
                    errors.append(f"{self.name} instance must be a mapping")
                    continue
                for field in ("index", "irqn", "irq_handler", "reg"):
                    if field not in inst:
                        errors.append(f"{self.name} instance missing '{field}'")

        if self.name == "gpio":
            if "port_count" not in self.data:
                errors.append("gpio requires port_count")
            if "pin_count" not in self.data:
                errors.append("gpio requires pin_count")
        return errors

    def ipcore_replacements(self, device: str) -> dict[str, str]:
        if not self.ipcore:
            return {}
        parts = self.ipcore.split("/")
        ip_name = parts[1] if len(parts) > 1 else parts[0]
        ip_lower = ip_name.lower()
        ip_upper = ip_name.upper()
        prefix_upper = self.api_prefix.upper()
        return {
            f"${{{prefix_upper}_IP}}": ip_upper,
            f"${{{self.api_prefix}_ip}}": ip_lower,
            f"${{SERIES/{prefix_upper}_IP}}": f"{device.upper()}_{prefix_upper}",
            "${SERIES/GPIO_IP}": f"{device.upper()}_GPIO",
        }


@dataclass(frozen=True)
class ChipConfig:
    raw: RawMap

    @classmethod
    def load(cls, path: Path) -> "ChipConfig":
        with path.open(encoding="utf-8") as f:
            loaded: Any = yaml.safe_load(f)
        if not isinstance(loaded, dict):
            raise ValueError("YAML config must be a mapping")
        return cls(cast(RawMap, loaded))

    @property
    def vendor(self) -> str:
        return text(self.raw["vendor"])

    @property
    def series(self) -> str:
        return text(self.raw["series"])

    @property
    def device(self) -> str:
        return text(self.raw["device"])

    @property
    def cpu(self) -> str:
        return text(self.raw["cpu"]).upper()

    @property
    def arch_pri_num(self) -> int:
        return int(self.raw["arch_pri_num"])

    @property
    def arch_pri_bit(self) -> int:
        return int(self.raw["arch_pri_bit"])

    @property
    def peripherals(self) -> list[Peripheral]:
        raw_peripherals = self.raw.get("peripherals", {})
        if not isinstance(raw_peripherals, dict):
            return []
        return [Peripheral(name, value) for name, value in cast(RawMap, raw_peripherals).items()]

    @property
    def arch_macro(self) -> str:
        return {
            "ARM": "__CPU_ARM__",
            "RV": "__CPU_RV__",
            "X86": "__CPU_X86__",
            "MCS51": "__CPU_MCS51__",
        }.get(self.cpu, f"__CPU_{self.cpu}__")

    @property
    def replacements(self) -> dict[str, str]:
        return {
            "${VENDOR}": self.vendor,
            "${SERIES}": self.series,
            "${DEVICE}": self.device,
            "__SERIES_NAME_A__": self.series,
            "__DEVICE_NAME_A__": self.device,
            "__series_name_a__": self.series.lower(),
            "__device_name_a__": self.device.lower(),
            "${ARCH_MACRO}": self.arch_macro,
            "vendor_header.h": f"{self.device}.h",
        }

    def validate(self) -> list[str]:
        errors: list[str] = []
        for key in REQUIRED_TOP:
            if key not in self.raw:
                errors.append(f"Missing required field: {key}")
        for peripheral in self.peripherals:
            errors.extend(peripheral.validate())
        return errors

    def peripheral(self, name: str) -> Peripheral | None:
        for peripheral in self.peripherals:
            if peripheral.name == name:
                return peripheral
        return None


class TemplateRenderer:
    def __init__(self, cfg: ChipConfig) -> None:
        self.cfg = cfg

    def replace(self, content: str, extra: dict[str, str] | None = None) -> str:
        replacements = dict(self.cfg.replacements)
        if extra is not None:
            replacements.update(extra)
        for old, new in replacements.items():
            content = content.replace(old, new)
        return content

    def _replace_zone(self, content: str, zone_name: str, replacement: str) -> str:
        begin = f"// {zone_name}\n"
        end = f"// {zone_name} end\n"
        start_pos = content.find(begin)
        end_pos = content.find(end, start_pos)
        if start_pos == -1 or end_pos == -1:
            return content
        return content[:start_pos] + replacement.rstrip("\n") + "\n" + content[end_pos + len(end):]

    def strip_role_blocks(self, content: str) -> str:
        lines: list[str] = []
        skipping_ipcore = False
        for line in content.splitlines():
            marker = line.strip()
            if marker == "// IPCore":
                skipping_ipcore = True
                continue
            if marker == "// IPCore end":
                skipping_ipcore = False
                continue
            if skipping_ipcore:
                continue
            if marker == "// HW" or marker.startswith("// HW "):
                continue
            if marker == "// HW end":
                continue
            if marker in ("// HW/IPCore", "// HW/IPCore end"):
                continue
            lines.append(line)
        return "\n".join(lines) + "\n"

    def device_h(self, template_dir: Path) -> str:
        content = self.replace((template_dir / "__series_name_a__" / "__device_name_a__" / "device.h").read_text(encoding="utf-8"))
        content = re.sub(r"(#\s*define\s+VSF_ARCH_PRI_NUM\s+)\d+", rf"\g<1>{self.cfg.arch_pri_num}", content)
        content = re.sub(r"(#\s*define\s+VSF_ARCH_PRI_BIT\s+)\d+", rf"\g<1>{self.cfg.arch_pri_bit}", content)
        return self._replace_zone(content, "peripheral defines", self.peripheral_defines())

    def device_dot_h(self, template_dir: Path) -> str:
        return self.replace((template_dir / "__series_name_a__" / "__device.h").read_text(encoding="utf-8"))

    def driver_h(self, template_dir: Path) -> str:
        content = self.replace((template_dir / "__series_name_a__" / "__device_name_a__" / "driver.h").read_text(encoding="utf-8"))
        content = self._replace_zone(content, "peripheral includes", self.peripheral_includes())
        return self._replace_zone(content, "template blocks", self.template_blocks())

    def peripheral_defines(self) -> str:
        lines = ["// Software interrupt provided by the device", "#define VSF_DEV_SWI_NUM                             0", ""]
        for peripheral in self.cfg.peripherals:
            if peripheral.is_usb:
                continue

            upper = peripheral.api_upper
            if peripheral.name == "gpio":
                port_count = int(peripheral.data.get("port_count", 0))
                pin_count = int(peripheral.data.get("pin_count", 32))
                lines.extend([
                    "// GPIO",
                    f"#define VSF_HW_GPIO_PORT_COUNT                  {port_count}",
                    f"#define VSF_HW_GPIO_PIN_COUNT                   {pin_count}",
                    "",
                ])
            elif peripheral.count is not None:
                lines.extend([f"// {upper}", f"#define VSF_HW_{upper}_COUNT                     {peripheral.count}", ""])
            elif peripheral.instances:
                indices = [inst.index for inst in peripheral.instances]
                contiguous = indices == list(range(indices[0], indices[-1] + 1))
                lines.append(f"// {upper}")
                lines.append(f"#define VSF_HW_{upper}_COUNT                    {len(peripheral.instances)}")
                if not (contiguous and indices[0] == 0):
                    mask = sum(1 << idx for idx in indices)
                    lines.append(f"#define VSF_HW_{upper}_MASK                     0x{mask:02X}")
                for inst in peripheral.instances:
                    lines.extend([
                        f"#define VSF_HW_{upper}{inst.index}_IRQN                    {inst.irqn}",
                        f"#define VSF_HW_{upper}{inst.index}_IRQHandler              {inst.irq_handler}",
                        f"#define VSF_HW_{upper}{inst.index}_REG                     {inst.reg}",
                    ])
                lines.append("")
        return "\n".join(lines)

    def peripheral_includes(self) -> str:
        lines: list[str] = []
        for peripheral in self.cfg.peripherals:
            if peripheral.is_usb:
                path = "usb/usb.h" if peripheral.usb_mode == "ip" else "usb_otg.nonip/usb_otg.nonip.h"
                lines.extend([f"// USB OTG ({peripheral.usb_mode})", f'#include "{path}"'])
                continue
            lines.extend([
                f"#if VSF_HAL_USE_{peripheral.api_upper} == ENABLED",
                f"#   include \"{peripheral.name}/{peripheral.name}.h\"",
                "#endif",
            ])
        return "\n".join(lines)

    def template_blocks(self) -> str:
        blocks: list[str] = []
        for peripheral in self.cfg.peripherals:
            if peripheral.is_usb:
                continue
            blocks.append(f"#if VSF_HAL_USE_{peripheral.api_upper} == ENABLED")
            if peripheral.name == "gpio":
                blocks.append("#   define VSF_GPIO_USE_IO_MODE_TYPE                        ENABLED")
            blocks.extend([
                f"#   include \"hal/driver/common/template/vsf_template_{peripheral.api_prefix}.h\"",
                "",
                f"#   define VSF_{peripheral.api_upper}_CFG_DEC_PREFIX                            vsf_hw",
                f"#   define VSF_{peripheral.api_upper}_CFG_DEC_UPCASE_PREFIX                     VSF_HW",
                f"#   include \"hal/driver/common/{peripheral.api_prefix}/{peripheral.api_prefix}_template.h\"",
                "#endif",
                "",
            ])
        return "\n".join(blocks)

    def startup_entries(self) -> tuple[str, str]:
        handlers: list[str] = []
        vectors: list[str] = []
        for peripheral in self.cfg.peripherals:
            if peripheral.is_usb:
                continue
            for inst in peripheral.instances:
                handlers.append(f"WEAK_ISR({inst.irq_handler})")
                vectors.append(f"    {inst.irq_handler},")
        return "\n".join(handlers), "\n".join(vectors)


class Scaffolder:
    def __init__(self, cfg: ChipConfig, driver_dir: Path, template_dir: Path | None = None) -> None:
        self.cfg = cfg
        self.driver_dir = driver_dir.resolve()
        self.template_dir = (template_dir or driver_dir / "template").resolve()
        self.renderer = TemplateRenderer(cfg)

    @property
    def vendor_path(self) -> Path:
        return self.driver_dir / self.cfg.vendor

    @property
    def device_path(self) -> Path:
        return self.vendor_path / self.cfg.device

    def run(self) -> list[str]:
        if not self.template_dir.exists():
            raise FileNotFoundError(f"template directory not found at {self.template_dir}")

        is_new_vendor = not (self.vendor_path / "driver.h").exists()
        self.device_path.mkdir(parents=True, exist_ok=True)
        (self.vendor_path / self.cfg.series / "common").mkdir(parents=True, exist_ok=True)

        self.write_device_files()
        generated_stubs = self.write_peripherals()
        self.write_series_common()
        self.register_vendor(is_new_vendor)
        print(f"Scaffold complete: {self.cfg.vendor}/{self.cfg.device} -> {self.device_path}")
        print(f"Peripherals: {', '.join(generated_stubs) or '(none)'}")
        return generated_stubs

    def write_device_files(self) -> None:
        self.write("device.h", self.renderer.device_h(self.template_dir))
        self.write("__device.h", self.renderer.device_dot_h(self.template_dir))
        self.write("driver.h", self.renderer.driver_h(self.template_dir))

        self.copy_template_file("__device_name_a__/driver.c", "driver.c")
        startup = self.template_dir / "__series_name_a__" / "__device_name_a__" / "startup.c"
        if startup.exists():
            content = self.renderer.replace(startup.read_text(encoding="utf-8"))
            handlers, vectors = self.renderer.startup_entries()
            content = content.replace("{{INTERRUPT_HANDLERS}}", handlers)
            content = content.replace("{{INTERRUPT_VECTORS}}", vectors)
            self.write(f"startup_{self.cfg.device}.c", content)
        self.copy_template_file("__device_name_a__/CMakeLists.txt", "CMakeLists.txt")

    def write_peripherals(self) -> list[str]:
        generated: list[str] = []
        for peripheral in self.cfg.peripherals:
            if peripheral.is_usb:
                self.write_usb(peripheral)
                generated.append("usb")
                continue
            self.write_peripheral(peripheral)
            generated.append(peripheral.name)
        return generated

    def write_peripheral(self, peripheral: Peripheral) -> None:
        src_dir = self.template_dir / "__series_name_a__" / "common" / peripheral.api_prefix
        dst_dir = self.device_path / peripheral.name
        dst_dir.mkdir(parents=True, exist_ok=True)

        if not (src_dir.exists() and any(src_dir.iterdir())):
            return

        extra = peripheral.ipcore_replacements(self.cfg.device)
        for src in src_dir.iterdir():
            if not (src.is_file() and src.suffix in (".h", ".c")):
                continue
            content = self.renderer.strip_role_blocks(src.read_text(encoding="utf-8"))
            content = self.renderer.replace(content, extra)
            dst_name = src.name
            if peripheral.api_prefix != peripheral.name and src.stem == peripheral.api_prefix:
                dst_name = peripheral.name + src.suffix
            (dst_dir / dst_name).write_text(content, encoding="utf-8")


    def write_usb(self, peripheral: Peripheral) -> None:
        dst_dir = self.device_path / ("usb" if peripheral.usb_mode == "ip" else "usb_otg.nonip")
        dst_dir.mkdir(parents=True, exist_ok=True)
        src_dir = self.template_dir / "__series_name_a__" / "common" / "usb_otg.nonip"
        if not src_dir.exists():
            return
        for src in src_dir.iterdir():
            if src.is_file():
                content = self.renderer.replace(src.read_text(encoding="utf-8"))
                (dst_dir / src.name).write_text(content, encoding="utf-8")

    def write_series_common(self) -> None:
        src = self.template_dir / "__series_name_a__" / "common" / "CMakeLists.txt"
        if src.exists():
            dst = self.vendor_path / self.cfg.series / "common" / "CMakeLists.txt"
            dst.write_text(self.renderer.replace(src.read_text(encoding="utf-8")), encoding="utf-8")

    def register_vendor(self, is_new_vendor: bool) -> None:
        if is_new_vendor:
            self.register_new_vendor()
        else:
            self.append_device_to_vendor()

    def register_new_vendor(self) -> None:
        top_driver = self.driver_dir / "driver.h"
        if not top_driver.exists():
            top_driver.write_text(
                "#include \"hal/vsf_hal_cfg.h\"\n\n"
                "#undef VSF_HAL_DRIVER_HEADER\n\n"
                "#if 0\n"
                "#else\n"
                "#   error No supported device found.\n"
                "#endif\n\n"
                "#ifdef VSF_HAL_DRIVER_HEADER\n"
                "#   include VSF_HAL_DRIVER_HEADER\n"
                "#endif\n",
                encoding="utf-8",
            )
        if f"defined(__{self.cfg.vendor}__)" in top_driver.read_text(encoding="utf-8"):
            return

        self.vendor_path.mkdir(parents=True, exist_ok=True)
        content = self.renderer.replace((self.template_dir / "driver.h").read_text(encoding="utf-8"))
        content = content.replace("defined(__DEVICE_NAME_A__)", f"defined(__{self.cfg.device}__)")
        content = content.replace("__DEVICE_NAME_A__", self.cfg.device)
        content = content.replace("__VSF_HAL_DRIVER_${VENDOR}_H__", f"__HAL_DRIVER_{self.cfg.vendor.upper()}_H__")
        content = content.replace("./__SERIES_NAME_A__/", f"./{self.cfg.device}/")
        (self.vendor_path / "driver.h").write_text(content, encoding="utf-8")
        self.insert_before_else(top_driver, [f"#elif defined(__{self.cfg.vendor}__)", f'#   include "./{self.cfg.vendor}/driver.h"'])

    def append_device_to_vendor(self) -> None:
        vendor_driver = self.vendor_path / "driver.h"
        content = vendor_driver.read_text(encoding="utf-8")
        if f"defined(__{self.cfg.device}__)" in content:
            return
        branch = [
            f"#elif   defined(__{self.cfg.device}__)",
            f'#   define  VSF_{self.cfg.vendor.upper()}_DRIVER_HEADER           "./{self.cfg.device}/driver.h"',
        ]
        self.insert_before_else(vendor_driver, branch)

    def insert_before_else(self, path: Path, new_lines: list[str]) -> None:
        lines = path.read_text(encoding="utf-8").split("\n")
        result: list[str] = []
        inserted = False
        for line in lines:
            if not inserted and "".join(line.split()).startswith("#else"):
                result.extend(new_lines)
                inserted = True
            result.append(line)
        path.write_text("\n".join(result) + "\n", encoding="utf-8")

    def copy_template_file(self, template_rel: str, output_name: str) -> None:
        src = self.template_dir / "__series_name_a__" / template_rel
        if src.exists():
            self.write(output_name, self.renderer.replace(src.read_text(encoding="utf-8")))

    def write(self, relative: str, content: str) -> None:
        (self.device_path / relative).write_text(content, encoding="utf-8")



def scaffold(config_path: str, driver_dir: str, template_dir: str | None = None) -> None:
    try:
        cfg = ChipConfig.load(Path(config_path))
    except ValueError as err:
        print(f"Error: {err}", file=sys.stderr)
        sys.exit(1)

    errors = cfg.validate()
    if errors:
        print("Validation errors:", file=sys.stderr)
        for error in errors:
            print(f"  - {error}", file=sys.stderr)
        sys.exit(1)

    try:
        Scaffolder(cfg, Path(driver_dir), Path(template_dir) if template_dir else None).run()
    except FileNotFoundError as err:
        print(f"Error: {err}", file=sys.stderr)
        sys.exit(1)


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate VSF HAL driver skeleton for a new chip.")
    parser.add_argument("--driver-dir", required=True, help="Path to output driver directory")
    parser.add_argument("--config", required=True, help="Path to YAML chip configuration file")
    parser.add_argument("--template-dir", help="Path to VSF template directory (default: <driver-dir>/template)")
    args = parser.parse_args()
    scaffold(args.config, args.driver_dir, args.template_dir)


if __name__ == "__main__":
    main()
