"""usart_request_cancel scenario host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for
the test framework summary line and asserts all cases passed.

TX via fifo2req adapter; re-uses UART1.
"""

from pathlib import Path
from vsf_bench import LogicAnalyzerInstrument, SerialInstrument

SCENARIOS = ["usart_request_cancel"]


def run(project_root: Path, serial: SerialInstrument) -> None:
    serial.expect_test_summary("usart_request_cancel")
