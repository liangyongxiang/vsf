from vsf_bench import SerialInstrument

def run(serial: SerialInstrument) -> None:
    serial.expect_test_summary("rng_basic", timeout=5.0)
