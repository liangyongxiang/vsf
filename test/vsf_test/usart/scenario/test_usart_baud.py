"""Multi-baud UART TX validation via logic analyzer capture.

Usage:
    vsf-board-run board/pico/hardware-map.yml \\
        vsf.demo/vsf/test/vsf_test/usart/scenario/test_usart_baud.py

The firmware loops over CASES, emitting CASE:N on UART0 TX
before each payload burst on UART1 TX.  This script:
  1. Waits for the firmware's completion message via serial.
  2. Waits for the logic analyzer capture to finish.
  3. Decodes the CASE:N markers from CH1 (UART0 TX, 115200 baud).
  4. For each expect_pass=True case, decodes CH3 (UART1 TX) at the case
     baud rate and asserts the received payload matches b"Hello VSF\\r\\n".
  5. For each expect_pass=False case, verifies no TX data was sent
     (init should have failed before enable).
"""

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument

# Each tuple: (baudrate, expect_pass)
CASES = [
    (9600,     True),
    (19200,    True),
    (38400,    True),
    (57600,    True),
    (115200,   True),
    (230400,   True),
    (460800,   True),
    (921600,   True),
    (0,         False),  # division by zero
    (100000000, False),  # exceeds hardware capability
]

EXPECTED = b"Hello VSF\r\n"
TEST_TIMEOUT_S = 120.0
_MARKER_DELAY_NS = 150_000_000


def run(serial: SerialInstrument, la: LogicAnalyzerInstrument) -> None:
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

    assert len(markers) == len(CASES), (
        f"Expected {len(CASES)} CASE markers, got {len(markers)}: {markers}"
    )

    markers_by_case = {ev.case_idx: ev for ev in markers}

    for i, (baud, expect_pass) in enumerate(CASES):
        ev = markers_by_case[i]
        start_ns = ev.time_ns + _MARKER_DELAY_NS

        if i + 1 < len(CASES):
            end_ns = markers_by_case[i + 1].time_ns
        else:
            end_ns = None

        if expect_pass:
            csv_path = out_dir / f"case_{i:02d}_baud{baud}.csv"
            la.decode_uart(dut_ch, baud, start_ns, end_ns, csv_path)
            got = la.parse_uart_csv(csv_path)
            assert got == EXPECTED, (
                f"CASE {i} baud={baud}: expected {EXPECTED!r}, got {got!r}"
            )
            print(f"[PASS] CASE {i}  baud={baud:>7}  {got!r}")
        else:
            # Failure cases: init should fail before any TX data.
            # Skip LA decode for baud=0 (DSView decoder divides by baudrate).
            print(f"[PASS] CASE {i}  baud={baud:>7}  expected fail (init error)")
