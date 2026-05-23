---
name: vsf-hal-driver
description: |
  Create, implement, modify, or debug VSF HAL drivers (UART, I2C, SPI, GPIO, ADC, PWM, etc.).
  **UTILITY SKILL** — INVOKES: vsf-bench (for verification).
  USE FOR: porting new chips, adding peripherals to existing chips, fixing driver bugs.
  DO NOT USE FOR: build/flash/test (use vsf-bench), BSP-only pinmux changes.
  FOR SINGLE OPERATIONS: prefer direct edits over full template copy if driver already exists.
metadata:
  version: "1.0"
  license: Apache-2.0
---

# VSF HAL Driver

## Quickstart

**New chip:** follow `PORTING.md` R0→R5.

**Add peripheral:**
```bash
scaffold_peripheral.py --driver-dir source/hal/driver --chip Vendor/Chip --periph <name>
# edit .c/.h → implement register logic
check-driver-structure.py --periph <name> --side header <name>.h
check-driver-structure.py --periph <name> --side source <name>.c
check-driver-quality.py <name>.c
vsf-bench --all board/pico/hardware-map.yml --suite <name>
```

**Audit a port:**
```bash
audit-port.py --chip Vendor/Chip
```

## Scripts

`scaffold_chip.py`, `scaffold_peripheral.py`, `generate-device-peripheral-macros.py`, `check-driver-structure.py`, `check-driver-quality.py`, `audit-port.py`, `enable-periph.py`

## Before debugging: verify IO wiring

If a peripheral test fails, run the `gpio_io_check` suite first:

```bash
vsf-bench --all board/<board>/hardware-map.yml --suite gpio_io_check
```

This catches swapped TX/RX, missing pull-ups, and broken traces before register-level debugging.

## Conventions

See `REFERENCE.md` for register read side effects, unimplemented API stubs, `get_configuration` convention, and full driver quality rules. See `PORTING.md` for the R0→R5 ladder.
