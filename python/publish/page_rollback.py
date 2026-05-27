#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json


def main() -> int:
    parser = argparse.ArgumentParser(description="Rollback an active page to the previous version.")
    parser.add_argument("--page-key", required=True)
    args = parser.parse_args()
    print(json.dumps({"action": "rollback", "page_key": args.page_key}, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
