#!/usr/bin/env python3

"""作品集项目写入。"""

from __future__ import annotations

import json

from lib.db import execute, query_one


def _json_text(value: object, fallback: object) -> str:
    if value is None:
        value = fallback
    return json.dumps(value, ensure_ascii=False)


def upsert_portfolio_project(params: dict) -> dict:
    slug = params["slug"]
    product_code = params.get("product_code", "")
    product_id = None
    if product_code:
        product = query_one(
            "SELECT id FROM products WHERE code = ? COLLATE NOCASE",
            (product_code,),
        )
        if product is None:
            raise ValueError(f"product_code 不存在: {product_code}")
        product_id = product["id"]

    values = {
        "product_id": product_id,
        "slug": slug,
        "name": params["name"],
        "category": params["category"],
        "description": params.get("description", ""),
        "long_description": params.get("long_description", ""),
        "featured": int(bool(params.get("featured", False))),
        "status": params.get("status", "Live"),
        "year": str(params.get("year", "")),
        "platform": params.get("platform", ""),
        "role": params.get("role", ""),
        "project_type": params.get("project_type", params.get("type", "")),
        "cover_image_url": params.get("cover_image_url", ""),
        "hero_image_url": params.get("hero_image_url", ""),
        "tech_stack_json": _json_text(params.get("tech_stack"), []),
        "features_json": _json_text(params.get("features"), []),
        "screenshots_json": _json_text(params.get("screenshots"), []),
        "architecture_json": _json_text(params.get("architecture"), []),
        "challenge": params.get("challenge", ""),
        "solution": params.get("solution", ""),
        "result": params.get("result", ""),
        "links_json": _json_text(params.get("links"), {}),
        "visibility": params.get("visibility", "public"),
        "sort_order": int(params.get("sort_order", 100)),
    }

    existing = query_one("SELECT id FROM portfolio_projects WHERE slug = ?", (slug,))
    if existing is None:
        columns = list(values.keys())
        placeholders = ", ".join("?" for _ in columns)
        execute(
            f"""
            INSERT INTO portfolio_projects ({", ".join(columns)})
            VALUES ({placeholders})
            """,
            tuple(values[column] for column in columns),
        )
        action = "created"
    else:
        update_columns = [column for column in values if column != "slug"]
        assignments = ", ".join(f"{column} = ?" for column in update_columns)
        execute(
            f"""
            UPDATE portfolio_projects
               SET {assignments},
                   updated_at = CURRENT_TIMESTAMP
             WHERE slug = ?
            """,
            tuple(values[column] for column in update_columns) + (slug,),
        )
        action = "updated"

    return {
        "section": "portfolio_project",
        "action": action,
        "slug": slug,
        "name": values["name"],
    }
