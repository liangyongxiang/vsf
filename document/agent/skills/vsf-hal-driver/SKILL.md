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

**Check quality:** `scripts/check-driver-quality.py <file>...` flags anti-patterns (instance-index branching, hardcoded IRQ/reset/clock/address, missing `VSF_MCONNECT`, pinmux in driver). Run before opening a PR. Suppress a finding inline with `// quality: allow-<rule-id>`.

## Template locations

`template/__series_name_a__/common/` under `source/hal/driver/`. Reference: `RaspberryPi/RP2040/`. Inc: `<periph>_template.inc`.

## Key patterns

**Direct mode:** struct `{.reg, .isr}`, implement all APIs.

**IPCore mode:** `implement(vsf_<ip>_<periph>_t)` embeds the base class — don't duplicate. Set `__VSF_HAL_${IP}_<PERIPH>_CLASS_INHERIT__`. Chip adds reset/NVIC/clock.

**Include convention:** peripheral drivers pull the chip header through `#include "hal/driver/vendor_driver.h"` — never `#include "RP2040.h"` (or other chip filename) directly. Vendor peripheral struct/reg headers (`hardware/structs/<periph>.h`, `hardware/regs/<periph>.h`) live in **the chip's `device.h` main block** and reach the driver transitively — driver `.c` files do not include them directly. IPCore `*_reg.h` headers (VSF-owned, not vendor) stay as direct includes in the driver. See REFERENCE.md "Include convention".

**Unimplemented APIs (chip drivers only):** in actual chip drivers, every stub must `VSF_HAL_ASSERT(0);` AND return an error (`VSF_ERR_NOT_SUPPORT` for "hardware can't") or a sentinel (`0`/`-1`) — never a silent `return VSF_ERR_NONE;` after only a pointer assert. Template skeletons under `template/__series_name_a__/` are exempt — they're scaffolding the porter edits. See REFERENCE.md "Unimplemented API convention".

**Register reads have side effects:** read each peripheral register **once** into a local and operate on the cached value — clear-on-read status, FIFO pop, and auto-increment are the default assumption. See REFERENCE.md "Register access: read side effects and caching".

**Thin wrapper:** the HAL exposes exactly what the hardware natively supports. If hardware can't do an operation, return `VSF_ERR_NOT_SUPPORT` — never emulate missing features inside the driver. See REFERENCE.md "Thin wrapper philosophy".

**Non-blocking:** all HAL APIs must be non-blocking. No busy-wait loops. See REFERENCE.md "Non-blocking API requirement".

Pitfalls: if IP aliases IRQ values (PL011 `RX_IDLE==RX_TIMEOUT`), use `IRQ_MASK_CHECK_UNIQUE=LOOSE` with `#undef <ALIAS>` before template include. Use `MODE_CHECK_UNIQUE=LOOSE` when mode bits overlap zero. Include `_reg.h` for `irq_clear`.

**Board:** pinmux must live outside the peripheral driver (typically `vsf_board.c`, but the file is conventional, not mandatory). Reset/clock/NVIC live in the peripheral driver's `init()` — never in the board file. Whether `vsf_board.c` calls `vsf_hw_<periph>_init(...)` is the developer's choice. Per-instance base/IRQn come from `device.h` macros, never from literal addresses in the driver.
