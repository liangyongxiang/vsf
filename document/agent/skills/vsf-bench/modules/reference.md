# vsf-bench Reference

## CLI Interface

Tool name: **`vsf-bench`**

```bash
# Full pipeline: build + program + test all suites
vsf-bench --all --hardware-map board/hardware-map.yml

# Run specific suite (all cases)
vsf-bench --all --hardware-map board/hardware-map.yml --suite usart_baud

# Run specific case by parameter value
vsf-bench --all --hardware-map board/hardware-map.yml --suite usart_baud --case 921600

# Run specific case by index (fallback)
vsf-bench --all --hardware-map board/hardware-map.yml --suite usart_baud --case-index 7

# Run multiple suites
vsf-bench --all --hardware-map board/hardware-map.yml --suite usart_baud --suite usart_mode

# Individual steps
vsf-bench --build  --hardware-map board/hardware-map.yml
vsf-bench --program  --hardware-map board/hardware-map.yml --board b1
vsf-bench --test     --hardware-map board/hardware-map.yml --board b1

# Pipeline (the one and only orchestration concept)
vsf-bench --pipeline uart_flash_regression --board b1 --hardware-map board/hardware-map.yml

# Pipeline with overrides
vsf-bench --pipeline bt_stress --board b1 --repeat 1000 --set pair_wait.delay=3.0
```

## Pipeline: the unified orchestration model

Pipeline is the **only** concept. Every step type is equal — no "deployment" vs "test" distinction.

### Step Primitives

| Primitive | YAML | Parameters |
|-----------|------|-----------|
| `build` | `- build: <project>` | project name (from hardware-map.yml) |
| `program` | `- program: <project>` | project name |
| `power_cycle` | `- power_cycle` | — |
| `power_off` | `- power_off` | — |
| `power_on` | `- power_on` | — |
| `delay` | `- delay: 1.0` | float (seconds) |
| `serial_send` | `- serial_send: "cmd\r\n"` | string (sent over serial to firmware) |
| `wait_for` | `- wait_for: { expect: [...], timeout: 10 }` | `expect: [{pattern: regex, verdict: pass\|fail}], timeout: float` |
| `gpio_set` | `- gpio_set: { level: low, duration: 0.5 }` | `level: high\|low`, `duration?: float` |
| `la_start` | `- la_start: {}` | `channel?: str` (inherits board.la) |
| `la_stop` | `- la_stop: {}` | — |
| `la_decode` | `- la_decode: { baudrate: 2000000 }` | `baudrate: int`, `channel?: str` |
| `run` | `- run: "python analyze.py $RUN_DIR/..."` | string (shell cmd; `$RUN_DIR` env var set) |
| `loop` | `- loop: { repeat: 100, steps: [...] }` | `repeat: int`, `steps: [step]` |

### Step `id` and `--set` overrides

Any step can have an optional `id`:

```yaml
steps:
  - id: boot_wait
    delay: 1.0
  - id: trigger_pair
    gpio_set:
      level: low
      duration: 0.5
```

```bash
# Global override: all matching keys
vsf-bench --pipeline bt_stress --board b1 --set delay=2.0

# ID-targeted override
vsf-bench --pipeline bt_stress --board b1 --set pair_wait.delay=3.0
```

### `--repeat` override

```bash
# Override all loop.repeat values
vsf-bench --pipeline bt_stress --board b1 --repeat 5
```

### wait_for semantics

- `expect` list is ordered — **first matching pattern wins**
- `verdict: pass` → step succeeds, continue
- `verdict: fail` → step fails, pipeline stops
- `timeout` with no match → fail

### LA parameter inheritance

- `la_start` / `la_decode`: device, samplerate, channels inherit from `board.logic_analyzer`
- Step params only override differences
- `la_decode.baudrate` must be explicitly declared

### GPIO

- References `hardware-map.yml` `gpio_adapters` section
- `adapter` must match a key in `gpio_adapters`

## Output Path Convention

```
logs/<YYYYMMDD>-<HHMMSS>-<board>-<pipeline>/
  ├── run.log              ← TeeLogger combined log (host + firmware)
  ├── test-events.jsonl    ← Test suite audit + verdict
  ├── test-report.junit.xml ← JUnit XML
  ├── la-CH8.dsl           ← LA capture (non-loop)
  ├── la-serial.log        ← LA serial output
  ├── la-decode-CH8-<baud>.csv ← UART decode
  └── runs/                ← Loop iteration subdirectories
      ├── 01/
      │   ├── la-CH8.dsl
      │   ├── la-serial.log
      │   └── la-decode-CH8-<baud>.csv
      ├── 02/
      │   └── ...
      └── ...
```

- `logs/` is hardcoded — no `--log-dir`, no `log_dir` in config
- Directory name is self-describing: timestamp + board + pipeline
- Files prefixed by category: `test-`, `la-`, `run.`

## Orchestrator Behavior

```
build → program → for each suite: send trigger → run script
```

Trigger commands sent to firmware:
- No `--suite` → `vsf-test run all` (filtered to firmware-known suites)
- `--suite usart_baud` → `vsf-test run usart_baud`
- `--suite usart_baud --case 921600` → `vsf-test run usart_baud.7` (resolved from YAML)

## Script Behavior

```python
def run(serial: SerialInstrument, test_params_yml: str) -> None:
    params = load_test_params(test_params_yml)
    serial.expect_test_summary("gpio_toggle")
```

For scripts that need LA decode:

```python
def run(serial: SerialInstrument, la: LogicAnalyzer | None = None) -> None:
    serial.expect_test_summary("usart_baud", timeout=120.0)

def decode(la: LogicAnalyzer,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None) -> None:
    # decode markers, assert results...
```

## SerialInstrument API

`send(data)` — writes to serial, logs to audit log.

`expect(pattern, timeout=5.0)` — reads until regex matches accumulated buffer, returns matched line. Raises `TimeoutError` on timeout.

`expect_any(patterns: list[tuple[str, str]], timeout: float)` — reads until any pattern matches. Returns `(index, matched_line)`. `patterns[i] = (regex, verdict)` where verdict is `"pass"` or `"fail"`. Raises `TimeoutError` on timeout.

`read_all(timeout=2.0)` — reads until no new data arrives for `timeout` seconds.

`expect_test_summary(name, timeout=30.0)` — waits for test completion, parses Pass/Fail/Skip summary.

## Logic Analyzer

LA is auto-detected: if a script defines `decode()`, the orchestrator treats it as LA-needing.

Capture modes (`--la-mode`):
- `shared` (default): one continuous capture for the entire run
- `per-suite`: one capture per suite

Hardware config: `board.logic_analyzer` in hardware-map.yml.

### Installing dsview-cli

Download from https://github.com/liangyongxiang/DSView/releases, extract to `%LOCALAPPDATA%\DSView\`, add directory to `PATH`.

## vsf-bench-debug CLI

Tool name: **`vsf-bench-debug`**

Hardware requirement: CMSIS-DAP or J-Link probe connected to board SWD pins.
Dependencies: `pip install pyocd pyelftools`

```bash
# All subcommands share --board and <board_dir> positional
vsf-bench-debug <cmd> --board <name> [options] <board_dir>
```

`<board_dir>` is the directory containing `hardware-map.yml` (usually `board/`).

### ELF auto-discovery

ELF is resolved automatically (no `--elf` needed if project configured):

1. `--elf <path>` — explicit path (highest priority)
2. `--project <name>` → `project.build.artifacts` (format: "elf" or "out") resolved relative to `build_dir`
3. Board's embedded `build` config artifacts (flat-format)

### crash-dump

Halt CPU, capture full crash context, output JSON, resume.

```bash
vsf-bench-debug crash-dump --board b1 --project application-standalone board/
vsf-bench-debug crash-dump --board b1 --elf build/app.out board/
```

Output (JSON):
```json
{
  "fault": "HARD_FAULT",
  "pc": "0x08001234",
  "sp": "0x2000FF00",
  "lr": "0x08001000",
  "cfsr": "0x00008200",
  "hfsr": "0x40000000",
  "mmfar": "0x00000000",
  "bfar": "0xE000ED38",
  "regs": {"R0": "0x00000000", ...},
  "stack": [{"pc": "0x08001234", "function": "fault_handler", "file": "main.c", "line": 42}, ...],
  "pc_func": "HardFault_Handler",
  "lr_func": "main",
  "assertion": {"caller_func": "gpio_init", "file": "gpio.c", "line": 128, "expression": "port != NULL"}
}
```

When PC is in `vsf_trace_assert` and no hardware fault is active, `fault` = `"Assertion"` with
assertion context extracted automatically (caller_func, file, line, func, expression).

### backtrace

Halt CPU, walk call stack with symbol resolution, resume.

```bash
vsf-bench-debug backtrace --board b1 --project application-standalone board/
```

Output:
```
[vsf-bench-debug] ELF: build/application-standalone.out
  #0: PC=0x08001234  SP=0x2000FF00  LR=0x08001001  <HardFault_Handler>  // startup.c:156
  #1: PC=0x08001000  LR=0x08000801  <main>  // main.c:42
  #2: PC=0x08000800  <Reset_Handler>
```

Frame #0 shows live PC/SP/LR. Subsequent frames show PC decoded from LR on stack.

### regs

Halt CPU, dump all core registers (R0-R12, SP, LR, PC, XPSR) with optional symbol resolution.

```bash
vsf-bench-debug regs --board b1 --project application-standalone board/
```

Output:
```
[vsf-bench-debug] ELF: build/application-standalone.out
  R0   = 0x20001000
  R1   = 0x00000001
  ...
  SP   = 0x2000FF00
  LR   = 0x08001001  <main+0x10>
  PC   = 0x08001234  <HardFault_Handler>
  XPSR = 0x61000000
```

### read

Read a memory block and display as hexdump.

```bash
vsf-bench-debug read --board b1 --addr 0x20000000 --len 256 board/
```

Output (hexdump with ASCII sidebar):
```
  0x20000000: 48 65 6C 6C 6F 00 00 00  01 00 00 00 00 00 00 00  Hello...........
  0x20000010: ...
```

`--addr` accepts hex (`0x...`) or decimal. `--len` defaults to 256.

### vars

Read global/static variables by name from ELF symbol table. Requires ELF.

```bash
# Single variable (exact name)
vsf-bench-debug vars --board b1 --project application-standalone --name vsf_trace_rx_buff board/

# Multiple variables
vsf-bench-debug vars --board b1 --project application-standalone --name rx_buff --name tx_buff board/

# fnmatch pattern (wildcards *, ?)
vsf-bench-debug vars --board b1 --project application-standalone --name "vsf_*" board/
vsf-bench-debug vars --board b1 --project application-standalone --name "*buffer*" board/

# With project (auto-includes project.debug_vars list)
vsf-bench-debug vars --board b1 --project application-standalone board/
```

Output per variable:
```
  vsf_trace_rx_buff @ 0x20001000  (128 bytes)
        raw: 48656c6c6f...
      uint8: 72  uint16: 25960  uint32: 1819043144  uint64: 7694652266186761800
    pointer: 0x20001000
     string: "Hello VSF"
```

### break

Halt CPU, set hardware breakpoint at address or symbol, resume and wait for hit.

```bash
# By symbol (requires ELF)
vsf-bench-debug break --board b1 --project application-standalone main board/
vsf-bench-debug break --board b1 --project application-standalone gpio_isr board/

# By hex address
vsf-bench-debug break --board b1 0x08001234 board/
```

Output:
```
[vsf-bench-debug] Halted at 0x08000800
[vsf-bench-debug] Breakpoint at 0x08001000 <main>
[vsf-bench-debug] ELF: build/application-standalone.out
```

Breakpoints are hardware breakpoints (set via pyOCD). They persist until the DebugSession
disconnects. After a breakpoint hit, use `regs`, `vars`, `read`, or `backtrace` to inspect
state (each opens a new session).

### continue

Resume CPU from halted state (after manual halt via other tool, or after breakpoint hit).

```bash
vsf-bench-debug continue --board b1 board/
```

Output:
```
[vsf-bench-debug] Resumed
```

### debug_probe configuration (hardware-map.yml)

```yaml
boards:
  - name: b1
    platform: bh1098h
    # ...
    debug_probe:
      target: cortex_m       # pyOCD target name
      probe: "5AB3006412"    # optional probe unique_id (disambiguates multiple probes)
```

### debug_vars configuration (hardware-map.yml)

```yaml
projects:
  application-standalone:
    # ...
    debug_vars:              # auto-dumped with vsf-bench-debug vars --project
      - vsf_trace_rx_buff
      - g_active_conn
      - vsf_bt_state
```
