"""USART RX timeout validation: PC sends partial data, firmware detects timeout.

Usage:
    vsf-board-run board/pico/hardware-map.yml \
        vsf.demo/vsf/test/vsf_test/usart/scenario/test_usart_rx_timeout.py
"""

from dataclasses import dataclass
from pathlib import Path

import serial
from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
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

def run(project_root: Path, serial: SerialInstrument, la: LogicAnalyzerInstrument) -> None:
    params = load_test_params(project_root)

    scenario = params.get("rx_timeout", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, 'No cases found in test_params'

    marker_cfg = params.get("marker", {})
    marker_baud = int(marker_cfg.get("baudrate", 115200))
    timeout_s = float(scenario.get("timeout_s", 120.0))

    dut = scenario.get("dut", {})
    dut_port = dut.get("port", "/dev/ttyUSB2")

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for c in cases:
        serial.expect(f"RX_TIMEOUT:CASE:{c.idx}:READY", timeout=timeout_s)

        # Send only 2 bytes (partial data) — firmware should timeout
        aux.write(b"AB")
        aux.flush()

        serial._ser.write(f"RX_TIMEOUT:CASE:{c.idx}:DONE\r\n".encode())
        serial._ser.flush()

    serial.expect_test_summary("usart_rx_timeout", timeout=timeout_s)
    la.stop()
    la.wait(timeout=120.0)

    aux.close()

    print(f"[PASS] rx_timeout: {len(cases)} case(s) completed")
