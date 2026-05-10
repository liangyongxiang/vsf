---
name: add-chip-support
description: Guide for adding a new chip to the VSF HAL driver framework. Use when porting a new MCU, adding full chip support, or setting up the VSF driver skeleton for a new device.
---

# Add Chip Support

## Overview

Four levels, each verified before proceeding:

| Level | Goal | Verify |
|-------|------|--------|
| L0 | SDK baseline: vendor SDK UART echo works | serial output on board |
| L1 | VSF skeleton: scaffold builds | cmake build passes |
| L2 | Minimal peripheral: first VSF UART works | board-run + test script PASS |
| L3 | Other peripherals: I2C, SPI, GPIO, etc. | per-peripheral test script |

## L0 — SDK Baseline

Use the vendor SDK to run a minimal UART echo on the board. This validates hardware, toolchain, and debug link before touching VSF.

Do NOT proceed past L0 until the board prints output over serial.

## L1 — VSF Skeleton

### Pre-flight: Collect hardware information

Study the vendor SDK to fill in the YAML config. Common locations:

| Information | Where to look | Used for |
|-------------|---------------|----------|
| CPU type | SDK headers, datasheet | `__device.h` arch selection (`arm`, `rv`, `x86`, `mcs51`) |
| Interrupt priority count/bits | NVIC/SCS headers, `core_cm*.h` | `arch_pri_num`, `arch_pri_bit` |
| XOSC / system clock freq | `board.h`, `system_*.c` | `driver.c` clock setup |
| Per-peripheral IRQ numbers | `startup_*.s` / vector table | `device.h` `VSF_HW_*_IRQN` |
| Per-peripheral register base | vendor header (e.g. `stm32f4xx.h`) | `device.h` `VSF_HW_*_REG` |
| Per-peripheral IRQ handler name | `startup_*.s` | `VSF_HW_*_IRQHandler` |
| Per-peripheral clock enable/reset bits | vendor headers | `driver.c` init |

For peripherals backed by an IPCore driver, also identify the IPCore path (e.g. `ARM/PL011` for UART, `Synopsys/DW_apb_i2c` for I2C).

### Write the YAML config

```yaml
# chip_config.yml
vendor: RaspberryPi
series: RP2040
device: RP2040
cpu: arm
arch_pri_num: 4
arch_pri_bit: 2
xosc_khz: 12000
sys_clk_khz: 125000
peripherals:
  uart:
    ipcore: ARM/PL011
    instances:
      - { index: 0, irqn: UART0_IRQ_IRQn, irq_handler: UART0_IRQHandler, reg: UART0_BASE }
      - { index: 1, irqn: UART1_IRQ_IRQn, irq_handler: UART1_IRQHandler, reg: UART1_BASE }
  gpio:
    port_count: 1
    pin_count: 32
  i2c:
    ipcore: Synopsys/DW_apb_i2c
    instances:
      - { index: 0, irqn: I2C0_IRQ_IRQn, irq_handler: I2C0_IRQHandler, reg: I2C0_BASE }
      - { index: 1, irqn: I2C1_IRQ_IRQn, irq_handler: I2C1_IRQHandler, reg: I2C1_BASE }
```

Instance `index` is the hardware instance number (0-based). For non-contiguous instances (e.g. I2C0 and I2C2 but no I2C1), declare both with their actual indices — the script generates `VSF_HW_I2C_MASK` automatically.

USB uses `mode: nonip` or `mode: ip`:

```yaml
  usb:
    mode: nonip
```

### Run scaffold_chip.py

**Important: this script only copies and fills template stubs. It does not implement any real driver logic. All generated `.c` files are skeletons with placeholder bodies.**

Two invocation contexts:

**Inside vsf** — run from the vsf repo root:
```bash
python document/agent/skills/add-chip-support/scaffold_chip.py \
  --driver-dir source/hal/driver \
  --config chip_config.yml
```

**Outside vsf** — run from the project root, output goes to `board/` or a user-specified path:
```bash
python vsf/document/agent/skills/add-chip-support/scaffold_chip.py \
  --driver-dir board/<BOARD>/driver \
  --template-dir vsf/source/hal/driver/template \
  --config chip_config.yml
```
`--template-dir` must point to the vsf template directory (`source/hal/driver/template` inside the vsf repo). If the output location is non-standard, pass the actual path to `--driver-dir`.

**Preconditions:**
- If `driver/<VENDOR>/<DEVICE>/` already exists, the script **overwrites all generated files without prompting**. Existing hand-written implementations will be lost. Only run on a fresh device, or accept the overwrite.

What the script generates (all files are overwritten if present):

```
driver/<VENDOR>/
  driver.h                        ← new vendor: created; existing vendor: device branch appended
  <DEVICE>/
    device.h                      ← VSF_HW_* macros (IRQN, REG, IRQHandler per instance)
    __device.h                    ← arch selection macro
    driver.h                      ← peripheral includes + template declaration blocks
    driver.c                      ← clock/PLL init stub
    startup_<DEVICE>.c            ← vector table with per-peripheral IRQ handler stubs
    CMakeLists.txt                ← build config stub
    <peripheral>/                 ← one directory per peripheral in YAML
      <peripheral>.h              ← peripheral header stub
      <peripheral>.c              ← peripheral implementation stub (bodies: return VSF_ERR_NONE)
      CMakeLists.txt
<VENDOR>/<SERIES>/common/
  CMakeLists.txt                  ← series-level common build config
```

Top-level `driver/driver.h` is also mutated to insert a `#elif defined(__<VENDOR>__)` branch when the vendor is new.

### After scaffold: make it build

1. Edit `device.h` — verify all `VSF_HW_*` macros match the SDK
2. Edit `driver.c` — implement `vsf_driver_init()`: reset blocks, configure clocks/PLLs, unreset peripherals
3. Edit `CMakeLists.txt` — add SDK include paths and source files
4. Edit `startup_<DEVICE>.c` — verify the vector table matches the actual device
5. Build: `cmake --build build/<board>` (use `build-firmware` skill)

## L2 — Minimal Peripheral (UART)

With the skeleton building, implement the first VSF peripheral — typically UART, so the board can communicate.

1. Read the existing RP2040 UART driver as reference: `driver/RaspberryPi/RP2040/uart/uart.c`
2. Fill in the generated `uart/uart.c` stub with actual register-level logic
3. Wire up IRQ: in the `VSF_USART_CFG_IMP_LV0` macro, set up `NVIC_SetPriority`/`NVIC_EnableIRQ` and the IRQ handler
4. Build, flash, verify with `board-run`

If using an IPCore (e.g. `ARM/PL011`), the generated `.c` already includes the IPCore header. Implement each function by delegating to the IPCore, then add IRQ setup around it. See `driver/RaspberryPi/RP2040/uart/uart.c` for the pattern.

## L3 — Other Peripherals

Repeat the L2 pattern for each needed peripheral:

1. Read the IPCore driver header (if applicable) to understand the API
2. Fill in the generated `.c` stub
3. Wire `VSF_<PERIPHERAL>_CFG_IMP_LV0` with IRQ handlers
4. Build, flash, verify with a test script

For peripherals without an IPCore (e.g. GPIO), implement register operations directly. The generated `.c` stub includes a skeleton with all required function signatures.

### Peripheral implementation pattern (IPCore-backed)

Every peripheral `.c` follows this structure:

```c
#include "../driver.h"
#if VSF_HAL_USE_<PERIPHERAL> == ENABLED
#define __VSF_HAL_<IPCORE>_CLASS_INHERIT__
#include "hal/vsf_hal.h"

// 1. Define struct wrapping the IPCore type
typedef struct vsf_hw_<peripheral>_t {
    implement(vsf_<ipcore>_t)
    IRQn_Type irqn;
} vsf_hw_<peripheral>_t;

// 2. Implement each API function by delegating to IPCore
vsf_err_t vsf_hw_<peripheral>_init(vsf_hw_<peripheral>_t *ptr, vsf_<peripheral>_cfg_t *cfg) {
    vsf_err_t err = vsf_<ipcore>_init(&ptr->use_as__vsf_<ipcore>_t, cfg, clock_get_hz(clk_sys));
    if (err == VSF_ERR_NONE && cfg->isr.handler_fn) {
        NVIC_SetPriority(ptr->irqn, cfg->isr.prio);
        NVIC_EnableIRQ(ptr->irqn);
    }
    return err;
}
// ... fini, enable, disable, irq_enable, irq_disable, status, ...

// 3. Instantiate: define CFG_IMP_LV0, include .inc template
#define VSF_<PERIPHERAL>_CFG_REIMPLEMENT_API_CAPABILITY    ENABLED
#define VSF_<PERIPHERAL>_CFG_IMP_PREFIX                    vsf_hw
#define VSF_<PERIPHERAL>_CFG_IMP_UPCASE_PREFIX             VSF_HW
#define VSF_<PERIPHERAL>_CFG_IMP_LV0(__IDX, __HAL_OP)                      \
    vsf_hw_<peripheral>_t vsf_hw_<peripheral> ## __IDX = {                  \
        .reg  = (vsf_<ipcore>_reg_t *)VSF_HW_<PERIPHERAL> ## __IDX ## _REG, \
        .irqn = VSF_HW_<PERIPHERAL> ## __IDX ## _IRQN,                     \
        __HAL_OP                                                            \
    };                                                                      \
    void VSF_HW_<PERIPHERAL> ## __IDX ## _IRQHandler(void)                  \
    {                                                                       \
        uintptr_t ctx = vsf_hal_irq_enter();                                \
        vsf_<ipcore>_irqhandler(&vsf_hw_<peripheral> ## __IDX .use_as__vsf_<ipcore>_t); \
        vsf_hal_irq_leave(ctx);                                            \
    }
#include "hal/driver/common/<peripheral>/<peripheral>_template.inc"
#endif
```

## Key files (reference)

| File | Purpose |
|------|---------|
| `template/README.md` | Template usage guide (HW / IPCore / emulated) |
| `driver/RaspberryPi/RP2040/` | Complete working example — use as primary reference |
| `driver/RaspberryPi/RP2040/device.h` | Hardware config pattern (VSF_HW_* macros) |
| `driver/RaspberryPi/RP2040/driver.h` | Device dispatch with template includes |
| `driver/RaspberryPi/RP2040/driver.c` | Clock/PLL/reset init pattern |
| `driver/RaspberryPi/RP2040/uart/uart.c` | IPCore-backed peripheral pattern (PL011) |
| `driver/RaspberryPi/RP2040/i2c/i2c.c` | IPCore-backed peripheral pattern (DW_apb_i2c) |
| `driver/common/<peripheral>/<peripheral>_template.inc` | Instance generation template |
