"""rtc_set_get suite host harness."""

from pathlib import Path
from vsf_bench import SerialInstrument


def run(project_root: Path, serial: SerialInstrument,
        la=None) -> None:
    serial.expect_test_summary("rtc_set_get", timeout=10.0)
