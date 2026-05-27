#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
from pathlib import Path

ALLOWED_BLOCKS = {
    "HeroBlock",
    "FeatureGridBlock",
    "RichTextBlock",
    "ImageTextBlock",
    "DownloadPanelBlock",
    "TimelineBlock",
    "FaqBlock",
    "FooterCtaBlock",
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Validate and stage a page manifest.")
    parser.add_argument("--manifest", required=True, help="Manifest JSON path")
    parser.add_argument("--assets", default="", help="Static asset directory")
    return parser.parse_args()


def validate_manifest(payload: dict) -> None:
    required_keys = {"page_key", "route", "title", "blocks"}
    missing = required_keys - payload.keys()
    if missing:
        raise ValueError(f"Manifest missing required keys: {sorted(missing)}")

    if not str(payload["route"]).startswith("/"):
        raise ValueError("route must start with '/'")

    for block in payload["blocks"]:
        block_type = block.get("type", "")
        if block_type not in ALLOWED_BLOCKS:
            raise ValueError(f"Unsupported block type: {block_type}")


def main() -> int:
    args = parse_args()
    manifest_path = Path(args.manifest)
    if not manifest_path.exists():
        raise FileNotFoundError(f"Manifest not found: {manifest_path}")

    payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    validate_manifest(payload)

    result = {
        "page_key": payload["page_key"],
        "route": payload["route"],
        "title": payload["title"],
        "assets_dir": str(Path(args.assets).resolve()) if args.assets else "",
        "block_count": len(payload["blocks"]),
    }
    print(json.dumps(result, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
