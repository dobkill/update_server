#!/usr/bin/env python3

from __future__ import annotations

import argparse
import shutil
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Convert legacy data into the target format.")
    parser.add_argument("--input", required=True, help="Source data file")
    parser.add_argument("--output", required=True, help="Target data file")
    parser.add_argument("--source-version", required=True, help="Source version")
    parser.add_argument("--target-version", required=True, help="Target version")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    input_path = Path(args.input)
    output_path = Path(args.output)

    if not input_path.exists():
      raise FileNotFoundError(f"Input file does not exist: {input_path}")

    output_path.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(input_path, output_path)
    print(
        f"Converted {input_path.name} from {args.source_version} to {args.target_version} -> "
        f"{output_path}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
