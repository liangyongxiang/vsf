---
name: vsf-hal-driver
description: |
  Create, implement, modify, audit, or debug VSF HAL LV0 (register-level) drivers.
  **UTILITY SKILL** — INVOKES: vsf-bench (for verification).
  USE FOR: porting new chips, adding peripherals, fixing driver bugs.
  DO NOT USE FOR: build/flash/test (use vsf-bench), BSP pinmux changes.
metadata:
  version: "1.0"
  license: Apache-2.0
---

# VSF HAL Driver

## Quickstart

**New chip:** define registers in device.h, implement init/fini, run `check-driver-structure.py` then `check-driver-quality.py`, wire IRQ handlers, run vsf-bench.

**Add peripheral:**
```bash
scaffold_peripheral.py --driver-dir source/hal/driver --chip Vendor/Chip --periph <name>
# edit .c/.h, then run check-driver-quality.py before vsf-bench
```

**Audit:** `audit-port.py --chip Vendor/Chip` → fix issues → re-run until clean.

## Concepts

- LV0: register-level driver. LV1/LV2 framework abstractions are out of scope.
- IMP_LV0: macro that expands per-instance structs and IRQ handlers from device.h.
- VSF_MCONNECT: token-paste macro for building per-instance names.

## Conventions

- Per-instance values via device.h + VSF_MCONNECT in IMP_LV0.
- Spin-wait loops need comment with <X us.
- No pinmux in driver (board file only).
- Unimplemented APIs: return VSF_ERR_NOT_SUPPORT + VSF_HAL_ASSERT(0).
- Enable clocks before access; fini() disables IRQs, aborts DMA.

## Error handling

- Fix: edit .c/.h directly; run `check-driver-quality.py` before `vsf-bench`.
- scaffold fails: verify `--chip` path; edit directly if dir exists.
- Test fails: run `gpio_io_check` first; check hardware-map.yml for errors.
- False positive: only suppress with `// quality: allow-<rule-id>` after confirming; always prefer root cause fix.
