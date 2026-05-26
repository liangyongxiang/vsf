---
name: vsf-hal-driver
description: |
  **UTILITY SKILL** — delegates build/flash/test to vsf-bench skill.
  Create, implement, audit, or debug VSF HAL LV0 drivers. Handles register-level bugs inside driver `.c` files (wrong register writes, missing clock gates, IRQ handler errors — not application crashes).

  USE FOR:
  - Full chip port: vendor UART echo → VSF skeleton → test framework → peripheral → clock verify → full suite
  - Adding peripherals to existing chip ports
  - Debugging LV0 driver register/IRQ/clock/DMA bugs

  DO NOT USE FOR:
  - Build, flash, or test only (use vsf-bench — even if driver code was changed)
  - Pinmux-only changes to vsf_board.c with no driver code written
  - LV1/LV2 driver layers or framework wrappers (use diagnose skill for layer isolation)
  - Crashes where a bug ABOVE LV0 (application or LV1 glue) passes bad data to a driver — even if the crash site is inside a driver `.c` file, the root cause is not an LV0 register-level bug
metadata:
  version: "1.0"
  license: Apache-2.0
---

# VSF HAL Driver

## Quickstart

### New chip port (full flow, 6 rungs)

| Rung | Goal | How | Verify |
|------|------|-----|--------|
| R0 | Vendor UART echo on wired pin pair | Build & flash vendor SDK UART example; wire chip TX/RX to host USB-serial adapter | Host byte echoes within ~100ms |
| R1 | VSF skeleton compiles | `scaffold_chip.py --config chip.yaml` (chip.yaml: `vendor`, `chip`, `cpu` fields); generate instance macros; implement `driver.c` clock setup + watchdog tick + IMP_LV0 | `cmake --build` succeeds; `printf` after `vsf_driver_init()` prints |
| R2 | Test framework shell over serial | Route debug stream to UART in `vsf_board.c`; enable `VSF_USE_TEST`; flash | `vsf-test scene --list` responds over serial |
| R3 | First VSF peripheral | Run scaffold → implement .c/.h → static checks → audit → vsf-bench (see "Add peripheral" below) | vsf-bench scenario passes |
| R4 | System clock verified | Toggle GPIO at systimer-derived rate (e.g., 100ms); wire GPIO to logic analyzer; run scenario | LA measures all gaps within ±5% of expected |
| R5 | Remaining peripherals one-by-one | For each: read `peripherals/<name>.md` → scaffold → implement → checks → audit → bench | Each peripheral's vsf-bench scenario passes |

Do not skip rungs. Each rung assumes earlier rungs hold — advancing with a broken earlier rung wastes debugging time. All rungs must pass. For detailed steps per rung see `PORTING.md`.

### Add peripheral to existing chip

1. `scripts/scaffold_peripheral.py --driver-dir source/hal/driver --chip Vendor/Chip --periph <name>` — copies template .c/.h
2. Implement register operations in .c/.h; use `VSF_MCONNECT` for instance prefixing, never hardcode instance names
3. Add IMP_LV0 invocation per instance (reg, irq, rst_bit fields from device.h macros)
4. Add pinmux to `board/<board>/vsf_board.c` using `vsf_gpio_port_config_pins()` — not raw register writes
5. Enable peripheral: `scripts/enable-periph.py --enable <periph> <vsf_usr_cfg.h>`
6. Static checks (see Concepts for exit code rules):
   - `scripts/check-driver-structure.py --periph <name> --side header <file.h>`
   - `scripts/check-driver-structure.py --periph <name> --side source <file.c>`
   - `scripts/check-driver-quality.py <file.c>`
7. Cross-file audit: `scripts/audit-port.py --chip Vendor/Chip`
8. Verify: `vsf-bench --all hardware-map.yml --suite <periph>_<scenario>`

### Audit existing driver

`scripts/audit-port.py --chip Vendor/Chip` → lists cross-file inconsistencies → fix each → re-run until exit 0 or 2.

## Concepts

- **LV0:** register-level driver — reads/writes hardware registers directly. LV1 (framework wrappers) and LV2 (application APIs) are out of scope. **Boundary note:** a crash inside the driver `.c` file is an LV0 bug (in scope); a crash in LV1 glue code that calls the driver incorrectly, even if the symptom appears as "driver returns error", is an LV1 bug (out of scope — use the `diagnose` skill or `vsf-bench` to isolate the layer).
- **IMP_LV0:** macro that expands into per-instance `struct` definitions and IRQ handler stubs, driven by macros in `device.h`.
- **VSF_MCONNECT:** token-paste macro `VSF_MCONNECT(prefix, suffix, __IDX)` → `prefix##__IDX##suffix` for building per-instance names.
- **Complete driver checklist:** `device.h` instance macros + `.h` API header + `.c` implementation + `IMP_LV0` block + `vsf_board.c` pinmux + `vsf_usr_cfg.h` enable flag.
- **Exit code semantics (all scripts):** exit 0 = pass; exit 2 = all findings are known-acceptable warnings (review and proceed); any other exit = errors that must be fixed. Applies to `check-driver-structure.py`, `check-driver-quality.py`, and `audit-port.py`.

## Conventions (enforced by `scripts/check-driver-quality.py`)

1. **No hardcoded instances:** per-instance values (reg base, IRQ number, clock bit, reset bit) must come from `device.h` macros expanded via `VSF_MCONNECT(..., __IDX)` in the IMP_LV0 block. Never write `vsf_hw_uart0` directly in `.c` files.
2. **Spin-wait annotated:** every `while (reg->flag);` must have a preceding `// < X us` comment with the expected upper-bound duration. Enforced by quality checker.
3. **No pinmux in driver:** GPIO function selection belongs in `vsf_board.c`, never in the peripheral driver `.c` file.
4. **Unimplemented APIs:** return `VSF_ERR_NOT_SUPPORT` with `VSF_HAL_ASSERT(0)`. Never emulate missing hardware features in software.
5. **IRQ in init():** if the peripheral supports interrupts, `init()` must set priority from `cfg_ptr->prio` (`NVIC_SetPriority(irqn, cfg_ptr->prio)`) **before** enabling the IRQ (`NVIC_EnableIRQ(irqn)`). Document if priority is not configurable on this chip. Also required: reset deassert + clock gate enable. Missing any one = driver compiles but produces no I/O.
6. **IRQ in fini():** `fini()` must disable NVIC IRQ (`NVIC_DisableIRQ(irqn)`) before aborting DMA, clearing peripheral interrupt enable bits, and releasing resources. The disable order matters: NVIC first to prevent new IRQ pends, then peripheral-level cleanup.
7. **Config fields consumed or documented:** every field in `vsf_<periph>_cfg_t` passed to `init()` must be either read/applied, or documented with `// field_name intentionally unused: <reason>` above the struct store. Never silently ignore config fields — silent ignores leave future readers guessing whether the omission is a bug or a hardware limitation.
8. **Mode/config bits map hardware registers:** for any peripheral where the chip's register fields naturally align with VSF config values (e.g., GPIO MODER/OTYPER/PUPDR, USART baud/mode bits, I2C speed modes, SPI frame formats), reimplement the corresponding enum/type so that values encode register bits directly. Driver code then extracts fields with shifts and masks instead of long `if/else` translation functions. This applies to all peripherals — GPIO is just the most common example. When hardware layout prevents perfect bit-to-bit mapping, minimize conversion to the unavoidable cases only.
9. **No debug logging in final driver:** `vsf_trace_info`, `printf`, and other diagnostic output are acceptable during bring-up, but must be removed before the driver is considered complete. Logging bloats firmware size, slows critical paths, and pollutes test output. Strip all trace calls after the peripheral passes vsf-bench.
10. **NVIC and peripheral IRQ separation:** `_<periph>_irq_disable()` must **only** clear peripheral-level interrupt enable bits (e.g., `reg->IER &= ~mask`). It must **never** call `NVIC_DisableIRQ()` — that belongs in `fini()`. Rationale: irq_disable/irq_enable are paired APIs; if irq_disable also disables NVIC, a subsequent irq_enable cannot receive interrupts because NVIC is still off. This applies to all peripherals.
11. **Init without ISR handler → disable IRQs:** if `init()` is called with `cfg_ptr->isr.handler_fn == NULL`, the driver must explicitly ensure the peripheral's interrupts are disabled (clear peripheral-level IRQ enable bits, do not touch NVIC). Do not leave IRQs enabled from a previous configuration or reset default. This prevents spurious interrupts when the user only wants polled mode.
12. **Invalid frequency → return error:** if `cfg_ptr->clock_hz` (or `freq`) is 0 or otherwise out of range, return `VSF_ERR_INVALID_PARAMETER`. Do not silently substitute a default frequency (e.g., `if (freq == 0) freq = 1000;`) — the caller should receive immediate feedback that the configuration is invalid.
13. **No magic numbers:** use named macros or `device.h` constants for all hardcoded numeric values (instance counts, register bit positions, timeout limits, etc.). `VSF_HAL_ASSERT(channel < 2)` is a bug — it should be `VSF_HAL_ASSERT(channel < VSF_HW_<PERIPH>_CHANNEL_COUNT)` or similar.
14. **`driver.h` includes chip-specific peripheral headers:** the chip-level `driver.h` (e.g., `source/hal/driver/Vendor/Chip/driver.h`) must include every chip-specific peripheral header (e.g., `gpio/gpio.h`, `uart/uart.h`, `i2c/i2c.h`) at the top of the file, before the `#if VSF_HAL_USE_<PERIPH>` template blocks. This makes chip-specific constants and reimplemented types (such as `VSF_GPIO_AF`, `GPIO_FUNC_UART`) visible to board code and other consumers that include `driver.h`. Do not rely on the template blocks to bring in these definitions — the templates are conditional and may be disabled.

## Examples

### Silent peripheral — init() compiles but no I/O on any pin

**Symptom:** `vsf_hw_<periph>_init()` returns `VSF_ERR_NONE` and the firmware boots, but the peripheral produces no output (TX edge, clock, data — nothing). Logic analyzer shows pins staying at idle level.

**Diagnosis:** Check `init()` for all three required steps:
- Reset deassert: `reset_hw->reset &= ~rst_bit;` (deassert the peripheral reset line)
- Clock gate: `clock_hw->enable |= clk_bit;` (enable the peripheral clock)
- IRQ enable: `NVIC_EnableIRQ(irqn);` and set priority from `cfg_ptr->prio` if configurable

Missing any one = no I/O. This is the most common LV0 driver bug across all peripherals.

### Adding a new peripheral to an existing chip port

Follow the "Add peripheral" flow in Quickstart. This example focuses on what can go wrong and how to catch it early:

- **Step 1-2 (scaffold/implement):** verify the template copied into `source/hal/driver/<Vendor>/<Chip>/<periph>/`. If the directory already exists, scaffold fails — edit directly.
- **Step 5 (static checks):** structure check catches missing API functions; quality check catches convention violations. Fix these before audit.
- **Step 6 (audit):** cross-file check catches IRQ handler declared in `device.h` but not defined in `.c`, or vice versa.
- **Step 7 (vsf-bench):** only run after all static checks + audit pass. If vsf-bench fails, re-run `gpio_io_check` to rule out wiring before suspecting driver logic.

### New chip port — common pitfalls per rung

Follow the 6-rung ladder in Quickstart. Typical failures:
- R1: forgetting watchdog tick or PLL config → `vsf_systimer_get_us()` returns 0 or drifts
- R2: debug stream routed to wrong serial instance → no shell prompt
- R3: pinmux via raw vendor registers instead of `vsf_port_config_pins()` → driver works only by accident
- R4: timer running at wrong frequency → 100× timing errors caught by LA tolerance check

### Unused config struct fields must be documented

**Symptom:** `vsf_<periph>_cfg_t` has a field (e.g., `prio`) that the chip hardware cannot configure. The `init()` function stores the whole struct but never reads that field — with no comment explaining why. A future maintainer cannot tell whether the omission is a driver bug or intentional.

**Fix:** Document each unused field directly above the struct store in `init()`:
```c
// cfg_ptr->prio intentionally unused: <chip> IRQ priority is fixed in hardware
dma_ptr->cfg = *cfg_ptr;
```
Applies to every peripheral type. For every config field: either use it in the driver, or document why the chip hardware doesn't support it.

## Error Handling and Troubleshooting

### Script failures

| Failure | Cause | Fix |
|---------|-------|-----|
| `scaffold_peripheral.py` fails | wrong `--chip` path or target dir already exists | verify path under `source/hal/driver/`; if dir exists, edit directly |
| `check-driver-structure.py` non-zero | missing required API, wrong prototype, or missing IMP_LV0 | read check output; add missing function/struct; rerun |
| `check-driver-quality.py` non-zero | style or convention violation | fix the violation; only suppress with `// quality: allow-<rule-id>` after confirming it's a false positive |
| `audit-port.py` non-zero | cross-file mismatch (e.g., IRQ handler declared but not defined) | fix mismatch; rerun |
| `enable-periph.py` fails | peripheral name typo or `vsf_usr_cfg.h` not at expected path | check peripheral name against `peripheral-registry.yml` |

### Runtime failures

| Symptom | Likely cause | Action |
|---------|-------------|--------|
| Boot hang / no shell prompt | `vsf_driver_init()` crash — clock setup or NULL deref | add printf after each init step; check PLL lock |
| Compiles, no I/O on any pin | system timer not running (watchdog tick missing) | verify `watchdog_hw->tick = N \| WATCHDOG_TICK_ENABLE_BITS` in `driver.c` |
| Single peripheral: compiles, no I/O | init() missing reset, clock, or IRQ step | check all three (see Example: Silent peripheral) |
| IRQ never fires | NVIC enable missing in init(), or IRQ handler name doesn't match IMP_LV0 expansion | verify `NVIC_EnableIRQ()` called; check handler name against generated macro |
| DMA transfer never completes | DMA clock not enabled, or channel not assigned to peripheral | check `RCC->AHBENR` DMA clock bit; verify channel mapping in reference manual |
| vsf-bench fails, all static checks passed | wiring issue or wrong baudrate | run `gpio_io_check` suite first to isolate wiring; check R4 system clock timing |
| Peripheral works intermittently | missing `volatile` on register pointers, or spin-wait missing `< X us` comment (compiler optimizes away delay) | add volatile; add duration comment |

### Iteration loop

```
edit .c/.h → structure check → quality check → audit → vsf-bench
                                                          ↑
                                          └── fix ────────┘
```

Stop when vsf-bench passes. If static checks keep failing after several iterations, pause and re-read `conventions.md` — repeated failures usually mean a structural rule is being violated, not a typo. If the same failure persists across 5+ iterations: stop and tell the user — the issue is likely a tooling bug, an undocumented hardware quirk, or a misunderstanding of the convention rules that needs human clarification.

### When tools or documents are unavailable

- `vsf-bench` not installed: `pip install -e vsf.demo/vsf/test/vsf_bench`
- `scripts/` not on PATH: invoke with full path under `vsf.demo/vsf/document/agent/skills/vsf-hal-driver/scripts/`
- `PORTING.md` / `REFERENCE.md` / `conventions.md` not accessible: all are in the same directory as this SKILL.md
- `peripheral-registry.yml` missing: read `scripts/check-specs/<periph>.yml` for per-peripheral API specs
- **No hardware available:** static checks (structure + quality + audit) can still verify correctness. If all exit 0 or 2, the code is structurally sound — flag to user that hardware testing is pending. This skill hands off to vsf-bench: use `Skill("vsf-bench")` to invoke it, or tell the user to run `vsf-bench --all hardware-map.yml --suite <periph>_<scenario>`.
- Undocumented vendor registers: flag to user; this skill cannot authoritatively infer NDA-only register behavior
