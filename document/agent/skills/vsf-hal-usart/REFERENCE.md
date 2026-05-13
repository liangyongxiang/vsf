# VSF HAL USART — Reference

## API function signatures (from `vsf_template_usart.h`)

All generated via `VSF_USART_APIS(__prefix_name)` macro:

| Function | Signature | Notes |
|----------|-----------|-------|
| `init` | `vsf_err_t (*)(t *p, vsf_usart_cfg_t *c)` | Configure baudrate, mode, FIFO, ISR, NVIC |
| `fini` | `void (*)(t *p)` | Disable peripheral |
| `get_configuration` | `vsf_err_t (*)(t *p, vsf_usart_cfg_t *c)` | Read back current config |
| `enable` | `fsm_rt_t (*)(t *p)` | Return `fsm_rt_cpl` when done |
| `disable` | `fsm_rt_t (*)(t *p)` | Return `fsm_rt_cpl` when done |
| `capability` | `vsf_usart_capability_t (*)(t *p)` | Report HW limits |
| `irq_enable` | `void (*)(t *p, vsf_usart_irq_mask_t m)` | Set interrupt mask |
| `irq_disable` | `void (*)(t *p, vsf_usart_irq_mask_t m)` | Clear interrupt mask |
| `irq_clear` | `vsf_usart_irq_mask_t (*)(t *p, vsf_usart_irq_mask_t m)` | Clear and return pending status |
| `status` | `vsf_usart_status_t (*)(t *p)` | Read status reg |
| `rxfifo_get_data_count` | `uint_fast32_t (*)(t *p)` | Return available RX data count |
| `rxfifo_read` | `uint_fast32_t (*)(t *p, void *b, uint_fast32_t c)` | Read from RX FIFO, return actual count |
| `txfifo_get_free_count` | `uint_fast32_t (*)(t *p)` | Return free space in TX FIFO |
| `txfifo_write` | `uint_fast32_t (*)(t *p, void *b, uint_fast32_t c)` | Write to TX FIFO, return actual count |
| `request_rx` | `vsf_err_t (*)(t *p, void *b, uint_fast32_t c)` | DMA-based RX (can be stubbed) |
| `request_tx` | `vsf_err_t (*)(t *p, void *b, uint_fast32_t c)` | DMA-based TX (can be stubbed) |
| `cancel_rx` | `vsf_err_t (*)(t *p)` | Cancel pending RX |
| `cancel_tx` | `vsf_err_t (*)(t *p)` | Cancel pending TX |
| `get_rx_count` | `int_fast32_t (*)(t *p)` | Return DMA RX progress |
| `get_tx_count` | `int_fast32_t (*)(t *p)` | Return DMA TX progress |
| `ctrl` | `vsf_err_t (*)(t *p, vsf_usart_ctrl_t c, void *param)` | Control command |

## Mode bits field layout (from template `vsf_usart_mode_t`)

| Field | Bits | Mandatory | Notes |
|-------|------|-----------|-------|
| Parity | 0-2 | NO, EVEN, ODD | FORCE_0/1 optional |
| Stop bits | 3-4 | 1_STOPBIT | 1.5, 0.5, 2 optional |
| Data bits | 5-7 | 8_BIT_LENGTH | 5/6/7/9/10 optional |
| HW control | 8 | NO_HWCONTROL | RTS/CTS optional |
| TX enable | 9 | TX_ENABLE | TX_DISABLE optional |
| RX enable | 10 | RX_ENABLE | RX_DISABLE optional |
| Sync clock | 11 | SYNC_CLOCK_DISABLE | SYNC_CLOCK_ENABLE optional |
| Half duplex | 14 | HALF_DUPLEX_DISABLE | HALF_DUPLEX_ENABLE optional |
| TX FIFO threshold | 15-16 | THRESHOLD_EMPTY | HALF_EMPTY, NOT_FULL optional |
| RX FIFO threshold | 17-18 | THRESHOLD_NOT_EMPTY | HALF_FULL, FULL optional |

When reimplementing mode, match each field to the corresponding register bits of your hardware.

## IRQ mask bits (from template `vsf_usart_irq_mask_t`)

| Bit | Name | Mandatory | #define required |
|-----|------|-----------|-----------------|
| 0 | TX_CPL (DMA) | Yes | No (template default) |
| 1 | RX_CPL (DMA) | Yes | No (template default) |
| 2 | TX (FIFO threshold) | Yes | No (template default) |
| 3 | RX (FIFO threshold) | Yes | No (template default) |
| 4 | RX_TIMEOUT | If supported | **Yes** |
| 5 | CTS | If supported | **Yes** |
| 6 | FRAME_ERR | If supported | **Yes** |
| 7 | BREAK_ERR | If supported | **Yes** |
| 8 | PARITY_ERR | If supported | **Yes** |
| 9 | RX_OVERFLOW_ERR | If supported | **Yes** |
| 12 | RX_IDLE | If supported | **Yes** |

**Without the `#define VSF_USART_IRQ_MASK_<X> VSF_USART_IRQ_MASK_<X>` macro, VSF treats the bit as unsupported** and may map it differently or ignore it entirely.

## Mandatory mode bits (must be present even if HW doesn't support)

These enum values must exist for build compatibility even when the hardware has no corresponding feature:

```c
VSF_USART_9_BIT_LENGTH  = (0x1ul << 24),  // won't match HW, just placeholder
VSF_USART_1_5_STOPBIT   = (0x1ul << 25),
VSF_USART_0_5_STOPBIT   = (0x1ul << 26),
VSF_USART_10_BIT_LENGTH = (0x1ul << 27),
VSF_USART_SYNC_CLOCK_ENABLE = (0x1ul << 28),
VSF_USART_SYNC_CLOCK_DISABLE = 0,
VSF_USART_HALF_DUPLEX_ENABLE = (0x1ul << 29),
VSF_USART_HALF_DUPLEX_DISABLE = 0,
```

## IRQ handler pattern

```c
void VSF_HW_USART<N>_IRQHandler(void)
{
    uintptr_t ctx = vsf_hal_irq_enter();
    // 1. Read raw interrupt status from HW
    // 2. Read enabled interrupt mask
    // 3. Compute: active_irqs = raw_status & enabled_mask
    // 4. Clear active interrupts (write-1-to-clear or write-0-to-clear as HW requires)
    // 5. If (active_irqs != 0) && (isr.handler_fn != NULL):
    //       isr.handler_fn(isr.target_ptr, (vsf_usart_t *)usart_ptr, active_irqs);
    vsf_hal_irq_leave(ctx);
}
```

## Template files index

| File | Purpose |
|------|---------|
| `source/hal/driver/common/template/vsf_template_usart.h` | API declarations, default types, mode/IRQ enums |
| `source/hal/driver/template/__series_name_a__/common/usart/usart.h` | Header skeleton with reimplementation macros |
| `source/hal/driver/template/__series_name_a__/common/usart/usart.c` | Implementation skeleton |
| `source/hal/driver/common/usart/usart_template.inc` | Instance instantiation via `VSF_USART_CFG_IMP_LV0` |
| `source/hal/driver/common/usart/usart_interface.c` | Multi-class dispatch layer |
| `source/hal/driver/common/usart/usart_common.c` | Default implementations (capability, ctrl, etc.) |

## Reference implementations

- **RP2040 (PL011 IPCore)**: `source/hal/driver/RaspberryPi/RP2040/uart/uart.c` — HW layer using IPCore
- **PL011 IPCore**: `source/hal/driver/IPCore/ARM/PL011/vsf_pl011_uart.h` and `.c` — register-level driver
- **Template (direct)**: `source/hal/driver/template/__series_name_a__/common/usart/usart.c` — skeleton for direct reg access

Further examples:
- `source/hal/driver/ST/STM32H7RSXX/common/usart/usart.c`
- `source/hal/driver/GigaDevice/GD32H7XX/common/usart/usart.c`
