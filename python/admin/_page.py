#!/usr/bin/env python3
"""项目页面写入。"""

from __future__ import annotations

from lib.db import execute, get_project_id, query_one

_PAGE_COLUMNS = [
    "title",
    "summary",
    "html_asset_key",
    "sort_order",
    "visibility",
]


def upsert_page(project_slug: str, data: dict) -> int:
    page_slug = str(data.get("slug", "")).strip()
    if not page_slug:
        raise ValueError("页面缺少 slug")

    project_id = get_project_id(project_slug)
    values = {
        "title": str(data.get("title", "")),
        "summary": str(data.get("summary", "")),
        "html_asset_key": str(data.get("html_asset_key", "")),
        "sort_order": int(data.get("sort_order", 100)),
        "visibility": str(data.get("visibility", "public")),
    }

    existing = query_one(
        "SELECT id FROM project_pages WHERE project_id = ? AND slug = ?",
        (project_id, page_slug),
    )

    if existing:
        assignments = ", ".join(f"{key} = ?" for key in _PAGE_COLUMNS)
        params = tuple(values[key] for key in _PAGE_COLUMNS) + (existing["id"],)
        execute(
            f"UPDATE project_pages SET {assignments}, updated_at = CURRENT_TIMESTAMP WHERE id = ?",
            params,
        )
        return existing["id"]

    columns = ", ".join(["project_id", "slug"] + _PAGE_COLUMNS)
    placeholders = ", ".join("?" for _ in ["project_id", "slug"] + _PAGE_COLUMNS)
    params = (project_id, page_slug) + tuple(values[key] for key in _PAGE_COLUMNS)
    cur = execute(
        f"INSERT INTO project_pages ({columns}) VALUES ({placeholders})",
        params,
    )
    return int(cur.lastrowid)
