"""adc_stream suite host harness."""

from pathlib import Path
from vsf_bench import SerialInstrument


def run(project_root: Path, serial: SerialInstrument,
        la=None) -> None:
    serial.expect_test_summary("adc_stream", timeout=15.0)
