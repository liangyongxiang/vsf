"""LogicAnalyzerInstrument — DSView CLI wrapper for UART capture and decode."""

import csv
import re
import subprocess
import threading
from dataclasses import dataclass
from pathlib import Path


@dataclass
class MarkerEvent:
    case_idx: int
    time_ns: int


class LogicAnalyzerInstrument:
    """Wraps dsview-cli for logic capture and offline UART protocol decode.

    Lifecycle (managed by board_run.py):
        la.start(duration_s)   → launches dsview-cli capture in background
        la.wait()              → blocks until capture finishes (idempotent)

    Decode (called by test script after la.wait()):
        la.decode_markers(...)  → find CASE:N markers in the marker channel CSV
        la.decode_uart(...)     → decode a time window of a DUT channel
        la.parse_uart_csv(...)  → convert decoded CSV to bytes
    """

    def __init__(
        self,
        cli_path: str | Path,
        device: str,
        samplerate: str,
        channels: dict[str, str],
        capture_path: Path,
    ):
        self._cli = Path(cli_path)
        self._device = device
        self._samplerate = samplerate
        # role → CH label, e.g. {"uart0_tx": "CH1", "uart1_rx": "CH2"}
        self._channels = channels
        self._capture_path = capture_path

        self._proc: subprocess.Popen | None = None
        self._thread: threading.Thread | None = None
        self._done = threading.Event()
        self._exit_code: int | None = None

    @property
    def output_dir(self) -> Path:
        """Directory containing the capture file; suitable for decode outputs."""
        return self._capture_path.parent

    def channel(self, role: str) -> str:
        """Resolve a logical role to a CH label (e.g. 'uart1_rx' → 'CH2')."""
        if role not in self._channels:
            raise KeyError(f"No channel for role '{role}'. Known: {list(self._channels)}")
        return self._channels[role]

    def start(self, duration_s: float) -> None:
        """Start a live capture to self._capture_path.

        If a capture is already running this call is a no-op.
        """
        if self._proc is not None:
            return

        self._capture_path.parent.mkdir(parents=True, exist_ok=True)
        self._done.clear()

        ch_sel = ",".join(
            f"{i}={label}"
            for i, label in enumerate(sorted(set(self._channels.values())))
        )

        cmd = [
            str(self._cli),
            "-d", self._device,
            "-c", f"samplerate={self._samplerate}",
            "--time", f"{int(duration_s)}s",
            "-C", ch_sel,
            "-O", "dsl",
            "-o", str(self._capture_path),
        ]

        print(f"[LA] capture start: {' '.join(cmd)}")
        self._proc = subprocess.Popen(
            cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            text=True, encoding="utf-8", errors="replace",
        )

        def _wait():
            stdout, stderr = self._proc.communicate()
            self._exit_code = self._proc.returncode
            if stdout.strip():
                print(f"[LA] stdout: {stdout.strip()}")
            if stderr.strip():
                print(f"[LA] stderr: {stderr.strip()}")
            self._done.set()

        self._thread = threading.Thread(target=_wait, daemon=True)
        self._thread.start()

    def wait(self, timeout: float = 300.0) -> None:
        """Block until the capture process exits. Idempotent."""
        if self._done.is_set() or self._proc is None:
            return
        if not self._done.wait(timeout=timeout):
            raise TimeoutError(f"LA capture did not finish within {timeout}s")
        if self._exit_code != 0:
            raise RuntimeError(f"dsview-cli capture exited with code {self._exit_code}")
        print(f"[LA] capture done: {self._capture_path}")

    # ------------------------------------------------------------------ decode

    def decode_markers(
        self,
        channel: str,
        baudrate: int,
        pattern: str,
        output_dir: Path | None = None,
    ) -> list[MarkerEvent]:
        """Decode the marker channel offline and extract case marker events.

        Args:
            channel: CH label of the marker USART channel (e.g. 'CH1').
            baudrate: baud rate of that channel (e.g. 115200).
            pattern: regex with one capture group for case index (e.g. r'CASE:(\\d+)').
            output_dir: directory for intermediate CSV; defaults to capture_path dir.

        Returns:
            List of MarkerEvent sorted by time_ns.
        """
        out_dir = output_dir or self._capture_path.parent
        csv_path = out_dir / f"markers_{channel}.csv"
        self._offline_decode(channel, baudrate, None, None, csv_path)

        rows = self._read_csv_rows(csv_path)
        text = ""
        timestamps: list[int] = []
        for time_ns, byte_val in rows:
            text += chr(byte_val)
            timestamps.append(time_ns)

        events: list[MarkerEvent] = []
        for m in re.finditer(pattern, text):
            case_idx = int(m.group(1))
            time_ns = timestamps[m.start()]
            events.append(MarkerEvent(case_idx=case_idx, time_ns=time_ns))

        events.sort(key=lambda e: e.time_ns)
        return events

    def decode_uart(
        self,
        channel: str,
        baudrate: int,
        start_ns: int | None,
        end_ns: int | None,
        output_csv: Path,
    ) -> Path:
        """Decode a time window of a DUT channel offline.

        Args:
            channel: CH label (e.g. 'CH2').
            baudrate: baud rate to decode at.
            start_ns: window start in nanoseconds since capture start (None = from beginning).
            end_ns: window end in nanoseconds (None = to end).
            output_csv: path for the decoded CSV output.

        Returns:
            output_csv path.
        """
        self._offline_decode(channel, baudrate, start_ns, end_ns, output_csv)
        return output_csv

    def parse_uart_csv(self, csv_path: Path) -> bytes:
        """Parse a decoded UART CSV and return the byte sequence.

        CSV format from dsview-cli:
          Header: Id,Time[ns],<decoder_name>
          Data bytes (printable ASCII 0x21-0x7E): stored as the character.
          Other bytes: stored as [HH] (uppercase hex).
        """
        rows = self._read_csv_rows(csv_path)
        return bytes(b for _, b in rows)

    # ---------------------------------------------------------------- internal

    def _ns_to_time_str(self, ns: int) -> str:
        ms = ns // 1_000_000
        return f"{ms}ms"

    def _offline_decode(
        self,
        channel: str,
        baudrate: int,
        start_ns: int | None,
        end_ns: int | None,
        output_csv: Path,
    ) -> None:
        output_csv.parent.mkdir(parents=True, exist_ok=True)
        cmd = [
            str(self._cli),
            "-i", str(self._capture_path),
            "-P", f"uart:rx={channel}:baudrate={baudrate}",
            "--decode-output", str(output_csv),
        ]
        if start_ns is not None:
            cmd += ["--decode-start", self._ns_to_time_str(start_ns)]
        if end_ns is not None:
            cmd += ["--decode-end", self._ns_to_time_str(end_ns)]

        print(f"[LA] decode: {' '.join(cmd)}")
        result = subprocess.run(
            cmd, capture_output=True, text=True,
            encoding="utf-8", errors="replace",
        )
        if result.returncode != 0:
            raise RuntimeError(
                f"dsview-cli decode failed (exit {result.returncode})\n"
                f"stderr: {result.stderr}"
            )

    def _read_csv_rows(self, csv_path: Path) -> list[tuple[int, int]]:
        """Read decoded UART CSV; return list of (time_ns, byte_value)."""
        if not csv_path.exists():
            raise FileNotFoundError(f"Decoded CSV not found: {csv_path}")

        result = []
        with open(csv_path, newline="", encoding="utf-8") as f:
            reader = csv.reader(f)
            next(reader)  # skip header
            for row in reader:
                if len(row) < 3:
                    continue
                time_ns = int(float(row[1]))
                text = row[2].strip()
                if not text:
                    continue
                byte_val = self._parse_csv_cell(text)
                if byte_val is not None:
                    result.append((time_ns, byte_val))
        return result

    @staticmethod
    def _parse_csv_cell(text: str) -> int | None:
        """Parse one CSV cell to a byte value.

        Printable ASCII (stored as single char): return ord(char).
        Non-printable (stored as [HH]): return int(HH, 16).
        Anything else (annotations like 'start'/'stop'): return None.
        """
        if len(text) == 1 and 33 <= ord(text) <= 126:
            return ord(text)
        if len(text) == 4 and text[0] == "[" and text[3] == "]":
            try:
                return int(text[1:3], 16)
            except ValueError:
                return None
        return None
