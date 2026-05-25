"""rtc_epoch suite host harness."""

from pathlib import Path
from vsf_bench import SerialInstrument


def run(project_root: Path, serial: SerialInstrument,
        la=None) -> None:
    serial.expect_test_summary("rtc_epoch", timeout=10.0)
