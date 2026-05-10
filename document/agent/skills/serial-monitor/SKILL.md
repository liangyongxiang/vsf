---
name: serial-monitor
description: Open serial connection to the board for send/expect interaction.
---

# serial-monitor

Open a programmatic serial connection to the board. Supports send/expect pattern and optional audit logging.

## Usage

```python
from pathlib import Path
from vsf_bench.hardware_map import load
from vsf_bench.instruments.serial_instrument import SerialInstrument

board = load("board/pico/hardware-map.yml")

with SerialInstrument(board.serial, board.baud) as ser:
    ser.expect("UART echo demo", timeout=3)
    ser.send("hello\r\n")
    ser.expect("hello", timeout=2)
```

With audit log:

```python
with SerialInstrument(board.serial, board.baud, audit_log=Path("logs/test.jsonl")) as ser:
    ser.send("test\r\n")
    output = ser.read_all(timeout=2)
```

## API

| Method                      | Description                                       |
|-----------------------------|---------------------------------------------------|
| `open()`                    | Open serial port, drain stale data                |
| `close()`                   | Close serial port                                 |
| `send(data: str)`           | Send string to board                              |
| `expect(pattern, timeout)`  | Read until regex pattern matches, raises TimeoutError |
| `read_all(timeout)`         | Read all data until silence, returns string       |
| Context manager (`with`)    | Auto open/close                                   |

## Leftover buffering

`expect()` preserves unconsumed data after the matched line in an internal buffer. The next `expect()` or `read_all()` call consumes it first, so no serial data is silently lost.

## Audit log

When `audit_log` path is provided, every send/recv is recorded as JSONL:

```json
{"ts": "2026-05-10T12:00:00+00:00", "direction": "send", "data": "hello", "verdict": "pending"}
{"ts": "2026-05-10T12:00:00+00:00", "direction": "recv", "data": "hello", "verdict": "pending"}
```

## Prerequisites

- Board serial port must be connected (check hardware-map.yml `serial` field)
- pyserial installed
