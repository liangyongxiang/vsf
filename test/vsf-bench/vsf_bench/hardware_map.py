"""Parse hardware-map.yml and return typed board descriptors."""

import yaml
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any


@dataclass
class RunnerConfig:
    type: str
    params: dict[str, Any] = field(default_factory=dict)


@dataclass
class BuildConfig:
    source_dir: str
    build_dir: str


@dataclass
class BoardConfig:
    platform: str
    connected: bool
    active_runner: str
    runners: dict[str, RunnerConfig]
    serial: str
    baud: int
    build: BuildConfig
    fixtures: list[str] = field(default_factory=list)


def load(path: str | Path) -> BoardConfig:
    """Load the first connected board from a hardware-map.yml file."""
    p = Path(path)
    with open(p) as f:
        entries = yaml.safe_load(f)

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
            source_dir=build_cfg["source_dir"],
            build_dir=build_cfg["build_dir"],
        )

        return BoardConfig(
            platform=entry["platform"],
            connected=entry.get("connected", True),
            active_runner=entry["active_runner"],
            runners=runners,
            serial=entry["serial"],
            baud=entry["baud"],
            build=build,
            fixtures=entry.get("fixtures", []),
        )

    raise RuntimeError("No connected board found in hardware-map.yml")
