# vsf-board-run Reference

## Gateway protocol

After flash, firmware may initiate a scenario negotiation:

1. Firmware prints `GATEWAY:HELLO` → tool replies `GATEWAY:HELLO\r\n`
2. For each scenario: firmware prints `SCENARIO:<name>:READY?` → tool replies `GO\r\n` or `SKIP\r\n`
3. Firmware prints `GATEWAY:DONE` → dialog ends, firmware runs selected scenarios

If firmware does not print `GATEWAY:HELLO` within 2s, tool skips the dialog silently.

## SCENARIOS semantics

- **All scripts omit `SCENARIOS`** → every `READY?` gets `GO` (run-all legacy).
- **Any script defines `SCENARIOS`** → only the union across all scripts gets `GO`; unlisted scenarios get `SKIP`.
- `SCENARIOS=[]` (empty list) → all scenarios get `SKIP` (run nothing).

## Single vs multi-script timing

| Mode | Behavior |
|------|----------|
| Single script (1 arg) | Gateway dialog → immediately call `run()`. Script interacts live with firmware. |
| Multi-script (2+ args) | Gateway dialog → wait for `All test cases completed` → run each script with `SerialAlreadyCompleted` wrapper (all `expect()` calls return instantly). |

Single-script is for interactive protocols (echo, command-response). Multi-script is for post-hoc log analysis.

## Logic analyzer

Configured via `hardware-map.yml` → `logic_analyzer` section. Capture starts before flash, stops before scripts run.

```python
def run(project_root, serial, la):
    # la is a LogicAnalyzerInstrument
    ...
```

## SerialInstrument details

`expect(pattern, timeout=5)` reads until regex matches accumulated buffer, returns matched line. Unconsumed data after match is preserved — next `expect()` or `read_all()` consumes it first. Raises `TimeoutError` on timeout.

`read_all(timeout=2)` reads until no new data arrives for `timeout` seconds, returns all accumulated data as string.

## Audit log

Written to `logs/<timestamp>-<run_name>/vsf-board-run.jsonl`. Each line is a JSON event (serial RX/TX, LA events, verdict). Final line: `{"verdict":"pass"}` or `{"verdict":"fail"}` with optional `"error"` field.

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| Build fails | Verify cmake, SDK paths, `build.source_dir` in hardware-map.yml |
| Flash fails | Check debug probe connection or enter BOOTSEL mode for UF2 |
| Test timeout | Verify board outputs expected pattern; confirm baud rate matches |
| No serial data | Verify port path in hardware-map.yml `serial` field |
| Garbled output | Verify baud rate matches board firmware config |
| Script sees `[TEST]` lines unexpectedly | Firmware is running a test framework, not interactive protocol. Either: (a) set `SCENARIOS=[]`, (b) drain with `read_all(timeout=2)` before sending, or (c) switch to multi-script mode |
| `GATEWAY:HELLO` not printed | Firmware doesn't support scenario gating — normal, tool skips dialog |
| `read_all()` returns empty | Firmware hasn't printed yet or already fully consumed — increase timeout or check serial open |
| Board not found / flash timeout | Check USB cable, power, and that the correct runner is `active_runner` in hardware-map.yml |
