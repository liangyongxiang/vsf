"""Parse hardware-map.yml and return typed board descriptors."""

import yaml
from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class RunnerConfig:
    type: str
    params: dict = field(default_factory=dict)


@dataclass
class BuildConfig:
    source_dir: str
    build_dir: str


@dataclass
class BoardConfig:
    platform: str
    active_runner: str
    runners: dict[str, RunnerConfig]
    serial: str
    baud: int
    build: BuildConfig
    connected: bool = True
    fixtures: list[str] = field(default_factory=list)

    def validate(self) -> None:
        missing = []
        if not self.platform:
            missing.append("platform")
        if not self.active_runner:
            missing.append("active_runner")
        if self.active_runner not in self.runners:
            missing.append(f"runners.{self.active_runner} (active runner not in runners map)")
        if not self.runners:
            missing.append("runners")
        if not self.serial:
            missing.append("serial")
        if not self.baud:
            missing.append("baud")
        if not self.build.source_dir:
            missing.append("build.source_dir")
        if not self.build.build_dir:
            missing.append("build.build_dir")
        if missing:
            raise ValueError(f"Missing required fields: {', '.join(missing)}")


def load(path: str | Path) -> BoardConfig:
    """Load the first connected board from a hardware-map.yml file."""
    p = Path(path)
    with open(p) as f:
        entries = yaml.safe_load(f)

    if not entries:
        raise RuntimeError(f"No entries in {p}")

    for entry in entries:
        if not entry.get("connected", False):
            continue

        runners = {}
        for name, cfg in entry.get("runners", {}).items():
            runners[name] = RunnerConfig(
                type=cfg["type"],
                params={k: v for k, v in cfg.items() if k != "type"},
            )

        build_cfg = entry.get("build", {})
        build = BuildConfig(
            source_dir=build_cfg.get("source_dir", ""),
            build_dir=build_cfg.get("build_dir", ""),
        )

        board = BoardConfig(
            platform=entry.get("platform", ""),
            connected=entry.get("connected", True),
            active_runner=entry.get("active_runner", ""),
            runners=runners,
            serial=entry.get("serial", ""),
            baud=entry.get("baud", 0),
            build=build,
            fixtures=entry.get("fixtures", []),
        )
        board.validate()
        return board

    raise RuntimeError("No connected board found in hardware-map.yml")
