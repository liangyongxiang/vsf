"""USART RX timeout validation: PC sends partial data, firmware detects timeout.

Usage:
    vsf-board-run board/pico/hardware-map.yml \
        vsf.demo/vsf/test/vsf_test/usart/scenario/test_usart_rx_timeout.py
"""

from dataclasses import dataclass
from pathlib import Path

import serial
import yaml

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument


@dataclass(frozen=True)
class Case:
    idx: int
    expect_pass: bool


def _find_project_root() -> Path:
    p = Path(__file__).resolve()
    while p.parent != p:
        if (p / "application" / "component" / "vsf-test").is_dir():
            return p
        p = p.parent
    raise RuntimeError("Cannot find project root")


def _load_params(yml_path: Path) -> dict:
    return yaml.safe_load(yml_path.read_text()) or {}


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        cases.append(Case(
            idx=int(case["idx"]),
            expect_pass=bool(case.get("expect_pass", True)),
        ))
    return cases


def run(serial: SerialInstrument, la: LogicAnalyzerInstrument) -> None:
    project_root = _find_project_root()
    yml_path = project_root / "application" / "component" / "vsf-test" / "test_params.yml"
    params = _load_params(yml_path)

    scenario = params.get("rx_timeout", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, f"No cases found in {yml_path}"

    marker_cfg = params.get("marker", {})
    marker_baud = int(marker_cfg.get("baudrate", 115200))
    timeout_s = float(scenario.get("timeout_s", 120.0))

    dut = scenario.get("dut", {})
    dut_port = dut.get("port", "/dev/ttyUSB2")

    aux = serial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for c in cases:
        serial.expect(f"RX_TIMEOUT:CASE:{c.idx}:READY", timeout=timeout_s)

        # Send only 2 bytes (partial data) — firmware should timeout
        aux.write(b"AB")
        aux.flush()

        serial._ser.write(f"RX_TIMEOUT:CASE:{c.idx}:DONE\r\n".encode())
        serial._ser.flush()

    serial.expect("All test cases completed", timeout=timeout_s)
    la.wait(timeout=120.0)

    aux.close()

    print(f"[PASS] rx_timeout: {len(cases)} case(s) completed")
