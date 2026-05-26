# vsf-bench Reference

## CLI Interface

Tool name: **`vsf-bench`**

```bash
# Full pipeline: build + flash + test all scenes
vsf-bench --all board/<board>/hardware-map.yml

# Run specific scene (all cases)
vsf-bench --all board/<board>/hardware-map.yml --suite usart_baud

# Run specific case by parameter value
vsf-bench --all board/<board>/hardware-map.yml --suite usart_baud --case 921600

# Run specific case by index (fallback)
vsf-bench --all board/<board>/hardware-map.yml --suite usart_baud --case-index 7

# Run multiple scenes
vsf-bench --all board/<board>/hardware-map.yml --suite usart_baud --suite usart_mode

# Override default script for a scene
vsf-bench --all board/<board>/hardware-map.yml --suite usart_baud --script path/to/custom.py

# Individual steps
vsf-bench --build  board/<board>/hardware-map.yml
vsf-bench --flash  board/<board>/hardware-map.yml
vsf-bench --test   board/<board>/hardware-map.yml
vsf-bench --build --flash board/<board>/hardware-map.yml
vsf-bench --build --test  board/<board>/hardware-map.yml
```

## Orchestrator Behavior

```
build → flash → for each scene: send trigger → run script
```

Trigger commands sent to firmware:
- No `--suite` → `vsf-test run all` (but filtered to firmware-known scenes)
- `--suite usart_baud` → `vsf-test run usart_baud`
- `--suite usart_baud --case 921600` → `vsf-test run usart_baud.7` (resolved from YAML)
- `--suite usart_baud --case-index 7` → `vsf-test run usart_baud.7`

After the trigger, the script runs immediately. The script waits for completion via `expect_test_summary()` or its own `expect()` calls.

## Script Behavior

Scripts are **validation-only**. They do NOT send trigger commands.

```python
def run(project_root: Path, serial: SerialInstrument) -> None:
    serial.expect_test_summary("gpio_toggle")
```

For scripts that need LA decode:

```python
def run(project_root: Path, serial: SerialInstrument, la: LogicAnalyzerInstrument) -> None:
    serial.expect_test_summary("usart_baud", timeout=120.0)
    la.stop()
    la.wait(timeout=30.0)
    # decode markers, assert results...
```

The orchestrator starts a per-scene LA capture for scripts that actually call `la.` methods. LA is stopped after the script returns (or by the script itself if it needs to decode before returning).

## Scene → Script Discovery

Scripts live alongside firmware sources:

```
vsf.demo/vsf/test/vsf_test/usart/suite/
  vsf_test_usart_baud.c
  vsf_test_usart_baud.h
  vsf_test_usart_baud.py   ← default script for "usart_baud"
  vsf_test_usart_mode.c
  vsf_test_usart_mode.h
  vsf_test_usart_mode.py   ← default script for "usart_mode"
```

Mapping rule: `vsf_test_<scene_name>.py` → scene `<scene_name>`

The orchestrator scans `vsf.demo/vsf/test/vsf_test/*/suite/` relative to the project root.

## SerialInstrument API

`send(data)` — writes to serial, logs to audit log.

`expect(pattern, timeout=5.0)` — reads until regex matches accumulated buffer, returns matched line. Unconsumed data after match is preserved in `_leftover` — next `expect()` or `read_all()` consumes it first. Raises `TimeoutError` on timeout. **On timeout, unconsumed data is preserved in `_leftover`**.

`read_all(timeout=2.0)` — reads until no new data arrives for `timeout` seconds, returns all accumulated data as string.

`expect_test_summary(name, timeout=30.0)` — waits for `"All test cases completed"`, then parses `Pass: N, Fail: N, Skip: N` summary line. Asserts `failed == 0` and `passed > 0`. Returns `(passed, failed, skipped)`.

## Logic Analyzer

Configured via `hardware-map.yml` → `logic_analyzer` section.

Per-scene capture: LA starts before trigger, stops after script returns (or when script calls `la.stop()`). `dsview-cli` needs ~3s to initialize; the orchestrator adds this delay automatically for LA-using scenes.

## Audit Log

Written to `logs/<timestamp>-<run_name>/vsf-bench.jsonl`. Each line is a JSON event (serial RX/TX, LA events, verdict). Final line: `{"verdict":"pass"}` or `{"verdict":"fail"}`.

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| Build fails | Verify cmake, SDK paths, `build.source_dir` in hardware-map.yml |
| Flash fails | Check debug probe connection or enter BOOTSEL mode for UF2 |
| Test timeout | Verify board outputs expected pattern; confirm baud rate matches |
| No serial data | Verify port path in hardware-map.yml `serial` field |
| Garbled output | Verify baud rate matches board firmware config |
| `Scene not found` in firmware | Scene is disabled in firmware config (e.g. `VSF_TEST_USART_RX_BAUD_ENABLE = DISABLED`). Use `--suite` to select only enabled scenes. |
| `LA capture did not finish` | dsview-cli device not found; check `logic_analyzer` section in hardware-map.yml |
| Script sees empty buffer | Previous `expect()` timed out but `_leftover` preserved data; should work with fixed serial_instrument |
