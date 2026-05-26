# Examples

## Full build-flash-test loop after driver changes

```bash
# After modifying HAL driver code, verify with the full pipeline:
vsf-bench --all board/pico/hardware-map.yml
```

## Debugging a failing peripheral with IO verification first

```bash
# Before debugging any peripheral, rule out wiring issues:
vsf-bench --all board/<board>/hardware-map.yml --suite gpio_io_check
# If gpio_io_check passes but usart_baud fails, the issue is in the driver, not wiring.
```

## Running a single test scenario during driver development

```bash
# Build, flash, and test only the UART baud rate scene:
vsf-bench --all board/<board>/hardware-map.yml --suite usart_baud
```
