"""Centralized runner type registry.

Both hardware_map.py (for validation) and pipeline.py (for dispatch)
import from here. Add new built-in runners here.
"""

from vsf_bench.adapters.cmsis_dap import CMSISDAPAdapter
from vsf_bench.adapters.uf2 import UF2Adapter
from vsf_bench.runners.plugin_runner import PluginRunner
from vsf_bench.adapters.dfu import DFUAdapter

RUNNER_TYPES: dict[str, type] = {
    "openocd": CMSISDAPAdapter,
    "plugin": PluginRunner,
    "uf2": UF2Adapter,
    "dfu": DFUAdapter,
}


def get_runner_class(type_name: str) -> type | None:
    return RUNNER_TYPES.get(type_name)
