# Conventions

## No hardcoded instances
Per-instance values (register base, IRQ, clock, reset) must come from `device.h` macros, expanded via `VSF_MCONNECT(..., __IDX)` in the IMP_LV0 macro block.

## No magic numbers for register bit-fields
When a driver reimplements `vsf_<periph>_mode_t` (or any packed configuration type) so that bit-fields directly encode hardware register values, all bit positions, masks, and hardware constants must be defined as `__VSF_HW`-prefixed macros in the header file. The enum values then compose from these macros, and the `.c` file extracts fields using the same macros — never bare literals like `0x1F`, `5`, `0xF`, or raw shift counts like `>> 7`.

Header (replace `<PERIPH>` and `<FIELD>` with the actual peripheral and field names):
```c
#define __VSF_HW_<PERIPH>_<FIELD>_SHIFT     N
#define __VSF_HW_<PERIPH>_<FIELD>_MASK      ((1u << M) - 1)
#define __VSF_HW_<PERIPH>_<FIELD>_VAL_X     Xu
#define __VSF_HW_<PERIPH>_<FIELD>_VAL_Y     Yu
...
typedef enum vsf_<periph>_mode_t {
    VSF_<PERIPH>_MODE_A = (__VSF_HW_<PERIPH>_<FIELD>_VAL_X << __VSF_HW_<PERIPH>_<FIELD>_SHIFT)
                        | (0 << __VSF_HW_<PERIPH>_<FLAG>_POS) | ...,
} vsf_<periph>_mode_t;
```

Source:
```c
uint32_t field = cfg_ptr->mode & __VSF_HW_<PERIPH>_<FIELD>_MASK;
bool flag      = (cfg_ptr->mode >> __VSF_HW_<PERIPH>_<FLAG>_POS) & 1;
```

This keeps the bit layout in one place, makes intent explicit in both header and source, and the `__VSF_HW` prefix signals internal-only constants that user code should not rely on.

## No spin-wait without comment
Every `while (reg->flag);` loop polling a hardware register must have a preceding comment explaining why spin-wait is required and the expected upper-bound duration (`< X us`). Enforced by `check-driver-quality.py`.

## No pinmux in driver
GPIO function selection belongs in the board file, never in the peripheral driver.

## Unsupported configuration → return error, never silently ignore
If the hardware (or this chip port) does not support a feature that the user can express through `cfg_ptr` fields — e.g. interrupts when no IRQ line exists, DMA when no DMA channel is wired, adjustable priority when priority is fixed — `init()` must reject the configuration by returning `VSF_ERR_NOT_SUPPORT` (or `VSF_ERR_INVALID_PARAMETER` for out-of-range numeric values). Never silently ignore the field: silent acceptance creates an expectation that the feature works, and the user will file a bug when it does not. The error return tells the caller immediately that their configuration is incompatible with this hardware.

## Unimplemented APIs
Return `VSF_ERR_NOT_SUPPORT` with `VSF_HAL_ASSERT(0)`. Never emulate missing hardware features in software (no busy-wait loops, no software state machines).

## Unused parameters → VSF_UNUSED_PARAM
When a parameter is intentionally unused (e.g. a no-op IRQ handler where the peripheral has no interrupt line), use `VSF_UNUSED_PARAM(param_name)` instead of the bare `(void)param_name;` cast. The macro is defined in `vsf/source/utilities/compiler/__common/__type.h` and provides a uniform, searchable pattern across all drivers. This prevents silent drift from the project convention and makes it immediately obvious that the parameter is deliberately unused rather than accidentally overlooked.

## Clock and teardown
Enable peripheral clocks before register access. In `fini()`, disable NVIC IRQs and abort any in-flight DMA/transfers before releasing resources.

## Document implemented vs. unimplemented capabilities
Every driver `.c` file must contain a block comment near the top (before `TYPES` or inside `MACROS`) that lists:
1. **Hardware capabilities** relevant to this peripheral (e.g., "RP2040 DMA: 12 channels, 2 IRQ lines, 4 transfer widths").
2. **What the driver currently implements**.
3. **What is intentionally not yet implemented**, with a brief `TODO` note explaining what would need to change to implement it.

This prevents future maintainers (human or AI) from assuming a feature works when it is only partially wired, and makes capability gaps discoverable without reading the entire datasheet. Any code location that implements a partial or placeholder behavior should also carry an inline `TODO` pointing back to the top-level block comment. Example: see `source/hal/driver/RaspberryPi/RP2040/dma/dma.c`.
