#!/usr/bin/env python3
"""
Cross-file port completeness auditor.

Scans a chip driver directory and reports structural gaps across:
- device.h (declarations)
- driver .h/.c files (existence)
- driver.h (template blocks)
- vsf_usr_cfg.h (enable flags)

Usage:
    audit-port.py --chip RaspberryPi/RP2040 [--driver-dir source/hal/driver] \
                  [--board-dir board/pico] [--vsf-usr-cfg path/to/vsf_usr_cfg.h]

Exit codes:
    0 = clean
    1 = errors (gaps that prevent compilation)
    2 = warnings (suspicious but compilable)
    3 = script error
"""

import argparse
import re
import sys
from pathlib import Path


# Map: peripheral short name -> VSF_HAL_USE_* macro name
PERIPH_MAP = {
    "adc": "ADC",
    "dac": "DAC",
    "dma": "DMA",
    "eth": "ETH",
    "flash": "FLASH",
    "gpio": "GPIO",
    "i2c": "I2C",
    "i2s": "I2S",
    "pwm": "PWM",
    "rng": "RNG",
    "rtc": "RTC",
    "sdio": "SDIO",
    "spi": "SPI",
    "timer": "TIMER",
    "uart": "USART",   # VSF uses USART for both UART and USART
    "usart": "USART",
    "usb": "USB",
    "wdt": "WDT",
}

# Reverse: upper -> canonical short name
UPPER_TO_SHORT = {
    "USART": "uart",  # prefer uart as canonical
}


def scan_device_h(device_h: Path) -> dict[str, dict]:
    """Parse device.h for VSF_HW_*_COUNT declarations."""
    text = device_h.read_text(encoding="utf-8")
    results: dict[str, dict] = {}

    # Match: #define VSF_HW_GPIO_PORT_COUNT  N   or   #define VSF_HW_ADC_COUNT  N
    count_re = re.compile(
        r'^\s*#\s*define\s+VSF_HW_([A-Z0-9]+)_(COUNT|PORT_COUNT)\s+(\d+)',
        re.MULTILINE,
    )
    for m in count_re.finditer(text):
        upper = m.group(1)
        kind = m.group(2)
        count = int(m.group(3))
        short = UPPER_TO_SHORT.get(upper, upper.lower())
        results[short] = {"count": count, "kind": kind, "upper": upper}

    # Also look for instance macros to detect peripherals without COUNT
    inst_re = re.compile(
        r'^\s*#\s*define\s+VSF_HW_([A-Z0-9]+)\d+_IRQN\s+',
        re.MULTILINE,
    )
    for m in inst_re.finditer(text):
        upper = m.group(1)
        short = UPPER_TO_SHORT.get(upper, upper.lower())
        if short not in results:
            results[short] = {"count": 1, "kind": "instance", "upper": upper}

    return results


def scan_driver_h(driver_h: Path) -> set[str]:
    """Find which peripherals have template blocks in driver.h."""
    text = driver_h.read_text(encoding="utf-8")
    found: set[str] = set()
    for short, upper in PERIPH_MAP.items():
        # Look for: #if VSF_HAL_USE_UPPER == ENABLED ... #include ".../vsf_template_<periph>.h"
        pat = rf'#if\s+VSF_HAL_USE_{upper}\s*==\s*ENABLED.*?#\s*include\s+"hal/driver/common/template/vsf_template_{short}'
        if re.search(pat, text, re.DOTALL):
            found.add(short)
        # Also match usart template block for uart peripheral
        if short == "uart" and re.search(rf'#if\s+VSF_HAL_USE_USART\s*==\s*ENABLED', text):
            found.add("uart")
            found.add("usart")
    return found


def scan_vsf_usr_cfg(cfg_path: Path) -> set[str]:
    """Find which VSF_HAL_USE_* are ENABLED in vsf_usr_cfg.h."""
    text = cfg_path.read_text(encoding="utf-8")
    enabled: set[str] = set()
    for short, upper in PERIPH_MAP.items():
        if re.search(rf'^\s*#\s*define\s+VSF_HAL_USE_{upper}\s+ENABLED', text, re.MULTILINE):
            enabled.add(short)
    return enabled


def scan_peripheral_files(chip_dir: Path) -> set[str]:
    """Find which peripheral subdirectories exist with .h/.c files."""
    found: set[str] = set()
    if not chip_dir.exists():
        return found
    for subdir in chip_dir.iterdir():
        if subdir.is_dir() and any(f.suffix in (".h", ".c") for f in subdir.iterdir() if f.is_file()):
            found.add(subdir.name)
    return found


def check_include_convention(chip_dir: Path) -> list[tuple[Path, str]]:
    """Check that .c files include hal/driver/vendor_driver.h, not bare chip headers."""
    findings: list[tuple[Path, str]] = []
    if not chip_dir.exists():
        return findings

    # Pattern: include of a bare vendor chip header like "RP2040.h" or "stm32h7xx.h"
    # Exempt: device.h and driver.c (chip-level integration)
    bare_chip_re = re.compile(r'#\s*include\s+"[A-Z][A-Za-z0-9_]*\.h"')
    exempt_names = {"device.h", "driver.c", "driver.h", "__device.h"}

    for cfile in chip_dir.rglob("*.c"):
        if cfile.name in exempt_names:
            continue
        text = cfile.read_text(encoding="utf-8")
        for m in bare_chip_re.finditer(text):
            inc = m.group(0)
            # Skip if it's a known peripheral header from device.h centralized block
            if "hardware/structs/" in inc or "hardware/regs/" in inc:
                continue
            # Skip if it's vendor_driver.h or vsf_hal.h
            if "vendor_driver.h" in inc or "vsf_hal.h" in inc or "vsf_hal_cfg.h" in inc:
                continue
            findings.append((cfile, inc))

    return findings


def audit(
    chip: str,
    driver_dir: Path,
    board_dir: Path | None,
    cfg_path: Path | None,
) -> int:
    parts = chip.split("/")
    if len(parts) != 2:
        print("Error: --chip must be Vendor/Chip", file=sys.stderr)
        return 3

    vendor, device = parts
    chip_dir = driver_dir / vendor / device
    device_h = chip_dir / "device.h"
    driver_h = chip_dir / "driver.h"

    if not chip_dir.exists():
        print(f"Error: chip directory not found: {chip_dir}", file=sys.stderr)
        return 3

    if not device_h.is_file():
        print(f"Error: device.h not found: {device_h}", file=sys.stderr)
        return 3

    errors = 0
    warnings = 0

    declarations = scan_device_h(device_h)
    files = scan_peripheral_files(chip_dir)
    template_blocks = scan_driver_h(driver_h) if driver_h.is_file() else set()
    enabled = set()
    if cfg_path and cfg_path.is_file():
        enabled = scan_vsf_usr_cfg(cfg_path)

    print(f"=== Auditing {chip} ===\n")

    # 1. Declaration gaps: declared in device.h but no files
    for short, info in sorted(declarations.items()):
        if info["count"] > 0 and short not in files:
            # Exception: some peripherals share a directory (uart/usart)
            if short == "usart" and "uart" in files:
                continue
            print(f"[declaration-gap] {short}: VSF_HW_{info['upper']}_COUNT={info['count']} but no {short}/ directory")
            errors += 1

    # 2. Enable gaps: declared but not enabled
    if cfg_path and cfg_path.is_file():
        for short, info in sorted(declarations.items()):
            if info["count"] > 0:
                mapped = PERIPH_MAP.get(short, short.upper())
                # Check if enabled in cfg
                cfg_text = cfg_path.read_text(encoding="utf-8")
                cfg_enabled = bool(
                    re.search(rf'^\s*#\s*define\s+VSF_HAL_USE_{mapped}\s+ENABLED', cfg_text, re.MULTILINE)
                )
                if not cfg_enabled:
                    print(f"[enable-gap] {short}: declared in device.h but VSF_HAL_USE_{mapped} not ENABLED in {cfg_path}")
                    warnings += 1

    # 3. Template block gaps: enabled but no template block in driver.h
    if driver_h.is_file() and cfg_path and cfg_path.is_file():
        for short in sorted(enabled):
            if short not in template_blocks:
                # Check if there are files for this peripheral
                has_files = short in files or (short == "usart" and "uart" in files)
                if has_files:
                    print(f"[template-block-gap] {short}: enabled in vsf_usr_cfg.h but no template block in driver.h")
                    errors += 1

    # 4. Stale declarations: COUNT=0 but files exist
    for short in sorted(files):
        if short in declarations and declarations[short]["count"] == 0:
            print(f"[stale-declaration] {short}: files exist but VSF_HW_{declarations[short]['upper']}_COUNT=0")
            warnings += 1
        elif short not in declarations:
            # Files exist but no declaration at all
            print(f"[stale-declaration] {short}: files exist but no VSF_HW_* declaration in device.h")
            warnings += 1

    # 5. Include convention
    include_findings = check_include_convention(chip_dir)
    for cfile, inc in include_findings:
        print(f"[include-convention] {cfile}: {inc} — use hal/driver/vendor_driver.h instead")
        warnings += 1

    print()
    if errors:
        print(f"FAIL: {errors} error(s), {warnings} warning(s)")
        return 1
    elif warnings:
        print(f"PASS: {warnings} warning(s)")
        return 2
    else:
        print("PASS: all checks passed")
        return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Cross-file port completeness auditor.")
    parser.add_argument("--chip", required=True, help="Chip path, e.g. RaspberryPi/RP2040")
    parser.add_argument("--driver-dir", default="source/hal/driver", help="Path to driver directory")
    parser.add_argument("--board-dir", help="Path to board directory (e.g. board/pico)")
    parser.add_argument("--vsf-usr-cfg", help="Path to vsf_usr_cfg.h (default: auto-discover from board-dir)")
    args = parser.parse_args()

    driver_dir = Path(args.driver_dir).resolve()

    cfg_path: Path | None = None
    if args.vsf_usr_cfg:
        cfg_path = Path(args.vsf_usr_cfg).resolve()
    elif args.board_dir:
        board_cfg = Path(args.board_dir) / "vsf_usr_cfg.h"
        if board_cfg.is_file():
            cfg_path = board_cfg.resolve()

    return audit(args.chip, driver_dir, Path(args.board_dir) if args.board_dir else None, cfg_path)


if __name__ == "__main__":
    sys.exit(main())
