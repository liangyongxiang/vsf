# Conventions

## No hardcoded instances
Per-instance values (register base, IRQ, clock, reset) must come from `device.h` macros, expanded via `VSF_MCONNECT(..., __IDX)` in the IMP_LV0 macro block.

## No spin-wait without comment
Every `while (reg->flag);` loop polling a hardware register must have a preceding comment explaining why spin-wait is required and the expected upper-bound duration (`< X us`). Enforced by `check-driver-quality.py`.

## No pinmux in driver
GPIO function selection belongs in the board file, never in the peripheral driver.

## Unimplemented APIs
Return `VSF_ERR_NOT_SUPPORT` with `VSF_HAL_ASSERT(0)`. Never emulate missing hardware features in software (no busy-wait loops, no software state machines).

## Clock and teardown
Enable peripheral clocks before register access. In `fini()`, disable NVIC IRQs and abort any in-flight DMA/transfers before releasing resources.
