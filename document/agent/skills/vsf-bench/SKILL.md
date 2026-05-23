---
name: vsf-bench
description: |
  **UTILITY SKILL** — INVOKES: none. Used standalone or after vsf-hal-driver changes.
  USE FOR: building VSF firmware, flashing to hardware, running automated test suites over UART, or the full build-flash-test loop.
  DO NOT USE FOR: porting HAL drivers (use vsf-hal-driver).
  FOR SINGLE OPERATIONS: use --build, --flash, or --test individually.
---

# vsf-bench

Build → flash → run test suites. Always rebuilds.

## Quickstart

```bash
# Full pipeline
vsf-bench --all board/<board>/hardware-map.yml

# Specific suite
vsf-bench --all board/<board>/hardware-map.yml --suite usart_baud

# Individual steps
vsf-bench --build  board/<board>/hardware-map.yml
vsf-bench --flash  board/<board>/hardware-map.yml
vsf-bench --test   board/<board>/hardware-map.yml
```

Scripts live in `vsf.demo/vsf/test/vsf_test/<peripheral>/scenario/` and are auto-discovered.

See `REFERENCE.md` for full CLI reference, SerialInstrument API, LA decode, and script signature.

## IO verification workflow

Before debugging any peripheral failure, verify wiring first:

```bash
vsf-bench --all board/<board>/hardware-map.yml --suite gpio_io_check
```

## Troubleshooting

| Symptom | Fix |
|---|---|
| Build fails | Verify cmake, SDK paths, `build.source_dir` in hardware-map.yml. |
| Test timeout | Verify board outputs expected pattern; confirm baud rate matches. |
| `Suite not found` in firmware | Suite disabled in firmware config. Use `--suite` to select only enabled suites. |
