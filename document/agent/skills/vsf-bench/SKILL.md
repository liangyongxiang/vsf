---
name: vsf-bench
description: Build, flash, and run automated test suites for VSF firmware on target hardware. Handles build-flash-test loop over UART with hardware-map driven config. Delegates driver changes to vsf-hal-driver.
metadata:
  version: "1.0"
  license: Apache-2.0
---

**UTILITY SKILL** — INVOKES: none. Used standalone or after vsf-hal-driver changes.

USE FOR:
- Building VSF firmware
- Flashing to hardware
- Running test suites over UART
- Full build-flash-test loop

DO NOT USE FOR:
- Porting HAL drivers or modifying driver source code (use vsf-hal-driver)
- For single operations only, use `--build`, `--flash`, or `--test` individually

## Quickstart

```bash
# Full pipeline
vsf-bench --all board/<board>/hardware-map.yml

# Specific suite
vsf-bench --all board/<board>/hardware-map.yml --suite <name>

# Individual steps
vsf-bench --build  board/<board>/hardware-map.yml
vsf-bench --flash  board/<board>/hardware-map.yml
vsf-bench --test   board/<board>/hardware-map.yml
```

## Examples

Common scenarios: full build-flash-test loop, IO verification before debugging, single scenario run. See [examples](modules/examples.md).

## Limitations

- **Hardware required for flash/test:** `--flash` and `--test` need a connected board. Use `--build` only if no hardware.
- **Test assumes firmware running:** `--test` alone does not flash first. For cold start, use `--all`.
- **hardware-map.yml required:** tool cannot infer board config.
- **No incremental builds:** always clean-rebuilds.

## MCP Integration

No MCP tools required. This skill operates entirely through the `vsf-bench` CLI.

## Reference

- [concepts](modules/concepts.md) — Orchestrator flow, script signature, hardware map, flash runners
- [examples](modules/examples.md) — Full loop, IO verification, single scenario
- [troubleshooting](modules/troubleshooting.md) — Build/flash/test failures, suite not found, LA errors
- [reference](modules/reference.md) — Detailed reference docs
