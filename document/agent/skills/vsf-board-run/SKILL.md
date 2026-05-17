---
name: vsf-board-run
description: |
  USE FOR: building VSF firmware, flashing to hardware, running automated test scripts over UART, the complete build-flash-test loop.
  DO NOT USE FOR: porting HAL drivers (use vsf-hal-driver).
---

# vsf-board-run

Build → flash → test loop. Always rebuilds.

## Usage

```bash
vsf-board-run board/<board>/hardware-map.yml test_script.py
vsf-board-run board/<board>/hardware-map.yml           # flash only, no test
```

Test script — Python file with `run(serial)`:
```python
def run(serial):
    serial.expect("UART echo demo", timeout=3)
    serial.send("hello\r\n")
    serial.expect("hello", timeout=2)
```

## Prerequisites

- vsf-bench installed (`pip install -e vsf.demo/vsf/test/vsf-bench`)
- pyyaml, pyserial installed
- cmake in PATH
- Board connected and powered
- hardware-map.yml configured
- Firmware must be built before flashing (Step 1 before Step 2)

## Exit codes

| Code | Meaning |
|------|---------|
| 0    | Success |
| 1    | FAIL (TimeoutError or AssertionError in test script) |

---

## Step 1 — Build

```python
from vsf_bench.hardware_map import load
from vsf_bench.runners.cmake_runner import CMakeRunner

board = load("board/<board>/hardware-map.yml")
cmake = CMakeRunner(board.build, project_root=".")
build_dir = cmake.build()
```

1. Reads `build.source_dir` and `build.build_dir` from hardware-map.yml
2. Creates build directory if needed
3. Runs `cmake -B <build_dir> -S <source_dir>` if no CMakeCache.txt
4. Runs `cmake --build <build_dir>`

Returns build directory path (contains `.elf`, `.uf2`, etc.). Non-zero exit on failure.

---

## Step 2 — Flash

```python
from pathlib import Path
from vsf_bench.runners.registry import get_runner_class

runner_cfg = board.runners[board.active_runner]
runner_cls = get_runner_class(runner_cfg.type)
if runner_cls is None:
    raise RuntimeError(f"Unknown runner type: {runner_cfg.type}")
runner = runner_cls(runner_cfg)
runner.flash(Path(board.build.build_dir))
```

Supported runners:

| type    | Class      | Artifact | Method                    |
|---------|------------|----------|---------------------------|
| openocd | SWDRunner  | .elf     | OpenOCD via CMSIS-DAP/SWD |
| uf2     | UF2Runner  | .uf2     | USB mass storage copy     |

---

## Step 3 — Serial / Test

```python
from vsf_bench.instruments.serial_instrument import SerialInstrument

with SerialInstrument(board.serial, board.baud) as ser:
    ser.expect("UART echo demo", timeout=3)
    ser.send("hello\r\n")
    ser.expect("hello", timeout=2)
```

### SerialInstrument API

| Method | Description |
|--------|-------------|
| `open()` / `close()` | Open/close serial port, drain stale data on open |
| `send(data)` | Send string to board |
| `expect(pattern, timeout=5)` | Read until regex matches, returns matched line; raises TimeoutError |
| `read_all(timeout=2)` | Read all until silence, returns string |
| `with SerialInstrument(...) as ser:` | Context manager, auto open/close |

`expect()` preserves unconsumed data after matched line — next `expect()` or `read_all()` consumes it first.

### Audit log

When audit_log path is provided, send/recv recorded as JSONL with timestamps. Final verdict: `{"verdict":"pass"}` or `{"verdict":"fail"}` written to `logs/<timestamp>-vsf-board-run.jsonl`.

---

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| Build fails | Check cmake, SDK include paths in CMakeLists.txt, `build.source_dir` in hardware-map.yml |
| Flash fails | Check board connection, debug probe, BOOTSEL mode for UF2 |
| Test timeout | Verify board outputs expected pattern; check baud rate |
| No serial data | Verify port path in hardware-map.yml `serial` field and cable |
| Garbled output | Verify baud rate matches board config |
