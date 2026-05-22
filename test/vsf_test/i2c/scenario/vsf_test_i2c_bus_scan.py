"""i2c_bus_scan scenario: scan I2C bus for devices.

Firmware scans the I2C bus and reports found device addresses.
"""

from pathlib import Path
from vsf_bench import SerialInstrument

SCENARIOS = ["i2c_bus_scan"]


def run(project_root: Path, serial: SerialInstrument) -> None:
    serial.expect_test_summary("i2c_bus_scan", timeout=10.0)
