"""gpio_output_input scenario: drive GP A high/low → read on GP B (loopback).

Usage:
    vsf-bench-test board/pico/hardware-map.yml \\
        --script vsf.demo/vsf/test/vsf_test/gpio/scenario/vsf_test_gpio_output_input.py \\
        --scene gpio_output_input

Requires the GPIO loopback fixture: a jumper between GP4 and GP5 (or
whichever pins the case lists in application/component/vsf-test/gpio.yml).

The firmware asserts internally via VSF_TEST_ASSERT; this script just
waits for the test framework summary line and asserts all cases passed.
"""

from pathlib import Path
from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument

SCENARIOS = ["gpio_output_input"]

def run(project_root: Path, serial: SerialInstrument, la: LogicAnalyzerInstrument | None = None) -> None:
    serial.expect_test_summary("gpio_output_input")
