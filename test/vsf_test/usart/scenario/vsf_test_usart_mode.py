"""Multi-mode UART TX validation via logic analyzer capture.

Usage:
    vsf-board-run board/pico/hardware-map.yml \\
        vsf.demo/vsf/test/vsf_test/usart/scenario/test_usart_mode.py

Test parameters are read from application/component/vsf-test/test_params.yml.
Each case is decoded with matching UART parity, data-bit, and stop-bit settings.
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

def run(project_root: Path, serial: SerialInstrument, la: LogicAnalyzerInstrument) -> None:
    params = load_test_params(project_root)

    scenario = params.get("tx_mode", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    # Global marker config
    marker_cfg = params.get("marker", {})
    marker_ch_name = marker_cfg.get("channel", "uart0_tx")
    marker_baud = int(marker_cfg.get("baudrate", 115200))
    marker_pattern = r"MODE:CASE:(\d+)"
    marker_delay_ms = int(marker_cfg.get("delay_ms", 200))
    marker_delay_ns = marker_delay_ms * 1_000_000

    # Scenario-specific config
    dut_ch_name = scenario.get("dut", {}).get("channel", "uart1_tx")
    payload = scenario.get("payload", "Hello VSF\r\n").encode()
    timeout_s = float(scenario.get("timeout_s", 120.0))

    # Wait for firmware completion, then finalize the per-scene LA capture.
    serial.expect_test_summary("usart_mode", timeout=timeout_s)
    la.stop()
    la.wait(timeout=120.0)

    marker_ch = la.channel(marker_ch_name)
    dut_ch = la.channel(dut_ch_name)
    out_dir = la.output_dir

    markers = la.decode_markers(
        channel=marker_ch,
        baudrate=marker_baud,
        pattern=marker_pattern,
        output_csv=out_dir / "mode_markers.csv",
    )

    markers_by_case = {ev.case_idx: ev for ev in markers}

    for i, c in enumerate(cases):
        if c.idx not in markers_by_case:
            continue  # case was not run (e.g. single-case selection)
        ev = markers_by_case[c.idx]
        start_ns = ev.time_ns + marker_delay_ns
        end_ns = markers_by_case[cases[i + 1].idx].time_ns if i + 1 < len(cases) and cases[i + 1].idx in markers_by_case else None

        csv_path = out_dir / f"mode_{c.idx:02d}_{c.decode_parity}{c.decode_data}{c.decode_stop}.csv"
        la.decode_uart(
            dut_ch, c.baud, start_ns, end_ns, csv_path,
            parity_type=c.decode_parity,
            num_data_bits=c.decode_data,
            num_stop_bits=c.decode_stop,
        )
        got = la.parse_uart_csv(csv_path)
        assert got == payload, (
            f"CASE {c.idx} mode={c.decode_parity}/{c.decode_data}/{c.decode_stop}: "
            f"expected {payload!r}, got {got!r}"
        )
        print(f"[PASS] CASE {c.idx}  mode={c.decode_parity}/{c.decode_data}/{c.decode_stop}  {got!r}")
