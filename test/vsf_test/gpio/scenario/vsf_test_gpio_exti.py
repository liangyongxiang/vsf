"""gpio_exti scenario host harness.

Self-triggered EXTI test: firmware uses SIO output to drive its own pin
and observes the falling edge via EXTI. No external wiring needed.
"""

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument

SCENARIOS = ["gpio_exti"]

def run(project_root: Path, serial: SerialInstrument, la: LogicAnalyzerInstrument | None = None) -> None:
    serial.expect_test_summary("gpio_exti")
