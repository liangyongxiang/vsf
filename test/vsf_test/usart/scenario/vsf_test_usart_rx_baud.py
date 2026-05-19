"""USART RX baud-rate validation: PC sends payload at varying baud rates.

Two-phase: `run()` per-case handshakes (READY → host writes payload, firmware
reads it, prints next READY); `decode()` walks the shared LA capture, slicing
each case's payload on uart1_rx by the boundary between consecutive READY
markers.
"""

from dataclasses import dataclass
from pathlib import Path

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument
from vsf_bench.test_params import load_test_params

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


def run(project_root: Path, serial: SerialInstrument) -> None:
    params = load_test_params(project_root)
    scenario = params.get("rx_baud", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    timeout_s = float(scenario.get("timeout_s", 120.0))
    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    payload = scenario.get("payload", "Hello VSF\r\n").encode()

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for c in cases:
        if not _expect_pass(c.baud):
            continue
        serial.expect(f"RX_BAUD:CASE:{c.idx}:READY", timeout=timeout_s)
        aux.baudrate = c.baud
        aux.write(payload)
        aux.flush()

    serial.expect_test_summary("usart_rx_baud", timeout=timeout_s)
    aux.close()


def decode(project_root: Path, la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None) -> None:
    params = load_test_params(project_root)
    scenario = params.get("rx_baud", {})
    cases = _parse_cases(scenario)
    pass_cases = [c for c in cases if _expect_pass(c.baud)]
    if not pass_cases:
        return

    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    dut_ch_name = scenario.get("dut", {}).get("channel", "uart1_rx")
    payload = scenario.get("payload", "Hello VSF\r\n").encode()

    marker_ch_tx = la.channel("uart0_tx")
    dut_ch = la.channel(dut_ch_name)
    out_dir = la.output_dir

    ready_markers = la.decode_markers(
        channel=marker_ch_tx,
        baudrate=marker_baud,
        pattern=r"RX_BAUD:CASE:(\d+):READY",
        output_csv=out_dir / "rx_baud_ready_markers.csv",
        start_ns=decode_start_ns,
        end_ns=decode_end_ns,
    )
    ready_by_case = {ev.case_idx: ev for ev in ready_markers}

    pass_baudrates = sorted({c.baud for c in pass_cases})
    for c in pass_cases:
        assert c.idx in ready_by_case, f"CASE {c.idx} baud={c.baud}: READY marker not found in LA decode"
    config_to_csv = {b: out_dir / f"rx_baud_full_{b}.csv" for b in pass_baudrates}
    la.batch_decode_uart([
        (dut_ch, b, decode_start_ns, decode_end_ns,
         config_to_csv[b], "none", 8, 1.0)
        for b in pass_baudrates
    ])

    # Slice each case by the next case's READY (or decode_end_ns for the last).
    sorted_cases = sorted(pass_cases, key=lambda c: ready_by_case[c.idx].time_ns)
    for i, c in enumerate(sorted_cases):
        start_ns = ready_by_case[c.idx].time_ns
        if i + 1 < len(sorted_cases):
            end_ns = ready_by_case[sorted_cases[i + 1].idx].time_ns
        else:
            end_ns = decode_end_ns if decode_end_ns is not None else start_ns + 5_000_000_000

        rows = la.read_csv_rows(config_to_csv[c.baud])
        got = bytes(b for t, b in rows if start_ns <= t < end_ns)
        assert got == payload, (
            f"CASE {c.idx} baud={c.baud}: expected {payload!r}, got {got!r}"
        )
        print(f"[PASS] CASE {c.idx}  baud={c.baud:>7}  {got!r}")
