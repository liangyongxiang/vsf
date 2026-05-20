"""Multi-baud UART TX validation via logic analyzer capture.

Test parameters are read from application/component/vsf-test/test_params.yml
so the script does not depend on firmware output for correctness.

Two-phase: `run()` triggers + waits for firmware completion; `decode()` runs
after the shared LA capture is stopped and validates per-case payloads.
"""

from dataclasses import dataclass
from pathlib import Path

from vsf_bench.capture_marker import read_framework_windows
from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.instruments.serial_instrument import SerialInstrument
from vsf_bench.test_params import load_test_params

RP2040_CLK_PERI = 125_000_000
MIN_BAUDRATE = RP2040_CLK_PERI // (16 * 65535)
MAX_BAUDRATE = RP2040_CLK_PERI // 16


def _expect_pass(baud: int) -> bool:
    return baud != 0 and MIN_BAUDRATE <= baud <= MAX_BAUDRATE


@dataclass(frozen=True)
class Case:
    idx: int
    baud: int


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        cases.append(Case(
            idx=int(case["idx"]),
            baud=int(case["baudrate"]),
        ))
    return cases


def run(project_root: Path, serial: SerialInstrument) -> None:
    params = load_test_params(project_root)
    scenario = params.get("tx_baud", {})
    timeout_s = float(scenario.get("timeout_s", 30.0))
    serial.expect_test_summary("usart_baud", timeout=timeout_s)


def decode(project_root: Path, la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None) -> None:
    params = load_test_params(project_root)
    scenario = params.get("tx_baud", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    dut_ch = la.channel(scenario.get("dut", {}).get("channel", "uart1_tx"))
    payload = scenario.get("payload", "Hello VSF\r\n").encode()
    out_dir = la.output_dir

    windows = read_framework_windows(
        la, "usart_baud", project_root,
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    for c in cases:
        if _expect_pass(c.baud):
            assert c.idx in window_by_idx, f"CASE {c.idx} baud={c.baud}: window missing"

    pass_baudrates = sorted({c.baud for c in cases if _expect_pass(c.baud)})
    la.batch_decode_uart([
        (dut_ch, baud, decode_start_ns, decode_end_ns,
         out_dir / f"baud_full_{baud}.csv", "none", 8, 1.0)
        for baud in pass_baudrates
    ])

    for c in cases:
        if not _expect_pass(c.baud):
            print(f"[PASS] CASE {c.idx}  baud={c.baud:>7}  expected fail (init error)")
            continue
        w = window_by_idx[c.idx]
        rows = la.read_csv_rows(out_dir / f"baud_full_{c.baud}.csv")
        got = bytes(b for t, b in rows if w.start_ns <= t < w.end_ns)
        assert got == payload, (
            f"CASE {c.idx} baud={c.baud}: expected {payload!r}, got {got!r}"
        )
        print(f"[PASS] CASE {c.idx}  baud={c.baud:>7}  {got!r}")
