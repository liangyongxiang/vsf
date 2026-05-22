"""adc_oneshot scenario: sample ADC channel 0 and verify 12-bit range.

The firmware asserts internally via VSF_TEST_ASSERT; this script waits
for the test framework summary line and asserts all cases passed.
"""

from pathlib import Path
from vsf_bench import SerialInstrument

SCENARIOS = ["adc_oneshot"]


def run(project_root: Path, serial: SerialInstrument) -> None:
    serial.expect_test_summary("adc_oneshot")
