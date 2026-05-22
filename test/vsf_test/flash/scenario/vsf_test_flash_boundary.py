"""flash_boundary scenario: test flash erase/write at sector/page boundaries.

Firmware tests cross-boundary operations internally.
"""

from pathlib import Path
from vsf_bench import SerialInstrument

SCENARIOS = ["flash_boundary"]


def run(project_root: Path, serial: SerialInstrument) -> None:
    serial.expect_test_summary("flash_boundary", timeout=10.0)
