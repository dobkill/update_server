#!/usr/bin/env python3

"""首页推荐项目写入。"""

from __future__ import annotations

from lib.db import execute, get_product_id_by_code, query_one


def upsert_recommendation(params: dict) -> dict:
    product_code = params["product_code"]
    product_id = get_product_id_by_code(product_code)
    status = params.get("status", "active")
    sort_order = int(params.get("sort_order", 100))

    existing = query_one(
        "SELECT id FROM recommendations WHERE product_id = ?",
        (product_id,),
    )
    if existing is None:
        cur = execute(
            """
            INSERT INTO recommendations (product_id, status, sort_order)
            VALUES (?, ?, ?)
            """,
            (product_id, status, sort_order),
        )
        recommendation_id = cur.lastrowid
        action = "created"
    else:
        recommendation_id = existing["id"]
        execute(
            """
            UPDATE recommendations
               SET status = ?,
                   sort_order = ?,
                   updated_at = CURRENT_TIMESTAMP
             WHERE id = ?
            """,
            (status, sort_order, recommendation_id),
        )
        action = "updated"

    return {
        "section": "recommendation",
        "action": action,
        "recommendation_id": recommendation_id,
        "product_code": product_code,
        "status": status,
    }
