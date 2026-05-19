"""USART RX mode validation: PC sends payload at varying parity/data/stop.

Two-phase: `run()` configures the aux serial to each case's mode and writes
payload; `decode()` confirms on-wire bytes match.
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
    decode_parity: str
    decode_data: int
    decode_stop: float


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        host = (case.get("host", {}) or {}).get("send", {}) or {}
        cases.append(Case(
            idx=int(case["idx"]),
            expect_pass=bool(case.get("expect_pass", True)),
            decode_parity=host.get("parity_type", "none"),
            decode_data=int(host.get("num_data_bits", 8)),
            decode_stop=float(host.get("num_stop_bits", 1.0)),
        ))
    return cases


def run(project_root: Path, serial: SerialInstrument) -> None:
    params = load_test_params(project_root)
    scenario = params.get("rx_mode", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    timeout_s = float(scenario.get("timeout_s", 120.0))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    payload = scenario.get("payload", "0123456789\r\n").encode()

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)
    parity_map = {"none": pyserial.PARITY_NONE, "even": pyserial.PARITY_EVEN, "odd": pyserial.PARITY_ODD}

    for c in cases:
        serial.expect(f"RX_MODE:CASE:{c.idx}:READY", timeout=timeout_s)
        aux.parity = parity_map.get(c.decode_parity, pyserial.PARITY_NONE)
        aux.bytesize = c.decode_data
        aux.stopbits = c.decode_stop
        aux.write(payload)
        aux.flush()

    serial.expect_test_summary("usart_rx_mode", timeout=timeout_s)
    aux.close()


def decode(project_root: Path, la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None) -> None:
    params = load_test_params(project_root)
    scenario = params.get("rx_mode", {})
    cases = _parse_cases(scenario)

    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    dut_ch_name = scenario.get("dut", {}).get("channel", "uart1_rx")
    payload = scenario.get("payload", "0123456789\r\n").encode()

    marker_ch_tx = la.channel("uart0_tx")
    dut_ch = la.channel(dut_ch_name)
    out_dir = la.output_dir

    ready_markers = la.decode_markers(
        channel=marker_ch_tx,
        baudrate=marker_baud,
        pattern=r"RX_MODE:CASE:(\d+):READY",
        output_csv=out_dir / "rx_mode_ready_markers.csv",
        start_ns=decode_start_ns,
        end_ns=decode_end_ns,
    )
    done_markers = la.decode_markers(
        channel=marker_ch_tx,
        baudrate=marker_baud,
        pattern=r"RX_MODE:CASE:(\d+):DONE",
        output_csv=out_dir / "rx_mode_done_markers.csv",
        start_ns=decode_start_ns,
        end_ns=decode_end_ns,
    )
    ready_by_case = {ev.case_idx: ev for ev in ready_markers}
    done_by_case = {ev.case_idx: ev for ev in done_markers}

    unique_configs = sorted({(c.decode_parity, c.decode_data, c.decode_stop) for c in cases})
    for c in cases:
        assert c.idx in ready_by_case, f"CASE {c.idx}: READY marker not found in LA decode"
        assert c.idx in done_by_case, f"CASE {c.idx}: DONE marker not found in LA decode"
    config_to_csv = {
        cfg: out_dir / f"rx_mode_full_{cfg[0]}_{cfg[1]}_{cfg[2]}.csv"
        for cfg in unique_configs
    }
    la.batch_decode_uart([
        (dut_ch, marker_baud, decode_start_ns, decode_end_ns,
         config_to_csv[(p, d, s)], p, d, s)
        for (p, d, s) in unique_configs
    ])

    for c in cases:
        start_ns = ready_by_case[c.idx].time_ns
        end_ns = done_by_case[c.idx].time_ns
        csv_path = config_to_csv[(c.decode_parity, c.decode_data, c.decode_stop)]
        rows = la.read_csv_rows(csv_path)
        got = bytes(b for t, b in rows if start_ns <= t < end_ns)
        assert got == payload, (
            f"CASE {c.idx} mode={c.decode_parity}/{c.decode_data}/{c.decode_stop}: "
            f"expected {payload!r}, got {got!r}"
        )
        print(f"[PASS] CASE {c.idx}  mode={c.decode_parity}/{c.decode_data}/{c.decode_stop}  {got!r}")
