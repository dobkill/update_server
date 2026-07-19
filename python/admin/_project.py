#!/usr/bin/env python3
"""项目写入。"""

from __future__ import annotations

import json
from typing import Any

from lib.db import execute, json_dumps, query_one

_PROJECT_COLUMNS = [
    "name",
    "category",
    "summary",
    "description",
    "status",
    "year",
    "role",
    "platform",
    "project_type",
    "challenge",
    "solution",
    "result",
    "cover_asset_key",
    "hero_asset_key",
    "tech_stack_json",
    "features_json",
    "screenshots_json",
    "architecture_json",
    "links_json",
    "featured",
    "visibility",
    "sort_order",
]


def _project_values(data: dict[str, Any]) -> dict[str, Any]:
    def json_field(key: str, default: Any) -> str:
        return json_dumps(data.get(key, default))

    return {
        "name": str(data.get("name", "")),
        "category": str(data.get("category", "")),
        "summary": str(data.get("summary", data.get("description", ""))),
        "description": str(data.get("long_description", data.get("description", ""))),
        "status": str(data.get("status", "active")),
        "year": str(data.get("year", "")),
        "role": str(data.get("role", "")),
        "platform": str(data.get("platform", "")),
        "project_type": str(data.get("project_type", data.get("type", ""))),
        "challenge": str(data.get("challenge", "")),
        "solution": str(data.get("solution", "")),
        "result": str(data.get("result", "")),
        "cover_asset_key": str(data.get("cover_asset_key", data.get("cover_image_url", ""))),
        "hero_asset_key": str(data.get("hero_asset_key", data.get("hero_image_url", ""))),
        "tech_stack_json": json_field("tech_stack", []),
        "features_json": json_field("features", []),
        "screenshots_json": json_field("screenshots", []),
        "architecture_json": json_field("architecture", []),
        "links_json": json_field("links", {}),
        "featured": 1 if data.get("featured") else 0,
        "visibility": str(data.get("visibility", "public")),
        "sort_order": int(data.get("sort_order", 100)),
    }


def upsert_project(data: dict[str, Any]) -> int:
    slug = str(data.get("slug", "")).strip()
    if not slug:
        raise ValueError("项目缺少 slug")

    values = _project_values(data)
    existing = query_one("SELECT id FROM projects WHERE slug = ?", (slug,))

    if existing:
        assignments = ", ".join(f"{key} = ?" for key in _PROJECT_COLUMNS)
        params = tuple(values[key] for key in _PROJECT_COLUMNS) + (slug,)
        execute(
            f"UPDATE projects SET {assignments}, updated_at = CURRENT_TIMESTAMP WHERE slug = ?",
            params,
        )
        return existing["id"]

    columns = ", ".join(["slug"] + _PROJECT_COLUMNS)
    placeholders = ", ".join("?" for _ in ["slug"] + _PROJECT_COLUMNS)
    params = (slug,) + tuple(values[key] for key in _PROJECT_COLUMNS)
    cur = execute(
        f"INSERT INTO projects ({columns}) VALUES ({placeholders})",
        params,
    )
    return int(cur.lastrowid)
