"""Case Window slicer for scenario decoders.

Owns the Capture Marker protocol: which marker types fire, which is start
vs end of each case, how the per-case `[start_ns, end_ns)` window is built.

Two semantics, see CONTEXT.md "Case Window":
  * `read_framework_windows()` for TX scenarios — bounded by the dispatcher's
    `<suite>:CASE:N` markers and the suite-level `<suite>:END` marker.
  * `read_handshake_windows()` for RX scenarios — bounded by the scenario's
    `<suite>:CASE:N:READY` and the dispatcher's `<suite>:CASE:N:DONE`.

Both share the same `test_params.yml` marker config (channel, baudrate,
delay_ms). Scenario decoders pass `project_root` so this module loads it
once internally; scenario code carries no Capture Marker knowledge.
"""

from dataclasses import dataclass
from pathlib import Path

from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.test_params import load_test_params


@dataclass(frozen=True)
class CaseWindow:
    case_idx: int
    start_ns: int
    end_ns: int


def _marker_config(project_root: Path) -> tuple[str, int, int]:
    """Returns (channel_role, baudrate, delay_ns) from test_params.yml."""
    params = load_test_params(project_root)
    marker = params.get("marker", {}) or {}
    channel = marker.get("channel", "uart0_tx")
    baud = int(marker.get("baudrate", 115200))
    delay_ns = int(marker.get("delay_ms", 0)) * 1_000_000
    return channel, baud, delay_ns


def read_framework_windows(
    la: LogicAnalyzerInstrument,
    suite_name: str,
    project_root: Path,
    decode_start_ns: int | None = None,
    decode_end_ns: int | None = None,
) -> list[CaseWindow]:
    """Per-case windows for TX scenarios.

    start_ns = dispatcher's `<suite>:CASE:N` + marker_delay
    end_ns   = next case's `<suite>:CASE:N+1`, or `<suite>:END` for the last
    """
    channel_role, marker_baud, delay_ns = _marker_config(project_root)
    ch = la.channel(channel_role)
    out_dir = la.output_dir

    starts = la.decode_markers(
        channel=ch, baudrate=marker_baud,
        pattern=rf"{suite_name}:CASE:(\d+)(?!:)",
        output_csv=out_dir / f"{suite_name}_starts.csv",
        start_ns=decode_start_ns, end_ns=decode_end_ns,
    )
    ends = la.decode_markers(
        channel=ch, baudrate=marker_baud,
        pattern=rf"{suite_name}:END",
        output_csv=out_dir / f"{suite_name}_end.csv",
        start_ns=decode_start_ns, end_ns=decode_end_ns,
    )

    if not starts:
        raise RuntimeError(f"{suite_name}: no CASE start markers found in LA decode")
    starts_sorted = sorted(starts, key=lambda e: e.time_ns)
    end_ns = ends[-1].time_ns if ends else None

    windows: list[CaseWindow] = []
    for i, ev in enumerate(starts_sorted):
        s = ev.time_ns + delay_ns
        if i + 1 < len(starts_sorted):
            e = starts_sorted[i + 1].time_ns
        elif end_ns is not None:
            e = end_ns
        else:
            raise RuntimeError(
                f"{suite_name}: last case has no upper bound "
                f"(missing `{suite_name}:END` marker in LA decode)"
            )
        windows.append(CaseWindow(case_idx=ev.case_idx, start_ns=s, end_ns=e))
    return windows


def read_handshake_windows(
    la: LogicAnalyzerInstrument,
    suite_name: str,
    project_root: Path,
    decode_start_ns: int | None = None,
    decode_end_ns: int | None = None,
) -> list[CaseWindow]:
    """Per-case windows for RX scenarios.

    start_ns = scenario-emitted `<suite>:CASE:N:READY`
    end_ns   = dispatcher's `<suite>:CASE:N:DONE`

    Only cases with BOTH a READY and DONE marker on the wire are returned;
    cases that fail USART init (no READY emitted) are absent — scenario
    code can check `cw.case_idx` to know which ones the host saw.
    """
    channel_role, marker_baud, _ = _marker_config(project_root)
    ch = la.channel(channel_role)
    out_dir = la.output_dir

    readys = la.decode_markers(
        channel=ch, baudrate=marker_baud,
        pattern=rf"{suite_name}:CASE:(\d+):READY",
        output_csv=out_dir / f"{suite_name}_ready.csv",
        start_ns=decode_start_ns, end_ns=decode_end_ns,
    )
    dones = la.decode_markers(
        channel=ch, baudrate=marker_baud,
        pattern=rf"{suite_name}:CASE:(\d+):DONE",
        output_csv=out_dir / f"{suite_name}_done.csv",
        start_ns=decode_start_ns, end_ns=decode_end_ns,
    )

    ready_by_case = {ev.case_idx: ev.time_ns for ev in readys}
    done_by_case = {ev.case_idx: ev.time_ns for ev in dones}

    windows: list[CaseWindow] = []
    for case_idx in sorted(ready_by_case):
        if case_idx not in done_by_case:
            raise RuntimeError(
                f"{suite_name}: case {case_idx} has READY but no DONE marker"
            )
        windows.append(CaseWindow(
            case_idx=case_idx,
            start_ns=ready_by_case[case_idx],
            end_ns=done_by_case[case_idx],
        ))
    return windows
