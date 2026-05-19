"""USART RX timeout validation: PC sends partial data, firmware detects timeout.

Pure handshake — no LA decode needed because the firmware asserts on the
timeout condition internally.
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
    scenario = params.get("rx_timeout", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    timeout_s = float(scenario.get("timeout_s", 120.0))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for c in cases:
        serial.expect(f"RX_TIMEOUT:CASE:{c.idx}:READY", timeout=timeout_s)
        # Send only 2 bytes (partial data) — firmware should timeout.
        aux.write(b"AB")
        aux.flush()

    serial.expect_test_summary("usart_rx_timeout", timeout=timeout_s)
    aux.close()
    print(f"[PASS] rx_timeout: {len(cases)} case(s) completed")
