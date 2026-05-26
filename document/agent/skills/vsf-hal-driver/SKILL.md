---
name: vsf-hal-driver
description: Create, implement, audit, and debug VSF HAL LV0 drivers. Handles register-level bugs in `.c` files (clock gates, IRQ handlers, register writes). Delegates build/flash/test to vsf-bench.
metadata:
  version: "1.0"
  license: Apache-2.0
---

**UTILITY SKILL** — delegates build/flash/test to vsf-bench skill.

USE FOR:
- Full chip port: vendor UART echo → VSF skeleton → test framework → peripheral → clock verify → full suite
- Adding peripherals to existing chip ports
- Debugging LV0 driver register/IRQ/clock/DMA bugs

DO NOT USE FOR:
- Build, flash, or test only (use vsf-bench)
- Pinmux-only changes with no driver code written
- LV1/LV2 layers or framework wrappers (use diagnose skill)
- Crashes where a bug ABOVE LV0 passes bad data to a driver

## Quickstart

### New chip port (6 rungs)

| Rung | Goal | Verify |
|------|------|--------|
| R0 | Vendor UART echo on wired pins | Host byte echoes within ~100ms |
| R1 | VSF skeleton compiles | `cmake --build` passes; printf after `vsf_driver_init()` works |
| R2 | Test framework shell over serial | `vsf-test scene --list` responds over serial |
| R3 | First VSF peripheral | vsf-bench scenario passes |
| R4 | System clock verified | Logic analyzer measures gaps within ±5% of expected |
| R5 | Remaining peripherals one-by-one | Each peripheral's vsf-bench scenario passes |

Do not skip rungs. Each rung assumes earlier rungs hold. Detailed per-rung steps: [porting](modules/porting.md).

### Add peripheral

1. Scaffold: `scripts/scaffold/peripheral.py --driver-dir source/hal/driver --chip Vendor/Chip --periph <name>`
2. Implement register operations in .c/.h; use `VSF_MCONNECT(..., __IDX)` for instance prefixing — `VSF_MCONNECT` is a token-paste macro that builds per-instance names from `device.h` macros; never hardcode instance names like `vsf_hw_uart0`
3. Add `IMP_LV0` invocation per instance — `IMP_LV0` expands per-instance structs and IRQ handlers from `device.h` macros (reg base, IRQn, rst_bit, clk_bit)
4. Add pinmux in `board/<board>/vsf_board.c` using `vsf_gpio_port_config_pins()` — not raw register writes
5. Enable: `scripts/util/enable.py --enable <periph> <vsf_usr_cfg.h>`
6. Skeleton check: `scripts/check/skeleton.py <template.c> <driver.c>` — verify function signatures and structs match the template
7. Static: `scripts/check/structure.py --periph <name> --side header <file.h>` and `--side source <file.c>`; `scripts/check/quality.py <file.c>`
8. Audit: `scripts/check/audit.py --chip Vendor/Chip` — cross-file consistency check
9. Verify: `vsf-bench --all hardware-map.yml --suite <periph>_<scenario>`

### Debug driver

When a driver compiles but produces no I/O (TX, clock, data — nothing on logic analyzer):

1. Check `init()` for the three required steps:
   - Reset deassert: `reset_hw->reset &= ~rst_bit;`
   - Clock gate: `clock_hw->enable |= clk_bit;`
   - NVIC enable: `NVIC_EnableIRQ(irqn);` with priority from `cfg_ptr->prio`
2. Check IRQ handler name matches the IMP_LV0 expansion — mismatch = ISR never fires
3. Check `volatile` on register pointers; spin-wait loops need `// < X us` annotation
4. Check DMA: `RCC->AHBENR` DMA clock bit enabled; channel mapped to peripheral per reference manual
5. Check `watchdog_hw->tick = N | WATCHDOG_TICK_ENABLE_BITS` in `driver.c` — missing this makes systimer run at wrong frequency, causing 100× timing errors caught by logic analyzer
6. Run `gpio_io_check` suite first to rule out wiring before suspecting driver logic

## Example: Silent peripheral

**Symptom:** `vsf_hw_<periph>_init()` returns `VSF_ERR_NONE`, firmware boots, but peripheral produces no output. Logic analyzer shows pins at idle level.

**Diagnosis:** `init()` is missing one of three required steps:
- Reset deassert: `reset_hw->reset &= ~rst_bit;`
- Clock gate: `clock_hw->enable |= clk_bit;`
- NVIC enable: `NVIC_EnableIRQ(irqn);` and set priority from `cfg_ptr->prio`

Missing any one = no I/O. This is the most common LV0 driver bug across all peripherals.

**Fix:** Add the missing step to `init()`. After fix, re-run `vsf-bench --all hardware-map.yml --suite <periph>_<scenario>`.

## Reference

- [concepts](modules/concepts.md) — LV0, IMP_LV0, VSF_MCONNECT, reimplement-type macros, exit codes
- [conventions](modules/conventions.md) — 15 conventions enforced by `scripts/check/quality.py`
- [examples](modules/examples.md) — Unsupported config, unused fields, mode bits via if/else
- [troubleshooting](modules/troubleshooting.md) — Script failures, runtime failures, iteration loop
- [porting](modules/porting.md) — Detailed per-rung steps for new chip port
- [reference](modules/reference.md) — Common patterns, script reference, peripheral specs
