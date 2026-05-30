"""Shared phase functions used by all vsf-bench CLI scripts.

Each phase function does one thing and is called by exactly one CLI entry:
  * `load_board()`   — YAML → BoardConfig (used by all)
  * `build_phase()`  — cmake build (vsf-bench-build)
  * `flash_phase()`  — runner flash (vsf-bench-flash)
  * `run_test_phase()` — multi-suite LA-aware test orchestration (vsf-bench-test)

The unified `vsf-bench` entry (`cli/run.py`) composes these in order based on
the `--build/--flash/--test/--all` flags.
"""

import inspect
import json
import sys
import time
from datetime import datetime
from pathlib import Path

from vsf_bench.hardware_map import load as load_hardware_map, validate_runners
from vsf_bench.builders.registry import get_builder_class
from vsf_bench.runners.registry import get_runner_class
from vsf_bench.instruments.serial_instrument import SerialInstrument
from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.suite import discover_suites, load_script_module, script_needs_la, resolve_suites
from vsf_bench.test_params_loader import load_test_params


def __bprint(*args, **kwargs):
    """Print with [vsf-bench] prefix and ISO timestamp."""
    ts = datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
    print(f"[vsf-bench] [{ts}]", *args, **kwargs)


def load_board(hardware_map_path: Path):
    """Read hardware-map.yml and return a validated board config."""
    board = load_hardware_map(str(hardware_map_path))
    validate_runners(board)
    return board


def build_phase(board) -> Path:
    """Run the configured build tool → return build_dir. Raises on error."""
    build_tool = board.build.build_tool
    builder_cfg = board.build.builders.get(build_tool)
    if builder_cfg is None:
        raise RuntimeError(f"Build tool '{build_tool}' not found in builders map")
    builder_cls = get_builder_class(builder_cfg.type)
    if builder_cls is None:
        raise RuntimeError(f"Unknown builder type: {builder_cfg.type}")
    __bprint(f"Building ({board.build.source_dir}) via {build_tool}...")
    builder = builder_cls(board.build)
    build_dir = builder.build()
    __bprint(f"Build complete: {build_dir}")
    return build_dir


def flash_phase(board, build_dir: Path) -> None:
    """Run the active flash runner. Raises on runner error."""
    runner_cfg = board.runners[board.active_runner]
    runner_cls = get_runner_class(runner_cfg.type)
    if runner_cls is None:
        raise RuntimeError(f"Unknown runner type: {runner_cfg.type}")
    runner = runner_cls(runner_cfg)
    __bprint(f"Flashing via {board.active_runner}...")
    runner.flash(build_dir)
    __bprint("Flash complete")


# ---------------------------------------------------------------------------
# Test phase
# ---------------------------------------------------------------------------

def _query_firmware_suites(ser: SerialInstrument) -> set[str]:
    ser.send("vsf-test list-suites\r\n")
    time.sleep(0.3)
    output = ser.read_all(timeout=2.0)
    suites: set[str] = set()
    for line in output.splitlines():
        line = line.strip()
        if line and line[0].isdigit():
            parts = line.split(None, 1)
            if len(parts) == 2:
                suites.add(parts[1])
    return suites


def _build_run_cmd(suite: str, case: str | None) -> str:
    if case:
        return f"vsf-test run-case {suite} {case}\r\n"
    return f"vsf-test run-suite {suite}\r\n"


def _find_scenario_for_suite(params: dict, suite_name: str) -> tuple[str, dict] | None:
    """Find YAML scenario key and data for a given suite name.

    Tries direct key match first, then common peripheral-prefix patterns
    ({peripheral}_{name}, {peripheral}_rx_{name}, {peripheral}_tx_{name}).
    """
    for key, value in params.items():
        if key == "marker" or not isinstance(value, dict):
            continue
        if key == suite_name:
            return key, value
        scenario_name = value.get("name", "")
        for prefix in (
            "usart", "gpio", "i2c", "spi", "adc", "pwm",
            "timer", "rtc", "flash", "wdt", "dma",
        ):
            if suite_name == f"{prefix}_{scenario_name}":
                return key, value
            if suite_name == f"{prefix}_rx_{scenario_name}":
                return key, value
            if suite_name == f"{prefix}_tx_{scenario_name}":
                return key, value
    return None


def _resolve_case_value(params: dict, suite_name: str, case_value: str) -> int:
    """Convert a case parameter value (e.g. '921600') to its case index.

    Iterates the matched scenario's cases and checks every field (except
    'idx', 'host', 'la') for an exact string match.
    """
    result = _find_scenario_for_suite(params, suite_name)
    if result is None:
        raise ValueError(f"Suite '{suite_name}' not found in test params")

    _, scenario = result
    cases = scenario.get("cases", [])
    for case in cases:
        for field, val in case.items():
            if field in ("idx", "host", "la"):
                continue
            if str(val) == case_value:
                return case["idx"]

    raise ValueError(
        f"Case value '{case_value}' not found in suite '{suite_name}'"
    )


def _drain_repl(ser: SerialInstrument) -> None:
    """Discard any stale REPL output left from a previous suite."""
    ser.read_all(timeout=0.1)


def _send_shuffle_seed(
    ser: SerialInstrument, suite_name: str, seed: int
) -> bool:
    """Shuffle is no longer supported in the simplified shell (removed in shell
    simplification). This is a no-op for backward CLI compatibility.
    --random still works on the host side (Python randomizes suite order)."""
    __bprint(f"{suite_name}: shuffle seed={seed} (host-side only)")
    return True


def _run_script_phase1(
    suite_name: str,
    case: str | None,
    script_module,
    project_root: Path,
    ser: SerialInstrument,
) -> bool:
    """Send trigger, run script.run(). Returns True on PASS."""
    case_tag = f".{case}" if case else ""
    cmd = _build_run_cmd(suite_name, case)
    _drain_repl(ser)
    ser.send(cmd)
    __bprint(f"Triggered: {cmd.strip()}")

    # vsf-test-shell emits "Suite ack: <name>" on a successful lookup or
    # "Suite not found: <name>" / "Case not found: <case>" otherwise. One
    # of these always fires within ~50 ms of the trigger.
    # Retry once on timeout: stale output from the previous suite can delay
    # the ack just past the 1 s boundary under heavy shared-LA load.
    ack = None
    for attempt, tmo in ((1, 1.0), (2, 2.0)):
        try:
            ack = ser.expect(r"Suite ack:|Suite not found:|Case not found:", timeout=tmo)
            break
        except TimeoutError:
            if attempt == 1:
                _drain_repl(ser)
                ser.send(cmd)
                __bprint(f"Retrying: {cmd.strip()}")
            else:
                __bprint(f"FAIL: {suite_name}{case_tag}: no shell ack within 1s")
                return False
    assert ack is not None
    if "not found" in ack:
        __bprint(f"FAIL: {suite_name}{case_tag}: {ack.strip()}")
        return False

    try:
        if script_module is not None:
            script_module.run(project_root, ser)
        else:
            ser.expect_test_summary(suite_name, timeout=1.5)
        __bprint(f"PASS phase1: {suite_name}{case_tag}")
        return True
    except (TimeoutError, AssertionError, RuntimeError, KeyError, AttributeError) as e:
        __bprint(f"FAIL: {suite_name}{case_tag}: {e}")
        return False


def _call_decode(mod, project_root: Path, la: LogicAnalyzerInstrument,
                 start_ns: int | None, end_ns: int | None,
                 marker_baud: int = 115200) -> None:
    """Invoke a script's decode() with whichever signature it accepts."""
    sig = inspect.signature(mod.decode)
    params = sig.parameters
    kwargs = {}
    if "decode_start_ns" in params:
        kwargs["decode_start_ns"] = start_ns
    if "decode_end_ns" in params:
        kwargs["decode_end_ns"] = end_ns
    if "marker_baud" in params:
        kwargs["marker_baud"] = marker_baud
    mod.decode(project_root, la, **kwargs)


def _new_la(la_cfg, cli_path: Path, capture_path: Path) -> LogicAnalyzerInstrument:
    return LogicAnalyzerInstrument(
        cli_path=cli_path,
        device=la_cfg.device,
        samplerate=la_cfg.samplerate,
        channels=la_cfg.channels,
        capture_path=capture_path,
    )


def _mk_log_dir(log_dir: Path | None, ordered_suites: list[tuple[str, Path | None]]) -> Path:
    if log_dir:
        run_dir = log_dir.resolve()
    else:
        timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
        tag = ordered_suites[0][0] if len(ordered_suites) == 1 else "vsf_test"
        run_dir = Path(f"logs/{timestamp}-{tag}")
    run_dir.mkdir(parents=True, exist_ok=True)
    return run_dir


def run_test_phase(
    board,
    project_root: Path,
    suite_names: list[str] | None,
    script_override: Path | None,
    case_specs: list[str],
    la_mode: str,
    log_dir: Path | None,
    shuffle_seed: int | None = None,
) -> bool:
    """Run suites against firmware that is already flashed and running.

    Returns True if every suite/case passed (phase1 + decode), False otherwise.
    Does NOT build or flash — that is the caller's responsibility.
    """
    discovered = discover_suites()
    ordered_suites = resolve_suites(suite_names, script_override, discovered)
    if not ordered_suites:
        raise RuntimeError("No suites discovered")

    __bprint(f"Suites: {[s for s, _ in ordered_suites]}")
    __bprint(f"LA mode: {la_mode}")

    if case_specs and len(ordered_suites) > 1:
        raise ValueError("--case/--case-index requires exactly one --suite")

    # Resolve non-numeric --case values to indices via YAML lookup
    if case_specs and len(ordered_suites) == 1:
        suite_name = ordered_suites[0][0]
        # test_params.yml is cwd-relative by convention
        test_params_yml = Path("application/component/vsf-test/test_params.yml")
        params = load_test_params(
            test_params_yml=test_params_yml,
            board_pins_path=board.board_pins,
        )
        resolved: list[str] = []
        for spec in case_specs:
            if spec.isdigit():
                resolved.append(spec)
            else:
                idx = _resolve_case_value(params, suite_name, spec)
                resolved.append(str(idx))
                __bprint(f"Resolved --case {spec} -> index {idx}")
        case_specs = resolved

    run_dir = _mk_log_dir(log_dir, ordered_suites)
    log_path = run_dir / "vsf-bench.jsonl"

    ser = SerialInstrument(board.serial, board.baud, audit_log=log_path)
    ser.open()

    la_cfg = board.logic_analyzer
    cli_path = Path(la_cfg.cli) if la_cfg else None

    try:
        ser.expect("VSF Test Ready", timeout=10.0)
    except TimeoutError:
        __bprint("Warning: REPL banner not seen, continuing anyway")

    # When no explicit --suite filter, intersect with what the firmware reports.
    if not suite_names:
        fw_suites = _query_firmware_suites(ser)
        if fw_suites:
            skipped = [s for s, _ in ordered_suites if s not in fw_suites]
            ordered_suites = [(s, p) for s, p in ordered_suites if s in fw_suites]
            if skipped:
                __bprint(f"Skipped (not in firmware): {skipped}")
        __bprint(f"Effective suites: {[s for s, _ in ordered_suites]}")

    loaded: list[tuple[str, Path | None, object | None, bool]] = []
    for suite_name, script_path in ordered_suites:
        if script_path is None:
            loaded.append((suite_name, None, None, False))
            continue
        mod = load_script_module(script_path)
        loaded.append((suite_name, script_path, mod, script_needs_la(script_path, mod)))

    any_needs_la = any(needs for _, _, _, needs in loaded) and la_cfg is not None and cli_path is not None
    overall_pass = True

    if la_mode == "shared" and any_needs_la:
        overall_pass = _test_loop_shared_la(
            loaded, project_root, ser, la_cfg, cli_path, run_dir, case_specs,
            board,
            shuffle_seed=shuffle_seed,
        )
    else:
        overall_pass = _test_loop_per_suite(
            loaded, project_root, ser, la_cfg, cli_path, run_dir, case_specs,
            shuffle_seed=shuffle_seed,
        )

    ser.close()

    with open(log_path, "a") as f:
        verdict = "pass" if overall_pass else "fail"
        f.write(json.dumps({"verdict": verdict}) + "\n")
    print()
    __bprint(f"{'PASS' if overall_pass else 'FAIL'}")
    return overall_pass


def _test_loop_shared_la(
    loaded, project_root, ser, la_cfg, cli_path, run_dir, case_specs,
    board,
    shuffle_seed: int | None = None,
) -> bool:
    """Shared LA mode: one capture spans the contiguous LA-needing block.

    `la_start_t` is recorded AFTER `wait_until_started()` returns, so it
    aligns tightly with the dsview-cli capture file's t=0. Each suite's
    decode window is padded by SHARED_WINDOW_PAD_NS on both sides as a
    small safety margin for residual scheduler / USB jitter.
    """
    SHARED_WINDOW_PAD_NS = 500_000_000  # 500 ms each side

    la_indices = [i for i, (_, _, _, n) in enumerate(loaded) if n]
    first_la_idx = la_indices[0]
    last_la_idx = la_indices[-1]

    shared_capture = run_dir / "shared-capture.dsl"
    shared_la: LogicAnalyzerInstrument | None = None
    la_start_t: float | None = None
    suite_windows: list[tuple[str, object, int, int]] = []
    overall_pass = True

    for i, (suite_name, _script_path, mod, _needs) in enumerate(loaded):
        if i == first_la_idx:
            shared_la = _new_la(la_cfg, cli_path, shared_capture)
            shared_la.start(300.0)
            shared_la.wait_until_started(timeout=5.0)
            la_start_t = time.monotonic()
            __bprint(f"Shared LA started -> {shared_capture}")

        cases_to_run = case_specs if case_specs else [None]
        for case in cases_to_run:
            if shuffle_seed is not None and case is None:
                if not _send_shuffle_seed(ser, suite_name, shuffle_seed):
                    overall_pass = False
                    continue
            case_tag = f".{case}" if case else ""
            __bprint(f"Suite: {suite_name}{case_tag}")
            t_start = int((time.monotonic() - la_start_t) * 1e9) if la_start_t is not None else 0
            ok = _run_script_phase1(suite_name, case, mod, project_root, ser)
            t_end = int((time.monotonic() - la_start_t) * 1e9) if la_start_t is not None else 0
            if not ok:
                overall_pass = False
            if mod is not None and hasattr(mod, "decode") and shared_la is not None:
                suite_windows.append((suite_name, mod, t_start, t_end))

        if i == last_la_idx and shared_la is not None:
            __bprint("Stopping shared LA...")
            shared_la.stop()
            try:
                shared_la.wait(timeout=30.0)
            except (TimeoutError, RuntimeError) as e:
                __bprint(f"LA wait warning: {e}")

    for suite_name, mod, t_start, t_end in suite_windows:
        decode_start = max(0, t_start - SHARED_WINDOW_PAD_NS)
        decode_end = t_end + SHARED_WINDOW_PAD_NS
        __bprint(f"Decoding (shared): {suite_name}  window=[{decode_start/1e9:.2f}s,{decode_end/1e9:.2f}s]")
        try:
            _call_decode(mod, project_root, shared_la, decode_start, decode_end, board.baud)
            __bprint(f"PASS decode: {suite_name}")
        except (AssertionError, RuntimeError, KeyError, AttributeError, FileNotFoundError) as e:
            __bprint(f"FAIL decode: {suite_name}: {e}")
            overall_pass = False

    return overall_pass


def _test_loop_per_suite(
    loaded, project_root, ser, la_cfg, cli_path, run_dir, case_specs,
    shuffle_seed: int | None = None,
) -> bool:
    """Per-suite LA mode: one capture per suite (or no LA at all)."""
    overall_pass = True

    for suite_name, _script_path, mod, needs_la in loaded:
        cases_to_run = case_specs if case_specs else [None]
        for case in cases_to_run:
            case_tag = f".{case}" if case else ""
            __bprint(f"Suite: {suite_name}{case_tag}")
            if shuffle_seed is not None and case is None:
                if not _send_shuffle_seed(ser, suite_name, shuffle_seed):
                    overall_pass = False
                    continue

            scene_la: LogicAnalyzerInstrument | None = None
            if needs_la and la_cfg is not None and cli_path is not None:
                label = f"{suite_name}{('_' + case) if case else ''}"
                capture_path = run_dir / f"{label}-capture.dsl"
                scene_la = _new_la(la_cfg, cli_path, capture_path)
                scene_la.start(180.0)
                scene_la.wait_until_started(timeout=5.0)

            ok = _run_script_phase1(suite_name, case, mod, project_root, ser)
            if not ok:
                overall_pass = False

            if scene_la is not None:
                scene_la.stop()
                try:
                    scene_la.wait(timeout=15.0)
                except (TimeoutError, RuntimeError) as e:
                    __bprint(f"LA wait warning: {e}")

            if ok and mod is not None and hasattr(mod, "decode") and scene_la is not None:
                try:
                    _call_decode(mod, project_root, scene_la, None, None, board.baud)
                    __bprint(f"PASS decode: {suite_name}{case_tag}")
                except (AssertionError, RuntimeError, KeyError, AttributeError, FileNotFoundError) as e:
                    __bprint(f"FAIL decode: {suite_name}{case_tag}: {e}")
                    overall_pass = False

    return overall_pass
