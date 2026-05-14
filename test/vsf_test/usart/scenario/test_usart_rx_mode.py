"""USART RX mode validation: PC sends payload with varying parity/data/stop.

Usage:
    vsf-board-run board/pico/hardware-map.yml \
        vsf.demo/vsf/test/vsf_test/usart/scenario/test_usart_rx_mode.py
"""

from dataclasses import dataclass
from pathlib import Path

import serial
import yaml

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument


@dataclass(frozen=True)
class Case:
    idx: int
    expect_pass: bool
    decode_parity: str
    decode_data: int
    decode_stop: float


def _find_project_root() -> Path:
    p = Path(__file__).resolve()
    while p.parent != p:
        if (p / "application" / "component" / "vsf-test").is_dir():
            return p
        p = p.parent
    raise RuntimeError("Cannot find project root")


def _load_params(yml_path: Path) -> dict:
    return yaml.safe_load(yml_path.read_text()) or {}


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        host = case.get("host", {}) or {}
        send = host.get("send", {}) or {}
        cases.append(Case(
            idx=int(case["idx"]),
            expect_pass=bool(case.get("expect_pass", True)),
            decode_parity=send.get("parity_type", "none"),
            decode_data=int(send.get("num_data_bits", 8)),
            decode_stop=float(send.get("num_stop_bits", 1.0)),
        ))
    return cases


def run(serial: SerialInstrument, la: LogicAnalyzerInstrument) -> None:
    project_root = _find_project_root()
    yml_path = project_root / "application" / "component" / "vsf-test" / "test_params.yml"
    params = _load_params(yml_path)

    scenario = params.get("rx_mode", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, f"No cases found in {yml_path}"

    marker_cfg = params.get("marker", {})
    marker_baud = int(marker_cfg.get("baudrate", 115200))
    timeout_s = float(scenario.get("timeout_s", 120.0))

    dut = scenario.get("dut", {})
    dut_port = dut.get("port", "/dev/ttyUSB2")
    dut_ch_name = dut.get("channel", "uart1_rx")
    payload = scenario.get("payload", "0123456789\r\n").encode()

    aux = serial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for c in cases:
        serial.expect(f"RX_MODE:CASE:{c.idx}:READY", timeout=timeout_s)

        # Reconfigure aux serial to match case mode
        parity_map = {"none": serial.PARITY_NONE, "even": serial.PARITY_EVEN, "odd": serial.PARITY_ODD}
        aux.parity = parity_map.get(c.decode_parity, serial.PARITY_NONE)
        aux.bytesize = c.decode_data
        aux.stopbits = c.decode_stop
        aux.write(payload)
        aux.flush()

        serial._ser.write(f"RX_MODE:CASE:{c.idx}:DONE\r\n".encode())
        serial._ser.flush()

    serial.expect("All test cases completed", timeout=timeout_s)
    la.wait(timeout=timeout_s)

    aux.close()

    # LA decode
    marker_ch_tx = la.channel("uart0_tx")
    marker_ch_rx = la.channel("uart0_rx")
    dut_ch = la.channel(dut_ch_name)
    out_dir = la.output_dir

    ready_markers = la.decode_markers(
        channel=marker_ch_tx, baudrate=marker_baud,
        pattern=r"RX_MODE:CASE:(\d+):READY",
        output_csv=out_dir / "rx_mode_ready_markers.csv",
    )
    done_markers = la.decode_markers(
        channel=marker_ch_rx, baudrate=marker_baud,
        pattern=r"RX_MODE:CASE:(\d+):DONE",
        output_csv=out_dir / "rx_mode_done_markers.csv",
    )

    assert len(ready_markers) == len(cases)
    assert len(done_markers) == len(cases)

    ready_by_case = {ev.case_idx: ev for ev in ready_markers}
    done_by_case = {ev.case_idx: ev for ev in done_markers}

    for c in cases:
        ready_ev = ready_by_case[c.idx]
        done_ev = done_by_case[c.idx]
        start_ns = ready_ev.time_ns
        end_ns = done_ev.time_ns

        csv_path = out_dir / f"rx_mode_{c.idx:02d}_{c.decode_parity}{c.decode_data}{c.decode_stop}.csv"
        la.decode_uart(
            dut_ch, marker_baud, start_ns, end_ns, csv_path,
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
