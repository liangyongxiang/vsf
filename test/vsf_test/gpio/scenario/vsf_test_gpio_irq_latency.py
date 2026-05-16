"""gpio_irq_latency scenario host harness.

Firmware measures EXTI ISR latency via vsf_systimer ticks (no LA needed
since the test self-triggers and measures internally). Host script just
checks the test summary.
"""

import re

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument

SCENARIOS = ["gpio_irq_latency"]


def run(serial: SerialInstrument, la: LogicAnalyzerInstrument | None = None) -> None:
    serial.expect("All test cases completed", timeout=30.0)
    summary = serial.expect(r"Pass: (\d+), Fail: (\d+), Skip: (\d+)", timeout=5.0)
    m = re.search(r"Pass: (\d+), Fail: (\d+), Skip: (\d+)", summary)
    assert m is not None, f"Could not parse test summary: {summary!r}"
    passed, failed, skipped = int(m.group(1)), int(m.group(2)), int(m.group(3))
    print(f"[gpio_irq_latency] pass={passed} fail={failed} skip={skipped}")
    assert failed == 0, f"{failed} assertion(s) failed in firmware"
    assert passed > 0, "no cases ran"
