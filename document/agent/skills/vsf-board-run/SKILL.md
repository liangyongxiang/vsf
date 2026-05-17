---
name: vsf-board-run
description: |
  USE FOR: building VSF firmware (compile-only), flashing to hardware, running automated test scripts over UART, or the full build-flash-test loop. Can be used for any single step — build, flash, or test — not just the full pipeline.
  DO NOT USE FOR: porting HAL drivers (use vsf-hal-driver).
---

# vsf-board-run

Build → flash → run test script(s) → return results. Always rebuilds.

## CLI

```bash
vsf-board-run board/<board>/hardware-map.yml [test_script ...]
vsf-board-run board/<board>/hardware-map.yml                    # build+flash only
vsf-board-run --project-root <dir> <hardware-map.yml>            # explicit project root
vsf-board-run --log-dir <dir> <hardware-map.yml> <script>        # explicit log dir
```

## Test script

Python file with `run(project_root, serial, la=None)`:

```python
def run(project_root, serial):
    serial.expect("UART echo demo", timeout=3)
    serial.send("hello\r\n")
    serial.expect("hello", timeout=2)
```

`project_root` — `Path`, always the first argument, set by CLI (`--project-root` or cwd).

Optional: `SCENARIOS` list for scenario gating. Firmware asks "should I run scenario X?" — only listed scenarios get GO.

```python
SCENARIOS = ["uart_echo", "uart_loopback"]

def run(project_root, serial):
    ...
```

## SerialInstrument API

Available inside `run()`:

| Method | Description |
|--------|-------------|
| `send(data)` | Send string to board |
| `expect(pattern, timeout=5)` | Read until regex matches, returns matched line; raises TimeoutError |
| `read_all(timeout=2)` | Read all until silence, returns string |

`expect()` preserves unconsumed data after matched line — next call consumes it first.

## Multi-script mode

Pass multiple test scripts. Tool flashes once, waits for firmware to print `All test cases completed`, then runs each script:
```bash
vsf-board-run board/pico/hardware-map.yml test_uart.py test_gpio.py
```

## Logic analyzer (optional)

If `hardware-map.yml` configures `logic_analyzer`, the tool starts capture before flash and stops before running scripts. Declare `la` parameter to receive it:
```python
def run(project_root, serial, la):
    ...
```

## Output

| Signal | Meaning |
|--------|---------|
| `[vsf-board-run] PASS` | All scripts passed |
| `[vsf-board-run] FAIL: <reason>` | Test failure |

## Exit codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | FAIL (TimeoutError or AssertionError in test script) |

## Audit log

Written to `logs/<timestamp>-<run_name>/vsf-board-run.jsonl`. Final line: `{"verdict":"pass"}` or `{"verdict":"fail"}`.

## Prerequisites

- vsf-bench installed (`pip install -e vsf.demo/vsf/test/vsf-bench`)
- pyyaml, pyserial, cmake in PATH
- Board connected and powered, hardware-map.yml configured

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| Build fails | Check cmake, SDK paths in CMakeLists.txt, `build.source_dir` in hardware-map.yml |
| Flash fails | Check board connection, debug probe, BOOTSEL mode for UF2 |
| Test timeout | Verify board outputs expected pattern; check baud rate |
| No serial data | Verify port path in hardware-map.yml `serial` field |
| Garbled output | Verify baud rate matches board config |
