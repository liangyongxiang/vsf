---
name: board-run
description: Main agent loop — build, flash, run test script, return results. Call once after every code change.
---

# board-run

The primary entry point for AI agent development. Chains the full development loop: build → flash → execute test script → return pass/fail.

**Always rebuilds** (no skip-build in MVP).

## Usage

```bash
board-run board/pico/hardware-map.yml test_script.py
```

Where `test_script.py` is a Python file with a `run(serial)` function:

```python
def run(serial):
    serial.expect("UART echo demo", timeout=3)
    serial.send("hello\r\n")
    serial.expect("hello", timeout=2)
```

## What it does

1. **Detect project root** — git `rev-parse --show-toplevel` from the hardware-map path
2. **Build** — cmake configure (if needed) + build
3. **Open serial** — before flash, to capture boot output
4. **Flash** — select runner from `active_runner` in hardware-map.yml, flash firmware
5. **Run test script** — load script, inject `SerialInstrument`, call `run(serial)`
6. **Report** — prints PASS or FAIL, writes audit log to `logs/<timestamp>-board-run.jsonl`
7. **Write final verdict** — appends `{"verdict": "pass"}` or `{"verdict": "fail", "error": "..."}` to the audit log

## Exit codes

| Code | Meaning |
|------|---------|
| 0    | PASS — test script completed without exception |
| 1    | FAIL — test script raised TimeoutError or AssertionError |

## Audit log

Events logged per step, plus a final verdict line:

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
