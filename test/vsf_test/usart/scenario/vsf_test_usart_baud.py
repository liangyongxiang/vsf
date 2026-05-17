"""Multi-baud UART TX validation via logic analyzer capture.

Usage:
    vsf-board-run board/pico/hardware-map.yml \\
        vsf.demo/vsf/test/vsf_test/usart/scenario/test_usart_baud.py

Test parameters are read from application/component/vsf-test/test_params.yml
so the script does not depend on firmware output for correctness.
"""

from dataclasses import dataclass
from pathlib import Path

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument
from vsf_bench.test_params import load_test_params

SCENARIOS = ["usart_baud"]

RP2040_CLK_PERI = 125_000_000
MIN_BAUDRATE = RP2040_CLK_PERI // (16 * 65535)
MAX_BAUDRATE = RP2040_CLK_PERI // 16

def _expect_pass(baud: int) -> bool:
    return baud != 0 and MIN_BAUDRATE <= baud <= MAX_BAUDRATE

@dataclass(frozen=True)
class Case:
    idx: int
    baud: int

def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        cases.append(Case(
            idx=int(case["idx"]),
            baud=int(case["baudrate"]),
        ))
    return cases

def run(project_root: Path, serial: SerialInstrument, la: LogicAnalyzerInstrument) -> None:
    params = load_test_params(project_root)

    scenario = params.get("tx_baud", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, f"No cases found in {yml_path}"

    # Global marker config
    marker_cfg = params.get("marker", {})
    marker_ch_name = marker_cfg.get("channel", "uart0_tx")
    marker_baud = int(marker_cfg.get("baudrate", 115200))
    marker_pattern = r"BAUD:CASE:(\d+)"
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
        output_csv=out_dir / "baud_markers.csv",
    )

    assert len(markers) == len(cases), (
        f"Expected {len(cases)} BAUD:CASE markers, got {len(markers)}: {markers}"
    )

    markers_by_case = {ev.case_idx: ev for ev in markers}

    for i, c in enumerate(cases):
        ev = markers_by_case[c.idx]
        start_ns = ev.time_ns + marker_delay_ns
        end_ns = markers_by_case[cases[i + 1].idx].time_ns if i + 1 < len(cases) else None

        if _expect_pass(c.baud):
            csv_path = out_dir / f"baud_{c.idx:02d}_{c.baud}.csv"
            la.decode_uart(dut_ch, c.baud, start_ns, end_ns, csv_path)
            got = la.parse_uart_csv(csv_path)
            assert got == payload, (
                f"CASE {c.idx} baud={c.baud}: expected {payload!r}, got {got!r}"
            )
            print(f"[PASS] CASE {c.idx}  baud={c.baud:>7}  {got!r}")
        else:
            print(f"[PASS] CASE {c.idx}  baud={c.baud:>7}  expected fail (init error)")
