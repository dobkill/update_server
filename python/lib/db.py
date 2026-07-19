#!/usr/bin/env python3
"""Showcase SQLite 连接与读写封装。"""

from __future__ import annotations

import json
import sqlite3
from pathlib import Path
from typing import Any

from lib.config import get_database_path, project_root

_conn: sqlite3.Connection | None = None


def get_db_path() -> Path:
    return get_database_path()


def run_migrations_if_needed(conn: sqlite3.Connection) -> None:
    cur = conn.execute(
        "SELECT name FROM sqlite_master WHERE type = 'table' AND name = 'projects'"
    )
    if cur.fetchone() is not None:
        return

    sql_file = project_root() / "migrations" / "001_init_showcase.sql"
    if not sql_file.exists():
        raise FileNotFoundError(f"迁移文件不存在: {sql_file}")

    conn.executescript(sql_file.read_text(encoding="utf-8"))
    conn.commit()


def get_connection() -> sqlite3.Connection:
    global _conn
    if _conn is not None:
        return _conn

    db_path = get_db_path()
    db_path.parent.mkdir(parents=True, exist_ok=True)

    _conn = sqlite3.connect(str(db_path))
    _conn.row_factory = sqlite3.Row
    _conn.execute("PRAGMA journal_mode = WAL")
    _conn.execute("PRAGMA foreign_keys = ON")
    run_migrations_if_needed(_conn)
    return _conn


def execute(sql: str, params: tuple = ()) -> sqlite3.Cursor:
    conn = get_connection()
    cur = conn.execute(sql, params)
    conn.commit()
    return cur


def query_one(sql: str, params: tuple = ()) -> dict | None:
    conn = get_connection()
    row = conn.execute(sql, params).fetchone()
    return dict(row) if row is not None else None


def query_all(sql: str, params: tuple = ()) -> list[dict]:
    conn = get_connection()
    return [dict(row) for row in conn.execute(sql, params).fetchall()]


def get_project_id(slug: str) -> int:
    row = query_one("SELECT id FROM projects WHERE slug = ?", (slug,))
    if row is None:
        raise ValueError(f"项目不存在: slug={slug}")
    return row["id"]


def json_dumps(value: Any) -> str:
    return json.dumps(value, ensure_ascii=False)
