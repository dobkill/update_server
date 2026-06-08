#!/usr/bin/env python3

"""从 JSON 文件导入数据到 SQLite。"""

from __future__ import annotations

import argparse
import json
import shutil
import sqlite3
import sys
from datetime import datetime
from pathlib import Path
from typing import Any

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from lib.db import get_db_path


TABLE_ORDER = [
    "products",
    "portfolio_projects",
    "releases",
    "release_assets",
    "release_channels",
    "update_packages",
    "convert_rules",
    "convert_tasks",
    "site_profile",
    "schema_migrations",
]


def resolve_db_path(raw: str | None) -> Path:
    if raw:
        return Path(raw).expanduser().resolve()
    return get_db_path().resolve()


def backup_database(db_path: Path) -> Path | None:
    if not db_path.exists():
        return None
    stamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    backup_path = db_path.with_suffix(db_path.suffix + f".bak.{stamp}")
    shutil.copy2(db_path, backup_path)
    return backup_path


def table_exists(conn: sqlite3.Connection, table: str) -> bool:
    row = conn.execute(
        "SELECT name FROM sqlite_master WHERE type = 'table' AND name = ?",
        (table,),
    ).fetchone()
    return row is not None


def table_columns(conn: sqlite3.Connection, table: str) -> list[str]:
    return [row[1] for row in conn.execute(f'PRAGMA table_info("{table}")').fetchall()]


def primary_key_columns(conn: sqlite3.Connection, table: str) -> list[str]:
    rows = conn.execute(f'PRAGMA table_info("{table}")').fetchall()
    return [row[1] for row in sorted(rows, key=lambda item: item[5]) if row[5]]


def ordered_tables(input_tables: dict[str, Any]) -> list[str]:
    known = [table for table in TABLE_ORDER if table in input_tables]
    extra = sorted(table for table in input_tables if table not in TABLE_ORDER)
    return known + extra


def insert_row(conn: sqlite3.Connection, table: str, row: dict[str, Any], mode: str) -> None:
    target_columns = table_columns(conn, table)
    columns = [column for column in row.keys() if column in target_columns]
    if not columns:
        return

    placeholders = ", ".join("?" for _ in columns)
    quoted_columns = ", ".join(f'"{column}"' for column in columns)
    values = [row[column] for column in columns]

    if mode == "insert":
        sql = f'INSERT INTO "{table}" ({quoted_columns}) VALUES ({placeholders})'
    elif mode == "upsert":
        pk_columns = primary_key_columns(conn, table)
        if pk_columns:
            conflict_columns = ", ".join(f'"{column}"' for column in pk_columns)
            update_columns = [column for column in columns if column not in pk_columns]
            if update_columns:
                assignments = ", ".join(
                    f'"{column}" = excluded."{column}"' for column in update_columns
                )
                sql = (
                    f'INSERT INTO "{table}" ({quoted_columns}) VALUES ({placeholders}) '
                    f"ON CONFLICT ({conflict_columns}) DO UPDATE SET {assignments}"
                )
            else:
                sql = (
                    f'INSERT INTO "{table}" ({quoted_columns}) VALUES ({placeholders}) '
                    f"ON CONFLICT ({conflict_columns}) DO NOTHING"
                )
        else:
            sql = f'INSERT OR REPLACE INTO "{table}" ({quoted_columns}) VALUES ({placeholders})'
    else:
        sql = f'INSERT INTO "{table}" ({quoted_columns}) VALUES ({placeholders})'

    conn.execute(sql, values)


def import_database(db_path: Path, input_path: Path, mode: str) -> Path | None:
    payload = json.loads(input_path.read_text(encoding="utf-8"))
    tables = payload.get("tables")
    if not isinstance(tables, dict):
        raise ValueError("JSON 文件缺少 tables 对象")

    db_path.parent.mkdir(parents=True, exist_ok=True)
    backup_path = backup_database(db_path)

    conn = sqlite3.connect(str(db_path))
    try:
        conn.execute("PRAGMA foreign_keys = ON")
        conn.execute("BEGIN")

        table_names = ordered_tables(tables)
        if mode == "replace":
            for table in reversed(table_names):
                if table_exists(conn, table):
                    conn.execute(f'DELETE FROM "{table}"')

        for table in table_names:
            if not table_exists(conn, table):
                raise ValueError(f"目标数据库缺少表: {table}")
            table_payload = tables[table]
            rows = table_payload.get("rows", [])
            if not isinstance(rows, list):
                raise ValueError(f"{table}.rows 不是数组")
            for row in rows:
                if not isinstance(row, dict):
                    raise ValueError(f"{table}.rows 包含非对象数据")
                insert_row(conn, table, row, mode)

        conn.commit()
        return backup_path
    except Exception:
        conn.rollback()
        raise
    finally:
        conn.close()


def main() -> int:
    parser = argparse.ArgumentParser(description="从 JSON 文件导入数据到 SQLite")
    parser.add_argument("--db", help="SQLite 数据库路径，默认读取 config/app.json")
    parser.add_argument("--in", dest="input", required=True, help="输入 JSON 文件路径")
    parser.add_argument(
        "--mode",
        choices=("insert", "upsert", "replace"),
        default="upsert",
        help="导入模式",
    )
    args = parser.parse_args()

    db_path = resolve_db_path(args.db)
    input_path = Path(args.input).expanduser().resolve()
    if not input_path.exists():
        raise FileNotFoundError(f"输入文件不存在: {input_path}")

    backup_path = import_database(db_path, input_path, args.mode)
    if backup_path:
        print(f"backup {backup_path}")
    print(f"imported {input_path} -> {db_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
