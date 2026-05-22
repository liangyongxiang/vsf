"""gpio_open_drain scenario host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for
the test framework summary line and asserts all cases passed.

Requires GP4↔GP5 jumper. Uses internal pull-up.
"""

from pathlib import Path
from vsf_bench import LogicAnalyzerInstrument, SerialInstrument

SCENARIOS = ["gpio_open_drain"]

def run(project_root: Path, serial: SerialInstrument, la: LogicAnalyzerInstrument | None = None) -> None:
    serial.expect_test_summary("gpio_open_drain")
