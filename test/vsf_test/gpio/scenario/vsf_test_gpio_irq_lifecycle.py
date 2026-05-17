"""gpio_irq_lifecycle scenario host harness.

Drives the full exti_irq_* API surface (config / enable / trigger /
disable / re-enable / clear / get_configuration) on a self-triggered
pin. Firmware asserts internally.
"""

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument

SCENARIOS = ["gpio_irq_lifecycle"]

def run(project_root: Path, serial: SerialInstrument, la: LogicAnalyzerInstrument | None = None) -> None:
    serial.expect_test_summary("gpio_irq_lifecycle")
