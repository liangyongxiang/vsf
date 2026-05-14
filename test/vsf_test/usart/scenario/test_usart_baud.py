"""Multi-baud UART TX validation via logic analyzer capture.

Usage:
    vsf-board-run board/pico/hardware-map.yml \\
        vsf.demo/vsf/test/vsf_test/usart/scenario/test_usart_baud.py

The firmware loops over BAUDRATES, emitting CASE:N on UART0 TX
before each payload burst on UART1 TX.  This script:
  1. Waits for the firmware's BAUD_TEST_DONE message via serial.
  2. Waits for the logic analyzer capture to finish.
  3. Decodes the CASE:N markers from CH1 (UART0 TX, 115200 baud).
  4. For each case, decodes CH3 (UART1 TX) at the case baud rate and
     asserts the received payload matches b"Hello VSF\\r\\n".
"""

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument

BAUDRATES = [9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600]
EXPECTED = b"Hello VSF\r\n"

# Must cover full run: 8 cases × (200 ms marker delay + ~700 ms drain + payload)
TEST_TIMEOUT_S = 120.0

# Firmware sleeps MARKER_DELAY_MS=200 ms between marker and payload start.
# Use 150 ms offset so the decode window opens just before the payload.
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

    assert len(markers) == len(BAUDRATES), (
        f"Expected {len(BAUDRATES)} CASE markers, got {len(markers)}: {markers}"
    )

    # Sort by case_idx to guarantee ordering matches BAUDRATES list.
    markers_by_case = {ev.case_idx: ev for ev in markers}

    for i, baud in enumerate(BAUDRATES):
        ev = markers_by_case[i]
        start_ns = ev.time_ns + _MARKER_DELAY_NS

        # End window just before the next CASE marker (or open-ended for last).
        if i + 1 < len(BAUDRATES):
            end_ns = markers_by_case[i + 1].time_ns
        else:
            end_ns = None

        csv_path = out_dir / f"case_{i:02d}_baud{baud}.csv"
        la.decode_uart(dut_ch, baud, start_ns, end_ns, csv_path)
        got = la.parse_uart_csv(csv_path)

        assert got == EXPECTED, (
            f"CASE {i} baud={baud}: expected {EXPECTED!r}, got {got!r}"
        )
        print(f"[PASS] CASE {i}  baud={baud:>7}  {got!r}")
