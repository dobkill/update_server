#!/usr/bin/env python3

"""站点资料写入。"""

from __future__ import annotations

from lib.db import execute, query_one


def set_site_profile(params: dict) -> dict:
    site_name = params.get("site_name", "YXX Works")
    subtitle = params.get("subtitle", "产品、插件与创作实验")
    github_url = params.get("github_url", "")
    email = params.get("email", "")

    existing = query_one("SELECT id FROM site_profile WHERE id = 1")
    if existing is None:
        execute(
            """
            INSERT INTO site_profile (id, site_name, subtitle, github_url, email)
            VALUES (1, ?, ?, ?, ?)
            """,
            (site_name, subtitle, github_url, email),
        )
        action = "created"
    else:
        execute(
            """
            UPDATE site_profile
               SET site_name = ?,
                   subtitle = ?,
                   github_url = ?,
                   email = ?,
                   updated_at = CURRENT_TIMESTAMP
             WHERE id = 1
            """,
            (site_name, subtitle, github_url, email),
        )
        action = "updated"

    return {
        "section": "site_profile",
        "action": action,
        "site_name": site_name,
    }
