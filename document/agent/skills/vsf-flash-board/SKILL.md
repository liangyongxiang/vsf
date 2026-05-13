---
name: vsf-flash-board
type: utility
description: |
  USE FOR: deploying firmware to hardware via SWD or UF2, selecting flash method from hardware-map.yml, flashing pre-built artifacts.
  DO NOT USE FOR: building firmware (use vsf-build-firmware), full build-flash-test loop (use vsf-board-run), serial interaction (use vsf-serial-monitor).
---

# vsf-flash-board

**UTILITY SKILL** — called by `vsf-board-run`. Also usable standalone.

## Overview

Flash a pre-built firmware binary to the board using the `active_runner` from hardware-map.yml.

## Usage

```python
from pathlib import Path
from vsf_bench.hardware_map import load
from vsf_bench.runners.registry import get_runner_class

board = load("board/<board>/hardware-map.yml")
runner_cfg = board.runners[board.active_runner]
runner_cls = get_runner_class(runner_cfg.type)
if runner_cls is None:
    raise RuntimeError(f"Unknown runner type: {runner_cfg.type}")
runner = runner_cls(runner_cfg)
runner.flash(Path(board.build.build_dir))
```

## Supported runners

| type    | Class      | Artifact | Method                    |
|---------|------------|----------|---------------------------|
| openocd | SWDRunner  | .elf     | OpenOCD via CMSIS-DAP/SWD |
| uf2     | UF2Runner  | .uf2     | USB mass storage copy     |

## What it does

1. Reads `active_runner` from hardware-map.yml
2. Selects runner class via `get_runner_class`
3. Finds artifact in the build directory
4. Calls `runner.flash(build_dir)`

## Prerequisites

- Firmware must be built first (use `vsf-build-firmware`)
- Board connected (SWD debugger or BOOTSEL mode for UF2)

## Troubleshooting

- **OpenOCD fails**: Check debug probe connection and interface/target config
- **UF2 copy fails**: Verify board is in BOOTSEL mode and mount point correct
- **Artifact not found**: Verify build completed and artifact name matches hardware-map.yml
