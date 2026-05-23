---
name: vsf-hal-driver
description: |
  Create, implement, modify, or debug VSF HAL drivers (UART, I2C, SPI, GPIO, ADC, PWM, etc.).
  **UTILITY SKILL** — INVOKES: vsf-bench (for verification).
  USE FOR: porting new chips, adding peripherals to existing chips, fixing driver bugs.
  DO NOT USE FOR: build/flash/test (use vsf-bench), BSP-only pinmux changes.
  FOR SINGLE OPERATIONS: prefer direct edits over full template copy if driver already exists.
metadata:
  version: "1.0"
  license: Apache-2.0
---

# VSF HAL Driver

## Quickstart

**New chip:** follow `PORTING.md` R0→R5.

**Add peripheral:**
```bash
scaffold_peripheral.py --driver-dir source/hal/driver --chip Vendor/Chip --periph <name>
# edit .c/.h → implement register logic
check-driver-structure.py --periph <name> --side header <name>.h
check-driver-structure.py --periph <name> --side source <name>.c
check-driver-quality.py <name>.c
vsf-bench --all board/pico/hardware-map.yml --suite <name>
```

**Audit a port:**
```bash
audit-port.py --chip Vendor/Chip
```

## Scripts

| Script | Use when... |
|---|---|
| `scaffold_chip.py` | New chip port |
| `scaffold_peripheral.py` | Add peripheral to existing chip |
| `generate-device-peripheral-macros.py` | Edit device.h instances |
| `check-driver-structure.py` | Verify header/source structure (data-driven) |
| `check-driver-quality.py` | Anti-pattern check |
| `audit-port.py` | Cross-file consistency |
| `enable-periph.py` | Toggle vsf_usr_cfg.h |

## Before debugging: verify IO wiring

If a peripheral test fails, run the `gpio_io_check` suite first before chasing driver bugs. It verifies that the physical pins are toggling as expected.

```bash
vsf-bench --all board/<board>/hardware-map.yml --suite gpio_io_check
```

This catches swapped TX/RX, missing pull-ups, and broken traces before you spend time in register-level debugging.

## Pitfalls & conventions

| Rule | Why it matters | See REFERENCE.md |
|---|---|---|
| **Register read side effects** — read a HW register once into a local; never re-read for multiple decisions. | Re-reading can clear-on-read flags or pop FIFOs, causing lost events. | "Register access: read side effects and caching" |
| **Unimplemented API** — every stub body must `VSF_HAL_ASSERT(0)` and return an error. | Silent stubs let callers proceed as if hardware worked; the failure surfaces far from the root cause. | "Unimplemented API convention" |

See `REFERENCE.md` for full conventions and `PORTING.md` for the full ladder.
