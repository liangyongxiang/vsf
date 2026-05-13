---
name: vsf-board-run
type: workflow
description: |
  USE FOR: running the build-flash-test loop after firmware changes, executing automated test scripts on hardware, getting PASS/FAIL with audit trails, iterating on driver implementation.
  DO NOT USE FOR: building only (use vsf-build-firmware), manual serial (use vsf-serial-monitor), flashing pre-built firmware (use vsf-flash-board).
---

# vsf-board-run

**PRIMARY ENTRY POINT** for AI agent firmware development. INVOKES: `vsf-build-firmware` → `vsf-flash-board` → `vsf-serial-monitor`.

## Overview

Build → flash → optionally run test script → return results. Always rebuilds.

## Usage

```bash
vsf-board-run board/<board>/hardware-map.yml test_script.py
vsf-board-run board/<board>/hardware-map.yml
```

Test script — Python file with `run(serial)`:
```python
def run(serial):
    serial.expect("UART echo demo", timeout=3)
    serial.send("hello\r\n")
    serial.expect("hello", timeout=2)
```

## What it does

1. Resolve project root (`--project-root` or cwd)
2. Build via cmake configure + build
3. Flash via active runner from hardware-map.yml
4. If test script: open serial, run test, print PASS/FAIL, write audit log
5. If no test script: exit 0 after flash

## Exit codes

| Code | Meaning |
|------|---------|
| 0    | Success |
| 1    | FAIL (TimeoutError or AssertionError in test script) |

## Audit log

JSONL per step, final verdict: `{"verdict":"pass"}` or `{"verdict":"fail"}`, written to `logs/<timestamp>-vsf-board-run.jsonl`.

## Prerequisites

- vsf-bench installed (`pip install -e vsf.demo/vsf/test/vsf-bench`)
- pyyaml, pyserial installed
- Board connected and powered
- hardware-map.yml configured

## Troubleshooting

- **Build fails**: Run `vsf-build-firmware` standalone
- **Flash fails**: Check board connection and active runner config
- **Test timeout**: Debug with `vsf-serial-monitor` standalone
