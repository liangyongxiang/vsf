# VSF HAL Driver Reference

## USART

### Architecture

Two modes: **IPCore** (reuse existing IP, e.g. PL011) and **Direct** (raw register access).

**Header:** Direct sets `REIMPLEMENT_TYPE_*=ENABLED`, defines mode/irq/status/cfg, includes `vsf_template_usart.h`. IPCore includes IPCore header only (types built-in).

**Source:** Direct uses struct `{.reg, .isr}` and implements all APIs. IPCore uses `implement(vsf_pl011_usart_t)`, chip provides reset/NVIC/clock. Set `__VSF_HAL_${IP}_USART_CLASS_INHERIT__`.

IMP_LV0:
```c
#define VSF_USART_CFG_IMP_LV0(ID, OP)                     \
    vsf_hw_usart_t vsf_hw_usart##ID = {.reg = REG, OP};   \
    void VSF_HW_USART##ID_IRQHandler(void) {               \
        vsf_pl011_usart_irqhandler(&vsf_hw_usart##ID.use); \
    }
#include "hal/driver/common/usart/usart_template.inc"
```

### API (VSF_USART_APIS)

Core: init, fini, enable, disable, capability, get_configuration.
IRQ: irq_enable, irq_disable, irq_clear, status.
FIFO: rxfifo_get_data_count, rxfifo_read, txfifo_get_free_count, txfifo_write.
DMA: request_rx, request_tx, cancel_rx, cancel_tx, get_rx_count, get_tx_count.
Control: ctrl.

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

Mandatory placeholders (must exist even if HW unsupported):
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
Extra bits (RX_TIMEOUT, CTS, FRAME_ERR, BREAK_ERR, PARITY_ERR, RX_OVERFLOW_ERR, RX_IDLE) need `#define VSF_USART_IRQ_MASK_<X>` in header or VSF treats as unsupported.

### IRQ handler

```c
void VSF_HW_USART<N>_IRQHandler(void) {
    uintptr_t ctx = vsf_hal_irq_enter();
    // read raw status, read enabled mask, active = raw & mask
    // clear active irqs, if active && handler_fn: call it
    vsf_hal_irq_leave(ctx);
}
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

### Reference implementations

- RP2040 (PL011 IPCore): `driver/RaspberryPi/RP2040/uart/uart.c`
- PL011 IPCore: `driver/IPCore/ARM/PL011/vsf_pl011_uart.{h,c}`
- Direct: `driver/template/__series_name_a__/common/usart/usart.c`
- STM32H7RSXX, GD32H7XX: `driver/<vendor>/<chip>/common/usart/`

---

## GPIO

### Architecture

GPIO is always **Direct** mode — no IPCore. Struct: `{.reg, .isr}`. Implement all API functions via register access.

IMP_LV0 with multi-class support:
```c
#if VSF_HW_GPIO_CFG_MULTI_CLASS == ENABLED
    vsf_gpio_t vsf_gpio;    // embedded base struct for polymorphic dispatch
#endif
    // driver-specific fields after base
```

### API (VSF_GPIO_APIS)

mandatory: init, fini, capability, set/clear, read, config_pins, get_pin_configuration, set_input/output, exti_irq_enable/disable/clear.
optional: toggle, output_and_set/clear, switch_direction, read_output_register.

### RP2040 lessons learned

**1. PADS register base matters.** Initial driver set PADS base to `0x16` (PDE=1, pull-down enabled). When caller enabled `VSF_GPIO_PULL_UP`, both PUE and PDE bits were set — RP2040 interprets this as "bus keep" (hold current level), not "pull-up and pull-down". Fix: base = `0x12` (DRIVE=01, SCHMITT=1, no pull). Always verify the default reset value of PADS registers before choosing a base.

**2. PADS.OD breaks atomic output transitions.** Setting `PADS.OD=1` for input mode disables the output buffer at PADS level. Even after SIO.OE is raised by `output_and_set`, the PADS output disable prevents the pin from driving. Fix: never set `PADS.OD` — let SIO.OE be the sole arbiter of high-Z vs drive. This enables atomic input-to-output transitions.

**3. Self-loopback (simultaneous input+output).** Some chips (RP2040 SIO) allow reading a pin's level even while it's driving output. Keep `PADS.IE=1` for output modes. Set `capability.can_read_in_gpio_output_mode=1`. This enables same-pin loopback — no external jumper needed for output→input tests.

**4. Open-drain emulation.** RP2040 has no hardware OD mode. Track OD pins in a driver-side `open_drain_mask`. For OD pins: keep `gpio_out=0`, use `gpio_oe=1` to drive low, `gpio_oe=0` to float high. `get_pin_configuration()` must check driver-side OD mask before inspecting hardware registers.

**5. `get_pin_configuration` read-back.** Implement by reading real hardware registers (PADS + SIO + IO_BANK0), not returning template defaults. Verify the re-derived mode matches what was configured — especially for OD, pull-up/down, and AF modes.

**6. AF mode detection without reimplementing mode enum.** If the driver doesn't reimplement `vsf_gpio_mode_t`, use a dual-detection strategy: mode base equals the template AF slot value OR `alternate_function` field is non-zero. Document the convention in the header.

**7. EXTI trigger bit encoding.** Per-pin trigger bits may be encoded in dense register layouts (e.g. 4-bit field per pin). Pre-compute trigger values during `port_config_pins()` and store in a driver array. Level bits are auto-track (read from hardware status), edge bits are write-1-clear. Don't clear level bits in the clear function.

**8. IMP_LV0 can hardcode instance name.** Single-port chips don't need template-generated names. Hardcoding `vsf_hw_gpio0` is fine when `PORT_COUNT=1`.

### Template files

| File | Purpose |
|------|---------|
| `common/template/vsf_template_gpio.h` | API declarations, types |
| `template/.../gpio/gpio.h` | Header skeleton |
| `template/.../gpio/gpio.c` | Implementation skeleton |
| `common/gpio/gpio_template.inc` | IMP_LV0 instantiation |
| `common/gpio/gpio_interface.c` | Multi-class dispatch |
| `common/gpio/gpio_common.c` | Default implementations |

### Reference

- RP2040: `driver/RaspberryPi/RP2040/gpio/gpio.c` — direct SIO access, OD emulation, EXTI

---

## I2C / SPI / ADC / PWM

Template skeleton in `source/hal/driver/template/__series_name_a__/common/<periph>/`.
Pattern: copy header+source, rename placeholders (`__SERIES__`, `__name0__`), implement API functions via register ops.

Common inc files: `<periph>_template.inc`, `<periph>_interface.c`, `<periph>_common.c`.
Reference: check `source/hal/driver/ST/STM32H7RSXX/common/` for working multi-peripheral examples.

---

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| Build fails | Verify macros match SDK, clock init, SDK paths in CMakeLists.txt |
| No output | Deassert reset in `_init()`, check IMP_LV0 IRQ wiring |
| Garbage (USART) | Wrong baudrate formula |
| Spurious IRQ | Check mask before FIFO/GPIO read |
| RX not firing | `RX_FIFO_THRESHOLD_NOT_EMPTY` (USART) |
| Pull-up not working | Check PADS base doesn't set conflicting pull bits |
| `output_and_set` no effect | Verify PADS.OD not set for the pin |
| Template overwrites existing | `scaffold_chip.py` for new chips only; edit existing |

## Examples

New chip, USART IPCore:
1. `scaffold_chip.py --driver-dir source/hal/driver/MyVendor/MyChip`
2. Copy uart.{h,c}, `implement(vsf_pl011_usart_t)`, add reset/NVIC/clock
3. `VSF_USART_CFG_IMP_LV0` with IRQ handler
4. board.c: pinmux → reset → init → enable → irq
5. Verify: `scripts/check-usart-*.py` then vsf-board-run

Existing chip, new GPIO:
1. Copy gpio.{h,c} from template, `REIMPLEMENT_TYPE_*=ENABLED`, struct `{.reg,.isr}`
2. Implement all API functions via register access, `VSF_GPIO_CFG_IMP_LV0`
3. Verify with vsf-board-run
