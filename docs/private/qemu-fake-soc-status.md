# QEMU Fake SoC Status

## Scope

This branch brings up a first VSF QEMU target family and validates UART output
 with a minimal hello-world program.

Implemented targets:

- `qemu/mps2-bridge`
- `qemu/fake-soc`

Implemented first UART IP:

- `CMSDK APB UART`

## Working Tree and Branch

- repository worktree: `C:\Users\yongxiang\work\vsfteam\vsf-qemu-fake-soc`
- branch: `feat/qemu-fake-soc-hello`

Public code branch:

- remote: `github`
- repository: `https://github.com/liangyongxiang/vsf`
- branch: `feat/qemu-fake-soc-hello`
- pushed content: code only, based on commit `f27828b6b`

The original `vsf` worktree was intentionally left untouched because it already
had unrelated local changes.

## Key Decisions

1. The first running path uses `CMSDK_CM7` with `mps2-an500`, not `CM4`.
   Reason: the VSF tree already contains the needed `CMSDK_CM7` startup and BSP
   path, which makes it the shortest route to a working image.

2. `qemu/fake-soc` is currently validated on the same QEMU backend as the
   bridge target.
   Reason: this proves the new VSF HAL directory structure, startup code, and
   UART wiring before a custom QEMU machine exists.

3. The first hello-world path writes directly to the UART registers.
   Reason: this removes `stdio` and `debug_stream` from the critical path and
   verifies CPU reset, linker layout, MMIO mapping, and serial output with the
   least moving parts.

4. Windows builds currently use `Unix Makefiles`.
   Reason: in this environment, CMake + Ninja emits response files with a UTF-8
   BOM. GNU Arm ld treats the BOM as part of the first object filename and the
   link fails.

5. Windows QEMU runs currently force `-accel tcg,tb-size=128`.
   Reason: QEMU 11.0.0 on the current Windows host attempted to allocate a very
   large TCG JIT buffer and failed because of page file limits.

## Files Added or Changed

High-level additions:

- new QEMU HAL vendor under `source/hal/driver/qemu`
- new target CMake files under `script/cmake/targets/qemu`
- new minimal demo at `example/template/demo/qemu_hello`
- two standalone example projects:
  - `example/template/project/cmake/qemu_mps2_bridge`
  - `example/template/project/cmake/qemu_fake_soc`
- QEMU-specific user config at
  `example/template/config/vsf_usr_cfg/vsf_usr_cfg_qemu.h`

Notable integration points:

- `source/hal/driver/driver.h`
- `example/template/config/vsf_usr_cfg.h`

## Validation Performed

Validated on Windows with:

- `arm-none-eabi-gcc` 15.2.1
- QEMU 11.0.0
- CMake with `Unix Makefiles`

Validation results:

- `qemu/mps2-bridge` builds and prints:
  - `qemu mps2 bridge hello world`
- `qemu/fake-soc` builds and prints:
  - `qemu fake soc hello world`

Validated QEMU machine:

- `mps2-an500`

## Known Boundaries

Current state is intentionally minimal.

- `qemu/fake-soc` is not yet backed by a custom QEMU machine
- complete `vsf_usart_t` support is not implemented yet
- `debug_uart` and `stdio` are not the validated hello-world path yet
- the fake-soc linker layout is temporarily aligned with the bridge target
- `docs/private` is intentionally not pushed to GitHub

## Recommended Next Steps

1. Keep `mps2-bridge` as the regression baseline while changing the fake-soc.
2. Implement the first minimal `vsf_usart_t` instance for `qemu/fake-soc`.
   Suggested order:
   - `init`
   - polling TX
   - polling RX
   - `enable` and `disable`
   - IRQ mask and status
3. After USART is stable, start a custom QEMU machine that reuses the same
   UART base address and IRQ mapping so the VSF side does not need a rewrite.
4. Only then expand to GPIO, SPI, and ADC companion IPs.
