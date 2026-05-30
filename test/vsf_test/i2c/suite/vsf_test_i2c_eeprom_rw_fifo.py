from vsf_bench import SerialInstrument

def run(serial: SerialInstrument) -> None:
    serial.expect_test_summary("i2c_eeprom_rw_fifo", timeout=10.0)
