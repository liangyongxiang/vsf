"""USART RX overflow-error validation: PC blasts a burst that exceeds the
PL011 32-byte FIFO; firmware detects it via VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR.

The firmware leaves the RX FIFO un-drained (only the OVERFLOW IRQ mask is
enabled), so a host write of 128 bytes at 115200 baud quickly overruns the
chip. The OE flag latches and the ISR sets ctx.overflow_err.
"""

import time
from dataclasses import dataclass
from pathlib import Path



@dataclass(frozen=True)
class Case:
    idx: int
    burst_size: int


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        host = (case.get("host", {}) or {})
        cases.append(Case(
            idx=int(case["idx"]),
            burst_size=int(host.get("burst_size", 128)),
        ))
    return cases


def run(project_root: Path, serial: SerialInstrument) -> None:
    params = load_test_params(project_root)
    scenario = params.get("rx_overflow_error", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    timeout_s = float(scenario.get("timeout_s", 1.5))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")

    import serial as pyserial
from vsf_bench import SerialInstrument, load_test_params
    aux = pyserial.Serial(dut_port, baudrate=115200, timeout=1)

    for c in cases:
        serial.expect(f"usart_rx_overflow_error:CASE:{c.idx}:READY", timeout=timeout_s)
        # PL011 RX FIFO is 32 bytes; 128+ bytes is plenty.
        aux.write(bytes([0xAA] * c.burst_size))
        aux.flush()
        time.sleep(0.05)

    serial.expect_test_summary("usart_rx_overflow_error", timeout=timeout_s)
    aux.close()
    print(f"[PASS] rx_overflow_error: {len(cases)} case(s) completed")
