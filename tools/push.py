#!/usr/bin/env python3
"""eezml hot-reload push tool: send a .uixml document to a device running
the eezml_uart transport.

Usage:
    python tools/push.py --port COM5 document.uixml [--baud 115200]

Requires pyserial (pip install pyserial). Close `idf.py monitor` first —
the tool needs exclusive access to the console UART.
"""

from __future__ import annotations

import argparse
import sys
import time
import zlib


def push(port: str, path: str, baud: int) -> int:
    try:
        import serial  # pyserial
    except ImportError:
        print("pyserial missing: pip install pyserial", file=sys.stderr)
        return 2

    data = open(path, "rb").read()
    crc = zlib.crc32(data) & 0xFFFFFFFF

    with serial.Serial(port, baud, timeout=3) as ser:
        time.sleep(0.15)                       # let the port settle
        ser.reset_input_buffer()
        ser.write(f"EEZML BEGIN doc {len(data)}\n".encode())
        ser.write(data)
        ser.write(f"EEZML END {crc:08x}\n".encode())
        ser.flush()

        # read ack lines (OK / ERR ...)
        deadline = time.time() + 8
        got = b""
        while time.time() < deadline:
            chunk = ser.read(64)
            if chunk:
                got += chunk
                if b"EEZML OK" in got or b"EEZML ERR" in got:
                    break
        out = got.decode(errors="replace").strip()
        print(out or "(no ack — is the device running eezml_uart?)")
        if "EEZML OK" in out:
            print(f"applied: {path} ({len(data)} bytes)")
            return 0
        return 1


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description="push eezml document over UART")
    ap.add_argument("document", help=".uixml file to push")
    ap.add_argument("--port", required=True, help="serial port, e.g. COM5")
    ap.add_argument("--baud", type=int, default=115200)
    args = ap.parse_args(argv)
    return push(args.port, args.document, args.baud)


if __name__ == "__main__":
    sys.exit(main())
