# VSF HAL Driver Reference

## Common patterns

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

**Mode bits**: each peripheral's mode field is a bitmask. Some bits are mandatory (must exist even if HW doesn't support them). Template defines the bit layout and mandatory placeholder values — match your HW register bits to the template fields. If the HW has no direct equivalent, put the placeholder value in a high bit range that won't conflict.

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
  common/template/vsf_template_<periph>.h    ← API declarations, default types
  common/<periph>/<periph>_template.inc      ← IMP_LV0 instantiation
  common/<periph>/<periph>_interface.c       ← multi-class dispatch
  common/<periph>/<periph>_common.c          ← default implementations
  template/__series_name_a__/common/<periph>/<periph>.h  ← header skeleton
  template/__series_name_a__/common/<periph>/<periph>.c  ← impl skeleton
```

### Board wiring

`vsf_board.c`: pinmux → reset → init → enable → irq. Expose instance pointer.

---

## USART

### Mode bits (vsf_usart_mode_t)

| Field | Bits | Mandatory |
|-------|------|-----------|
| Parity | 0-2 | NO, EVEN, ODD |
| Stop | 3-4 | 1_STOPBIT |
| Data | 5-7 | 8_BIT_LENGTH |
| HW ctrl | 8 | — |
| TX en | 9 | TX_ENABLE |
| RX en | 10 | RX_ENABLE |
| Sync clock | 11 | — |
| Half duplex | 14 | — |
| TX FIFO thresh | 15-16 | — |
| RX FIFO thresh | 17-18 | — |

Mandatory placeholders:
```c
VSF_USART_9_BIT_LENGTH  = (0x1ul << 24),
VSF_USART_1_5_STOPBIT   = (0x1ul << 25),
VSF_USART_0_5_STOPBIT   = (0x1ul << 26),
VSF_USART_10_BIT_LENGTH = (0x1ul << 27),
VSF_USART_SYNC_CLOCK_ENABLE = (0x1ul << 28),
VSF_USART_HALF_DUPLEX_ENABLE = (0x1ul << 29),
```

### IRQ mask (vsf_usart_irq_mask_t)

TX_CPL(0), RX_CPL(1), TX(2), RX(3) — template defaults.
Extra bits (RX_TIMEOUT, CTS, FRAME_ERR, BREAK_ERR, PARITY_ERR, RX_OVERFLOW_ERR, RX_IDLE) need `#define VSF_USART_IRQ_MASK_<X>`.

### API

Core: init, fini, enable, disable, capability, get_configuration.
IRQ: irq_enable/disable/clear, status.
FIFO: rxfifo_get_data_count/read, txfifo_get_free_count/write.
DMA: request_rx/tx, cancel_rx/tx, get_rx/tx_count.
Control: ctrl.

### IPCore IMP_LV0

```c
#define VSF_USART_CFG_IMP_LV0(ID, OP)                     \
    vsf_hw_usart_t vsf_hw_usart##ID = {.reg = REG, OP};   \
    void VSF_HW_USART##ID_IRQHandler(void) {               \
        vsf_pl011_usart_irqhandler(&vsf_hw_usart##ID.use); \
    }
#include "hal/driver/common/usart/usart_template.inc"
```

### Template files

| File | Purpose |
|------|---------|
| `common/template/vsf_template_usart.h` | API declarations, types |
| `template/.../usart/usart.h` | Header skeleton |
| `template/.../usart/usart.c` | Implementation skeleton |
| `common/usart/usart_template.inc` | IMP_LV0 instantiation |
| `common/usart/usart_interface.c` | Multi-class dispatch |
| `common/usart/usart_common.c` | Default implementations |

### Reference

- RP2040 (PL011 IPCore): `driver/RaspberryPi/RP2040/uart/uart.c`
- PL011 IPCore: `driver/IPCore/ARM/PL011/vsf_pl011_uart.{h,c}`
- Direct: `driver/template/__series_name_a__/common/usart/usart.c`
- STM32H7RSXX, GD32H7XX: `driver/<vendor>/<chip>/common/usart/`

---

## GPIO

Always Direct mode. Template: `template/.../gpio/gpio.{h,c}`. Common inc: `gpio_template.inc`.

### API (VSF_GPIO_APIS)

mandatory: init, fini, capability, set/clear, read, config_pins, get_pin_configuration, set_input/output, exti_irq_enable/disable/clear.
optional: toggle, output_and_set/clear, switch_direction, read_output_register.

### Design tips

- **Verify register reset defaults.** Picking a wrong base value for pin configuration registers can cause subtle bugs (e.g. bus-keep vs pull-up). Read the datasheet carefully for reset values.
- **Check output-disable bits.** Setting a global output-disable flag (like `PADS.OD`) for input pins can block later atomic output transitions. Prefer per-pin control via the output-enable register.
- **Self-loopback.** If the chip supports reading output level while driving, set `capability.can_read_in_gpio_output_mode=1`. Enables same-pin testing without external wiring.
- **OD emulation.** If HW lacks open-drain mode, emulate: track OD pins in driver-side mask, keep output register at 0, toggle output-enable to drive/float.
- **`get_pin_configuration`** should read real HW registers and re-derive the configured mode — not return template defaults.
- **AF detection without reimplementing mode enum:** check mode base OR `alternate_function != 0`.
- **EXTI: level vs edge.** Level bits auto-track HW status (read-only). Edge bits are write-1-clear. Never clear level bits in the clear function.
- **Single-port IMP_LV0** can hardcode instance name.

### Reference

- RP2040: `driver/RaspberryPi/RP2040/gpio/gpio.c`

---

## I2C / SPI / ADC / PWM

Template in `template/__series_name_a__/common/<periph>/`. Copy, rename placeholders, implement APIs via register access. Common inc: `<periph>_template.inc`, `<periph>_interface.c`, `<periph>_common.c`.

Reference: `driver/ST/STM32H7RSXX/common/` for multi-peripheral working examples.

---

## Style migration (old → template standard)

Older drivers (e.g. RP2040 uart.c) use hardcoded names: `vsf_hw_usart_init`, `vsf_hw_usart_t`. The current standard uses `VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)` with a configurable prefix. To migrate:

### Method: backup → copy template → fill logic

1. **Backup** the old `.c` file to e.g. `uart_old.c.bak`
2. **Copy** the template `.c` file from `template/__series_name_a__/common/<periph>/` over the old file
3. **Purge** irrelevant sections:
   - Remove `// IPCore` blocks (keep only `// HW` blocks for chip-level drivers)
   - Remove IPCore `IMP_PREFIX` definitions (`vsf_${IP}`), keep `vsf_hw`
4. **Fill in hardware logic** from the old driver into each template function body, preserving the template's function signature and structure
5. **Move old IMP_LV0 fields** (e.g. `.irqn`, `.reg` base address) into the template's IMP_LV0 macro
6. **Keep template includes** (`vsf_hal_cfg.h` → `vsf_hal.h` → vendor SDK), add any chip-specific vendor headers needed
7. **Update the header** (`.h` file) to use `VSF_MCONNECT` for the struct type

### Key replacements

| Old pattern | Template equivalent |
|---|---|
| `vsf_hw_usart_init(...)` | `VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)(...)` |
| `vsf_hw_usart_t *hw_usart_ptr` | `VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr` |
| `typedef struct vsf_hw_usart_t {` | `typedef struct VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) {` |
| `vsf_hw_usart0` | `VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, 0)` |
| `vsf_pl011_usart_init(&ptr->use_as__vsf_pl011_usart_t, ...)` | same — IPCore delegation is fine |
| `#include "../driver.h"` | `#include "hal/vsf_hal_cfg.h"` |
| `#include "RP2040.h"` | `#include "hal/driver/vendor_driver.h"` |

### IPCore delegation

If migrating an IPCore-based driver (chip wraps an existing IP like PL011), the delegation patterns don't change — only naming. `init()` still delegates to `vsf_<ip>_<periph>_init()`, `capability()` to `vsf_<ip>_<periph>_capability()`, etc. Move chip-specific add-ons (reset, NVIC, clock, extra IRQ mask bits) into the new template body.

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| No output | Deassert reset, check IMP_LV0 IRQ wiring |
| Garbage (USART) | Wrong baudrate formula |
| Spurious IRQ | Check mask before status read |
| Pull-up not working | Check PADS base doesn't set conflicting pull bits |
| `output_and_set` no effect | Verify PADS.OD not set |
| Template overwrites existing | `scaffold_chip.py` new chips only; edit existing directly |

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
