"""gpio_atomic scenario host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for
the test framework summary line and asserts all cases passed.

Requires GP4/GP5 jumper (shared mode).
"""

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument

SCENARIOS = ["gpio_atomic"]

def run(project_root: Path, serial: SerialInstrument, la: LogicAnalyzerInstrument | None = None) -> None:
    serial.expect_test_summary("gpio_atomic")
