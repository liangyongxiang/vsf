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
check-driver-structure.py --periph usart --side header  source/hal/driver/MyChip/uart/uart.h
check-driver-structure.py --periph usart --side source  source/hal/driver/MyChip/uart/uart.c
vsf-bench --all board/pico/hardware-map.yml --scene usart_baud
```

**Audit a port for wiring gaps:**
```bash
audit-port.py --chip RaspberryPi/RP2040
```

See `REFERENCE.md` for conventions (parameterization, includes, stubs, register reads, non-blocking) and `PORTING.md` for the full ladder.

## Script usage guide

### When to use each script

| Script | Use when... | Input | Exit 0 means |
|--------|------------|-------|--------------|
| `scaffold_chip.py` | Starting a brand-new chip port | YAML chip config | Skeleton directory created |
| `scaffold_peripheral.py` | Adding a peripheral to an existing chip | `--periph <name> --chip Vendor/Chip` | Template files copied and renamed |
| `generate-device-peripheral-macros.py` | Adding/editing peripheral instances in device.h | YAML instance map | Macros written to device.h zone |
| `check-driver-structure.py` | Verifying a driver file is structurally complete | `--periph <name> --side header\|source <file>` | All mandatory checks pass |
| `check-driver-quality.py` | Checking for anti-patterns (hardcoded instances, pinmux-in-driver, etc.) | One or more `.c`/`.h` files | Zero quality findings |
| `audit-port.py` | Cross-file consistency check across device.h, vsf_usr_cfg.h, driver files | `--chip Vendor/Chip` | No wiring gaps found |
| `enable-periph.py` | Toggling `VSF_HAL_USE_*` in vsf_usr_cfg.h | `--enable usart,spi --disable i2c` | All requested toggles applied |
| `vsf-bench` | Build + flash + run test scenes on hardware | hardware-map.yml | All scenes pass |

### Typical workflow order

```
1. scaffold_chip.py           ← once per chip (R1)
2. generate-device-peripheral-macros.py  ← edit device.h (R1)
3. scaffold_peripheral.py     ← one per peripheral (R3a/R3b/R5)
4. [edit driver .c / .h]      ← LLM: implement register logic
5. enable-periph.py           ← enable the peripheral (R2)
6. check-driver-structure.py  ← verify structural completeness
7. check-driver-quality.py    ← verify no anti-patterns
8. audit-port.py              ← verify cross-file consistency
9. vsf-bench                  ← build+flash+test on hardware
```

**Structure vs quality checkers** — `check-driver-structure.py` verifies the driver *has the right shape* (guard macros present, all mandatory APIs exist, template includes correct). `check-driver-quality.py` verifies the driver *doesn't have the wrong content* (no hardcoded instances, no pinmux-in-driver, no bare IRQ names). Run structure first (cheap, catches mechanical omissions), then quality.

**Peripheral specs** for the structure checker live in `scripts/check-specs/<periph>.yml`. Adding support for a new peripheral = adding a YAML file, not writing Python.

## Template locations

`source/hal/driver/template/__series_name_a__/common/<periph>/` — reference: `RaspberryPi/RP2040/`.

## Examples

**New chip USART (IPCore):** `scaffold_chip.py` → copy uart template → `implement(vsf_pl011_usart_t)` → `IMP_LV0` → board.c pinmux → verify with `check-driver-structure.py` + `vsf-bench`.

**Fix bug:** reproduce → compare with template + working reference driver.

## Troubleshooting

| Symptom | Fix |
|---|---|
| No output after init | Deassert reset in `init()`, verify `IMP_LV0` IRQ wiring. |
| Check script flags anti-pattern | Fix or suppress with `// quality: allow-<rule-id>`. |
| `check-driver-structure.py` reports missing API | Add the API stub with `VSF_HAL_ASSERT(0); return VSF_ERR_NOT_SUPPORT;` per REFERENCE.md. |
| `audit-port.py` reports enable gap | Run `enable-periph.py --enable <periph>`. |
