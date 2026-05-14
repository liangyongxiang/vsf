"""Multi-mode UART TX validation via logic analyzer capture.

Usage:
    vsf-board-run board/pico/hardware-map.yml \\
        vsf.demo/vsf/test/vsf_test/usart/scenario/test_usart_mode.py

Test parameters are read from application/component/vsf-test/test_params.yml.
Each case is decoded with matching UART parity, data-bit, and stop-bit settings.
"""

from dataclasses import dataclass
from pathlib import Path

import yaml

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument


@dataclass(frozen=True)
class Case:
    idx: int
    baud: int
    expect_pass: bool
    decode_parity: str
    decode_data: int
    decode_stop: float


def _find_project_root() -> Path:
    """Walk up from script location to find project root (contains application/component/vsf-test/)."""
    p = Path(__file__).resolve()
    while p.parent != p:
        if (p / "application" / "component" / "vsf-test").is_dir():
            return p
        p = p.parent
    raise RuntimeError("Cannot find project root (no application/component/vsf-test/)")


def _load_params(yml_path: Path) -> dict:
    """Load all test parameters from YAML."""
    return yaml.safe_load(yml_path.read_text()) or {}


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        la = case.get("la", {}) or {}
        decode = la.get("decode", {}) or {}
        cases.append(Case(
            idx=int(case["idx"]),
            baud=int(scenario["common"]["baudrate"]),
            expect_pass=bool(case.get("expect_pass", True)),
            decode_parity=decode.get("parity_type", "none"),
            decode_data=int(decode.get("num_data_bits", 8)),
            decode_stop=float(decode.get("num_stop_bits", 1.0)),
        ))
    return cases


def run(serial: SerialInstrument, la: LogicAnalyzerInstrument) -> None:
    project_root = _find_project_root()
    yml_path = project_root / "application" / "component" / "vsf-test" / "test_params.yml"
    params = _load_params(yml_path)

    scenario = params.get("tx_mode", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, f"No cases found in {yml_path}"

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

    serial.expect("All test cases completed", timeout=timeout_s)
    la.wait(timeout=timeout_s)

    marker_ch = la.channel(marker_ch_name)
    dut_ch = la.channel(dut_ch_name)
    out_dir = la.output_dir

    markers = la.decode_markers(
        channel=marker_ch,
        baudrate=marker_baud,
        pattern=marker_pattern,
        output_csv=out_dir / "mode_markers.csv",
    )

    assert len(markers) == len(cases), (
        f"Expected {len(cases)} MODE:CASE markers, got {len(markers)}: {markers}"
    )

    markers_by_case = {ev.case_idx: ev for ev in markers}

    for i, c in enumerate(cases):
        ev = markers_by_case[c.idx]
        start_ns = ev.time_ns + marker_delay_ns
        end_ns = markers_by_case[cases[i + 1].idx].time_ns if i + 1 < len(cases) else None

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
