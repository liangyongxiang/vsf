---
name: vsf-bench
description: |
  Build, flash, and run automated test suites for VSF firmware on target hardware.
  **UTILITY SKILL** — INVOKES: none. Used standalone or after vsf-hal-driver changes.
  USE FOR: building VSF firmware, flashing to hardware, running test suites over UART, or the full build-flash-test loop. When driver code has been modified and needs verification (build→flash→test), vsf-bench is the verification step — vsf-hal-driver handles the code changes.
  DO NOT USE FOR: porting HAL drivers or modifying driver source code (use vsf-hal-driver). If only a build or flash is needed, use the standalone --build / --flash flags.
  FOR SINGLE OPERATIONS: use --build, --flash, or --test individually.
metadata:
  version: "1.0"
  license: Apache-2.0
---

# vsf-bench

Build → flash → run test suites. Always rebuilds from source (no incremental builds — ensures firmware matches current code and config).

## Quickstart

```bash
# Full pipeline
vsf-bench --all board/<board>/hardware-map.yml

# Specific suite
vsf-bench --all board/<board>/hardware-map.yml --suite usart_baud

# Specific case (by parameter value or index)
vsf-bench --all board/<board>/hardware-map.yml --suite usart_baud --case 921600
vsf-bench --all board/<board>/hardware-map.yml --suite usart_baud --case-index 7

# Individual steps
vsf-bench --build  board/<board>/hardware-map.yml   # build only
vsf-bench --flash  board/<board>/hardware-map.yml   # flash only (build dir must exist)
vsf-bench --test   board/<board>/hardware-map.yml   # test only (firmware already running)
vsf-bench --build --flash board/<board>/hardware-map.yml  # build + flash, no test
```

## Concepts

- **Orchestrator flow:** `build → flash → for each scene: send trigger → run script`. The trigger is `vsf-test run <scene>` sent over serial.
- **Script signature:** `def run(project_root: Path, serial: SerialInstrument, la: LogicAnalyzerInstrument = None) -> None`. Scripts are validation-only — they do NOT send triggers; the orchestrator handles that. If a script raises an exception, the orchestrator records a FAIL verdict and continues to the next scene. If a script returns normally (no exception), the verdict is PASS.
- **Hardware map:** `hardware-map.yml` defines `build.source_dir`, `serial.port`/`baudrate`, `flash.runner` (swd or uf2), and optional `logic_analyzer.channels`. Only hardware listed in the map is supported.
- **Flash runners:** swd (OpenOCD + CMSIS-DAP) is the active runner. Fallback uf2 flashes via `/dev/sdb1`. Runner is selected by `flash.runner` in hardware-map.yml.

## Examples

### Full build-flash-test loop after driver changes

```bash
# After modifying HAL driver code, verify with the full pipeline:
vsf-bench --all board/pico/hardware-map.yml
```

### Debugging a failing peripheral with IO verification first

```bash
# Before debugging any peripheral, rule out wiring issues:
vsf-bench --all board/<board>/hardware-map.yml --suite gpio_io_check
# If gpio_io_check passes but usart_baud fails, the issue is in the driver, not wiring.
```

### Running a single test scenario during driver development

```bash
# Build, flash, and test only the UART baud rate scene:
vsf-bench --all board/<board>/hardware-map.yml --suite usart_baud
```

## Troubleshooting

| Symptom | Likely cause | Action |
|---------|-------------|--------|
| Build fails | cmake or SDK not installed, wrong `build.source_dir` in hardware-map.yml | verify build.source_dir path; check cmake/SDK installation |
| Flash fails | board not connected, wrong runner selected | check USB cable; verify `flash.runner` in hardware-map.yml (swd vs uf2) |
| Test timeout | firmware not outputting expected pattern, baud rate mismatch | verify serial.port and baudrate in hardware-map.yml match firmware config |
| `Suite not found` | suite disabled in firmware config (`vsf_usr_cfg.h`), or CLI `--suite` name doesn't match firmware registration | check `VSF_USE_TEST_<SUITE>` is ENABLED; verify suite name spelling against scene list in firmware (`vsf-test scene --list`) |
| No scenes listed (`vsf-test scene --list` empty) | test framework not enabled, or no suites compiled in | verify `VSF_USE_TEST = ENABLED` in `vsf_usr_cfg.h`; check suite config flags |
| LA (logic analyzer) errors | channel names in hardware-map.yml don't match LA wiring | verify `logic_analyzer.channels` matches physical DSLogic connections |
| Script fails with `suite not found` | script registered under wrong scene name | check script filename matches YAML `script:` field in test params |

## Limitations

- **Hardware required for flash/test:** building works offline, but `--flash` and `--test` require a connected board. If no hardware is available, use `--build` only and flag pending hardware verification.
- **Test assumes firmware running:** `--test` alone does not flash first — firmware must already be on the board. For a cold start, use `--build --flash --test` or `--all`.
- **Hardware-map.yml required:** the tool cannot infer board config. Missing or malformed hardware-map.yml blocks all operations.
- **No incremental builds:** always clean-rebuilds to prevent stale artifacts from masking config changes.
