"""usart_rx_bulk_irq scenario: RX IRQ-driven sustained bulk transfer.

Host sends a large incrementing-counter payload via aux UART. Firmware
receives via RX IRQ handler and asserts every byte matches.

Requires the aux serial fixture: host drives /dev/ttyUSB0 → Pico UART1 RX.
"""

from pathlib import Path


SCENARIOS = ["usart_rx_bulk_irq"]


def _gen_pattern(size: int) -> bytes:
    """Incrementing-counter pattern: byte[i] = i & 0xFF."""
    return bytes(i & 0xFF for i in range(size))


def run(project_root: Path, serial: SerialInstrument) -> None:
    params = load_test_params(project_root)
    scenario = params.get("rx_bulk_irq", {})
    cases = scenario.get("cases", [])
    if not cases:
        return

    timeout_s = float(scenario.get("timeout_s", 30.0))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))

    import serial as pyserial
from vsf_bench import SerialInstrument, load_test_params
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for case in cases:
        idx = int(case["idx"])
        sz = int(case.get("data_size_bytes", 1024))
        serial.expect(f"usart_rx_bulk_irq:CASE:{idx}:READY", timeout=timeout_s)
        payload = _gen_pattern(sz)
        aux.write(payload)
        aux.flush()

    serial.expect_test_summary("usart_rx_bulk_irq", timeout=timeout_s)
    aux.close()
