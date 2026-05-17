# VSF HAL USART Reference

## Architecture

Two modes: **IPCore** (reuse existing IP, e.g. PL011) and **Direct** (raw register access).

### Header (`chip/uart/uart.h`)

Direct: `REIMPLEMENT_TYPE_*=ENABLED`, define mode/irq/status/cfg, include `vsf_template_usart.h`.
IPCore: include IPCore header (types built-in).

### Source (`chip/uart/uart.c`)

Direct: struct `{.reg, .isr}`, implement all API functions.
IPCore: `implement(vsf_pl011_usart_t)`, chip provides reset/NVIC/clock. Set `__VSF_HAL_${IP}_USART_CLASS_INHERIT__`.

IMP_LV0:
```c
#define VSF_USART_CFG_IMP_LV0(ID, OP)                     \
    vsf_hw_usart_t vsf_hw_usart##ID = {.reg = REG, OP};   \
    void VSF_HW_USART##ID_IRQHandler(void) {               \
        vsf_pl011_usart_irqhandler(&vsf_hw_usart##ID.use); \
    }
#include "hal/driver/common/usart/usart_template.inc"
```

## API (VSF_USART_APIS)

Core: init, fini, enable, disable, capability, get_configuration.
IRQ: irq_enable, irq_disable, irq_clear, status.
FIFO: rxfifo_get_data_count, rxfifo_read, txfifo_get_free_count, txfifo_write.
DMA: request_rx, request_tx, cancel_rx, cancel_tx, get_rx_count, get_tx_count.
Control: ctrl.

## Mode bits (vsf_usart_mode_t)

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
VSF_USART_SYNC_CLOCK_DISABLE = 0,
VSF_USART_HALF_DUPLEX_ENABLE = (0x1ul << 29),
VSF_USART_HALF_DUPLEX_DISABLE = 0,
```

## IRQ mask (vsf_usart_irq_mask_t)

TX_CPL(0), RX_CPL(1), TX(2), RX(3) — template defaults.
RX_TIMEOUT(4), CTS(5), FRAME_ERR(6), BREAK_ERR(7), PARITY_ERR(8), RX_OVERFLOW_ERR(9), RX_IDLE(12) — needs `#define VSF_USART_IRQ_MASK_<X>`.

## IRQ handler

```c
void VSF_HW_USART<N>_IRQHandler(void) {
    uintptr_t ctx = vsf_hal_irq_enter();
    // read raw status, read enabled mask, active = raw & mask
    // clear active irqs, if active && handler_fn: call it
    vsf_hal_irq_leave(ctx);
}
```

## Template files index

| File | Purpose |
|------|---------|
| `common/template/vsf_template_usart.h` | API declarations, types |
| `template/.../usart/usart.h` | Header skeleton |
| `template/.../usart/usart.c` | Implementation skeleton |
| `common/usart/usart_template.inc` | IMP_LV0 instantiation |
| `common/usart/usart_interface.c` | Multi-class dispatch |
| `common/usart/usart_common.c` | Default implementations |

## Reference implementations

- RP2040 (PL011 IPCore): `driver/RaspberryPi/RP2040/uart/uart.c`
- PL011 IPCore: `driver/IPCore/ARM/PL011/vsf_pl011_uart.{h,c}`
- Direct template: `driver/template/__series_name_a__/common/usart/usart.c`
- STM32H7RSXX: `driver/ST/STM32H7RSXX/common/usart/usart.c`
- GD32H7XX: `driver/GigaDevice/GD32H7XX/common/usart/usart.c`

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| Build fails | Verify macros match SDK, clock init, SDK paths in CMakeLists.txt |
| No UART output | Deassert reset in `_init()`, check `CFG_IMP_LV0` IRQ wiring |
| Garbage output | Wrong baudrate formula |
| Spurious IRQ | Check mask before FIFO read |
| RX not firing | Use `RX_FIFO_THRESHOLD_NOT_EMPTY` |
| Stream hangs | Drain RX FIFO when full |
| Multi-instance mismatch | Ensure `IMP_LV0` expands per-instance with correct `.reg` and IRQ handler |
| Template overwrites existing | Use `scaffold_chip.py` for new chips only; edit existing files directly |

## Examples

New chip, USART IPCore mode:
1. `scaffold_chip.py --driver-dir source/hal/driver/MyVendor/MyChip`
2. Copy uart.{h,c} from template, `implement(vsf_pl011_usart_t)`, add reset/NVIC/clock
3. `VSF_USART_CFG_IMP_LV0` with IRQ handler
4. board.c: pinmux → reset → init → enable → irq
5. Verify: `scripts/check-usart-*.py` then vsf-board-run

New GPIO, direct mode:
1. Copy gpio.{h,c} from template, `REIMPLEMENT_TYPE_*=ENABLED`, struct `{.reg,.isr}`
2. Implement all API functions, `VSF_GPIO_CFG_IMP_LV0`
3. Verify with vsf-board-run
