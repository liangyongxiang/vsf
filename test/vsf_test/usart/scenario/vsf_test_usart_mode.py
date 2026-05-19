"""Multi-mode UART TX validation via logic analyzer capture.

Two-phase: `run()` waits for firmware completion; `decode()` walks the shared
LA capture and validates each case's UART frame at its specific parity / data /
stop configuration.
"""

from dataclasses import dataclass
from pathlib import Path

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument
from vsf_bench.test_params import load_test_params


@dataclass(frozen=True)
class Case:
    idx: int
    baud: int
    expect_pass: bool
    decode_parity: str
    decode_data: int
    decode_stop: float


def _parse_cases(scenario: dict) -> list[Case]:
    defaults = scenario.get("defaults", {}) or {}
    default_baud = int(defaults.get("baudrate", 115200))
    default_expect = bool(defaults.get("expect_pass", True))

    cases: list[Case] = []
    for case in scenario.get("cases", []):
        la = case.get("la", {}) or {}
        decode = la.get("decode", {}) or {}
        host = case.get("host", {}) or {}
        send = host.get("send", {}) or {}
        cases.append(Case(
            idx=int(case["idx"]),
            baud=int(send.get("baudrate", default_baud)),
            expect_pass=bool(case.get("expect_pass", default_expect)),
            decode_parity=decode.get("parity_type", "none"),
            decode_data=int(decode.get("num_data_bits", 8)),
            decode_stop=float(decode.get("num_stop_bits", 1.0)),
        ))
    return cases


def run(project_root: Path, serial: SerialInstrument) -> None:
    params = load_test_params(project_root)
    scenario = params.get("tx_mode", {})
    timeout_s = float(scenario.get("timeout_s", 120.0))
    serial.expect_test_summary("usart_mode", timeout=timeout_s)


def decode(project_root: Path, la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None) -> None:
    params = load_test_params(project_root)
    scenario = params.get("tx_mode", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    marker_cfg = params.get("marker", {})
    marker_ch_name = marker_cfg.get("channel", "uart0_tx")
    marker_baud = int(marker_cfg.get("baudrate", 115200))
    marker_delay_ms = int(marker_cfg.get("delay_ms", 200))
    marker_delay_ns = marker_delay_ms * 1_000_000

    dut_ch_name = scenario.get("dut", {}).get("channel", "uart1_tx")
    payload = scenario.get("payload", "Hello VSF\r\n").encode()

    marker_ch = la.channel(marker_ch_name)
    dut_ch = la.channel(dut_ch_name)
    out_dir = la.output_dir

    markers = la.decode_markers(
        channel=marker_ch,
        baudrate=marker_baud,
        pattern=r"MODE:CASE:(\d+)",
        output_csv=out_dir / "mode_markers.csv",
        start_ns=decode_start_ns,
        end_ns=decode_end_ns,
    )
    markers_by_case = {ev.case_idx: ev for ev in markers}

    unique_configs = sorted({(c.baud, c.decode_parity, c.decode_data, c.decode_stop) for c in cases})
    for c in cases:
        assert c.idx in markers_by_case, f"CASE {c.idx}: marker not found in LA decode"
    config_to_csv = {
        cfg: out_dir / f"mode_full_{cfg[1]}_{cfg[2]}_{cfg[3]}.csv"
        for cfg in unique_configs
    }
    if unique_configs:
        la.batch_decode_uart([
            (dut_ch, baud, decode_start_ns, decode_end_ns,
             config_to_csv[(baud, par, data, stop)], par, data, stop)
            for (baud, par, data, stop) in unique_configs
        ])

    for i, c in enumerate(cases):
        ev = markers_by_case[c.idx]
        start_ns = ev.time_ns + marker_delay_ns
        end_ns = markers_by_case[cases[i + 1].idx].time_ns if i + 1 < len(cases) and cases[i + 1].idx in markers_by_case else None

        csv_path = config_to_csv[(c.baud, c.decode_parity, c.decode_data, c.decode_stop)]
        rows = la.read_csv_rows(csv_path)
        got = bytes(b for t, b in rows if start_ns <= t and (end_ns is None or t < end_ns))
        assert got == payload, (
            f"CASE {c.idx} mode={c.decode_parity}/{c.decode_data}/{c.decode_stop}: "
            f"expected {payload!r}, got {got!r}"
        )
        print(f"[PASS] CASE {c.idx}  mode={c.decode_parity}/{c.decode_data}/{c.decode_stop}  {got!r}")
