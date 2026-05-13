---
name: vsf-hal-usart
description: |
  USE FOR: porting USART peripheral to VSF HAL, adding a new USART driver for a chip.
  DO NOT USE FOR: build/flash (use build-firmware), non-USART drivers.
---

# VSF HAL USART Driver Porting

Start by copying the templates:
`source/hal/driver/template/__series_name_a__/common/usart/usart.h` and
`usart.c` into your chip directory, then modify.

For existing drivers, also align the header style with the template.

Layers: **Driver** → **Board** → **Application**.

## Workflow

**1. Header** (`chip/uart/uart.h`): Enable `VSF_USART_CFG_REIMPLEMENT_TYPE_* = ENABLED`. Define mode/irq/status/cfg types matching HW regs. Include `vsf_template_usart.h`.

> `#define VSF_USART_IRQ_MASK_<X> VSF_USART_IRQ_MASK_<X>` required for non-mandatory bits (RX_TIMEOUT, CTS, FRAME_ERR, BREAK_ERR, PARITY_ERR, RX_OVERFLOW_ERR, RX_IDLE).

**2. Source** (`chip/uart/uart.c`): Struct `.reg`+`.isr` (direct) or `implement(vsf_pl011_usart_t)` (IPCore). Implement init/fini/enable/disable/irq/status/fifo. DMA stubbable. Instantiate via `VSF_USART_CFG_IMP_LV0` + `usart_template.inc`.

**3. Board** (`vsf_board.c`): Expose `vsf_usart_t *usart[N]`. Init: pinmux → reset → init → enable → irq.

**4. Application** (`main.c`): Pure VSF APIs only.

## Validation

`scripts/check-usart-header.py chip/uart/uart.h` and
`scripts/check-usart-source.py chip/uart/uart.c` (exit: 0=pass, 1=errors, 2=warnings).

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| No output | Deassert reset in `_init()` |
| Garbage data | Wrong baudrate formula |
| Spurious IRQ | Check mask before FIFO read |
| RX not firing | Use `RX_FIFO_THRESHOLD_NOT_EMPTY` |
| Stream hangs | Drain RX FIFO when buffer full |
