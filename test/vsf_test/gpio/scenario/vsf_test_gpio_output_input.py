"""gpio_output_input scenario: drive GP A high/low → read on GP B (loopback).

Usage:
    python3 -m vsf_bench.board_run board/pico/hardware-map.yml \\
        vsf.demo/vsf/test/vsf_test/gpio/scenario/vsf_test_gpio_output_input.py

Requires the GPIO loopback fixture: a jumper between GP4 and GP5 (or
whichever pins the case lists in application/component/vsf-test/gpio.yml).

The firmware asserts internally via VSF_TEST_ASSERT; this script just
waits for the test framework summary line and asserts all cases passed.
"""

import re

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument

SCENARIOS = ["gpio_output_input"]


def run(serial: SerialInstrument, la: LogicAnalyzerInstrument | None = None) -> None:
    serial.expect("All test cases completed", timeout=30.0)
    summary = serial.expect(r"Pass: (\d+), Fail: (\d+), Skip: (\d+)", timeout=5.0)
    m = re.search(r"Pass: (\d+), Fail: (\d+), Skip: (\d+)", summary)
    assert m is not None, f"Could not parse test summary: {summary!r}"
    passed, failed, skipped = int(m.group(1)), int(m.group(2)), int(m.group(3))

    print(f"[gpio_output_input] pass={passed} fail={failed} skip={skipped}")
    assert failed == 0, f"{failed} GPIO assertions failed in firmware"
    assert passed > 0, "no GPIO cases ran"
