"""wdt_basic scenario: verify watchdog can be fed and does not reset.

Firmware feeds the watchdog within the timeout window and asserts
no reset occurs.
"""

from pathlib import Path
from vsf_bench import SerialInstrument

SCENARIOS = ["wdt_basic"]


def run(project_root: Path, serial: SerialInstrument) -> None:
    serial.expect_test_summary("wdt_basic", timeout=10.0)
