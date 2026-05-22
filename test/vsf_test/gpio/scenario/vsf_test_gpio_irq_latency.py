"""gpio_irq_latency scenario host harness.

Firmware measures EXTI ISR latency via vsf_systimer ticks (no LA needed
since the test self-triggers and measures internally). Host script just
checks the test summary.
"""

from pathlib import Path
from vsf_bench import LogicAnalyzerInstrument, SerialInstrument

SCENARIOS = ["gpio_irq_latency"]

def run(project_root: Path, serial: SerialInstrument, la: LogicAnalyzerInstrument | None = None) -> None:
    serial.expect_test_summary("gpio_irq_latency")
