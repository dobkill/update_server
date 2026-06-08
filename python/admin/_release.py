#!/usr/bin/env python3

"""版本操作核心业务逻辑。"""

from __future__ import annotations

from datetime import datetime, timezone

from lib.db import execute, get_product_id_by_code, query_one


def add_release(params: dict) -> dict:
    """添加版本。

    params:
        product_code: str       必填
        version: str            必填
        release_note: str       可选
        data_schema_version: str 可选
        status: str             可选，默认 draft
        html_path: str           可选
        published_at: str       可选
    """
    product_code = params["product_code"]
    version = params["version"]
    release_note = params.get("release_note")
    data_schema_version = params.get("data_schema_version")
    status = params.get("status", "draft")
    html_path = params.get("html_path")

    # 查找产品 ID
    try:
        product_id = get_product_id_by_code(product_code)
    except ValueError as e:
        return {"section": "release", "action": "error", "reason": str(e)}

    existing = query_one(
        """
        SELECT id, release_note, data_schema_version, status, html_path, published_at
          FROM releases
         WHERE product_id = ? AND version = ?
        """,
        (product_id, version),
    )
    if existing is not None:
        published_at = params.get("published_at")
        if published_at is None:
            if status == "active" and not existing.get("published_at"):
                published_at = datetime.now(timezone.utc).isoformat()
            else:
                published_at = existing.get("published_at")

        next_values = {
            "release_note": release_note,
            "data_schema_version": data_schema_version,
            "status": status,
            "html_path": html_path,
            "published_at": published_at,
        }
        updates: list[str] = []
        values: list[object] = []
        for key, value in next_values.items():
            if existing.get(key) != value:
                updates.append(f"{key} = ?")
                values.append(value)

        if updates:
            values.append(existing["id"])
            execute(
                f"""
                UPDATE releases
                   SET {", ".join(updates)},
                       updated_at = CURRENT_TIMESTAMP
                 WHERE id = ?
                """,
                tuple(values),
            )
            return {
                "section": "release",
                "action": "updated",
                "release_id": existing["id"],
                "product_id": product_id,
                "product_code": product_code,
                "version": version,
                "status": status,
                "data_schema_version": data_schema_version,
                "published_at": published_at,
            }

        return {
            "section": "release",
            "action": "skipped",
            "reason": f"版本已是最新配置 (id={existing['id']}, version={version})",
            "release_id": existing["id"],
            "product_id": product_id,
            "product_code": product_code,
            "version": version,
        }

    # 处理 published_at
    published_at = params.get("published_at")
    if published_at is None and status == "active":
        published_at = datetime.now(timezone.utc).isoformat()

    cur = execute(
        """
        INSERT INTO releases (product_id, version, release_note, data_schema_version, status, html_path, published_at)
        VALUES (?, ?, ?, ?, ?, ?, ?)
        """,
        (product_id, version, release_note, data_schema_version, status, html_path, published_at),
    )

    release_id = cur.lastrowid
    return {
        "section": "release",
        "action": "created",
        "release_id": release_id,
        "product_id": product_id,
        "product_code": product_code,
        "version": version,
        "status": status,
        "data_schema_version": data_schema_version,
        "published_at": published_at,
    }
