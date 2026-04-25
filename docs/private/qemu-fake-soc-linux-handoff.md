# QEMU Fake SoC Linux Handoff

## Goal

Continue development of `feat/qemu-fake-soc-hello` on a Linux machine without
re-discovering the Windows-specific issues already identified.

## Linux Prerequisites

Install at least:

- `git`
- `cmake`
- `make`
- `gcc-arm-none-eabi`
- `binutils-arm-none-eabi`
- `qemu-system-arm`

Optional:

- `ninja-build`

## Current Recommended Build Path

The conservative path is still `Unix Makefiles` because it matches the validated
workflow exactly.

Bridge target:

```sh
cd /path/to/vsf-qemu-fake-soc/example/template/project/cmake/qemu_mps2_bridge
cmake -S . -B build-make -G "Unix Makefiles"
cmake --build build-make -- -j"$(nproc)"
```

Fake-soc target:

```sh
cd /path/to/vsf-qemu-fake-soc/example/template/project/cmake/qemu_fake_soc
cmake -S . -B build-make -G "Unix Makefiles"
cmake --build build-make -- -j"$(nproc)"
```

If Ninja is preferred on Linux, it should be re-validated there. The Windows
link failure was specifically caused by BOM-prefixed response files.

## Run Commands

Bridge target:

```sh
export QEMU_BIN="${QEMU_BIN:-qemu-system-arm}"
cd /path/to/vsf-qemu-fake-soc/example/template/project/cmake/qemu_mps2_bridge
./run_qemu_mps2_bridge.sh
```

Fake-soc target:

```sh
export QEMU_BIN="${QEMU_BIN:-qemu-system-arm}"
cd /path/to/vsf-qemu-fake-soc/example/template/project/cmake/qemu_fake_soc
./run_qemu_fake_soc.sh
```

Expected output:

- bridge: `qemu mps2 bridge hello world`
- fake-soc: `qemu fake soc hello world`

## What To Verify First On Linux

1. `qemu-system-arm --version`
2. `arm-none-eabi-gcc --version`
3. bridge target build
4. bridge target run
5. fake-soc target build
6. fake-soc target run

If bridge fails, do not continue to fake-soc changes yet.

## Current Runtime Model

Both targets currently run on:

- QEMU machine: `mps2-an500`
- CPU/BSP model: `CMSDK_CM7`
- UART IP: `CMSDK APB UART`

This is temporary for `qemu/fake-soc`. It exists to keep the VSF-side
implementation stable while a real fake-soc machine is still missing.

## Next Development Task

Start with the first minimal USART HAL instance inside:

- `source/hal/driver/qemu/fake-soc`

Keep the current hello-world path available until `vsf_usart_t` TX polling is
proven on the same backend.
