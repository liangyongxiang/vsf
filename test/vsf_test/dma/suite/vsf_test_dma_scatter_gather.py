"""dma_scatter_gather scenario: Verify SG API returns NOT_SUPPORT.

RP2040 DMA does not have native scatter-gather.  This scenario calls
vsf_dma_channel_sg_config_desc / sg_start and asserts that both return
VSF_ERR_NOT_SUPPORT.

No host-side serial interaction required — this is an internal test.
"""

from pathlib import Path
from vsf_bench import SerialInstrument, load_test_params


def run(project_root: Path, serial: SerialInstrument) -> None:
    params = load_test_params(project_root)
    scenario = params.get("dma_scatter_gather", {})
    timeout_s = float(scenario.get("timeout_s", 10.0))

    serial.expect_test_summary("dma_scatter_gather", timeout=timeout_s)
