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

**Defining extra IRQ masks in the driver header:**

When using an IPCore, hardware-specific IRQ masks are defined in the chip driver header **before** the IPCore header:

```c
// uart.h
enum {
    VSF_USART_IRQ_MASK_TX_CPL = (0x1ul << 16),
    VSF_USART_IRQ_MASK_RX_CPL = (0x1ul << 17),
};
#include "hal/driver/IPCore/ARM/PL011/vsf_pl011_uart.h"
```

**IRQ mask alias suppression:**

If the IPCore aliases two IRQ masks to the same value (e.g., PL011 `RX_IDLE == RX_TIMEOUT`), suppress the alias before including `usart_template.inc` to avoid `CHECK_UNIQUE` failure, then restore it:

```c
#undef VSF_USART_IRQ_MASK_RX_IDLE
#include "hal/driver/common/usart/usart_template.inc"
#define VSF_USART_IRQ_MASK_RX_IDLE  VSF_USART_IRQ_MASK_RX_IDLE
```

Also set `VSF_USART_CFG_IRQ_MASK_CHECK_UNIQUE = VSF_HAL_CHECK_MODE_LOOSE` if multiple aliases exist.

## API

Core: init, fini, enable, disable, capability, get_configuration.
IRQ: irq_enable/disable/clear, status.
FIFO: rxfifo_get_data_count/read, txfifo_get_free_count/write.
DMA: request_rx/tx, cancel_rx/tx, get_rx/tx_count.
Control: ctrl.

### Break control (`ctrl` operations)

Three `ctrl` operations exist to cover different hardware capabilities. The HAL is a **thin wrapper** — each driver implements only the operations the hardware natively supports, without software emulation.

| Operation | What hardware must provide | Example |
|-----------|---------------------------|---------|
| `VSF_USART_CTRL_SET_BREAK` | A register bit to assert break (TX line low). Write-only, returns immediately. | PL011 `UARTLCR_H.BRK = 1` |
| `VSF_USART_CTRL_CLEAR_BREAK` | A register bit to de-assert break. Write-only, returns immediately. | PL011 `UARTLCR_H.BRK = 0` |
| `VSF_USART_CTRL_SEND_BREAK` | Hardware auto-timed break — a single register write asserts break and hardware auto-clears after >= 1 frame time, raising an IRQ on completion. Non-blocking. | STM32 `USART_CR1.SBK` or `USART_RQR.SBKRQ` |

**Implementation rule:** If the hardware does not natively support an operation, return `VSF_ERR_NOT_SUPPORT`. Do NOT emulate it in software. PL011 only supports `SET_BREAK` / `CLEAR_BREAK` — it has no auto-timed break, so `SEND_BREAK` returns `VSF_ERR_NOT_SUPPORT`. A caller that needs a timed break on PL011 uses `SET_BREAK` + its own timer + `CLEAR_BREAK`.

**Rationale:** The HAL exposes hardware capability directly. Emulating missing features inside the driver (busy-wait loops, software timers) adds complexity, hides the true hardware capability from the caller, and creates blocking code paths. If the caller needs a higher-level abstraction (e.g., "send a complete break signal"), it can build that on top of `SET_BREAK` / `CLEAR_BREAK` using application-level timers.

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
