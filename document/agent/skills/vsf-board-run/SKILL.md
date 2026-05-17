---
name: vsf-board-run
description: |
  USE FOR: building VSF firmware, flashing to hardware, running automated test scripts over UART, or the full build-flash-test loop.
  DO NOT USE FOR: porting HAL drivers (use vsf-hal-driver).
---

# vsf-board-run

Build → flash → run test script(s). Always rebuilds.

```bash
vsf-board-run board/<board>/hardware-map.yml [script ...]
vsf-board-run --project-root <dir> <hardware-map.yml> [script ...]
```
Omit scripts for build+flash only.

## Test script

```python
def run(project_root, serial):
    ...
```

`project_root` — `Path`. Declare `la` param for logic analyzer. Optional `SCENARIOS` list: all scripts omit → all get `GO`; any defines → union gets `GO`, rest `SKIP`.

## SerialInstrument API

`send(data)`, `expect(pattern, timeout=5)` — reads until regex match, raises `TimeoutError`. `read_all(timeout=2)` — reads until silence. `expect()` preserves unconsumed data.

**Single script**: runs after gateway, live interaction. **Multi** (2+ `.py`): waits for `All test cases completed`, then post-hoc. Gateway protocol: `GATEWAY:HELLO` → negotiate `SCENARIO:<name>:READY?` → `GO`/`SKIP` → `GATEWAY:DONE`. No HELLO in 2s → skip.

## Output

`[vsf-board-run] PASS` (0) or `FAIL: <reason>` (1). Log to `logs/<ts>-<run>/vsf-board-run.jsonl`.

## Prerequisites

- vsf-bench: `pip install -e vsf.demo/vsf/test/vsf-bench`
- pyyaml, pyserial, cmake in PATH; board connected

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| Build/flash fails | Check SDK paths, debug probe, BOOTSEL mode |
| Test timeout | Verify baud rate, expected output |
| No serial data | Check `serial` port in hardware-map.yml |
| Script sees `[TEST]` output | Set `SCENARIOS=[]` or drain with `read_all(2)` |
