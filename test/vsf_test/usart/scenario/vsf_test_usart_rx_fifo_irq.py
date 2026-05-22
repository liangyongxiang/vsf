"""usart_rx_fifo_irq scenario host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for
the test framework summary line and asserts all cases passed.

Requires a host-side UART sender on /dev/ttyUSB-rx.
"""

from pathlib import Path
from vsf_bench import LogicAnalyzerInstrument, SerialInstrument

SCENARIOS = ["usart_rx_fifo_irq"]


def run(project_root: Path, serial: SerialInstrument, la: LogicAnalyzerInstrument | None = None) -> None:
    serial.expect_test_summary("usart_rx_fifo_irq")
