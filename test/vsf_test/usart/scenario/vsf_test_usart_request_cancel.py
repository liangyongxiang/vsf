"""usart_request_cancel scenario host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for
the test framework summary line and asserts all cases passed.

TX via fifo2req adapter; re-uses UART1.
"""

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument

SCENARIOS = ["usart_request_cancel"]


def run(project_root: Path, serial: SerialInstrument, la: LogicAnalyzerInstrument | None = None) -> None:
    serial.expect_test_summary("usart_request_cancel")
