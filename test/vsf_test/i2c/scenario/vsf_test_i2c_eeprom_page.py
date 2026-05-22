"""i2c_eeprom_page scenario: read/write EEPROM across page boundaries via I2C.

Firmware performs I2C EEPROM page-boundary write and verifies data internally.
"""

from pathlib import Path
from vsf_bench import SerialInstrument

SCENARIOS = ["i2c_eeprom_page"]


def run(project_root: Path, serial: SerialInstrument) -> None:
    serial.expect_test_summary("i2c_eeprom_page", timeout=10.0)
