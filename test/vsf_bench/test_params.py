"""Test parameter YAML loading with `include:` directive resolution.

Used by both host-side test scripts and gen_test_params.py (CMake).
"""

from __future__ import annotations

import sys
from pathlib import Path

import yaml


def _deep_merge(base: dict, overlay: dict) -> dict:
    result = {}
    for key, value in base.items():
        if key in overlay:
            if isinstance(value, dict) and isinstance(overlay[key], dict):
                result[key] = _deep_merge(value, overlay[key])
            else:
                result[key] = overlay[key]
        else:
            result[key] = value
    for key, value in overlay.items():
        if key not in base:
            result[key] = value
    return result


def _apply_defaults(params: dict) -> dict:
    for key, value in params.items():
        if not isinstance(value, dict):
            continue
        defaults = value.get("defaults")
        if defaults is None:
            continue
        cases = value.get("cases")
        if not isinstance(cases, list):
            continue
        for i, case in enumerate(cases):
            if isinstance(case, dict):
                cases[i] = _deep_merge(defaults, case)
    return params


def load_yaml_with_includes(yml_path: Path, stack: list[Path] | None = None) -> dict:
    yml_path = Path(yml_path).resolve()
    stack = stack or []
    if yml_path in stack:
        cycle = " -> ".join(str(p) for p in stack + [yml_path])
        print(f"Error: include cycle detected: {cycle}", file=sys.stderr)
        raise SystemExit(1)

    with open(yml_path) as f:
        params = yaml.safe_load(f) or {}

    if not isinstance(params, dict):
        return params

    includes = params.pop("include", None)
    if includes is None:
        return _apply_defaults(params)

    if isinstance(includes, str):
        includes = [includes]
    if not isinstance(includes, list):
        print(f"Error: 'include' in {yml_path} must be a string or list", file=sys.stderr)
        raise SystemExit(1)

    merged: dict = {}
    for inc in includes:
        inc_path = Path(inc)
        if not inc_path.is_absolute():
            inc_path = yml_path.parent / inc_path
        if not inc_path.exists():
            print(f"Error: include file not found: {inc_path} (from {yml_path})", file=sys.stderr)
            raise SystemExit(1)
        sub = load_yaml_with_includes(inc_path, stack + [yml_path])
        for key, value in sub.items():
            if key in merged:
                print(f"Warning: duplicate key '{key}' in {inc_path} overrides earlier include", file=sys.stderr)
            merged[key] = value

    for key, value in params.items():
        if key in merged:
            print(f"Warning: duplicate key '{key}' in {yml_path} overrides include", file=sys.stderr)
        merged[key] = value

    return _apply_defaults(merged)


def load_test_params(project_root: str | Path) -> dict:
    """Load aggregated test params from the standard project location."""
    yml_path = Path(project_root) / "application" / "component" / "vsf-test" / "test_params.yml"
    return load_yaml_with_includes(yml_path)
