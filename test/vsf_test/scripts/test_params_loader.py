"""Shared YAML loader for test_params files with `include:` resolution.

Used by both:
- `gen_test_params.py` (the C-header generator invoked by CMake)
- The per-scenario test scripts (`vsf_test_<scenario>.py`)

The `include:` directive lets the aggregator `test_params.yml` pull in
per-peripheral or per-scenario YAMLs (e.g. `usart.yml`, `gpio.yml`).
Resolution semantics:
  - Includes are loaded in order; later files override earlier ones on
    top-level key collision.
  - The entry file's own top-level keys merge on top of all includes.
  - Relative include paths resolve against the directory of the file
    declaring them.
  - Cycle detection: a file currently in the stack cannot be re-included.
"""

from __future__ import annotations

import sys
from pathlib import Path

import yaml


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
        return params

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
                print(
                    f"Warning: duplicate key '{key}' in {inc_path} overrides earlier include",
                    file=sys.stderr,
                )
            merged[key] = value

    for key, value in params.items():
        if key in merged:
            print(
                f"Warning: duplicate key '{key}' in {yml_path} overrides include",
                file=sys.stderr,
            )
        merged[key] = value

    return merged
