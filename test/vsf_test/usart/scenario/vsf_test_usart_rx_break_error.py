"""USART RX break-error validation: PC asserts BRK condition on the line,
firmware detects it via VSF_USART_IRQ_MASK_BREAK_ERR.

The host opens the aux tty, waits for the firmware ":READY" marker, then
calls pyserial's send_break() to hold the TX line low for ~50 ms. PL011
emits BREAK_ERR when the line stays low past one full character frame.
"""

import time
from dataclasses import dataclass
from pathlib import Path

from vsf_bench.instruments.serial_instrument import SerialInstrument
from vsf_bench.test_params import load_test_params


@dataclass(frozen=True)
class Case:
    idx: int
    send_break_ms: int


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        host = (case.get("host", {}) or {})
        cases.append(Case(
            idx=int(case["idx"]),
            send_break_ms=int(host.get("send_break_ms", 50)),
        ))
    return cases


def run(project_root: Path, serial: SerialInstrument) -> None:
    params = load_test_params(project_root)
    scenario = params.get("rx_break_error", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    timeout_s = float(scenario.get("timeout_s", 1.5))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=115200, timeout=1)

    for c in cases:
        serial.expect(f"usart_rx_break_error:CASE:{c.idx}:READY", timeout=timeout_s)
        # Hold the line low for send_break_ms — well beyond one frame at 115200
        # baud (~87 µs per frame), so PL011 latches a break condition.
        aux.send_break(duration=c.send_break_ms / 1000.0)
        time.sleep(0.01)

    serial.expect_test_summary("usart_rx_break_error", timeout=timeout_s)
    aux.close()
    print(f"[PASS] rx_break_error: {len(cases)} case(s) completed")
