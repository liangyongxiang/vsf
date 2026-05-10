---
name: build-firmware
description: Build firmware for the target board using cmake.
---

# build-firmware

Run cmake configure (if needed) and build for the board specified in hardware-map.yml.

## Usage

```bash
python -m vsf_bench.runners.cmake_runner --hardware-map board/pico/hardware-map.yml
```

Or programmatically:

```python
from vsf_bench.hardware_map import load
from vsf_bench.runners.cmake_runner import CMakeRunner

board = load("board/pico/hardware-map.yml")
cmake = CMakeRunner(board.build, project_root=".")
build_dir = cmake.build()
print(f"Build output: {build_dir}")
```

## What it does

1. Reads `build.source_dir` and `build.build_dir` from hardware-map.yml
2. Creates build directory if it doesn't exist
3. Runs `cmake -B <build_dir> -S <source_dir>` if no CMakeCache.txt exists
4. Runs `cmake --build <build_dir>`

## Returns

- Build directory path (contains `vsf_demo.elf`, `vsf_demo.uf2`, etc.)
- Non-zero exit on build failure
