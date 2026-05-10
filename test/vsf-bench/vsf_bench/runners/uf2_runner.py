"""UF2Runner — flash via USB mass storage (BOOTSEL mode)."""

import shutil
import subprocess
import sys
from pathlib import Path

from vsf_bench.hardware_map import RunnerConfig


class UF2Runner:
    def __init__(self, config: RunnerConfig):
        self.mount_point = Path(config.params["mount_point"])

    def flash(self, build_dir: Path) -> None:
        """Flash firmware via UF2 mass storage copy."""
        uf2_files = list(build_dir.glob("*.uf2"))
        if not uf2_files:
            raise FileNotFoundError(f"No .uf2 found in {build_dir}")
        uf2 = uf2_files[0]

        try:
            subprocess.run(["mount", str(self.mount_point)], check=True, capture_output=True)
        except subprocess.CalledProcessError:
            # Mount point might already be auto-mounted
            if not self.mount_point.is_dir():
                print(f"UF2 mount point {self.mount_point} not available (board not in BOOTSEL mode?)", file=sys.stderr)
                raise

        try:
            shutil.copy2(str(uf2), str(self.mount_point))
        finally:
            subprocess.run(["umount", str(self.mount_point)], capture_output=True)
