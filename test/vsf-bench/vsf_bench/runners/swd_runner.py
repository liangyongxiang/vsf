"""SWDRunner — flash via OpenOCD."""

import subprocess
from pathlib import Path

from vsf_bench.hardware_map import RunnerConfig


class SWDRunner:
    def __init__(self, config: RunnerConfig):
        self.interface = config.params["interface"]
        self.target = config.params["target"]
        self.speed = config.params.get("speed", 5000)

    def flash(self, build_dir: Path) -> None:
        """Flash firmware via OpenOCD SWD."""
        elf = build_dir / "vsf_demo.elf"
        if not elf.exists():
            raise FileNotFoundError(f"ELF not found: {elf}")

        cmd = [
            "openocd",
            "-f", f"interface/{self.interface}",
            "-f", f"target/{self.target}",
            "-c", f"adapter speed {self.speed}",
            "-c", f"program {elf} verify reset exit",
        ]
        subprocess.run(cmd, check=True, capture_output=True)
