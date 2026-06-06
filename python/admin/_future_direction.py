#!/usr/bin/env python3

"""未来方向写入。"""

from __future__ import annotations

from lib.db import execute, query_one


def upsert_future_direction(params: dict) -> dict:
    title = params["title"]
    comment = params["comment"]
    icon_path = params.get("icon_path", "")
    status = params.get("status", "active")
    sort_order = int(params.get("sort_order", 100))

    existing = query_one("SELECT id FROM future_directions WHERE title = ?", (title,))
    if existing is None:
        cur = execute(
            """
            INSERT INTO future_directions (title, comment, icon_path, status, sort_order)
            VALUES (?, ?, ?, ?, ?)
            """,
            (title, comment, icon_path, status, sort_order),
        )
        direction_id = cur.lastrowid
        action = "created"
    else:
        direction_id = existing["id"]
        execute(
            """
            UPDATE future_directions
               SET comment = ?,
                   icon_path = ?,
                   status = ?,
                   sort_order = ?,
                   updated_at = CURRENT_TIMESTAMP
             WHERE id = ?
            """,
            (comment, icon_path, status, sort_order, direction_id),
        )
        action = "updated"

    return {
        "section": "future_direction",
        "action": action,
        "direction_id": direction_id,
        "title": title,
        "status": status,
    }
