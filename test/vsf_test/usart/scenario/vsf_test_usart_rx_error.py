"""USART RX error validation: PC sends mismatched data, firmware detects errors.

Usage:
    vsf-board-run board/pico/hardware-map.yml \
        vsf.demo/vsf/test/vsf_test/usart/scenario/test_usart_rx_error.py
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

    marker_cfg = params.get("marker", {})
    marker_baud = int(marker_cfg.get("baudrate", 115200))

    aux = None
    total_cases = 0

    import serial as pyserial

    for scenario_name, marker_prefix in (
        ("rx_parity_error", "RX_PARITY"),
        ("rx_frame_error", "RX_FRAME"),
    ):
        scenario = params.get(scenario_name, {})
        cases = _parse_cases(scenario)
        if not cases:
            continue

        timeout_s = float(scenario.get("timeout_s", 120.0))
        dut = scenario.get("dut", {})
        dut_port = dut.get("port", "/dev/ttyUSB2")
        payload = scenario.get("payload", "Hello VSF\r\n").encode()

        if aux is None:
            aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

        for c in cases:
            serial.expect(f"{marker_prefix}:CASE:{c.idx}:READY", timeout=timeout_s)

            aux.write(payload)
            aux.flush()

            serial._ser.write(f"{marker_prefix}:CASE:{c.idx}:DONE\r\n".encode())
            serial._ser.flush()

        total_cases += len(cases)

    serial.expect_test_summary("usart_rx_error", timeout=120.0)
    la.stop()
    la.wait(timeout=120.0)

    if aux:
        aux.close()

    print(f"[PASS] rx_error: {total_cases} case(s) completed")
