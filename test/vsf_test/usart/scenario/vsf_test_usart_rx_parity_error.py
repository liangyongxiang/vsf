"""USART RX parity-error validation: PC sends payload at wrong parity, firmware asserts mismatch.

Firmware configures UART1 RX for the case's parity setting (e.g. EVEN), then waits
for a payload. Host sends at the YAML-specified host send parity (typically NONE),
so the firmware sees parity errors and asserts via VSF_TEST_ASSERT.
"""

from dataclasses import dataclass
from pathlib import Path

from vsf_bench.instruments.serial_instrument import SerialInstrument
from vsf_bench.test_params import load_test_params


@dataclass(frozen=True)
class Case:
    idx: int
    host_parity: str
    host_data_bits: int
    host_stop_bits: float
    host_baud: int


def _parse_cases(scenario: dict) -> list[Case]:
    defaults = scenario.get("defaults", {}) or {}
    default_send = (defaults.get("host", {}) or {}).get("send", {}) or {}
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        host_send = (case.get("host", {}) or {}).get("send", {}) or {}
        cases.append(Case(
            idx=int(case["idx"]),
            host_parity=host_send.get("parity_type", default_send.get("parity_type", "none")),
            host_data_bits=int(host_send.get("num_data_bits", default_send.get("num_data_bits", 8))),
            host_stop_bits=float(host_send.get("num_stop_bits", default_send.get("num_stop_bits", 1.0))),
            host_baud=int(host_send.get("baudrate", default_send.get("baudrate", 115200))),
        ))
    return cases


def run(project_root: Path, serial: SerialInstrument) -> None:
    params = load_test_params(project_root)
    scenario = params.get("rx_parity_error", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    timeout_s = float(scenario.get("timeout_s", 1.5))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    payload = scenario.get("payload", "Hello VSF\r\n").encode()

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=115200, timeout=1)

    parity_map = {"none": pyserial.PARITY_NONE, "even": pyserial.PARITY_EVEN, "odd": pyserial.PARITY_ODD}

    for c in cases:
        serial.expect(f"usart_rx_parity_error:CASE:{c.idx}:READY", timeout=timeout_s)
        aux.baudrate = c.host_baud
        aux.parity = parity_map.get(c.host_parity, pyserial.PARITY_NONE)
        aux.bytesize = c.host_data_bits
        aux.stopbits = c.host_stop_bits
        aux.write(payload)
        aux.flush()

    serial.expect_test_summary("usart_rx_parity_error", timeout=timeout_s)
    aux.close()
    print(f"[PASS] rx_parity_error: {len(cases)} case(s) completed")
