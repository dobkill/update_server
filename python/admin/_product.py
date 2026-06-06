#!/usr/bin/env python3

"""产品操作核心业务逻辑。"""

from __future__ import annotations

from lib.db import execute, query_one


def add_product(params: dict) -> dict:
    """添加产品。

    params:
        name: str           必填
        code: str           必填
        description: str    可选
        icon_path: str      可选
        github_url: str     可选
        status: str         可选，默认 active
    """
    name = params["name"]
    code = params["code"]
    description = params.get("description")
    icon_path = params.get("icon_path")
    github_url = params.get("github_url", "")
    status = params.get("status", "active")

    # 校验 code 不重复
    existing = query_one(
        "SELECT id, name, description, icon_path, github_url, status FROM products WHERE code = ?",
        (code,),
    )
    if existing is not None:
        updates: list[str] = []
        values: list[object] = []
        next_values = {
            "name": name,
            "description": description,
            "icon_path": icon_path,
            "github_url": github_url,
            "status": status,
        }
        for key, value in next_values.items():
            if existing.get(key) != value:
                updates.append(f"{key} = ?")
                values.append(value)

        if updates:
            values.append(code)
            execute(
                f"UPDATE products SET {', '.join(updates)}, updated_at = CURRENT_TIMESTAMP WHERE code = ?",
                tuple(values),
            )
            return {
                "section": "product",
                "action": "updated",
                "product_id": existing["id"],
                "name": name,
                "code": code,
                "status": status,
            }

        return {
            "section": "product",
            "action": "skipped",
            "reason": f"产品编码已存在 (id={existing['id']}, code={code})",
            "product_id": existing["id"],
            "name": name,
            "code": code,
        }

    cur = execute(
        """
        INSERT INTO products (name, code, github_url, description, icon_path, status)
        VALUES (?, ?, ?, ?, ?, ?)
        """,
        (name, code, github_url, description, icon_path, status),
    )

    product_id = cur.lastrowid
    return {
        "section": "product",
        "action": "created",
        "product_id": product_id,
        "name": name,
        "code": code,
        "status": status,
    }
