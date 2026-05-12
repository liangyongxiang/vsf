---
name: add-chip-support
type: workflow
description: |
  USE FOR: porting a new MCU to VSF HAL, generating driver skeleton for a new chip, implementing VSF peripheral drivers (UART, I2C, SPI, GPIO), setting up IPCore-backed or direct-register peripherals.
  DO NOT USE FOR: adding features to supported chips, fixing driver bugs, board-level BSP changes.
---

# add-chip-support

**WORKFLOW SKILL**. INVOKES: `scaffold_chip.py`, `build-firmware`, `board-run`.

## Overview

| Level | Goal | Verify |
|-------|------|--------|
| L0 | Vendor SDK UART echo works | serial output |
| L1 | VSF skeleton builds | cmake passes |
| L2 | First VSF UART works | board-run PASS |
| L3 | Other peripherals | per-peripheral test |

## L0 — SDK Baseline

Run minimal UART echo with vendor SDK. Do NOT proceed without serial output.

## L1 — VSF Skeleton

Collect from vendor SDK: CPU type, IRQ prio bits, clock freqs, per-peripheral IRQ numbers, register bases, IRQ handler names. Identify IPCore path (e.g. `ARM/PL011`).

Run `scaffold_chip.py --driver-dir <dir> --config chip_config.yml`. Script copies templates — no real driver logic. **Warning**: overwrites existing files.

Make it build: verify `device.h` macros, implement `driver.c` clock init, add SDK paths to `CMakeLists.txt`. Build with `build-firmware`.

## L2 — Minimal Peripheral (UART)

Reference: an existing UART driver for a supported chip (e.g. `driver/<VENDOR>/<DEVICE>/uart/uart.c`). Fill `.c` stub, wire IRQ in `VSF_USART_CFG_IMP_LV0`. IPCore pattern: wrap type in struct, delegate init/fini, instantiate via `CFG_IMP_LV0` with `IRQHandler`. Verify with `board-run`.

## L3 — Other Peripherals

Repeat L2 per peripheral. Non-IPCore (GPIO): register ops directly. Verify each with `board-run`.

## Troubleshooting

- **Build fails**: Verify macros match SDK, clock init, SDK paths
- **No UART output**: Check `driver.c` clock and `CFG_IMP_LV0` IRQ wiring
