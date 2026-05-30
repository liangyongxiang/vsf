"""flash_boundary scenario: test flash erase/write at sector/page boundaries.

Firmware tests cross-boundary operations internally.
"""

from vsf_bench import SerialInstrument



def run(serial: SerialInstrument) -> None:
    serial.expect_test_summary("flash_boundary", timeout=10.0)
