# VSF HAL Driver Reference

## Common patterns

### Per-instance parameterization in device.h

All per-instance differences (register base, IRQn, IRQ handler name, clock ID) are **parameterized as macros in `device.h`** (or `__device.h`), never hardcoded in the driver `.c` file. This makes the driver generic: the same `uart.c` works for any instance count because it only references `VSF_HW_USART##N##_REG` and `VSF_HW_USART##N##_IRQN`.

**Naming convention** (must match what `IMP_LV0` consumes):

```c
// device.h
#define VSF_HW_<PERIPH>_COUNT       N

// Per-instance macros (indexed 0..N-1)
#define VSF_HW_<PERIPH>0_IRQN       UART0_IRQ_IRQn
#define VSF_HW_<PERIPH>0_IRQHandler UART0_IRQHandler
#define VSF_HW_<PERIPH>0_REG        UART0_BASE
#define VSF_HW_<PERIPH>1_IRQN       UART1_IRQ_IRQn
#define VSF_HW_<PERIPH>1_IRQHandler UART1_IRQHandler
#define VSF_HW_<PERIPH>1_REG        UART1_BASE
```

**How `driver.h` consumes them:**

```c
// driver.h
#if VSF_HAL_USE_USART == ENABLED
#   include "hal/driver/common/template/vsf_template_usart.h"
#   define VSF_USART_CFG_DEC_PREFIX         vsf_hw
#   define VSF_USART_CFG_DEC_UPCASE_PREFIX  VSF_HW
#   include "hal/driver/common/usart/usart_template.h"
#endif
```

`usart_template.h` -> `vsf_template_instance_declaration.h` uses `VSF_HW_USART_COUNT`/`VSF_HW_USART_MASK` to emit extern declarations for `vsf_hw_usart0`...`vsf_hw_usartN`.

**How `IMP_LV0` consumes them:**

```c
#define VSF_USART_CFG_IMP_LV0(ID, OP)                                \
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t)                 \
        VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, ID) = {       \
        .reg  = (void *)VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX,\
                                    _USART, ID, _REG),               \
        .irqn = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX,        \
                             _USART, ID, _IRQN),                     \
        OP                                                           \
    };                                                               \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX,  \
                                   _USART, ID, _IRQHandler)(void)    \
    { ... }
```

**Rule:** if a new peripheral is added, first add its `VSF_HW_<PERIPH>_COUNT` and per-instance macros to `device.h`, then write the driver -- the driver must never contain literal addresses like `0x40034000` or `UART0_IRQ_IRQn`.

### Macro prefix convention

In driver `.c` files, prefix internal/local macros with `__` to avoid colliding with headers:

| Category | Example | Prefix |
|---|---|---|
| Template config macros (consumed by `<periph>_template.inc`) | `VSF_USART_CFG_IMP_LV0`, `VSF_USART_CFG_IMP_PREFIX` | none -- template system requires exact names |
| OOC class control macros | `__VSF_HAL_PL011_UART_CLASS_INHERIT__`, `__VSF_HAL_DW_APB_I2C_CLASS_IMPLEMENT` | `__` |
| Driver-local helpers | `__vsf_hw_usart_irqhandler`, `__uart_tx_fifo_depth` | `__` |

**Why:** vendor SDK headers and VSF template headers define many unprefixed macros. A local helper like `REG` or `IRQN` can silently shadow or be shadowed by a header macro. Always `__` prefix macros that are not part of the VSF template API surface.

### Architecture: IPCore vs Direct

- **IPCore**: chip reuses existing IP block (e.g. ARM PL011 for USART). Driver calls `implement(vsf_<ip>_<periph>_t)`, IPCore handles register/IQ/baudrate. Chip provides reset, NVIC, clock. Set `__VSF_HAL_${IP}_<PERIPH>_CLASS_INHERIT__`.
- **Direct**: raw register access. Struct `{.reg, .isr}`, implement all APIs via register ops.

### IMP_LV0

Every driver defines a `VSF_<PERIPH>_CFG_IMP_LV0(ID, OP)` macro before including `<periph>_template.inc`. Instantiates the `vsf_hw_<periph>##ID` struct and (optionally) its IRQ handler. Single-port chips may hardcode instance name instead of using `ID`.

### MULTI_CLASS

When `VSF_HW_<PERIPH>_CFG_MULTI_CLASS == ENABLED`, the driver struct embeds `vsf_<periph>_t` as first member for polymorphic dispatch through a vtable (`ptr->op->fn`). `<periph>_common.c` provides default implementations; driver overrides by setting `REIMPLEMENT_API_<FN> = ENABLED`.

### REIMPLEMENT macros

Two categories in `<periph>.h`:
- `REIMPLEMENT_TYPE_*=ENABLED`: redefine mode/irq/status/cfg enums and structs from template defaults
- `REIMPLEMENT_API_<FN>=ENABLED`: replace template's default function body with driver-specific implementation

### Mode and IRQ definitions

**Mode bits**: each peripheral's mode field is a bitmask. Some bits are mandatory (must exist even if HW doesn't support them). Template defines the bit layout and mandatory placeholder values -- match your HW register bits to the template fields. If the HW has no direct equivalent, put the placeholder value in a high bit range that won't conflict.

**IRQ mask**: each peripheral's IRQ mask is a bitmask. Template provides default bits. Extra bits need explicit `#define VSF_<PERIPH>_IRQ_MASK_<X>` or VSF treats them as unsupported.

**IRQ handler pattern**:
```c
void VSF_HW_<PERIPH><N>_IRQHandler(void) {
    uintptr_t ctx = vsf_hal_irq_enter();
    // read raw status, read enabled mask, active = raw & mask
    // clear active irqs, if active && handler_fn: call it
    vsf_hal_irq_leave(ctx);
}
```

### Template file convention

```
source/hal/driver/
  common/template/vsf_template_<periph>.h    <- API declarations, default types
  common/<periph>/<periph>_template.inc      <- IMP_LV0 instantiation
  common/<periph>/<periph>_interface.c       <- multi-class dispatch
  common/<periph>/<periph>_common.c          <- default implementations
  template/__series_name_a__/common/<periph>/<periph>.h  <- header skeleton
  template/__series_name_a__/common/<periph>/<periph>.c  <- impl skeleton
```

### Board wiring

`vsf_board.c`: pinmux -> reset -> init -> enable -> irq. Expose instance pointer.

---

## Peripheral guides

| Peripheral | File |
|---|---|
| USART | [peripherals/usart.md](peripherals/usart.md) |
| GPIO | [peripherals/gpio.md](peripherals/gpio.md) |
| I2C | [peripherals/i2c.md](peripherals/i2c.md) |
| SPI | [peripherals/spi.md](peripherals/spi.md) |
| ADC | [peripherals/adc.md](peripherals/adc.md) |
| PWM | [peripherals/pwm.md](peripherals/pwm.md) |

---

## Style migration (old -> template standard)

Older drivers (e.g. RP2040 uart.c) use hardcoded names: `vsf_hw_usart_init`, `vsf_hw_usart_t`. The current standard uses `VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)` with a configurable prefix. To migrate:

### Method: backup -> copy template -> fill logic

1. **Backup** the old `.c` file to e.g. `uart_old.c.bak`
2. **Copy** the template `.c` file from `template/__series_name_a__/common/<periph>/` over the old file
3. **Purge** irrelevant sections:
   - Remove `// IPCore` blocks (keep only `// HW` blocks for chip-level drivers)
   - Remove IPCore `IMP_PREFIX` definitions (`vsf_${IP}`), keep `vsf_hw`
4. **Fill in hardware logic** from the old driver into each template function body, preserving the template's function signature and structure
5. **Port config macros.** The template's `#include "<periph>_template.inc"` block is preceded by `REIMPLEMENT_API_*`, `CHECK_MODE`, and `IMP_LV0` macros. Migrate the old driver's macro values into the template's corresponding slots -- don't copy-paste the entire block from the old file.
6. **Move old IMP_LV0 fields** (e.g. `.irqn`, `.reg` base address) into the template's IMP_LV0 macro
7. **Keep template includes** (`vsf_hal_cfg.h` -> `vsf_hal.h` -> vendor SDK), add any chip-specific vendor headers needed
8. **Update the header** (`.h` file) to use `VSF_MCONNECT` for the struct type

### Key replacements

| Old pattern | Template equivalent |
|---|---|
| `vsf_hw_usart_init(...)` | `VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)(...)` |
| `vsf_hw_usart_t *hw_usart_ptr` | `VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr` |
| `typedef struct vsf_hw_usart_t {` | `typedef struct VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) {` |
| `vsf_hw_usart0` | `VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, 0)` |
| `vsf_pl011_usart_init(&ptr->use_as__vsf_pl011_usart_t, ...)` | same -- IPCore delegation is fine |
| `#include "../driver.h"` | `#include "hal/vsf_hal_cfg.h"` |
| `#include "RP2040.h"` | `#include "hal/driver/vendor_driver.h"` |

### IPCore delegation

If migrating an IPCore-based driver (chip wraps an existing IP like PL011), the delegation patterns don't change -- only naming. `init()` still delegates to `vsf_<ip>_<periph>_init()`, `capability()` to `vsf_<ip>_<periph>_capability()`, etc. Move chip-specific add-ons (reset, NVIC, clock, extra IRQ mask bits) into the new template body.

### IPCore migration pitfalls

| Pitfall | Symptom | Fix |
|---------|---------|-----|
| Duplicate base-class member | `duplicate member 'vsf_usart'` | `implement(vsf_pl011_usart_t)` already embeds `vsf_usart` when MULTI_CLASS is on -- don't declare it again in the wrapping struct |
| `IRQ_MASK_CHECK_UNIQUE` with aliased IRQs | `static assertion failed: Enum values must have disjoint bits: VSF_USART_IRQ_MASK_RX_TIMEOUT and VSF_USART_IRQ_MASK_RX_IDLE` | Use `VSF_HAL_CHECK_MODE_LOOSE` (not STRICT) and `#undef` the alias macro (e.g. `VSF_USART_IRQ_MASK_RX_IDLE`) right before `#include "...usart_template.inc"`, then `#define` it back. This keeps the check active for all non-alias bits. |
| `MODE_CHECK_UNIQUE` with zero-valued mode bits | static assertion on mode values sharing bit 0 | PL011 places several unsupported modes at high bits but `HALF_DUPLEX_DISABLE=0` overlaps `NO_PARITY=0`. Use `VSF_USART_CFG_MODE_CHECK_UNIQUE = VSF_HAL_CHECK_MODE_LOOSE` |
| `irq_clear` needs IP register access | undefined `reg->UARTICR` | Include the IP's `_reg.h` (e.g. `hal/driver/IPCore/ARM/PL011/vsf_pl011_uart_reg.h`) and cast `usart_ptr->reg` to the register struct type |
| Type mismatch on FIFO functions | implicit conversion warnings | Template uses `uint_fast32_t` for `rxfifo_read`/`txfifo_write`/`_get_data_count`/`_get_free_count`. IP may use `uint_fast16_t` -- cast in the wrapper |

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| No output | Deassert reset, check IMP_LV0 IRQ wiring |
| Garbage (USART) | Wrong baudrate formula |
| Spurious IRQ | Check mask before status read |
| Pull-up not working | Check PADS base doesn't set conflicting pull bits |
| `output_and_set` no effect | Verify PADS.OD not set |
| Template overwrites existing | `scaffold_chip.py` new chips only; edit existing directly |
| `duplicate member` in IPCore struct | Remove explicit `vsf_<periph>_t` -- `implement(vsf_<ip>_<periph>_t)` already includes it |
| `CHECK_UNIQUE` failure on mode/IRQ bits | See IPCore migration pitfalls table above |

## Examples

New chip USART (IPCore):
1. `scaffold_chip.py --driver-dir source/hal/driver/MyVendor/MyChip`
2. Copy uart.{h,c}, `implement(vsf_pl011_usart_t)`, add reset/NVIC/clock
3. `VSF_USART_CFG_IMP_LV0`, board.c pinmux+init
4. Verify: `check-usart-*.py` then vsf-board-run

Existing chip, new GPIO:
1. Copy gpio.{h,c}, `REIMPLEMENT_TYPE_*=ENABLED`, struct `{.reg,.isr}`
2. Implement APIs via register access, `VSF_GPIO_CFG_IMP_LV0`
3. Verify with vsf-board-run
