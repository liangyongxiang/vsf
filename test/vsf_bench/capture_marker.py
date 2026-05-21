"""Case Window slicer for scenario decoders.

Owns the Capture Marker protocol: which marker types fire, which is start
vs end of each case, how the per-case `[start_ns, end_ns)` window is built.

`read_framework_windows()` handles both TX and RX scenarios:
  * TX: windows bounded by `CASE:N` → next `CASE:N+1` (or `END` for last)
  * RX: when READY markers are present, windows bounded by `CASE:N:READY`
    → `CASE:N:DONE`; otherwise falls back to CASE:N → next CASE/END.

Scenario decoders pass `project_root` so this module loads marker config
(channel, baudrate) once internally; scenario code carries no Capture Marker
knowledge.
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


def _marker_config(project_root: Path) -> tuple[str, int]:
    """Returns (channel_role, baudrate) from test_params.yml."""
    params = load_test_params(project_root)
    marker = params.get("marker", {}) or {}
    channel = marker.get("channel", "uart0_tx")
    baud = int(marker.get("baudrate", 115200))
    return channel, baud


def read_framework_windows(
    la: LogicAnalyzerInstrument,
    suite_name: str,
    project_root: Path,
    decode_start_ns: int | None = None,
    decode_end_ns: int | None = None,
) -> list[CaseWindow]:
    """Per-case windows for both TX and RX scenarios.

    If READY markers are found for this suite, windows are bounded by
    READY → DONE. Otherwise, windows are bounded by CASE:N → CASE:N+1
    (or END for the last case).
    """
    channel_role, marker_baud = _marker_config(project_root)
    ch = la.channel(channel_role)
    out_dir = la.output_dir

    starts = la.decode_markers(
        channel=ch, baudrate=marker_baud,
        pattern=rf"{suite_name}:CASE:(\d+)(?![\d:])",
        output_csv=out_dir / f"{suite_name}_starts.csv",
        start_ns=decode_start_ns, end_ns=decode_end_ns,
    )
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

    # If any READY markers were found for this suite, use READY→DONE windows.
    use_ready = len(readys) > 0
    ready_by_case = {ev.case_idx: ev.time_ns for ev in readys}
    done_by_case = {ev.case_idx: ev.time_ns for ev in dones}

    windows: list[CaseWindow] = []
    for i, ev in enumerate(starts_sorted):
        case_idx = ev.case_idx
        if use_ready:
            if case_idx not in ready_by_case:
                raise RuntimeError(
                    f"{suite_name}: case {case_idx} missing READY marker"
                )
            if case_idx not in done_by_case:
                raise RuntimeError(
                    f"{suite_name}: case {case_idx} has READY but no DONE marker"
                )
            s = ready_by_case[case_idx]
            e = done_by_case[case_idx]
        else:
            s = ev.time_ns
            if i + 1 < len(starts_sorted):
                e = starts_sorted[i + 1].time_ns
            elif end_ns is not None:
                e = end_ns
            else:
                raise RuntimeError(
                    f"{suite_name}: last case has no upper bound "
                    f"(missing `{suite_name}:END` marker in LA decode)"
                )
        windows.append(CaseWindow(case_idx=case_idx, start_ns=s, end_ns=e))
    return windows
