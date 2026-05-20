---
name: vsf-hal-driver
description: |
  USE FOR: VSF HAL drivers — create, implement, modify, fix bugs, extend API coverage (UART, I2C, SPI, GPIO).
  DO NOT USE FOR: build/flash/test (use vsf-board-run), BSP changes.
---

# VSF HAL Driver

## Porting a new chip

Follow `PORTING.md` — a numbered ladder from vendor SDK serial to full peripheral suite. Each rung has a concrete pass criterion. Read it sequentially.

## Operations

**New chip:** L0→L3. `scaffold_chip.py` → copy from template. Verify with vsf-board-run.

**Add periph:** Copy template → `REIMPLEMENT_*` → `IMP_LV0` → board.c. Parameterize instances in `device.h` (base addr, IRQn, IRQ handler) — never hardcode in driver `.c`.

**Migrate old:** Copy template over old, port HW logic, replace names with `VSF_MCONNECT`. Enable `REIMPLEMENT_API_*` for missing APIs (`irq_clear`/`ctrl`/`get_configuration` are common gaps). See REFERENCE.md.

**Add API:** `REIMPLEMENT_API_<FN>=ENABLED`, implement body.

**Fix bug:** Reproduce, compare with template + working reference.

## Template locations

`template/__series_name_a__/common/` under `source/hal/driver/`. Reference: `RaspberryPi/RP2040/`. Inc: `<periph>_template.inc`.

## Key patterns

**Direct mode:** struct `{.reg, .isr}`, implement all APIs.

**IPCore mode:** `implement(vsf_<ip>_<periph>_t)` embeds the base class — don't duplicate. Set `__VSF_HAL_${IP}_<PERIPH>_CLASS_INHERIT__`. Chip adds reset/NVIC/clock.

Pitfalls: if IP aliases IRQ values (PL011 `RX_IDLE==RX_TIMEOUT`), use `IRQ_MASK_CHECK_UNIQUE=LOOSE` with `#undef <ALIAS>` before template include. Use `MODE_CHECK_UNIQUE=LOOSE` when mode bits overlap zero. Include `_reg.h` for `irq_clear`.

**Board:** pinmux must live outside the peripheral driver (typically `vsf_board.c`, but the file is conventional, not mandatory). Reset/clock/NVIC live in the peripheral driver's `init()` — never in the board file. Whether `vsf_board.c` calls `vsf_hw_<periph>_init(...)` is the developer's choice. Per-instance base/IRQn come from `device.h` macros, never from literal addresses in the driver.
