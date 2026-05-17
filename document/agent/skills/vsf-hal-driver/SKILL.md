---
name: vsf-hal-driver
description: |
  USE FOR: VSF HAL drivers — create, implement, modify, fix bugs, extend API coverage (UART, I2C, SPI, GPIO).
  DO NOT USE FOR: build/flash/test (use vsf-board-run), BSP changes.
---

# VSF HAL Driver

## Workflow

| L | Goal | How |
|---|------|-----|
| 0 | SDK UART echo | Vendor SDK, stop until serial works |
| 1 | VSF skeleton | `scaffold_chip.py`, `device.h`, clock init in `driver.c` |
| 2 | First UART | Copy template, IMP_LV0 + IRQ, pinmux in board.c |
| 3 | Other periph | Repeat L2 per peripheral |

## Operations

**New chip:** L0→L3. `scaffold_chip.py` → copy from `source/hal/driver/template/`. Verify each L with vsf-board-run.

**Add periph to chip:** L2/L3. Copy template → set `REIMPLEMENT_*` → wire `IMP_LV0` → board.c.

**Add missing API:** Find signature in `common/template/vsf_template_<periph>.h`, `REIMPLEMENT_API_<FN>=ENABLED`, implement body.

**Fix bug:** Reproduce with vsf-board-run, compare with template + working reference chip, fix, retest.

## Template locations

All under `source/hal/driver/template/__series_name_a__/common/`. Reference implementations: `RaspberryPi/RP2040/` (USART, GPIO). Common inc: `usart_template.inc`, `gpio_template.inc`, etc.

## Key patterns

**Direct mode:** struct `{.reg, .isr}`, implement all APIs, `VSF_<PERIPH>_CFG_IMP_LV0`.

**IPCore mode:** `implement(vsf_<ip>_<periph>_t)`, chip provides reset/NVIC/clock. Set `__VSF_HAL_${IP}_<PERIPH>_CLASS_INHERIT__`.

**Board wiring:** pinmux → reset → init → enable → irq. Expose instance ptr.
