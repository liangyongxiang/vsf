"""gpio_concurrent_prio scenario host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for
the test framework summary line and asserts all cases passed.

Robustness — no loopback needed (counts callback vs main).
"""

from pathlib import Path
from vsf_bench import load_test_params, read_framework_windows, LogicAnalyzerInstrument, SerialInstrument

SCENARIOS = ["gpio_concurrent_prio"]

def run(project_root: Path, serial: SerialInstrument) -> None:
    serial.expect_test_summary("gpio_concurrent_prio")

def decode(project_root: Path, la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None,
           marker_baud: int = 115200) -> None:
    params = load_test_params(project_root)
    scenario = params.get("gpio_concurrent_prio", {})
    cases = scenario.get("cases", [])
    if not cases:
        return

    la_channel = cases[0].get("la_channel", "gp25")
    try:
        ch = la.channel(la_channel)
    except KeyError:
        print(f"[SKIP] gpio_concurrent_prio decode: no LA channel for '{la_channel}'")
        return

    windows = read_framework_windows(
        la, "gpio_concurrent_prio", project_root,
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
        marker_baud=marker_baud,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    for case in cases:
        idx = int(case["idx"])
        assert idx in window_by_idx, f"CASE {idx}: window missing"
        w = window_by_idx[idx]
        edges = la.read_digital_edges(ch, start_ns=w.start_ns, end_ns=w.end_ns)
        assert len(edges) > 0, f"CASE {idx}: no edges detected on {la_channel}"
        print(f"[PASS] CASE {idx}  gpio_concurrent_prio  edges={len(edges)}")
