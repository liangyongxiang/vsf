---
name: vsf-hal-usart
description: |
  USE FOR: porting USART peripheral to VSF HAL, adding a new USART driver for a chip.
  DO NOT USE FOR: build/flash (use build-firmware), non-USART drivers.
---

# VSF HAL USART Driver Porting

Copy `source/hal/driver/template/__series_name_a__/common/usart/usart.{h,c}`, then modify. For existing drivers, align header style with template.

## Workflow

**1. Header** (`chip/uart/uart.h`):
- Direct: set `REIMPLEMENT_TYPE_*=ENABLED`, define mode/irq/status/cfg, include `vsf_template_usart.h`.
- IPCore: include IPCore header (provides types directly).

> `#define VSF_USART_IRQ_MASK_<X> VSF_USART_IRQ_MASK_<X>` for non-mandatory bits (RX_TIMEOUT, CTS, FRAME_ERR, BREAK_ERR, PARITY_ERR, RX_OVERFLOW_ERR, RX_IDLE).

**2. Source** (`chip/uart/uart.c`):
- Direct: struct `.reg`+`.isr`, implement all APIs manually.
- IPCore: struct `implement(vsf_pl011_usart_t)`. IPCore handles types/baudrate/reg/IRQ dispatch. Chip only: reset, NVIC, clock. Set `__VSF_HAL_${IP}_USART_CLASS_INHERIT__` before `#include "hal/vsf_hal.h"`.

Both: DMA stubbable. Instantiate via `IMP_LV0` + `usart_template.inc`.

**3. Board** (`vsf_board.c`): Expose `vsf_usart_t *usart[N]`. Init: pinmux → reset → init → enable → irq.

**4. Application** (`main.c`): Pure VSF APIs only.

## Validation

`scripts/check-usart-*.py chip/uart/uart.{h,c}` (exit: 0=pass, 1=errors, 2=warnings).

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| No output | Deassert reset in `_init()` |
| Garbage | Wrong baudrate formula |
| Spurious IRQ | Check mask before FIFO read |
| RX not firing | Use `RX_FIFO_THRESHOLD_NOT_EMPTY` |
| Stream hangs | Drain RX FIFO when full |
