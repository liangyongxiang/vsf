---
name: vsf-build-firmware
type: utility
description: |
  USE FOR: building VSF firmware for a board, verifying cmake builds, resolving build directory paths.
  DO NOT USE FOR: flashing firmware (use vsf-flash-board), running tests on hardware (use vsf-board-run), creating new cmake projects.
---

# vsf-build-firmware

**UTILITY SKILL** — called by `vsf-board-run`. Also usable standalone.

## Overview

Run cmake configure (if needed) and build for the board specified in hardware-map.yml.

## Usage

```python
from vsf_bench.hardware_map import load
from vsf_bench.runners.cmake_runner import CMakeRunner

board = load("board/<board>/hardware-map.yml")
cmake = CMakeRunner(board.build, project_root=".")
build_dir = cmake.build()
```

## What it does

1. Reads `build.source_dir` and `build.build_dir` from hardware-map.yml
2. Creates build directory if needed
3. Runs `cmake -B <build_dir> -S <source_dir>` if no CMakeCache.txt
4. Runs `cmake --build <build_dir>`

## Returns

- Build directory path (contains `.elf`, `.uf2`, etc.)
- Non-zero exit on build failure

## Troubleshooting

- **cmake not found**: Install cmake and ensure it's in PATH
- **Source dir missing**: Verify `build.source_dir` in hardware-map.yml
- **Build errors**: Check SDK include paths in CMakeLists.txt
