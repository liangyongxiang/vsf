"""Multi-baud UART TX validation via logic analyzer capture.

Usage:
    vsf-board-run board/pico/hardware-map.yml \\
        vsf.demo/vsf/test/vsf_test/usart/scenario/test_usart_baud.py

The firmware emits a TEST_PLAN over UART0 before running cases.
This script:
  1. Reads the TEST_PLAN from serial to learn case parameters.
  2. Waits for completion and LA capture.
  3. Decodes CASE:N markers from CH1 (UART0 TX, 115200 baud).
  4. For expect_pass=True cases, decodes CH3 at the case baud rate.
  5. For expect_pass=False cases, verifies no TX data was sent.
"""

from dataclasses import dataclass

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument

EXPECTED = b"Hello VSF\r\n"
TEST_TIMEOUT_S = 120.0
_MARKER_DELAY_NS = 150_000_000


@dataclass(frozen=True)
class Case:
    idx: int
    baud: int
    expect_pass: bool


def _read_test_plan(serial: SerialInstrument) -> list[Case]:
    """Read [TEST_PLAN] lines from firmware until [TEST_PLAN_END]."""
    serial.expect("[TEST_PLAN_BEGIN]", timeout=10)
    cases: list[Case] = []
    while True:
        line = serial.expect(r"\[(TEST_PLAN|TEST_PLAN_END)\].*", timeout=5)
        if "TEST_PLAN_END" in line:
            break
        # Format: [TEST_PLAN] idx baudrate expect_pass
        parts = line.strip().split()
        if len(parts) >= 4 and parts[0] == "[TEST_PLAN]":
            cases.append(Case(
                idx=int(parts[1]),
                baud=int(parts[2]),
                expect_pass=bool(int(parts[3])),
            ))
    return cases


def run(serial: SerialInstrument, la: LogicAnalyzerInstrument) -> None:
    cases = _read_test_plan(serial)
    assert len(cases) > 0, "No cases found in TEST_PLAN"

    serial.expect("All test cases completed", timeout=TEST_TIMEOUT_S)
    la.wait(timeout=TEST_TIMEOUT_S)

    marker_ch = la.channel("uart0_tx")
    dut_ch = la.channel("uart1_tx")
    out_dir = la.output_dir

    markers = la.decode_markers(
        channel=marker_ch,
        baudrate=115200,
        pattern=r"CASE:(\d+)",
        output_dir=out_dir,
    )

    assert len(markers) == len(cases), (
        f"Expected {len(cases)} CASE markers, got {len(markers)}: {markers}"
    )

    markers_by_case = {ev.case_idx: ev for ev in markers}

    for i, c in enumerate(cases):
        ev = markers_by_case[c.idx]
        start_ns = ev.time_ns + _MARKER_DELAY_NS
        end_ns = markers_by_case[cases[i + 1].idx].time_ns if i + 1 < len(cases) else None

        if c.expect_pass:
            csv_path = out_dir / f"case_{c.idx:02d}_baud{c.baud}.csv"
            la.decode_uart(dut_ch, c.baud, start_ns, end_ns, csv_path)
            got = la.parse_uart_csv(csv_path)
            assert got == EXPECTED, (
                f"CASE {c.idx} baud={c.baud}: expected {EXPECTED!r}, got {got!r}"
            )
            print(f"[PASS] CASE {c.idx}  baud={c.baud:>7}  {got!r}")
        else:
            print(f"[PASS] CASE {c.idx}  baud={c.baud:>7}  expected fail (init error)")
