"""Multi-baud UART TX validation via logic analyzer capture.

Usage:
    vsf-board-run board/pico/hardware-map.yml \\
        vsf.demo/vsf/test/vsf_test/usart/scenario/test_usart_baud.py

Test parameters are read from application/component/vsf-test/test_params.yml
so the script does not depend on firmware output for correctness.
"""

from dataclasses import dataclass
from pathlib import Path

import yaml

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument

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


def _find_project_root() -> Path:
    """Walk up from script location to find project root (contains application/component/vsf-test/)."""
    p = Path(__file__).resolve()
    while p.parent != p:
        if (p / "application" / "component" / "vsf-test").is_dir():
            return p
        p = p.parent
    raise RuntimeError("Cannot find project root (no application/component/vsf-test/)")


def _load_params(yml_path: Path) -> dict:
    """Load all test parameters from YAML, resolving `include:` directives.

    The aggregator (`test_params.yml`) lists per-peripheral YAMLs via the
    `include:` directive. We mirror the C generator's resolution semantics
    so the host-side scripts read the same flattened view.
    """
    # Import the shared loader via sys.path so this stays self-contained
    # whether the script is invoked from a checkout or an installed wheel.
    import sys as _sys
    _loader_dir = (Path(__file__).resolve().parent.parent.parent / "scripts")
    if str(_loader_dir) not in _sys.path:
        _sys.path.insert(0, str(_loader_dir))
    from test_params_loader import load_yaml_with_includes
    return load_yaml_with_includes(yml_path)


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        cases.append(Case(
            idx=int(case["idx"]),
            baud=int(case["baudrate"]),
        ))
    return cases


def run(serial: SerialInstrument, la: LogicAnalyzerInstrument) -> None:
    project_root = _find_project_root()
    yml_path = project_root / "application" / "component" / "vsf-test" / "test_params.yml"
    params = _load_params(yml_path)

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
