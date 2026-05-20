"""USART RX data validation: PC sends payload at 115200, firmware verifies via ASSERT.

Two-phase: `run()` per-case handshake (READY → host writes payload, firmware
reads it, emits DONE); `decode()` confirms the on-wire data matches `payload`
within each case's firmware-emitted READY → DONE window.
"""

from dataclasses import dataclass
from pathlib import Path

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


def run(project_root: Path, serial: SerialInstrument) -> None:
    params = load_test_params(project_root)
    scenario = params.get("rx_data", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    timeout_s = float(scenario.get("timeout_s", 120.0))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    payload = scenario.get("payload", "Hello VSF\r\n").encode()

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for c in cases:
        serial.expect(f"usart_rx_data:CASE:{c.idx}:READY", timeout=timeout_s)
        aux.write(payload)
        aux.flush()

    serial.expect_test_summary("usart_rx_data", timeout=timeout_s)
    aux.close()


def decode(project_root: Path, la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None) -> None:
    params = load_test_params(project_root)
    scenario = params.get("rx_data", {})
    cases = _parse_cases(scenario)

    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    dut_ch_name = scenario.get("dut", {}).get("channel", "uart1_rx")
    payload = scenario.get("payload", "Hello VSF\r\n").encode()

    marker_ch_tx = la.channel("uart0_tx")
    dut_ch = la.channel(dut_ch_name)
    out_dir = la.output_dir

    ready_markers = la.decode_markers(
        channel=marker_ch_tx,
        baudrate=marker_baud,
        pattern=r"usart_rx_data:CASE:(\d+):READY",
        output_csv=out_dir / "rx_data_ready_markers.csv",
        start_ns=decode_start_ns,
        end_ns=decode_end_ns,
    )
    done_markers = la.decode_markers(
        channel=marker_ch_tx,
        baudrate=marker_baud,
        pattern=r"usart_rx_data:CASE:(\d+):DONE",
        output_csv=out_dir / "rx_data_done_markers.csv",
        start_ns=decode_start_ns,
        end_ns=decode_end_ns,
    )
    ready_by_case = {ev.case_idx: ev for ev in ready_markers}
    done_by_case = {ev.case_idx: ev for ev in done_markers}

    full_csv = out_dir / f"rx_data_full_{marker_baud}.csv"
    la.batch_decode_uart([
        (dut_ch, marker_baud, decode_start_ns, decode_end_ns, full_csv, "none", 8, 1.0)
    ])
    rows = la.read_csv_rows(full_csv)

    for c in cases:
        if not c.expect_pass:
            print(f"[PASS] CASE {c.idx}  rx_data  expected fail")
            continue
        assert c.idx in ready_by_case, f"CASE {c.idx}: READY marker not found in LA decode"
        assert c.idx in done_by_case, f"CASE {c.idx}: DONE marker not found in LA decode"
        start_ns = ready_by_case[c.idx].time_ns
        end_ns = done_by_case[c.idx].time_ns
        got = bytes(b for t, b in rows if start_ns <= t < end_ns)
        assert got == payload, f"CASE {c.idx}: expected {payload!r}, got {got!r}"
        print(f"[PASS] CASE {c.idx}  rx_data  {got!r}")
