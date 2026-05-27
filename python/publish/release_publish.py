#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Validate and register a release package.")
    parser.add_argument("--package", required=True, help="Path to the release package")
    parser.add_argument("--version", required=True)
    parser.add_argument("--platform", required=True)
    parser.add_argument("--arch", default="x64")
    parser.add_argument("--channel", default="stable")
    parser.add_argument("--package-type", choices=["full", "incremental"], required=True)
    parser.add_argument("--base-version", default="")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    package_path = Path(args.package)
    if not package_path.exists():
        raise FileNotFoundError(f"Package not found: {package_path}")

    result = {
        "package": str(package_path.resolve()),
        "version": args.version,
        "platform": args.platform,
        "arch": args.arch,
        "channel": args.channel,
        "package_type": args.package_type,
        "base_version": args.base_version,
        "file_size": package_path.stat().st_size,
    }
    print(json.dumps(result, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
