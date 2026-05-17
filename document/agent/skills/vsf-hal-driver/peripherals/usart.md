# USART

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

Mandatory placeholders:
```c
VSF_USART_9_BIT_LENGTH  = (0x1ul << 24),
VSF_USART_1_5_STOPBIT   = (0x1ul << 25),
VSF_USART_0_5_STOPBIT   = (0x1ul << 26),
VSF_USART_10_BIT_LENGTH = (0x1ul << 27),
VSF_USART_SYNC_CLOCK_ENABLE = (0x1ul << 28),
VSF_USART_HALF_DUPLEX_ENABLE = (0x1ul << 29),
```

## IRQ mask (vsf_usart_irq_mask_t)

TX_CPL(0), RX_CPL(1), TX(2), RX(3) — template defaults.
Extra bits (RX_TIMEOUT, CTS, FRAME_ERR, BREAK_ERR, PARITY_ERR, RX_OVERFLOW_ERR, RX_IDLE) need `#define VSF_USART_IRQ_MASK_<X>`.

## API

Core: init, fini, enable, disable, capability, get_configuration.
IRQ: irq_enable/disable/clear, status.
FIFO: rxfifo_get_data_count/read, txfifo_get_free_count/write.
DMA: request_rx/tx, cancel_rx/tx, get_rx/tx_count.
Control: ctrl.

## IPCore IMP_LV0

```c
#define VSF_USART_CFG_IMP_LV0(ID, OP)                     \
    vsf_hw_usart_t vsf_hw_usart##ID = {.reg = REG, OP};   \
    void VSF_HW_USART##ID_IRQHandler(void) {               \
        vsf_pl011_usart_irqhandler(&vsf_hw_usart##ID.use); \
    }
#include "hal/driver/common/usart/usart_template.inc"
```

## Template files

| File | Purpose |
|------|---------|
| `common/template/vsf_template_usart.h` | API declarations, types |
| `template/.../usart/usart.h` | Header skeleton |
| `template/.../usart/usart.c` | Implementation skeleton |
| `common/usart/usart_template.inc` | IMP_LV0 instantiation |
| `common/usart/usart_interface.c` | Multi-class dispatch |
| `common/usart/usart_common.c` | Default implementations |

## Reference

- RP2040 (PL011 IPCore): `driver/RaspberryPi/RP2040/uart/uart.c`
- PL011 IPCore: `driver/IPCore/ARM/PL011/vsf_pl011_uart.{h,c}`
- Direct: `driver/template/__series_name_a__/common/usart/usart.c`
- STM32H7RSXX, GD32H7XX: `driver/<vendor>/<chip>/common/usart/`
