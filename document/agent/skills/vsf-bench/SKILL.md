---
name: vsf-bench
description: |
  **UTILITY SKILL** — INVOKES: none. Used standalone or after vsf-hal-driver changes.
  USE FOR: building VSF firmware, flashing to hardware, running automated test scenes over UART, or the full build-flash-test loop.
  DO NOT USE FOR: porting HAL drivers (use vsf-hal-driver).
  FOR SINGLE OPERATIONS: use --build, --flash, or --test individually.
---

# vsf-bench

Build → flash → run test scenes. Always rebuilds.

## Quickstart

```bash
# Full pipeline
vsf-bench --all board/<board>/hardware-map.yml

# Specific scene
vsf-bench --all board/<board>/hardware-map.yml --suite usart_baud

# Individual steps
vsf-bench --build  board/<board>/hardware-map.yml
vsf-bench --flash  board/<board>/hardware-map.yml
vsf-bench --test   board/<board>/hardware-map.yml
```

Scripts live in `vsf.demo/vsf/test/vsf_test/<peripheral>/scenario/vsf_test_<scene>.py` and are auto-discovered. The orchestrator handles triggering; scripts only validate output.

New scripts can start from the template at `templates/vsf_test_template.py`.

See `REFERENCE.md` for:
- Full CLI reference, SerialInstrument API, LA decode
- Script signature (`run(project_root, serial[, la])`)
- Scene → script discovery rules
- Audit log format and troubleshooting table

## Examples

**Validate a driver change:**
```bash
vsf-bench --all board/pico/hardware-map.yml --suite usart_baud
```

**Custom script override:**
```bash
vsf-bench --all board/pico/hardware-map.yml --suite usart_baud --script my_validate.py
```

## Troubleshooting

| Symptom | Fix |
|---|---|
| Build fails | Verify cmake, SDK paths, `build.source_dir` in hardware-map.yml. |
| Test timeout | Verify board outputs expected pattern; confirm baud rate matches. |
| `Scene not found` in firmware | Scene disabled in firmware config. Use `--suite` to select only enabled scenes. |
