from pathlib import Path
from vsf_bench import SerialInstrument

def run(project_root: Path, serial: SerialInstrument) -> None:
    serial.expect_test_summary("i2c_eeprom_rw_fifo", timeout=10.0)
