"""USART RX IRQ validation: PC sends payload, firmware receives via ISR.

Pure handshake — no LA decode needed because the firmware asserts on the
received bytes internally.
"""

from dataclasses import dataclass
from pathlib import Path

from vsf_bench.instruments.serial_instrument import SerialInstrument
from vsf_bench.test_params import load_test_params


@dataclass(frozen=True)
class Case:
    idx: int
    expect_pass: bool


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        cases.append(Case(
            idx=int(case["idx"]),
            expect_pass=bool(case.get("expect_pass", True)),
        ))
    return cases


def run(project_root: Path, serial: SerialInstrument) -> None:
    params = load_test_params(project_root)
    scenario = params.get("rx_irq", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    timeout_s = float(scenario.get("timeout_s", 120.0))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    payload = scenario.get("payload", "Hello VSF\r\n").encode()

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for c in cases:
        serial.expect(f"usart_rx_irq:CASE:{c.idx}:READY", timeout=timeout_s)
        aux.write(payload)
        aux.flush()

    serial.expect_test_summary("usart_rx_irq", timeout=timeout_s)
    aux.close()
    print(f"[PASS] rx_irq: {len(cases)} case(s) completed")
