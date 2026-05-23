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
  - **Global fallback**: if a relative include is not found in the local
    directory, the loader falls back to `VSF_TEST_GLOBAL_PARAMS_DIR`.
  - Cycle detection: a file currently in the stack cannot be re-included.
"""

from __future__ import annotations

import os
import sys
from pathlib import Path

import yaml


GLOBAL_PARAMS_ENV = "VSF_TEST_GLOBAL_PARAMS_DIR"


def _get_global_base() -> Path | None:
    env = os.environ.get(GLOBAL_PARAMS_ENV)
    return Path(env) if env else None


def _deep_merge(base: dict, overlay: dict) -> dict:
    """Deep merge overlay into base. overlay values override base values."""
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
    """For each scenario, deep-merge scenario-level defaults into each case.

    The `defaults` key is preserved in the scenario dict so that consumers
    (e.g. gen_test_params.py) can still emit scenario-level macros from it.
    Only `host` and `la` subtrees are skipped from the C-generation view.
    """
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


def _resolve_include(inc: str, local_dir: Path, global_base: Path | None) -> Path:
    """Resolve an include path: local first, then global fallback."""
    inc_path = Path(inc)
    if inc_path.is_absolute():
        return inc_path

    # Try local first
    local_path = local_dir / inc_path
    if local_path.exists():
        return local_path

    # Fallback to global base
    if global_base is not None:
        global_path = global_base / inc_path
        if global_path.exists():
            return global_path

    # Return local path so the existing error message is accurate
    return local_path


def load_yaml_with_includes(
    yml_path: Path,
    stack: list[Path] | None = None,
    global_base: Path | None = None,
) -> dict:
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

    global_base = global_base or _get_global_base()

    merged: dict = {}
    for inc in includes:
        inc_path = _resolve_include(inc, yml_path.parent, global_base)
        if not inc_path.exists():
            print(f"Error: include file not found: {inc_path} (from {yml_path})", file=sys.stderr)
            raise SystemExit(1)
        sub = load_yaml_with_includes(inc_path, stack + [yml_path], global_base)
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

    return _apply_defaults(merged)
