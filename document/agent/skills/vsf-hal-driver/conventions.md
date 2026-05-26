# Conventions

## No hardcoded instances
Per-instance values (register base, IRQ, clock, reset) must come from `device.h` macros, expanded via `VSF_MCONNECT(..., __IDX)` in the IMP_LV0 macro block.

## No spin-wait without comment
Every `while (reg->flag);` loop polling a hardware register must have a preceding comment explaining why spin-wait is required and the expected upper-bound duration (`< X us`). Enforced by `check-driver-quality.py`.

## No pinmux in driver
GPIO function selection belongs in the board file, never in the peripheral driver.

## Unimplemented APIs
Return `VSF_ERR_NOT_SUPPORT` with `VSF_HAL_ASSERT(0)`. Never emulate missing hardware features in software (no busy-wait loops, no software state machines).

## IRQ enable in init()
If the peripheral supports interrupts, `init()` must:
1. Enable the NVIC IRQ line: `NVIC_EnableIRQ(irqn)`
2. Set interrupt priority from the config struct: `NVIC_SetPriority(irqn, cfg_ptr->prio)` — or document with a comment if the chip hardware does not support configurable priority

Without NVIC enable, registered ISRs will never fire even if the peripheral raises interrupt flags.

## IRQ disable in fini()
`fini()` must disable the NVIC IRQ line (`NVIC_DisableIRQ(irqn)`) **before** aborting DMA transfers, clearing peripheral interrupt enable bits, and releasing resources. The order matters: disable NVIC first to prevent new IRQ pends from racing with the teardown, then clean up the peripheral-level state.

## Clock and reset
Enable peripheral clocks before any register access. `init()` must also deassert the peripheral reset line. Missing either clock or reset = driver compiles but produces no I/O.

## Config struct fields must be consumed or documented
Every field in every `vsf_<periph>_cfg_t` config struct passed to `init()` must be either:
1. **Used** — read and applied by the driver to configure hardware registers, or
2. **Documented** — explained with a `// field_name intentionally unused: <reason>` comment directly above the struct store.

Never silently ignore config fields. If a field is inapplicable to the specific chip hardware (e.g., priority not configurable, feature not present in this chip), document why.

## Unsupported hardware features must be honest
If the chip hardware does not support a VSF HAL feature (e.g., no hardware interrupt line, no DMA, no configurable priority), the driver must **not** emulate it in software and must **not** lie in `capability()`:
- `capability()->irq_mask` must be `0` when the peripheral has no hardware interrupt.
- `irq_enable()` / `irq_disable()` must be no-ops with a comment explaining why.
- Erase/write/read completion callbacks must **not** be invoked synchronously inside the operation function — that is not an interrupt, it is a callback, and it misleads the user into thinking the peripheral supports async operation.

Document the limitation in a comment on `capability()`, `init()`, or the IRQ functions. Never let the user discover the missing feature by debugging a silent no-op or a misleading capability mask.
