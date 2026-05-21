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
check-<periph>-header.py <name>.h
check-<periph>-source.py <name>.c
check-driver-quality.py <name>.c
vsf-bench --all board/pico/hardware-map.yml --scene <name>
```

**Audit a port:**
```bash
audit-port.py --chip Vendor/Chip
```

## Scripts

| Script | Use when... |
|---|---|
| `scaffold_chip.py` | New chip port |
| `scaffold_peripheral.py` | Add peripheral to existing chip |
| `generate-device-peripheral-macros.py` | Edit device.h instances |
| `check-<periph>-header.py` | Verify header structure |
| `check-<periph>-source.py` | Verify source structure |
| `check-driver-quality.py` | Anti-pattern check |
| `audit-port.py` | Cross-file consistency |
| `enable-periph.py` | Toggle vsf_usr_cfg.h |

See `REFERENCE.md` for conventions and `PORTING.md` for the full ladder.
