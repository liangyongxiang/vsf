"""USART RX baud-rate validation: PC sends payload at varying baud rates.

Usage:
    vsf-board-run board/pico/hardware-map.yml \\
        vsf.demo/vsf/test/vsf_test/usart/scenario/test_usart_rx_baud.py
"""

from dataclasses import dataclass
from pathlib import Path

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument
from vsf_bench.test_params import load_test_params

SCENARIOS = ["usart_rx_baud"]

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

    scenario = params.get("rx_baud", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, 'No cases found in test_params'

    marker_cfg = params.get("marker", {})
    marker_baud = int(marker_cfg.get("baudrate", 115200))
    timeout_s = float(scenario.get("timeout_s", 120.0))

    dut = scenario.get("dut", {})
    dut_port = dut.get("port", "/dev/ttyUSB2")
    dut_ch_name = dut.get("channel", "uart1_rx")
    payload = scenario.get("payload", "Hello VSF\r\n").encode()

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    pass_cases = [c for c in cases if _expect_pass(c.baud)]

    for c in cases:
        if _expect_pass(c.baud):
            serial.expect(f"RX_BAUD:CASE:{c.idx}:READY", timeout=timeout_s)

            aux.baudrate = c.baud
            aux.write(payload)
            aux.flush()

            serial._ser.write(f"RX_BAUD:CASE:{c.idx}:DONE\r\n".encode())
            serial._ser.flush()
        else:
            # For cases where firmware should reject the baud rate, no READY
            # marker is produced — wait for the CASE marker delay only
            pass

    serial.expect_test_summary("usart_rx_baud", timeout=timeout_s)
    la.stop()
    la.wait(timeout=120.0)

    aux.close()

    # LA decode
    marker_ch_tx = la.channel("uart0_tx")
    marker_ch_rx = la.channel("uart0_rx")
    dut_ch = la.channel(dut_ch_name)
    out_dir = la.output_dir

    ready_markers = la.decode_markers(
        channel=marker_ch_tx,
        baudrate=marker_baud,
        pattern=r"RX_BAUD:CASE:(\d+):READY",
        output_csv=out_dir / "rx_baud_ready_markers.csv",
    )
    done_markers = la.decode_markers(
        channel=marker_ch_rx,
        baudrate=marker_baud,
        pattern=r"RX_BAUD:CASE:(\d+):DONE",
        output_csv=out_dir / "rx_baud_done_markers.csv",
    )

    assert len(ready_markers) == len(pass_cases), f"Expected {len(pass_cases)} READY markers, got {len(ready_markers)}"
    assert len(done_markers) == len(pass_cases), f"Expected {len(pass_cases)} DONE markers, got {len(done_markers)}"

    ready_by_case = {ev.case_idx: ev for ev in ready_markers}
    done_by_case = {ev.case_idx: ev for ev in done_markers}

    for c in cases:
        if _expect_pass(c.baud):
            ready_ev = ready_by_case[c.idx]
            done_ev = done_by_case[c.idx]
            start_ns = ready_ev.time_ns
            end_ns = done_ev.time_ns
            csv_path = out_dir / f"rx_baud_{c.idx:02d}_{c.baud}.csv"
            la.decode_uart(dut_ch, c.baud, start_ns, end_ns, csv_path)
            got = la.parse_uart_csv(csv_path)
            assert got == payload, (
                f"CASE {c.idx} baud={c.baud}: expected {payload!r}, got {got!r}"
            )
            print(f"[PASS] CASE {c.idx}  baud={c.baud:>7}  {got!r}")
        else:
            print(f"[PASS] CASE {c.idx}  baud={c.baud:>7}  expected fail")
