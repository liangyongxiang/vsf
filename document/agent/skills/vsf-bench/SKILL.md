---
name: vsf-bench
description: Build, flash, and run automated test suites for VSF firmware on target hardware. Requires board for flash/test; build-only offline. Delegates driver changes to vsf-hal-driver.
metadata:
  version: "1.0"
  license: Apache-2.0
---

**UTILITY SKILL** — used standalone or after vsf-hal-driver changes.

USE FOR:
- Build-flash-test loop
- Build-only without hardware
- Flash and test on hardware

DO NOT USE FOR:
- HAL driver porting (use vsf-hal-driver)
- Driver debugging (use vsf-hal-driver)
- Modifying test scripts (use write-a-skill)

## Concepts

- **Orchestrator flow:** `build → flash → for each scene: send trigger → run script`. Trigger is `vsf-test run <scene>` over serial.
- **Hardware map:** `hardware-map.yml` defines `build.source_dir`, `serial.port`/`baudrate`, `flash.runner` (swd or uf2).
- **Script signature:** `def run(project_root, serial, la=None)`. Scripts validate only — orchestrator sends triggers. Exception = FAIL, normal return = PASS.

## Quickstart

```bash
# Full pipeline (build + flash + test)
vsf-bench --all board/<board>/hardware-map.yml

# Specific suite
vsf-bench --all board/<board>/hardware-map.yml --suite <name>

# Individual steps (for isolating failures)
vsf-bench --build  board/<board>/hardware-map.yml
vsf-bench --flash  board/<board>/hardware-map.yml
vsf-bench --test   board/<board>/hardware-map.yml
```

## Example: Full loop after driver changes

```bash
# Build, flash, and run all test suites:
vsf-bench --all board/pico/hardware-map.yml

# If build succeeds but flash fails: stop — do not test
# If flash succeeds but test fails: check logs for timeout, LA, or driver bug
#   - Timeout/LA → check hardware-map.yml serial/config
#   - Driver bug → run `Skill("vsf-hal-driver")` with test logs attached
```

## Error handling

| Failure | Cause | Action |
|---------|-------|--------|
| Build fails | cmake/SDK missing, wrong `build.source_dir` | verify path and cmake installation |
| Flash fails | board disconnected, wrong runner | check USB; verify `flash.runner` (swd vs uf2) |
| Test timeout | baudrate mismatch, firmware not responding | verify `serial.port`/`baudrate` match firmware |
| Suite not found | suite disabled in `vsf_usr_cfg.h` | check `VSF_USE_TEST_<SUITE>` is ENABLED |
| No scenes listed | test framework not enabled | verify `VSF_USE_TEST = ENABLED` |
| Hardware unresponsive | USB/connection issue | verify connection and `flash.runner` |

If vsf-bench not installed: `pip install -e vsf.demo/vsf/test/vsf_bench`.

## Limitations

- **Hardware required for flash/test:** `--flash` and `--test` need a connected board. Use `--build` only if no hardware.
- **Test assumes firmware running:** `--test` alone does not flash first. For cold start, use `--all`.
- **hardware-map.yml required:** tool cannot infer board config. If missing, copy from `board/pico/hardware-map.yml` as template and edit for your board.
- **No incremental builds:** always clean-rebuilds.

## Reference (optional supplementary reading)

- [concepts](modules/concepts.md) — Orchestrator flow, script signature, hardware map, flash runners
- [examples](modules/examples.md) — IO verification, single scenario
- [troubleshooting](modules/troubleshooting.md) — Detailed failure analysis
- [reference](modules/reference.md) — Complete reference docs
