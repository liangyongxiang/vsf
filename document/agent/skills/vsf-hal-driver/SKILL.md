---
name: vsf-hal-driver
description: |
  **UTILITY SKILL** — INVOKES: vsf-bench (for verification).
  USE FOR: creating, implementing, modifying, or debugging VSF HAL drivers (UART, I2C, SPI, GPIO, etc.).
  DO NOT USE FOR: build/flash/test (use vsf-bench), BSP-only pinmux changes.
  FOR SINGLE OPERATIONS: prefer direct edits over full template copy if driver already exists.
---

# VSF HAL Driver

## Quickstart

**New chip:** follow `PORTING.md` R0→R5 sequentially.

**Add peripheral:**
```bash
check-driver-quality.py source/hal/driver/MyChip/uart/uart.c
vsf-bench --all board/pico/hardware-map.yml --scene usart_baud
```

See `REFERENCE.md` for conventions (parameterization, includes, stubs, register reads, non-blocking) and `PORTING.md` for the full ladder.

## Template locations

`source/hal/driver/template/__series_name_a__/common/<periph>/` — reference: `RaspberryPi/RP2040/`.

## Examples

**New chip USART (IPCore):** `scaffold_chip.py` → copy uart template → `implement(vsf_pl011_usart_t)` → `IMP_LV0` → board.c pinmux → verify with `vsf-bench`.

**Fix bug:** reproduce → compare with template + working reference driver.

## Troubleshooting

| Symptom | Fix |
|---|---|
| No output after init | Deassert reset in `init()`, verify `IMP_LV0` IRQ wiring. |
| Check script flags anti-pattern | Fix or suppress with `// quality: allow-<rule-id>`. |
