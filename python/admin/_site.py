#!/usr/bin/env python3
"""站点配置写入。"""

from __future__ import annotations

from lib.db import execute


def upsert_site_profile(data: dict) -> None:
    fields = [
        "owner_name",
        "site_name",
        "subtitle",
        "hero_label",
        "hero_title",
        "hero_description",
        "github_url",
        "email",
        "resume_url",
        "linkedin_url",
        "twitter_url",
    ]
    values = {key: str(data.get(key, "")) for key in fields}

    exists = execute("SELECT 1 FROM site_profile WHERE id = 1").fetchone()
    if exists:
        assignments = ", ".join(f"{key} = ?" for key in fields)
        execute(
            f"UPDATE site_profile SET {assignments}, updated_at = CURRENT_TIMESTAMP WHERE id = 1",
            tuple(values.values()),
        )
    else:
        columns = ", ".join(fields)
        placeholders = ", ".join("?" for _ in fields)
        execute(
            f"INSERT INTO site_profile (id, {columns}) VALUES (1, {placeholders})",
            tuple(values.values()),
        )
