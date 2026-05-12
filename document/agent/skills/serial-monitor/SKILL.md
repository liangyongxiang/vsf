---
name: serial-monitor
type: utility
description: |
  USE FOR: interacting with a board over UART, writing test scripts with send/expect, recording serial communication with audit logging, debugging board output.
  DO NOT USE FOR: building firmware (use build-firmware), flashing firmware (use flash-board), full workflow (use board-run).
---

# serial-monitor

**UTILITY SKILL** — used by `board-run` test scripts. Also usable standalone.

## Overview

Open a programmatic serial connection to the board. Supports send/expect pattern and optional audit logging.

## Usage

```python
from vsf_bench.hardware_map import load
from vsf_bench.instruments.serial_instrument import SerialInstrument

board = load("board/<board>/hardware-map.yml")
with SerialInstrument(board.serial, board.baud) as ser:
    ser.expect("UART echo demo", timeout=3)
    ser.send("hello\r\n")
    ser.expect("hello", timeout=2)
```

## API

| Method                     | Description                                    |
|----------------------------|------------------------------------------------|
| `open()`                   | Open serial port, drain stale data             |
| `close()`                  | Close serial port                              |
| `send(data)`               | Send string to board                           |
| `expect(pattern, timeout=5)` | Read until regex matches, returns matched line, raises TimeoutError |
| `read_all(timeout=2)`      | Read all until silence, returns string         |
| Context manager (`with`)   | Auto open/close                                |

## Leftover buffering

`expect()` preserves unconsumed data after the matched line. Next `expect()` or `read_all()` consumes it first — no data is lost.

## Audit log (optional)

When `audit_log` path is provided, send/recv is recorded as JSONL with timestamps and verdict fields.

## Prerequisites

- Board serial port connected (check hardware-map.yml `serial` field)
- pyserial installed

## Troubleshooting

- **Timeout on expect**: Verify board outputs the expected pattern; baud rate mismatch
- **No serial data**: Check port path and cable connection
- **Garbled output**: Verify baud rate matches board config
