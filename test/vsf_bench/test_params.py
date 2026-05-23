"""Test parameter YAML loading with `include:` directive resolution.

Used by both host-side test scripts and gen_test_params.py (CMake).

Environment variable:
    VSF_TEST_GLOBAL_PARAMS_DIR  —  absolute or project-relative path to the
                                    global test-params directory.  If unset,
                                    the loader defaults to
                                    <project_root>/vsf.demo/vsf/test/vsf_test/params.
"""

from __future__ import annotations

import os
import sys
from pathlib import Path

import yaml

GLOBAL_PARAMS_ENV = "VSF_TEST_GLOBAL_PARAMS_DIR"


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

    merged: dict = {}
    for inc in includes:
        inc_path = _resolve_include(inc, yml_path.parent, global_base)
        if not inc_path.exists():
            print(f"Error: include file not found: {inc_path} (from {yml_path})", file=sys.stderr)
            raise SystemExit(1)
        sub = load_yaml_with_includes(inc_path, stack + [yml_path], global_base)
        for key, value in sub.items():
            if key in merged:
                print(f"Warning: duplicate key '{key}' in {inc_path} overrides earlier include", file=sys.stderr)
            merged[key] = value

    for key, value in params.items():
        if key in merged:
            print(f"Warning: duplicate key '{key}' in {yml_path} overrides include", file=sys.stderr)
        merged[key] = value

    return _apply_defaults(merged)


def load_test_params(
    project_root: str | Path,
    global_base: str | Path | None = None,
) -> dict:
    """Load aggregated test params from the standard project location.

    Local YAMLs in application/component/vsf-test/ override global YAMLs.
    The global base is resolved in this order:
      1. Explicit `global_base` argument (absolute or project-relative).
      2. Environment variable `VSF_TEST_GLOBAL_PARAMS_DIR`.
      3. Default: <project_root>/vsf.demo/vsf/test/vsf_test/params
    """
    local_dir = Path(project_root) / "application" / "component" / "vsf-test"
    yml_path = local_dir / "test_params.yml"

    if global_base is not None:
        gb = Path(global_base)
        if not gb.is_absolute():
            gb = Path(project_root) / gb
    elif os.environ.get(GLOBAL_PARAMS_ENV):
        gb = Path(os.environ[GLOBAL_PARAMS_ENV])
    else:
        gb = Path(project_root) / "vsf.demo" / "vsf" / "test" / "vsf_test" / "params"

    return load_yaml_with_includes(yml_path, global_base=gb)
