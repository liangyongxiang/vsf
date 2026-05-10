"""SWDRunner — flash via OpenOCD."""

import subprocess
import sys
from pathlib import Path

from vsf_bench.hardware_map import RunnerConfig


class SWDRunner:
    def __init__(self, config: RunnerConfig):
        self.interface = config.params["interface"]
        self.target = config.params["target"]
        self.speed = config.params.get("speed", 5000)

    def flash(self, build_dir: Path) -> None:
        """Flash firmware via OpenOCD SWD."""
        # Find the ELF: use the first .elf in build dir or expected name
        elves = list(build_dir.glob("*.elf"))
        if not elves:
            raise FileNotFoundError(f"No .elf found in {build_dir}")
        elf = elves[0]

        cmd = [
            "openocd",
            "-f", f"interface/{self.interface}",
            "-f", f"target/{self.target}",
            "-c", f"adapter speed {self.speed}",
            "-c", f"program {elf} verify reset exit",
        ]
        try:
            subprocess.run(cmd, check=True, capture_output=True)
        except subprocess.CalledProcessError as e:
            print(f"OpenOCD failed (exit {e.returncode}):\n{e.stderr.decode()}", file=sys.stderr)
            raise
