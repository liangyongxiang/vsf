"""UF2Runner — flash via USB mass storage (BOOTSEL mode)."""

import shutil
import subprocess
from pathlib import Path

from vsf_bench.hardware_map import RunnerConfig


class UF2Runner:
    def __init__(self, config: RunnerConfig):
        self.mount_point = Path(config.params["mount_point"])

    def flash(self, build_dir: Path) -> None:
        """Flash firmware via UF2 mass storage copy."""
        uf2 = build_dir / "vsf_demo.uf2"
        if not uf2.exists():
            raise FileNotFoundError(f"UF2 not found: {uf2}")

        subprocess.run(["mount", str(self.mount_point)], check=True)
        try:
            shutil.copy2(str(uf2), str(self.mount_point))
        finally:
            subprocess.run(["umount", str(self.mount_point)], check=True)
