"""Pipeline step execution engine — unified steps model.

build, program, power_cycle, delay, wait_for, serial_send, gpio_set,
la_start/stop/decode, run, loop are all equal step types.
"""

from __future__ import annotations

import subprocess
import time
from dataclasses import dataclass, field
from pathlib import Path
from typing import TYPE_CHECKING, Any

if TYPE_CHECKING:
    from vsf_bench.config.models import (
        BoardConfig, PipelineConfig, ProjectConfig, StepConfig,
    )
    from vsf_bench.utils.serial import SerialInstrument

from vsf_bench.utils.tee_logger import get_logger as _get_logger


def _log(message: str) -> None:
    try:
        _get_logger().event(message)
    except RuntimeError:
        print(f"[vsf-bench] {message}")


@dataclass
class PipelineContext:
    """Serial session lifecycle manager.

    Replaces the six module-level globals (_debug_ser, _debug_buf, ...)
    with a single object that owns all named serial sessions.
    """
    board: "BoardConfig"
    run_dir: Path
    _serial_sessions: dict[str, "SerialInstrument"] = field(default_factory=dict)
    _debug_buf: str = ""
    _vendor_buf: bytes = b""

    def serial(self, name: str = "debug") -> "SerialInstrument":
        """Get-or-create a named serial session. One open handle per name."""
        from vsf_bench.utils.serial import SerialInstrument
        if name not in self._serial_sessions:
            sp = self.board.serial_ports.get(name)
            if not sp or not sp.port:
                raise RuntimeError(
                    f"Board '{self.board.name}' has no serial port '{name}'. "
                    f"Add it to serial_ports in hardware-map.yml."
                )
            ser = SerialInstrument(sp.port, sp.baudrate)
            ser.open()
            self._serial_sessions[name] = ser
        return self._serial_sessions[name]

    def close_all(self) -> None:
        """Close all serial sessions."""
        for _name, ser in self._serial_sessions.items():
            try:
                ser.close()
            except Exception:
                pass
        self._serial_sessions.clear()

    def write_debug_buf(self) -> None:
        """Flush debug stream buffer to run_dir/debug_stream.log."""
        if not self._debug_buf or not self.run_dir:
            return
        normalized = self._debug_buf.replace("\r\n", "\n").replace("\r", "\n")
        output = self.run_dir / "debug_stream.log"
        mode = "ab" if output.exists() else "wb"
        with open(output, mode) as f:
            f.write(normalized.encode("utf-8"))
        _log(f"  debug_stream: {len(normalized)} bytes -> {output}")
        self._debug_buf = ""

    def write_vendor_buf(self) -> None:
        """Flush vendor log buffer to run_dir/vendor_bt.log."""
        if not self._vendor_buf or not self.run_dir:
            return
        output = self.run_dir / "vendor_bt.log"
        with open(output, "wb") as f:
            f.write(self._vendor_buf)
        _log(f"  vendor_log: {len(self._vendor_buf)} bytes -> {output}")
        self._vendor_buf = b""


def execute_pipeline(
    pipeline: PipelineConfig,
    board: BoardConfig,
    run_dir: Path,
    project_map: dict[str, ProjectConfig],
    overrides: dict[str, Any] | None = None,
    repeat_override: int | None = None,
) -> bool:
    """Execute all steps. Returns True if all pass."""
    overrides = overrides or {}
    ctx = PipelineContext(board=board, run_dir=run_dir)
    try:
        return _execute_steps(
            pipeline.steps, ctx, project_map,
            overrides, repeat_override, pipeline.timeout,
        )
    finally:
        _stop_debug_stream_capture(ctx)
        ctx.close_all()


def _execute_steps(
    steps: list[StepConfig],
    ctx: PipelineContext,
    project_map: dict[str, ProjectConfig],
    overrides: dict[str, Any],
    repeat_override: int | None,
    timeout: float | None,
) -> bool:
    t0 = time.monotonic()
    all_ok = True

    for step in steps:
        if timeout and (time.monotonic() - t0) > timeout:
            _log(f"Skipping remaining {len(steps)} steps — pipeline timeout ({timeout}s)")
            break
        if repeat_override is not None and step.type.value == "loop":
            step.params["repeat"] = repeat_override
        _apply_overrides(step, overrides)
        ok = _execute_step_with_retry(step, ctx, project_map, overrides)
        if not ok:
            all_ok = False
            if step.on_failure:
                _log(f"Step '{step.id or step.type.value}' failed — executing on_failure")
                _execute_steps(step.on_failure, ctx, project_map, overrides, None, None)
            if not step.continue_on_error:
                _log(f"Pipeline stopped at {step.id or step.type.value}")
                return False
    return all_ok


def _apply_overrides(step: StepConfig, overrides: dict[str, Any]) -> None:
    if not overrides:
        return
    st = step.type.value
    for key, val in overrides.items():
        # id.key match
        if step.id:
            prefix = f"{step.id}."
            if key.startswith(prefix):
                param_key = key[len(prefix):]
                step.params[param_key] = val
                return
        # type.key match (e.g. loop.repeat=1)
        prefix = f"{st}."
        if key.startswith(prefix):
            param_key = key[len(prefix):]
            step.params[param_key] = val
            return
        # bare key match
        if "." not in key and key in step.params:
            step.params[key] = val


def _execute_step_with_retry(
    step: StepConfig, ctx: PipelineContext,
    project_map: dict[str, ProjectConfig], overrides: dict[str, Any],
) -> bool:
    max_tries = step.max_retries
    for attempt in range(1, max_tries + 1):
        try:
            _execute_one_step(step, ctx, project_map, overrides)
            return True
        except Exception as e:
            _log(f"Step '{step.id or step.type.value}' attempt {attempt}/{max_tries} failed: {e}")
            if attempt < max_tries:
                time.sleep(0.5)
    return False


def _execute_one_step(
    step: StepConfig, ctx: PipelineContext,
    project_map: dict[str, ProjectConfig], overrides: dict[str, Any],
) -> None:
    st = step.type.value
    p = step.params
    board = ctx.board
    _log(f"  [{st}] {step.id or ''} {p}")

    if st == "build":
        _step_build(p, project_map, ctx.run_dir)
    elif st == "program":
        _step_program(p, ctx, project_map)
    elif st == "power_cycle":
        _step_power_cycle(board, p)
    elif st == "power_off":
        _step_power(board, state=0)
    elif st == "power_on":
        _step_power(board, state=1)
    elif st == "delay":
        dur = float(p.get("duration", p.get("delay", 1.0)))
        _log(f"  delay {dur}s")
        time.sleep(dur)
    elif st == "wait_for":
        _step_wait_for(p, ctx)
    elif st == "serial_send":
        _step_serial_send(p, ctx)
    elif st == "gpio_set":
        _step_gpio_set(p, board)
    elif st == "la_start":
        _step_la_start(p, board, ctx.run_dir)
    elif st == "la_stop":
        _step_la_stop(board, ctx.run_dir)
    elif st == "la_decode":
        _step_la_decode(p, board, ctx.run_dir)
    elif st == "debug_stream_start":
        _start_debug_stream_capture(ctx)
    elif st == "debug_stream_stop":
        _stop_debug_stream_capture(ctx)
    elif st == "vendor_log_start":
        _start_vendor_log_capture(ctx, p)
    elif st == "vendor_log_stop":
        _stop_vendor_log_capture(ctx, p, ctx.run_dir)
    elif st == "vendor_log":
        _capture_vendor_log(ctx, p, ctx.run_dir)
    elif st == "run":
        _step_run(p, ctx.run_dir)
    elif st == "loop":
        loop_repeat = p.get("repeat", 1)
        loop_steps = step.steps or []
        _log(f"  loop x{loop_repeat}")
        for i in range(loop_repeat):
            iter_dir = ctx.run_dir / "runs" / f"{i+1:02d}"
            iter_dir.mkdir(parents=True, exist_ok=True)
            _log(f"  loop iteration {i+1}/{loop_repeat} → {iter_dir}")
            ctx.run_dir = iter_dir
            _execute_steps(loop_steps, ctx, project_map, overrides, None, None)
    else:
        raise RuntimeError(f"Unknown step type: {st}")


# ------------------------------------------------------------------ step impls

# Global state for LA capture session
_la_adapter = None


def _step_build(params, project_map, run_dir):
    project_name = params.get("build") or params.get("project") or next(iter(project_map))
    _ensure_project_loaded(project_name, project_map)
    project = project_map[project_name]
    from vsf_bench.phases.build import build_phase
    build_phase(project)


def _ensure_project_loaded(project_name: str, project_map: dict) -> None:
    if project_name not in project_map:
        from vsf_bench.config.map import load_project
        hw_path = Path("board/hardware-map.yml")  # fallback
        project_map[project_name] = load_project(str(hw_path), project_name)


def _step_program(params, ctx: PipelineContext, project_map):
    project_name = params.get("program") or params.get("project") or next(iter(project_map))
    _ensure_project_loaded(project_name, project_map)
    project = project_map[project_name]
    board = ctx.board
    for _rcfg in project.runners.values():
        p = _rcfg.params
        sp_prog = board.serial_ports.get("program")
        sp_dbg = board.serial_ports.get("debug")
        p.setdefault("program_port", sp_prog.port if sp_prog else "")
        p.setdefault("debug_port", sp_dbg.port if sp_dbg else "")
        p.setdefault("debug_baudrate", sp_dbg.baudrate if sp_dbg else 0)
    from vsf_bench.phases.program import program_phase
    build_dir = Path(project.build.build_dir)
    program_phase(board, build_dir, project=project)


def _step_power_cycle(board, params=None):
    from vsf_bench.board import power_cycle as _board_power_cycle
    delay = float((params or {}).get("delay", 0.5))
    _board_power_cycle(board, delay_off_s=delay)


def _step_power(board, state):
    from vsf_bench.board import find_hub_by_addr as _find_hub_by_addr
    from smartusbhub import SmartUSBHub
    hub_com = _find_hub_by_addr(board.power.hub_addr)
    if hub_com:
        hub = SmartUSBHub(hub_com)
        hub.set_channel_power(board.power.port, state=state)
        hub.disconnect()


def _step_wait_for(params, ctx: PipelineContext):
    import re, time
    timeout = float(params.get("timeout", 10))
    patterns = params.get("expect") or params.get("patterns") or params.get("data")
    if isinstance(patterns, str):
        patterns = [patterns]
    if not patterns:
        patterns = []

    ser = ctx.serial("debug")
    result = ser.expect_any(patterns, timeout=timeout)
    _log(f"  wait_for matched: {result}")


def _step_serial_send(params, ctx: PipelineContext):
    data = params.get("data") or params.get("text") or ""
    ctx.serial("debug").send(data)


def _step_gpio_set(params, board):
    from vsf_bench.adapters.ft232h import FT232HAdapter
    adapter_name = params.get("adapter", "")
    pin = int(params.get("pin", 0))
    level = params.get("level", "low")
    duration = float(params.get("duration", 0))
    active = level == "low"  # active_low=true by default

    # Resolve adapter from hardware-map
    gpio_cfg = _resolve_gpio_adapter(adapter_name, board)
    adapter = FT232HAdapter(
        serial=gpio_cfg.get("adapter_serial", ""),
        pin=pin, port=gpio_cfg.get("port", "AD"),
        active_low=gpio_cfg.get("active_low", True),
    )
    adapter.open()
    try:
        adapter.set(active)
        if duration > 0:
            time.sleep(duration)
        adapter.set(not active)
    finally:
        adapter.close()


def _resolve_gpio_adapter(name: str, board) -> dict:
    """Resolve GPIO adapter config from board's gpio_adapter_serial or hardware-map."""
    return {
        "adapter_serial": board.gpio_adapter_serial or "FT96OF9L",
        "port": "AD",
        "active_low": True,
    }


def _step_la_start(params, board, run_dir):
    global _la_adapter
    from vsf_bench.adapters.dsview import DSViewAdapter
    la_cfg = board.logic_analyzer
    if la_cfg is None:
        raise RuntimeError("No logic_analyzer config for board")
    from vsf_bench.phases.la import resolve_la_cli
    cli = resolve_la_cli(la_cfg)
    channel = params.get("channel", "CH8")
    duration = float(params.get("duration", 30))
    _la_adapter = DSViewAdapter(cli, la_cfg.device, la_cfg.samplerate, {"capture": channel})
    capture_path = run_dir / f"la-{channel}.dsl"
    _la_adapter.start(capture_path, duration)
    _la_adapter.wait_until_started()
    _log(f"  la_start: {channel} for {duration}s")


def _step_la_stop(board, run_dir):
    global _la_adapter
    if _la_adapter:
        _la_adapter.wait(timeout=60)
        _la_adapter = None


def _step_la_decode(params, board, run_dir):
    from vsf_bench.adapters.dsview import DSViewAdapter
    from vsf_bench.config.models import UARTConfig
    from vsf_bench.utils.core import parse_uart_csv
    la_cfg = board.logic_analyzer
    if la_cfg is None:
        raise RuntimeError("No logic_analyzer config for board")
    from vsf_bench.phases.la import resolve_la_cli
    cli = resolve_la_cli(la_cfg)
    channel = params.get("channel", "CH8")
    baudrate = int(params.get("baudrate", 2000000))
    cfg = UARTConfig(baudrate=baudrate)
    adapter = DSViewAdapter(cli, la_cfg.device, la_cfg.samplerate, {})
    capture_path = run_dir / f"la-{channel}.dsl"
    csv = adapter.decode_uart(capture_path, channel, cfg)
    data = parse_uart_csv(csv)
    text = data.decode("utf-8", errors="replace")
    output = run_dir / f"la-decode-{channel}-{baudrate}.txt"
    output.write_text(text, encoding="utf-8")
    _log(f"  la_decode: {len(data)} bytes -> {output}")


def _step_run(params, run_dir):
    import os
    cmd = params.get("cmd") or params.get("command") or ""
    env = {**os.environ, "RUN_DIR": str(run_dir)}
    _log(f"  run: {cmd}")
    subprocess.run(cmd, shell=True, cwd=str(run_dir), env=env)


def _start_debug_stream_capture(ctx: PipelineContext):
    import threading, time
    ctx._debug_buf = ""
    ser = ctx.serial("debug")

    def _read_loop():
        while ser._ser and ser._ser.is_open:
            try:
                chunk = ser.read_all(timeout=0.5)
                if chunk:
                    ctx._debug_buf += chunk
                    _get_logger().device(chunk.rstrip())
            except Exception:
                break

    threading.Thread(target=_read_loop, daemon=True).start()


def _stop_debug_stream_capture(ctx: PipelineContext):
    ctx.write_debug_buf()


def _start_vendor_log_capture(ctx: PipelineContext, params):
    baudrate = int(params.get("baudrate", 1500000))
    ctx._vendor_buf = b""
    # Open vendor port — using ctx.serial() with raw read
    # Vendor log is binary, so we access the underlying pySerial handle
    sp = ctx.board.serial_ports.get("vendor") or ctx.board.serial_ports.get("program")
    if not sp or not sp.port:
        raise RuntimeError("No vendor/program serial port configured for vendor log")
    import serial
    ser = serial.Serial(sp.port, baudrate, timeout=0.5)
    ctx._serial_sessions["vendor"] = ser  # store raw handle, not SerialInstrument
    _log(f"  vendor_log_start: opened {sp.port} @ {baudrate}")


def _stop_vendor_log_capture(ctx: PipelineContext, params, run_dir):
    import re, time
    ser = ctx._serial_sessions.pop("vendor", None)
    if ser:
        deadline = time.monotonic() + 5.0
        while time.monotonic() < deadline:
            chunk = ser.read(4096)
            if chunk:
                ctx._vendor_buf += chunk
                deadline = time.monotonic() + 1.0
            time.sleep(0.05)
        ser.close()
    if run_dir:
        ctx.run_dir = run_dir  # ensure buffer writes to correct dir
        ctx.write_vendor_buf()
        pattern = params.get("expect")
        if pattern:
            text = ctx._vendor_buf.decode("utf-8", errors="replace") if ctx._vendor_buf else ""
            if not re.search(pattern, text):
                raise RuntimeError(f"vendor_log: pattern '{pattern}' not found")
            _log(f"  vendor_log: matched '{pattern}'")


def _capture_vendor_log(ctx: PipelineContext, params, run_dir):
    """Capture program UART (vendor Bluetooth HCI log) for *duration* seconds."""
    import time
    duration = float(params.get("duration", 10))
    baudrate = int(params.get("baudrate", 1500000))
    min_bytes = int(params.get("min_bytes", 1))

    sp = ctx.board.serial_ports.get("vendor") or ctx.board.serial_ports.get("program")
    if not sp or not sp.port:
        raise RuntimeError("No vendor/program serial port configured")
    import serial
    ser = serial.Serial(sp.port, baudrate, timeout=0.1)
    buf = b""
    deadline = time.monotonic() + duration
    try:
        while time.monotonic() < deadline:
            chunk = ser.read(4096)
            if chunk:
                buf += chunk
            else:
                time.sleep(0.05)
    finally:
        ser.close()
    if buf and run_dir:
        output = run_dir / "vendor_bt.log"
        with open(output, "wb") as f:
            f.write(buf)
        _log(f"  vendor_log: {len(buf)} bytes -> {output}")
    if len(buf) < min_bytes:
        raise RuntimeError(f"vendor_log: expected >= {min_bytes} bytes, got {len(buf)}")
    pattern = params.get("expect")
    if pattern:
        import re
        text = buf.decode("utf-8", errors="replace")
        if not re.search(pattern, text):
            raise RuntimeError(f"vendor_log: pattern '{pattern}' not found in captured data")


