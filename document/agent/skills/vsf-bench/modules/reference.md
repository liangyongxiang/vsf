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
# All subcommands share --board, --core, and <board_dir> positional
vsf-bench-debug <cmd> --board <name> [--core 0|1] [options] <board_dir>
```

`<board_dir>` is the directory containing `hardware-map.yml` (usually `board/`).

### Multi-core (`--core N`)

All debug commands support `--core N` (default 0) for dual-core targets like BH1098H.
Core 1 is typically held in reset until CPU0 firmware releases it.

```bash
vsf-bench-debug --core 1 live --board b1 board/     # CPU1 health check
```

### ELF auto-discovery

ELF is resolved automatically (no `--elf` needed if project configured):

1. `--elf <path>` — explicit path (highest priority)
2. `--project <name>` → `project.build.artifacts` (format: "elf" or "out") resolved relative to `build_dir`
3. Board's embedded `build` config artifacts (flat-format)

### live — system health snapshot

One-shot live state: CPUID, DWT cycle counter, FPB, MPU, stack limits, SHCSR, current exception mode (IPSR), PC/SP/LR/MSP/PSP/CONTROL, interrupt summary. **No crash needed — works on any running firmware.**

```bash
vsf-bench-debug live --board b1 board/
```

Output:
```
CPU: 0x631F1320 (impl=Arm China, part=0x132, var=1, rev=r0p15)
DWT: 0x48000001 (4 comparators, cycle counter=121562)
FPB: 0x10000080 (9 breakpoints, 1 literal)
MPU: not present
Stack limits: MSPLIM=0x00000001 PSPLIM=0x28027B14
SHCSR: 0x00000400

  Mode: PendSV (IPSR=14)
  PC=0x020080DC  SP=0x28027B14  LR=0x02008137
  MSP=0x28027B14  PSP=0x2802A2E0  CONTROL=0x00000004

  Interrupts: 14 enabled
  Active:  #14 PendSV
```

### intc — NVIC interrupt dump

Full interrupt controller state: vector table with handler addresses (resolved to function names via ELF), peripheral names from `soc_config.h`, enable/pending/active flags, dynamic priority bit detection from hardware. Header includes CPUID, DWT, FPB, MPU, stack limits, SHCSR.

```bash
vsf-bench-debug intc --board b1 --project application-standalone board/
```

Output:
```
Core: 0  VTOR: 0x20000200  NVIC: 4-bit priority (3 preempt + 1 sub)
CPUID: Arm China Star-MC1 r0p15  DWT: 4 comparators, cycle counter=7958369
FPB: 9 breakpoints  MPU: not present  Stack: MSPLIM=0x... PSPLIM=0x...
SHCSR: 0x00000400

Enabled: 14  Pending: 0  Active: 1

 IRQ Name                     Lvl Pre Sub E P A  Handler
  1 Reset                      0   0   0 . . .  Reset_Handler
  3 HardFault                  0   0   0 . . .  HardFault_Handler
 17 IRQ001 (MBOX)              0   0   0 E . .  isr_wrapper
 45 IRQ029 (SCAL)              2   1   0 E . .  __wrapped_SCAL_IRQHandler
 19 IRQ003 (I2S)              12   6   0 E . .  __wrapped_I2S_IRQHandler
 14 PendSV                    14   7   0 . . A  __wrapped_PendSV_Handler
```

Columns: `Lvl`=effective priority (0=highest), `Pre`=preempt group, `Sub`=subgroup, `E`=Enabled, `P`=Pending, `A`=Active.

### crash-dump — enhanced crash analysis

Halt CPU, DWARF unwind, fault registers, **stack scan** for hardware-stacked exception frame (R0-R3,R12,LR_fault,PC_fault,xPSR), fault PC resolved to source file:line, debug caps overview, active/pending IRQ list.

```bash
vsf-bench-debug crash-dump --board b1 --project application-standalone board/
```

Output (JSON, abridged):
```json
{
  "fault": "UNDEFINSTR",
  "pc": "0x020318E8", "sp": "0x28041880", "lr": "0x02020389",
  "cfsr": "0x00010000", "hfsr": "0x40000000",
  "regs": {"R0": "0x00000003", "SP": "0x28041880", "PC": "0x020318E8", ...},
  "stack": [{"pc": "0x020318E8", "file": "vsf_os.c", "line": 154}],
  "fault_frame": {
    "ipsr": 3, "mode": "HardFault",
    "exc_return": "LR=0x02020389 (overwritten, not EXC_RETURN)",
    "frame_offset": 208,
    "frame_at_SP": {
      "R0": "0xAAAAAAAA", "R1": "0xAAAAAAAA",
      "R12": "0x28041F30", "LR_fault": "0x0202407F",
      "PC_fault": "0x02023FE1", "xPSR": "0x00000008"
    },
    "fault_pc": "0x02023FE0",
    "fault_pc_file": "vsf_os.c", "fault_pc_line": 154
  }
}
```

When HardFault handler has pushed extra registers (R4-R11+LR), `frame_offset` reports how far the decoder had to scan upward to find the real exception frame. `PC_fault` is the actual faulting instruction address; `LR_fault` is the return address at fault time.

### backtrace — DWARF stack unwind

Uses pyOCD's embedded GDBServer (`GDBServer(session, core=N)`) + one-shot `arm-none-eabi-gdb -batch bt`. Accurately handles tail-call optimization, inline functions, MSP/PSP selection, and leaf functions. Falls back to heuristic stack walk when GDB is unavailable.

```bash
vsf-bench-debug backtrace --board b1 --project application-standalone board/
```

Output:
```
  #0: PC=0x020080DC  SP=0x00000000  LR=0x00000000  <vsf_test_busy_wait_ms>  // vsf_test.c:196
  #1: PC=0x02008136  <__read_line>  // vsf_test_shell.c:33
  #2: PC=0xAAAAAAAA  <??>
```

### regs

Halt CPU, dump all core registers (R0-R12, SP, LR, PC, XPSR).

```bash
vsf-bench-debug regs --board b1 board/
```

### read

Read a memory block and display as hexdump.

```bash
vsf-bench-debug read --board b1 --addr 0x20000000 --len 256 board/
```

### vars

Read global/static variables by name from ELF symbol table.

```bash
vsf-bench-debug vars --board b1 --project application-standalone --name "vsf_*" board/
```

### break / continue

Set hardware breakpoint at address or symbol, resume and wait for hit.

```bash
vsf-bench-debug break --board b1 --project application-standalone main board/
vsf-bench-debug continue --board b1 board/
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
