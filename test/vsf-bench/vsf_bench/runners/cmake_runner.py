"""CMakeRunner — cmake configure + build."""

import subprocess
from pathlib import Path

from vsf_bench.hardware_map import BuildConfig


class CMakeRunner:
    def __init__(self, build_config: BuildConfig, project_root: str | Path):
        self.source_dir = Path(project_root) / build_config.source_dir
        self.build_dir = Path(project_root) / build_config.build_dir
        self.project_root = Path(project_root)

    def build(self) -> Path:
        """Run cmake configure (if needed) + build. Returns build directory."""
        self.build_dir.mkdir(parents=True, exist_ok=True)

        if not (self.build_dir / "CMakeCache.txt").exists():
            subprocess.run(
                [
                    "cmake",
                    "-B", str(self.build_dir),
                    "-S", str(self.source_dir),
                ],
                check=True,
                cwd=str(self.project_root),
            )

        subprocess.run(
            ["cmake", "--build", str(self.build_dir)],
            check=True,
            cwd=str(self.project_root),
        )

        return self.build_dir
