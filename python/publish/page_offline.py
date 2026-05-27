#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json


def main() -> int:
    parser = argparse.ArgumentParser(description="Offline an active page.")
    parser.add_argument("--page-key", required=True)
    args = parser.parse_args()
    print(json.dumps({"action": "offline", "page_key": args.page_key}, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
