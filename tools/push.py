#!/usr/bin/env python3
"""eezml hot-reload push tool: send a .uixml document to a device running
the eezml_uart transport.

Usage:
    python tools/push.py --port COM5 document.uixml [--baud 115200] [--persist]
    python tools/push.py --port COM5 --wipe

--persist stores the document in flash after a successful apply: it then
survives a reboot (the device boots it instead of the compiled-in UI).
--wipe forgets the persisted document (back to the compiled-in UI).

Note: opening/closing the serial port may reset the board (USB-Serial/JTAG
DTR/RTS lines) — that is harmless with --persist, and without it the pushed
UI is exactly what does not survive the reset.

Requires pyserial (pip install pyserial). Close `idf.py monitor` first —
the tool needs exclusive access to the console UART.
"""

from __future__ import annotations

import argparse
import sys
import time
import zlib


def wipe(port: str, baud: int) -> int:
    try:
        import serial  # pyserial
    except ImportError:
        print("pyserial missing: pip install pyserial", file=sys.stderr)
        return 2

    with serial.Serial(port, baud, timeout=3) as ser:
        time.sleep(0.15)
        ser.reset_input_buffer()
        ser.write(b"EEZML WIPE\n")
        ser.flush()

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
        return 0 if "EEZML OK" in out else 1


def push(port: str, path: str, baud: int, persist: bool = False) -> int:
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
        begin = f"EEZML BEGIN doc {len(data)}{' persist' if persist else ''}\n"
        ser.write(begin.encode())
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
    ap.add_argument("document", nargs="?", help=".uixml file to push")
    ap.add_argument("--port", required=True, help="serial port, e.g. COM5")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--persist", action="store_true",
                    help="store in flash after apply — survives reboot")
    ap.add_argument("--wipe", action="store_true",
                    help="forget the persisted document")
    args = ap.parse_args(argv)
    if args.wipe:
        return wipe(args.port, args.baud)
    if not args.document:
        ap.error("document is required unless --wipe")
    return push(args.port, args.document, args.baud, args.persist)


if __name__ == "__main__":
    sys.exit(main())
