---
name: flash-board
description: Flash firmware to the target board using the active runner from hardware-map.yml.
---

# flash-board

Flash a pre-built firmware binary to the board. Uses the `active_runner` field from hardware-map.yml to select the flash method.

## Usage

```bash
python -m vsf_bench.runners.swd_runner --build-dir build/rp2040 --hardware-map board/pico/hardware-map.yml
```

Or programmatically:

```python
from vsf_bench.hardware_map import load
from vsf_bench.runners.swd_runner import SWDRunner
from vsf_bench.runners.uf2_runner import UF2Runner

board = load("board/pico/hardware-map.yml")
runner_cfg = board.runners[board.active_runner]

if runner_cfg.type == "openocd":
    runner = SWDRunner(runner_cfg)
elif runner_cfg.type == "uf2":
    runner = UF2Runner(runner_cfg)

runner.flash(Path("build/rp2040"))
```

## Supported runners

| type     | Class      | Artifact | Method                        |
|----------|------------|----------|-------------------------------|
| openocd  | SWDRunner  | .elf     | OpenOCD via CMSIS-DAP/SWD     |
| uf2      | UF2Runner  | .uf2     | USB mass storage copy         |

## What it does

1. Reads `active_runner` from hardware-map.yml
2. Selects the corresponding runner class
3. Calls `runner.flash(build_dir)` — the runner picks the correct artifact automatically

## Prerequisites

- Firmware must be built first (use build-firmware skill)
- Board must be connected (SWD debugger or in BOOTSEL mode for UF2)
