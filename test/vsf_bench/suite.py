"""Suite discovery, script loading, and resolution.

Pure functions — no hardware interaction. Testable without a board.
"""

import importlib.util
import sys
from pathlib import Path


def discover_suites(project_root: Path) -> dict[str, Path]:
    """Walk the test tree and return {suite_name: script_path}.

    Each `vsf.demo/vsf/test/vsf_test/<peripheral>/scenario/vsf_test_<suite>.py`
    file is registered under `<suite>`.
    """
    suites: dict[str, Path] = {}
    base = project_root / "vsf.demo" / "vsf" / "test" / "vsf_test"
    if not base.exists():
        return suites
    for peripheral_dir in base.iterdir():
        scenario_dir = peripheral_dir / "scenario"
        if not scenario_dir.is_dir():
            continue
        for f in scenario_dir.glob("vsf_test_*.py"):
            stem = f.stem
            if stem.startswith("vsf_test_"):
                suite_name = stem[len("vsf_test_"):]
                suites[suite_name] = f
    return suites


def load_script_module(path: Path):
    """Import a test script `.py` file as a module."""
    if not path.exists():
        raise FileNotFoundError(f"Test script not found: {path}")
    spec = importlib.util.spec_from_file_location("test_script", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Cannot create module spec for {path}")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    if not hasattr(mod, "run"):
        raise AttributeError(f"Test script {path} must define run(project_root, serial)")
    return mod


def script_needs_la(script_path: Path | None, mod=None) -> bool:
    """A script needs LA iff it defines `decode()`."""
    if mod is not None:
        return hasattr(mod, "decode")
    if script_path is None:
        return False
    try:
        source = script_path.read_text()
        return "def decode(" in source
    except Exception:
        return False


def resolve_suites(
    requested: list[str] | None,
    script_override: Path | None,
    discovered: dict[str, Path],
) -> list[tuple[str, Path | None]]:
    """Apply --suite / --script arguments to the discovered suite map.

    * No --suite → run every discovered suite in alphabetical order.
    * One or more --suite names → run those, in given order.
    * --script overrides the script path for a single --suite.
    """
    if requested:
        if script_override and len(requested) > 1:
            raise ValueError("--script can only be used with a single --suite")
        ordered: list[tuple[str, Path | None]] = []
        for name in requested:
            if script_override:
                ordered.append((name, script_override.resolve()))
            elif name in discovered:
                ordered.append((name, discovered[name]))
            else:
                raise KeyError(
                    f"Suite not found: {name}. Discovered: {sorted(discovered.keys())}"
                )
        return ordered
    return [(name, discovered[name]) for name in sorted(discovered.keys())]
