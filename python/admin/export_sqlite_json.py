#!/usr/bin/env python3

"""导出 SQLite 数据库到 JSON 文件。"""

from __future__ import annotations

import argparse
import json
import sqlite3
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from lib.db import get_db_path


EXCLUDED_TABLES = {"sqlite_sequence"}


def now_iso() -> str:
    return datetime.now(timezone.utc).astimezone().isoformat()


def resolve_db_path(raw: str | None) -> Path:
    if raw:
        return Path(raw).expanduser().resolve()
    return get_db_path().resolve()


def list_tables(conn: sqlite3.Connection) -> list[str]:
    rows = conn.execute(
        """
        SELECT name
          FROM sqlite_master
         WHERE type = 'table'
           AND name NOT LIKE 'sqlite_%'
         ORDER BY name
        """
    ).fetchall()
    return [row[0] for row in rows if row[0] not in EXCLUDED_TABLES]


def table_columns(conn: sqlite3.Connection, table: str) -> list[str]:
    return [row[1] for row in conn.execute(f'PRAGMA table_info("{table}")').fetchall()]


def schema_version(conn: sqlite3.Connection) -> str:
    exists = conn.execute(
        "SELECT name FROM sqlite_master WHERE type = 'table' AND name = 'schema_migrations'"
    ).fetchone()
    if exists is None:
        return ""
    row = conn.execute(
        "SELECT version FROM schema_migrations ORDER BY applied_at DESC, version DESC LIMIT 1"
    ).fetchone()
    return row[0] if row else ""


def export_database(db_path: Path) -> dict[str, Any]:
    if not db_path.exists():
        raise FileNotFoundError(f"数据库文件不存在: {db_path}")

    conn = sqlite3.connect(str(db_path))
    conn.row_factory = sqlite3.Row
    try:
        tables: dict[str, Any] = {}
        for table in list_tables(conn):
            columns = table_columns(conn, table)
            rows = conn.execute(f'SELECT * FROM "{table}"').fetchall()
            tables[table] = {
                "columns": columns,
                "rows": [{key: row[key] for key in row.keys()} for row in rows],
            }

        return {
            "meta": {
                "exported_at": now_iso(),
                "database": str(db_path),
                "schema_version": schema_version(conn),
            },
            "tables": tables,
        }
    finally:
        conn.close()


def main() -> int:
    parser = argparse.ArgumentParser(description="导出 SQLite 数据库到 JSON 文件")
    parser.add_argument("--db", help="SQLite 数据库路径，默认读取 config/app.json")
    parser.add_argument("--out", required=True, help="输出 JSON 文件路径")
    parser.add_argument("--overwrite", action="store_true", help="允许覆盖已有输出文件")
    args = parser.parse_args()

    db_path = resolve_db_path(args.db)
    out_path = Path(args.out).expanduser().resolve()

    if out_path.exists() and not args.overwrite:
        raise FileExistsError(f"输出文件已存在: {out_path}")

    out_path.parent.mkdir(parents=True, exist_ok=True)
    payload = export_database(db_path)
    out_path.write_text(json.dumps(payload, ensure_ascii=False, indent=2), encoding="utf-8")
    print(f"exported {db_path} -> {out_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
