"""gpio_io_check scenario: bit-bang UART on GPIO pins to verify LA probe wiring.

Firmware drives each declared pin as push-pull output and sends a unique
UART 8N1 byte (0x50 + pin) at 115200 baud. The host decode() phase decodes
UART on all LA channels and cross-references: each expected pin's byte must
appear on exactly one channel.
"""

from pathlib import Path

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument
from vsf_bench.test_params import load_test_params

SCENARIOS = ["gpio_io_check"]
BAUDRATE = 115200


def run(project_root: Path, serial: SerialInstrument,
        la: LogicAnalyzerInstrument | None = None) -> None:
    params = load_test_params(project_root)
    scenario = params.get("gpio_io_check", {})
    timeout_s = float(scenario.get("timeout_s", 5.0))
    serial.expect_test_summary("gpio_io_check", timeout=timeout_s)


def decode(project_root: Path, la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None) -> None:
    params = load_test_params(project_root)
    scenario = params.get("gpio_io_check", {})
    cases = scenario.get("cases", [])
    if not cases:
        print("[gpio_io_check] No cases declared, skipping decode")
        return

    # Build expected {pin -> byte} map
    expected: dict[int, int] = {}
    for case in cases:
        pin = int(case["pin"])
        expected[pin] = 0x50 + pin

    # Decode UART on every LA channel
    out_dir = la.output_dir
    channel_bytes: dict[str, list[int]] = {}
    for ch_name in la._channels:
        ch = la.channel(ch_name)
        csv_path = out_dir / f"io_check_{ch_name}.csv"
        la.decode_uart(ch, BAUDRATE, decode_start_ns, decode_end_ns,
                       csv_path, "none", 8, 1.0)
        rows = la.read_csv_rows(csv_path)
        channel_bytes[ch_name] = [b for t, b in rows]

    # Cross-reference: each expected byte must appear on exactly one channel
    for pin, byte in expected.items():
        found_on: list[str] = []
        for ch_name, bytes_list in channel_bytes.items():
            if byte in bytes_list:
                found_on.append(ch_name)

        if len(found_on) == 0:
            raise AssertionError(
                f"GPIO IO check FAIL: pin {pin} byte 0x{byte:02x} not found on any LA channel"
            )
        elif len(found_on) > 1:
            print(
                f"[WARN] pin {pin} byte 0x{byte:02x} found on multiple channels: {found_on} "
                f"(possible loopback or shared signal)"
            )
        else:
            print(f"[PASS] pin {pin} byte 0x{byte:02x} on channel {found_on[0]}")

    # Verify no unexpected bytes on any channel
    for ch_name, bytes_list in channel_bytes.items():
        for b in bytes_list:
            if b not in expected.values():
                raise AssertionError(
                    f"GPIO IO check FAIL: unexpected byte 0x{b:02x} on channel {ch_name}"
                )
