---
name: vsf-bench
description: Build, program, and test VSF firmware on target hardware. Requires board for program/test; build-only offline. Supports multi-stage pipelines, multi-board parallel test, and capabilities/adapters architecture.
metadata:
  version: "2.0"
  license: Apache-2.0
---

**UTILITY SKILL** — used standalone or after vsf-hal-driver changes.

USE FOR:
- Build-program-test loop
- Build-only without hardware
- Program and test on hardware
- Multi-project pipelines (bootloader → app → test)
- Multi-board parallel testing
- Hardware-interaction tests: GPIO trigger + serial send/expect + LA capture + post-process. Declarative YAML — no Python required.
- Stress/loop testing with `--repeat N` and `--set` overrides
- **Crash analysis:** halt CPU, DWARF stack unwind (GDB), fault register decode, stacked frame scan, interrupt state dump, debug capability overview
- **Live health check:** one-shot snapshot of CPU mode, registers, MSP/PSP, enabled/pending/active IRQs, DWT cycle counter
- **NVIC interrupt dump:** full vector table with peripheral names, enable/pending/active status, dynamic priority detection
- **Breakpoint debugging:** set hardware breakpoints at address/symbol, run until hit, inspect registers/variables
- **Variable inspection:** read global/static variables by name or fnmatch pattern from ELF symbol table
- **Memory inspection:** read arbitrary memory blocks via SWD
- **Multi-core support:** `--core N` for dual-core targets (BH1098H CPU0/CPU1)

DO NOT USE FOR:
- HAL driver porting (use vsf-hal-driver)
- Driver debugging (use vsf-hal-driver)
- Interactive step-by-step debugging (use IDE+GDB; vsf-bench-debug is for crash analysis and breakpoint inspection, not single-stepping)

## Concepts

- **Orchestrator flow:** `build → program → for each suite: send trigger → run script`. Trigger is `vsf-test run <suite>` over serial.
- **Hardware map:** `hardware-map.yml` defines `build.source_dir`, serial port (descriptive matching by VID/PID/serial), power config, pipelines, board-level properties (dfu_key, debug_probe), and `gpio_adapters` for FT232H/CH347 IO control.
- **Program abstraction:** `ProgramCapability` ABC replaces "flash" — DFU, SWD, and UART HCI are all programming methods. The old `FlashRunner` name is preserved as a backward-compat alias.
- **Pipeline:** Multi-stage build/program/test sequence defined in hardware-map.yml `pipelines:` section. Each stage references a project and can override the runner and flash parameters. Supports `scenario` action for inline scenario execution.
- **Pipeline（流水线）：** The one and only orchestration concept. A pipeline is a `steps:` list — every step type is equal. Supports `timeout` (global), `matrix` (parameter combinations), step-level `continue-on-error` / `max-retries` / `on-failure`.
- **Step `id`:** Optional named reference on any step, enabling `--set id.key=value` targeted overrides (e.g. `--set pair_wait.delay=3.0`).
- **`loop` block:** Repeat a block of steps N times. `--repeat N` CLI flag overrides all loop.repeat values.
- **`matrix`:** Cartesian product of parameter values — one run per combination. `--matrix key=val` filters. `${{ matrix.key }}` syntax in step params.
- **`continue-on-error`:** Step failure doesn't stop pipeline. Combined with `on-failure` for diagnostic dump steps.
- **`max-retries`:** Auto-retry a failed step N times before declaring failure.
- **`--set key=value`:** Override any step parameter from CLI without editing YAML. Global (`--set delay=2.0`) or id-targeted (`--set pair_wait.delay=3.0`).
- **Capabilities/Adapters:** `capabilities/` defines abstract interfaces (ProgramCapability, GPIO, LogicAnalyzer). `adapters/` implements them per hardware entity (DFUAdapter, FT232HAdapter, DSViewAdapter). One file per hardware device.
- **Script signature:** `def run(serial, la=None)`. Scripts validate only — orchestrator sends triggers. Exception = FAIL, normal return = PASS.
- **Output path convention:** All run artifacts go under `logs/<ts>-<board>-<pipeline>/`. Loop iterations get `runs/01/`, `runs/02/` subdirectories. Files prefixed by category: `run.log` (combined log), `test-*` (test artifacts), `la-*` (LA artifacts). No `--log-dir` or `log_dir` config — `logs/` is hardcoded.
- **Path model:** All paths are cwd-relative or absolute. No `project_root` parameter.
- **vsf-bench-debug:** Separate CLI (`vsf-bench-debug`) for crash analysis, live health checks, and breakpoint inspection. Uses pyOCD + CMSIS-DAP/J-Link probe. Requires `pip install pyocd pyelftools`. Board must have `debug_probe` in hardware-map.yml.
- **DWARF backtrace:** `backtrace` uses pyOCD's embedded GDBServer (`GDBServer(session, core=N)`) + one-shot `arm-none-eabi-gdb -batch bt` for accurate stack unwinding (tail-call, inline, MSP/PSP). Falls back to heuristic stack walk when GDB is unavailable.
- **ELF auto-discovery:** `vsf-bench-debug` resolves ELF path from `--project` → project build artifacts, or `--elf` for explicit path. ELF enables symbol resolution (function names, source locations, variable lookup).
- **Multi-core:** `--core N` selects CPU core (0=primary, 1=secondary). GDBServer auto-manages per-core ports. `live` / `intc` / `crash-dump` all support `--core`.
- **Breakpoint lifecycle:** `break` halts CPU, sets hardware breakpoint at address/symbol, resumes; CPU halts on hit. `continue` resumes from halted state. Breakpoints are cleared on session disconnect.
- **Variable inspection:** `vars` reads global/static variables from ELF symbol table. Supports fnmatch patterns (`vsf_*`, `*buffer*`). Project `debug_vars` list auto-dumps with `--project`.
- **Interrupt dump (`intc`):** Full NVIC state — vector table (VTOR), system exceptions + peripheral IRQs with names parsed from `soc_config.h`, enable/pending/active flags, dynamic priority detection (write-test IPR, read AIRCR.PRIGROUP), sorted by preempt priority. Header shows CPUID, DWT cycle counter, FPB, MPU, stack limits (MSPLIM/PSPLIM), SHCSR system handler status.
- **Live health check (`live`):** One-shot snapshot — CPUID + DWT + FPB + MPU + stack limits + SHCSR + current exception mode (IPSR) + PC/SP/LR/MSP/PSP/CONTROL + interrupt summary (enabled/active/pending counts and names).
- **Crash dump:** `crash-dump` halts CPU, DWARF unwinds stack, reads all core + fault registers, decodes CFSR/HFSR fault flags, scans stack upward to locate the hardware-stacked exception frame (R0-R3,R12,LR_fault,PC_fault,xPSR), resolves fault PC to source file:line, includes debug caps and active/pending IRQ list. Outputs JSON.

## Quickstart

```bash
# Full regression (build + program + ALL suites) — SLOW, final gate only
vsf-bench --all --hardware-map board/hardware-map.yml

# Development: build + program + single suite (fast)
vsf-bench --all --hardware-map board/hardware-map.yml --suite <name>

# Fastest iteration: test-only, no rebuild (firmware already programmed)
vsf-bench --test --hardware-map board/hardware-map.yml --suite <name>

# Individual steps (for isolating failures)
vsf-bench --build   --hardware-map board/hardware-map.yml
vsf-bench --program --hardware-map board/hardware-map.yml --board <board>
vsf-bench --test    --hardware-map board/hardware-map.yml --board <board>

# Pipeline (build + program multiple projects in sequence)
vsf-bench --pipeline <name> --board <board> --hardware-map board/hardware-map.yml

# Multi-board parallel test
vsf-bench test --all-boards --project <project> --hardware-map board/hardware-map.yml

# List available pipelines
vsf-bench --list-pipelines --hardware-map board/hardware-map.yml

# Pipeline with overrides
vsf-bench --pipeline bt_stress --board b1 --repeat 1000 --set pair_wait.delay=3.0
```

## CLI changes (v2.0)

| Old (v1.x) | New (v2.0) | Reason |
|-----------|-----------|--------|
| `--flash` | `--program` | "flash" reserved for flash memory hardware; program covers DFU/SWD/UART |
| `vsf-bench-flash` | `vsf-bench-program` | Same |
| `FlashRunner` | `ProgramCapability` (FlashRunner kept as compat alias) | capability abstraction |
| — | `--repeat N` | Override all `loop.repeat` values from CLI |
| — | `--set key=value` | Override step params without editing YAML; supports `id.key=value` |
| — | `--matrix key=val` | Filter matrix combinations from CLI |

## Example: BH1098 pipeline (build + flash)

```bash
# Build bootloader (UART) → flash → build app → program via UART
vsf-bench --pipeline uart_flash_regression --board b1 --hardware-map board/hardware-map.yml
```

## Example: BT pairing stress test pipeline

```yaml
# hardware-map.yml
pipelines:
  bt_stress:
    description: "BT 配对压力测试 — build/flash 一次 + 配对循环 100 次"
    steps:
      - build: application-standalone
      - program: application-standalone
      - power_cycle
      - wait_for:
          expect:
            - pattern: "bt_stack_ready"
              verdict: pass
            - pattern: "BT_ERROR"
              verdict: fail
          timeout: 10
      - loop:
          repeat: 100
          steps:
            - power_cycle
            - id: boot_wait
              delay: 1.0
            - wait_for:
                expect:
                  - pattern: "bt_stack_ready"
                    verdict: pass
                  - pattern: "BT_ERROR"
                    verdict: fail
                timeout: 10
            - id: trigger_pair
              gpio_set:
                level: low
                duration: 0.5
            - serial_send: "bt scan\r\n"
            - la_start: {}
            - id: pair_wait
              delay: 2.0
            - wait_for:
                expect:
                  - pattern: "PAIRED"
                    verdict: pass
                  - pattern: "DISCONNECT"
                    verdict: fail
                timeout: 30
            - la_stop: {}
            - la_decode: { baudrate: 2000000 }
            - run: "python collect.py $RUN_DIR/la-decode-*.csv"
```

```bash
# 标准运行
vsf-bench --pipeline bt_stress --board b1

# 调试：跑 5 次，配对等待时间改 3 秒
vsf-bench --pipeline bt_stress --board b1 --repeat 5 --set pair_wait.delay=3.0
```

### Step primitives reference

| Primitive | YAML | Purpose |
|-----------|------|---------|
| `build` | `- build: <project>` | Compile firmware |
| `program` | `- program: <project>` | Flash firmware |
| `power_cycle` | `- power_cycle` | Power off→0.5s→on (SmartUSBHub) |
| `power_off` | `- power_off` | Power off only |
| `power_on` | `- power_on` | Power on only |
| `delay` | `- delay: 1.0` | Sleep N seconds |
| `serial_send` | `- serial_send: "cmd\r\n"` | Send data over serial to firmware |
| `wait_for` | `- wait_for: { expect: [{pattern, verdict}], timeout: 10 }` | Multi-pattern serial match (first hit wins) |
| `gpio_set` | `- gpio_set: { level: low, duration: 0.5 }` | FT232H IO toggle |
| `la_start` / `la_stop` | `- la_start: {}` | LA capture start/stop (params inherit from board.la) |
| `la_decode` | `- la_decode: { baudrate: 2000000 }` | UART decode from .dsl |
| `run` | `- run: "python analyze.py $RUN_DIR/..."` | Shell command ($RUN_DIR env var) |
| `loop` | `- loop: { repeat: 100, steps: [...] }` | Repeat steps N times |

### Step-level control flow (any step can have these)

| Property | YAML | Purpose |
|----------|------|---------|
| `continue-on-error` | `continue-on-error: true` | Don't stop pipeline on failure |
| `max-retries` | `max-retries: 3` | Retry up to N times before declaring fail |
| `on-failure` | `on-failure: <steps>` | Steps to run only if this step fails (diagnostic dump) |

### Pipeline-level properties

| Property | YAML | Purpose |
|----------|------|---------|
| `timeout` | `timeout: 600` | Global timeout in seconds |
| `matrix` | `matrix: { board: [b1,b3], baud: [115200,2M] }` | Cartesian product — one run per combo |

## Debug Quickstart

```bash
# Install debug dependencies (once)
pip install pyocd pyelftools

# Live health check — mode, registers, IRQs, caps (no crash needed)
vsf-bench-debug live --board b1 board/

# Full NVIC dump — vector table, priorities, enable/pending/active, peripheral names
vsf-bench-debug intc --board b1 --project application-standalone board/

# Crash dump — DWARF unwind + fault frame + interrupt state + debug caps
vsf-bench-debug crash-dump --board b1 --project application-standalone board/

# Backtrace — DWARF stack unwind (GDB), falls back to heuristic
vsf-bench-debug backtrace --board b1 --project application-standalone board/

# Registers — halt CPU, dump all core registers
vsf-bench-debug regs --board b1 board/

# Memory read — read 256 bytes from address
vsf-bench-debug read --board b1 --addr 0x2000FF00 --len 256 board/

# Variable inspection — read named variable (exact match)
vsf-bench-debug vars --board b1 --project application-standalone --name vsf_trace_rx_buff board/

# Variable inspection — fnmatch pattern (all variables matching)
vsf-bench-debug vars --board b1 --project application-standalone --name "vsf_*" board/

# Breakpoint — halt, set BP at symbol, resume, wait for hit
vsf-bench-debug break --board b1 --project application-standalone main board/

# Continue — resume after manual halt or breakpoint hit
vsf-bench-debug continue --board b1 board/

# Dual-core — target CPU1 on BH1098H
vsf-bench-debug --core 1 live --board b1 board/
```

## Error handling

| Failure | Cause | Action |
|---------|-------|--------|
| Build fails | cmake/IAR/SDK missing, wrong `build.source_dir` | verify path and build tool installation |
| Program fails | board disconnected, wrong runner, handshake timeout | check USB; verify runner config; check serial port matching |
| Test timeout | baudrate mismatch, firmware not responding | verify serial port/baudrate match firmware |
| Suite not found | suite disabled in `vsf_usr_cfg.h` | check `VSF_USE_TEST_<SUITE>` is ENABLED |
| No suites listed | test framework not enabled | verify `VSF_USE_TEST = ENABLED` |
| Hardware unresponsive | USB/connection issue | verify connection and runner config |
| Multi-device DFU conflict | multiple boards in DFU mode with same VID/PID | power-cycle only target board with DFU key; assign unique serial numbers |
| Pipeline wait_for timeout | pattern not matched within timeout | check firmware log in `run.log`; increase timeout or adjust pattern regex |
| Pipeline gpio_set fails | FT232H not connected or pyftdi missing | check USB; `pip install pyftdi` |
| Pipeline la_start fails | DSLogic not connected or dsview-cli not in PATH | check USB; verify dsview-cli installation |
| Pipeline run exits non-zero | shell command returned error | check script path and arguments; verify `$RUN_DIR` is set |
| Pipeline serial_send no effect | serial port not open or firmware not listening | verify step serial_send precedes wait_for to read response |
| Debug: "No debug_probe" | board entry missing `debug_probe` in hardware-map.yml | add `debug_probe: {target: cortex_m, probe: <unique_id>}` to board config |
| Debug: pyOCD import error | pyocd not installed | `pip install pyocd` |
| Debug: pyelftools import error | pyelftools not installed | `pip install pyelftools` |
| Debug: ELF not found | no --elf/--project or artifact format mismatch | use `--elf <path>` or verify project `build.artifacts` has format "elf"/"out" |
| Debug: "Symbol not found" | function/variable name misspelled or not in ELF | check `.symtab` with `arm-none-eabi-nm`; verify name spelling |
| Debug: breakpoint not hit | address wrong or code path not executed | verify address with `arm-none-eabi-objdump -d`; check code path reached |
| Debug: probe not found | multiple CMSIS-DAP probes, no unique_id | set `debug_probe.probe` to target probe serial number |
| Debug: target locked | CPU in low-power or locked state | power-cycle board; check debug port not disabled in firmware |
| Debug: Core N not available | Multi-core target with fewer cores than requested | verify `--core` value; use `live` without `--core` to check available cores |
| Debug: DWARF backtrace fails | GDB not found or GDBServer conflict | ensure `arm-none-eabi-gdb` in PATH; check pyOCD session is closed |
| Debug: intc shows "IRQ" without peripheral name | soc_config.h not found at expected path | BH1098 only — names parsed from vendor BSP header |
| Debug: fault_frame shows "overwritten, not EXC_RETURN" | HardFault handler called subroutines (LR overwritten) | normal; decoder scans stack for real frame — check frame_offset and PC_fault |

If vsf-bench not installed: `pip install -e vsf/test/vsf_bench`.

## Regression failure isolation

When a full `--all` regression fails, **always isolate to a single suite** before blaming the driver:

```bash
# 1. Reprogram to ensure a clean device state
vsf-bench --program --hardware-map board/hardware-map.yml --board <board>

# 2. Run only the failing suite
vsf-bench --test --hardware-map board/hardware-map.yml --board <board> --suite <failed_suite>

# 3. If it passes alone, the failure is caused by suite interaction
#    (state left behind by a previous suite). Check the suite just before it.
# 4. If it still fails alone, the driver has a genuine bug — use vsf-hal-driver.
```

**Rule:** a suite that passes in isolation but fails in a full run is NOT a driver bug; it is a test-sequence/state-cleanup bug.

## Reference (optional supplementary reading)

- [concepts](modules/concepts.md) — Orchestrator flow, script signature, hardware map, capabilities/adapters
- [examples](modules/examples.md) — IO verification, single scenario, BH1098 pipeline
- [troubleshooting](modules/troubleshooting.md) — Detailed failure analysis
- [reference](modules/reference.md) — Complete CLI reference
