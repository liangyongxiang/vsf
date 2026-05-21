---
name: vsf-bench
description: |
  USE FOR: building VSF firmware, flashing to hardware, running automated test scenes over UART, or the full build-flash-test loop.
  DO NOT USE FOR: porting HAL drivers (use vsf-hal-driver).
---

# vsf-bench

Build → flash → run test scenes. Always rebuilds.

```bash
# Full pipeline: build + flash + test all scenes
vsf-bench --all board/<board>/hardware-map.yml

# Run specific scene (all cases)
vsf-bench --all board/<board>/hardware-map.yml --scene usart_baud

# Run specific case by parameter value
vsf-bench --all board/<board>/hardware-map.yml --scene usart_baud --case 921600

# Run specific case by index (fallback)
vsf-bench --all board/<board>/hardware-map.yml --scene usart_baud --case-index 7

# Run multiple scenes
vsf-bench --all board/<board>/hardware-map.yml --scene usart_baud --scene usart_mode

# Override default script for a scene
vsf-bench --all board/<board>/hardware-map.yml --scene usart_baud --script path/to/custom.py

# Individual steps
vsf-bench --build  board/<board>/hardware-map.yml
vsf-bench --flash  board/<board>/hardware-map.yml
vsf-bench --test   board/<board>/hardware-map.yml
vsf-bench --build --flash board/<board>/hardware-map.yml
```

## Test script

Scripts live in `vsf.demo/vsf/test/vsf_test/<peripheral>/scenario/vsf_test_<scene_name>.py` and are discovered automatically.

```python
def run(project_root, serial):
    # Pure validation: assert on firmware output
    serial.expect_test_summary("scene_name")
```

Scripts that need logic analyzer decode accept `la`:

```python
def run(project_root, serial, la):
    serial.expect_test_summary("usart_baud", timeout=120.0)
    la.stop()
    la.wait(timeout=30.0)
    # decode markers, assert results...
```

The orchestrator handles all triggering; scripts must NOT send `vsf-test run ...` commands.

## SerialInstrument API

`send(data)`, `expect(pattern, timeout=5)` — reads until regex match, raises `TimeoutError`. `read_all(timeout=2)` — reads until silence. `expect()` preserves unconsumed data (including on timeout).

`expect_test_summary(name, timeout=30)` — waits for `"All test cases completed"`, parses `Pass/Fail/Skip` summary, asserts `failed==0` and `passed>0`.

## Scene discovery

Mapping: `vsf_test_<scene_name>.py` → scene `<scene_name>`. Scanned from `vsf.demo/vsf/test/vsf_test/*/scenario/`.

When `--all` is used without `--scene`, the orchestrator queries the firmware for its scene list and only runs scenes present in both filesystem and firmware.

## Output

`[vsf-bench] PASS` (0) or `FAIL` (1). Log to `logs/<ts>-<run_name>/vsf-bench.jsonl`.

## Prerequisites

- vsf-bench: `pip install -e vsf.demo/vsf/test/vsf_bench`
- pyyaml, pyserial, cmake in PATH; board connected
