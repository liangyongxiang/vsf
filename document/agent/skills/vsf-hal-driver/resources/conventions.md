# Conventions

## No hardcoded instances
Per-instance values (register base, IRQ, clock, reset) must come from `device.h` macros, expanded via `VSF_MCONNECT(..., __IDX)` in the IMP_LV0 macro block.

## No spin-wait without comment
Every `while (reg->flag);` loop polling a hardware register must have a preceding comment explaining why spin-wait is required and the expected upper-bound duration (`< X us`). Enforced by `check-driver-quality.py`.

## No pinmux in driver
GPIO function selection belongs in the board file, never in the peripheral driver.

## Unsupported configuration → return error, never silently ignore
If the hardware (or this chip port) does not support a feature that the user can express through `cfg_ptr` fields — e.g. interrupts when no IRQ line exists, DMA when no DMA channel is wired, adjustable priority when priority is fixed — `init()` must reject the configuration by returning `VSF_ERR_NOT_SUPPORT` (or `VSF_ERR_INVALID_PARAMETER` for out-of-range numeric values). Never silently ignore the field: silent acceptance creates an expectation that the feature works, and the user will file a bug when it does not. The error return tells the caller immediately that their configuration is incompatible with this hardware.

## Unimplemented APIs
Return `VSF_ERR_NOT_SUPPORT` with `VSF_HAL_ASSERT(0)`. Never emulate missing hardware features in software (no busy-wait loops, no software state machines).

## Clock and teardown
Enable peripheral clocks before register access. In `fini()`, disable NVIC IRQs and abort any in-flight DMA/transfers before releasing resources.
