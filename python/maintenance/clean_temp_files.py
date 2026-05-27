#!/usr/bin/env python3

from __future__ import annotations

import argparse
import time
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Remove expired temporary files.")
    parser.add_argument("--directory", required=True, help="Directory to clean")
    parser.add_argument("--older-than-hours", type=int, default=24)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    directory = Path(args.directory)
    if not directory.exists():
        return 0

    cutoff = time.time() - args.older_than_hours * 3600
    removed = 0
    for path in directory.rglob("*"):
        if not path.is_file():
            continue
        if path.stat().st_mtime < cutoff:
            path.unlink()
            removed += 1

    print(f"Removed {removed} expired files from {directory}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
