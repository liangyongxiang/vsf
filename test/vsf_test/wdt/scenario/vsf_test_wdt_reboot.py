"""wdt_reboot scenario: verify watchdog triggers system reset.

Firmware intentionally does not feed the watchdog, triggering a reset.
This test is typically DISABLED as it reboots the device.
"""

from pathlib import Path
from vsf_bench import SerialInstrument

SCENARIOS = ["wdt_reboot"]


def run(project_root: Path, serial: SerialInstrument) -> None:
    serial.expect_test_summary("wdt_reboot", timeout=10.0)
