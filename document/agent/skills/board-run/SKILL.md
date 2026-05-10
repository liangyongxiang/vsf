---
name: board-run
description: Main agent loop — build, flash, run test script, return results. Call once after every code change.
---

# board-run

The primary entry point for AI agent development. Chains the development loop: build → flash → optionally execute test script → return results.

**Always rebuilds** (no skip-build in MVP).

## Usage

```bash
# Build + flash + verify
board-run board/pico/hardware-map.yml test_script.py

# Build + flash only (no test)
board-run board/pico/hardware-map.yml

# Explicit project root (defaults to cwd)
board-run --project-root /path/to/project board/pico/hardware-map.yml test_script.py
```

The `test_script.py` is optional. When omitted, board-run runs build + flash only — no serial is opened, no audit log is created, exit code is always 0 on success.

When provided, `test_script.py` is a Python file with a `run(serial)` function:

```python
def run(serial):
    serial.expect("UART echo demo", timeout=3)
    serial.send("hello\r\n")
    serial.expect("hello", timeout=2)
```

## What it does

1. **Resolve project root** — `--project-root` flag, defaults to cwd
2. **Build** — cmake configure (if needed) + build
3. **Flash** — select runner from `active_runner` in hardware-map.yml, flash firmware
4. **If test script provided:**
   - Open serial
   - Run test script (inject `SerialInstrument`, call `run(serial)`)
   - Print PASS or FAIL
   - Write audit log + final verdict to `logs/<timestamp>-board-run.jsonl`
5. **If no test script:** exit 0 after flash

## Exit codes

| Code | Meaning |
|------|---------|
| 0    | Success — build+flash OK (or test script PASS) |
| 1    | FAIL — test script raised TimeoutError or AssertionError |

## Audit log

Only created when a test script is provided. Events logged per step, plus a final verdict line:

```jsonl
{"ts":"...", "direction":"recv", "data":"UART echo demo ...", "verdict":"pending"}
{"ts":"...", "direction":"send", "data":"hello", "verdict":"pending"}
{"ts":"...", "direction":"recv", "data":"hello", "verdict":"pending"}
{"verdict":"pass"}
```

## Workflow for AI agents

After modifying firmware source code:

```
1. Generate or reuse a test script (e.g. test_uart_echo.py)
2. Run: board-run board/pico/hardware-map.yml test_uart_echo.py
3. Check exit code and output
4. If PASS → proceed to next task
5. If FAIL → read error message and audit log, modify code, go to 1
```

## Test script format

Plain Python file with a single `run(serial)` function. The `serial` parameter is a `SerialInstrument` instance with `send()`, `expect()`, and `read_all()` methods. See serial-monitor skill for full API.

## Prerequisites

- vsf-bench installed (`pip install -e vsf.demo/vsf/test/vsf-bench`)
- pyyaml, pyserial installed
- Board connected and powered
- hardware-map.yml configured with correct serial port and runner
