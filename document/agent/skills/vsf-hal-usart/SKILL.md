---
name: vsf-hal-usart
description: |
  USE FOR: porting USART peripheral to VSF HAL, adding a new USART driver for a chip.
  DO NOT USE FOR: build/flash (use vsf-build-firmware), non-USART drivers.
---

# VSF HAL USART Driver Porting

Copy `source/hal/driver/template/__series_name_a__/common/usart/usart.{h,c}`, then modify (existing: align header style).

## Workflow

**1. Header** (`chip/uart/uart.h`): Direct: `REIMPLEMENT_TYPE_*=ENABLED`, mode/irq/status/cfg, include `vsf_template_usart.h`. IPCore: include IPCore header (types built-in).

**2. Source** (`chip/uart/uart.c`): Direct: struct `.reg+.isr`, all APIs. IPCore: `implement(vsf_pl011_usart_t)`, IPCore handles baudrate/reg/IRQ, chip needs reset/NVIC/clock. Set `__VSF_HAL_${IP}_USART_CLASS_INHERIT__`.

Both instantiate with IMP_LV0:
```c
#define VSF_USART_CFG_IMP_LV0(ID, OP)                     \
    vsf_hw_usart_t vsf_hw_usart##ID = {.reg = REG, OP};   \
    void VSF_HW_USART##ID##_IRQHandler(void) {            \
        vsf_pl011_usart_irqhandler(&vsf_hw_usart##ID.use);\
    }
#include "hal/driver/common/usart/usart_template.inc"
```

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
