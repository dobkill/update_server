#!/usr/bin/env python3

"""站点资料写入。"""

from __future__ import annotations

from lib.db import execute, query_one


def set_site_profile(params: dict) -> dict:
    owner_name = params.get("owner_name", "LIANG Y.")
    site_name = params.get("site_name", "Personal Software Lab")
    subtitle = params.get("subtitle", "Self-built apps, tools, systems, and experiments.")
    hero_label = params.get("hero_label", "DEVELOPER & BUILDER")
    hero_title = params.get("hero_title", "Personal Software Lab")
    hero_description = params.get(
        "hero_description",
        "A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity.",
    )
    github_url = params.get("github_url", "")
    email = params.get("email", "")
    resume_url = params.get("resume_url", "")
    linkedin_url = params.get("linkedin_url", "")
    twitter_url = params.get("twitter_url", "")

    existing = query_one("SELECT id FROM site_profile WHERE id = 1")
    if existing is None:
        execute(
            """
            INSERT INTO site_profile (
                id, owner_name, site_name, subtitle, hero_label, hero_title, hero_description,
                github_url, email, resume_url, linkedin_url, twitter_url
            )
            VALUES (1, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            """,
            (
                owner_name,
                site_name,
                subtitle,
                hero_label,
                hero_title,
                hero_description,
                github_url,
                email,
                resume_url,
                linkedin_url,
                twitter_url,
            ),
        )
        action = "created"
    else:
        execute(
            """
            UPDATE site_profile
               SET owner_name = ?,
                   site_name = ?,
                   subtitle = ?,
                   hero_label = ?,
                   hero_title = ?,
                   hero_description = ?,
                   github_url = ?,
                   email = ?,
                   resume_url = ?,
                   linkedin_url = ?,
                   twitter_url = ?,
                   updated_at = CURRENT_TIMESTAMP
             WHERE id = 1
            """,
            (
                owner_name,
                site_name,
                subtitle,
                hero_label,
                hero_title,
                hero_description,
                github_url,
                email,
                resume_url,
                linkedin_url,
                twitter_url,
            ),
        )
        action = "updated"

    return {
        "section": "site_profile",
        "action": action,
        "owner_name": owner_name,
        "site_name": site_name,
    }
