"""adc_temperature scenario: sample internal temperature sensor (channel 4).

The firmware asserts internally via VSF_TEST_ASSERT; this script waits
for the test framework summary line and asserts all cases passed.
"""

from pathlib import Path
from vsf_bench import SerialInstrument

SCENARIOS = ["adc_temperature"]


def run(project_root: Path, serial: SerialInstrument) -> None:
    serial.expect_test_summary("adc_temperature")
