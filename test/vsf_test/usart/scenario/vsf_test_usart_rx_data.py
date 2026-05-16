"""USART RX data validation: PC sends payload, firmware verifies via ASSERT.

Usage:
    vsf-board-run board/pico/hardware-map.yml \\
        vsf.demo/vsf/test/vsf_test/usart/scenario/test_usart_rx_data.py

Dual verification:
1. LA decodes UART1 RX to verify PC sent correctly
2. Firmware ASSERT verifies Pico received correctly
"""

from dataclasses import dataclass
from pathlib import Path

import yaml

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument


@dataclass(frozen=True)
class Case:
    idx: int
    expect_pass: bool


def _find_project_root() -> Path:
    """Walk up from script location to find project root."""
    p = Path(__file__).resolve()
    while p.parent != p:
        if (p / "application" / "component" / "vsf-test").is_dir():
            return p
        p = p.parent
    raise RuntimeError("Cannot find project root")


def _load_params(yml_path: Path) -> dict:
    """Load all test parameters from YAML, resolving `include:` directives."""
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
            expect_pass=bool(case.get("expect_pass", True)),
        ))
    return cases


def run(serial: SerialInstrument, la: LogicAnalyzerInstrument) -> None:
    project_root = _find_project_root()
    yml_path = project_root / "application" / "component" / "vsf-test" / "test_params.yml"
    params = _load_params(yml_path)

    scenario = params.get("rx_data", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, f"No cases found in {yml_path}"

    # Marker config (UART0)
    marker_cfg = params.get("marker", {})
    marker_baud = int(marker_cfg.get("baudrate", 115200))
    timeout_s = float(scenario.get("timeout_s", 120.0))

    # DUT config (UART1)
    dut = scenario.get("dut", {})
    dut_port = dut.get("port", "/dev/ttyUSB2")
    dut_ch_name = dut.get("channel", "uart1_rx")
    payload = scenario.get("payload", "Hello VSF\r\n").encode()

    # Open auxiliary serial port for UART1
    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    # Interact with firmware case by case
    for c in cases:
        serial.expect(f"RX_DATA:CASE:{c.idx}:READY", timeout=timeout_s)

        # Send payload via UART1
        aux.write(payload)
        aux.flush()  # wait for physical transmission

        # Send DONE marker via UART0 (for LA timing)
        serial._ser.write(f"RX_DATA:CASE:{c.idx}:DONE\r\n".encode())
        serial._ser.flush()

    # Wait for overall completion
    serial.expect("All test cases completed", timeout=timeout_s)
    la.wait(timeout=120.0)

    aux.close()

    # --- LA offline decode ---
    marker_ch_tx = la.channel("uart0_tx")
    marker_ch_rx = la.channel("uart0_rx")
    dut_ch = la.channel(dut_ch_name)
    out_dir = la.output_dir

    # Decode READY markers from UART0 TX
    ready_markers = la.decode_markers(
        channel=marker_ch_tx,
        baudrate=marker_baud,
        pattern=r"RX_DATA:CASE:(\d+):READY",
        output_csv=out_dir / "rx_ready_markers.csv",
    )

    # Decode DONE markers from UART0 RX
    done_markers = la.decode_markers(
        channel=marker_ch_rx,
        baudrate=marker_baud,
        pattern=r"RX_DATA:CASE:(\d+):DONE",
        output_csv=out_dir / "rx_done_markers.csv",
    )

    assert len(ready_markers) == len(cases), (
        f"Expected {len(cases)} READY markers, got {len(ready_markers)}"
    )
    assert len(done_markers) == len(cases), (
        f"Expected {len(cases)} DONE markers, got {len(done_markers)}"
    )

    ready_by_case = {ev.case_idx: ev for ev in ready_markers}
    done_by_case = {ev.case_idx: ev for ev in done_markers}

    for c in cases:
        ready_ev = ready_by_case[c.idx]
        done_ev = done_by_case[c.idx]
        start_ns = ready_ev.time_ns
        end_ns = done_ev.time_ns

        if c.expect_pass:
            csv_path = out_dir / f"rx_data_{c.idx:02d}.csv"
            la.decode_uart(dut_ch, marker_baud, start_ns, end_ns, csv_path)
            got = la.parse_uart_csv(csv_path)
            assert got == payload, (
                f"CASE {c.idx}: expected {payload!r}, got {got!r}"
            )
            print(f"[PASS] CASE {c.idx}  rx_data  {got!r}")
        else:
            print(f"[PASS] CASE {c.idx}  rx_data  expected fail")
