#!/usr/bin/env python3

"""执行 SQLite 结构迁移。"""

from __future__ import annotations

import argparse
import shutil
import sqlite3
import sys
from datetime import datetime
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from lib.db import get_db_path


MIGRATION_VERSION = "20260606_portfolio_home"
PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent


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


def column_exists(conn: sqlite3.Connection, table: str, column: str) -> bool:
    if not table_exists(conn, table):
        return False
    rows = conn.execute(f'PRAGMA table_info("{table}")').fetchall()
    return any(row[1] == column for row in rows)


def migration_applied(conn: sqlite3.Connection, version: str) -> bool:
    if not table_exists(conn, "schema_migrations"):
        return False
    row = conn.execute(
        "SELECT version FROM schema_migrations WHERE version = ?",
        (version,),
    ).fetchone()
    return row is not None


def execute_init_sql(conn: sqlite3.Connection) -> None:
    sql_path = PROJECT_ROOT / "migrations" / "002.init.sql"
    if not sql_path.exists():
        raise FileNotFoundError(f"初始化 SQL 不存在: {sql_path}")
    conn.executescript(sql_path.read_text(encoding="utf-8"))


def ensure_schema(conn: sqlite3.Connection) -> None:
    conn.execute(
        """
        CREATE TABLE IF NOT EXISTS schema_migrations (
            version TEXT PRIMARY KEY,
            applied_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            note TEXT NOT NULL DEFAULT ''
        )
        """
    )

    if not column_exists(conn, "products", "github_url"):
        conn.execute("ALTER TABLE products ADD COLUMN github_url TEXT NOT NULL DEFAULT ''")

    conn.execute(
        """
        CREATE TABLE IF NOT EXISTS product_careers (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            product_id INTEGER NOT NULL,
            career TEXT NOT NULL,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(product_id) REFERENCES products(id),
            UNIQUE(product_id, career)
        )
        """
    )

    conn.execute(
        """
        CREATE TABLE IF NOT EXISTS recommendations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            product_id INTEGER NOT NULL,
            status TEXT NOT NULL DEFAULT 'active',
            sort_order INTEGER NOT NULL DEFAULT 100,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(product_id) REFERENCES products(id),
            CHECK(status IN ('active', 'disabled'))
        )
        """
    )
    if not column_exists(conn, "recommendations", "sort_order"):
        conn.execute("ALTER TABLE recommendations ADD COLUMN sort_order INTEGER NOT NULL DEFAULT 100")

    conn.execute(
        """
        CREATE TABLE IF NOT EXISTS site_profile (
            id INTEGER PRIMARY KEY CHECK(id = 1),
            site_name TEXT NOT NULL DEFAULT 'YXX Works',
            subtitle TEXT NOT NULL DEFAULT '产品、插件与创作实验',
            github_url TEXT NOT NULL DEFAULT '',
            email TEXT NOT NULL DEFAULT '',
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
        )
        """
    )

    conn.execute(
        """
        CREATE TABLE IF NOT EXISTS future_directions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            comment TEXT NOT NULL,
            icon_path TEXT NOT NULL DEFAULT '',
            status TEXT NOT NULL DEFAULT 'active',
            sort_order INTEGER NOT NULL DEFAULT 100,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            CHECK(status IN ('active', 'disabled'))
        )
        """
    )
    if not column_exists(conn, "future_directions", "sort_order"):
        conn.execute("ALTER TABLE future_directions ADD COLUMN sort_order INTEGER NOT NULL DEFAULT 100")


def seed_defaults(conn: sqlite3.Connection) -> None:
    conn.execute(
        """
        INSERT INTO site_profile (id, site_name, subtitle, github_url, email)
        VALUES (1, 'YXX Works', '产品、插件与创作实验', '', '')
        ON CONFLICT(id) DO NOTHING
        """
    )

    directions = [
        ("Obsidian 插件增强", "持续优化记录、任务与知识管理体验。", "", 10),
        ("效率工具", "开发更轻量、专注的个人效率工具。", "", 20),
        ("Web 实验", "尝试小型 Web 产品与交互实验，探索新的表达方式。", "", 30),
    ]
    for title, comment, icon_path, sort_order in directions:
        conn.execute(
            """
            INSERT INTO future_directions (title, comment, icon_path, status, sort_order)
            SELECT ?, ?, ?, 'active', ?
             WHERE NOT EXISTS (
                   SELECT 1 FROM future_directions WHERE title = ?
             )
            """,
            (title, comment, icon_path, sort_order, title),
        )

    daily = conn.execute(
        "SELECT id FROM products WHERE code = ? COLLATE NOCASE AND status = 'active'",
        ("Daily",),
    ).fetchone()
    if daily is not None:
        conn.execute(
            """
            INSERT INTO recommendations (product_id, status, sort_order)
            SELECT ?, 'active', 10
             WHERE NOT EXISTS (
                   SELECT 1 FROM recommendations WHERE product_id = ?
             )
            """,
            (daily[0], daily[0]),
        )


def migrate(db_path: Path, target: str, force: bool) -> Path | None:
    db_path.parent.mkdir(parents=True, exist_ok=True)
    backup_path = backup_database(db_path)
    conn = sqlite3.connect(str(db_path))
    try:
        conn.execute("PRAGMA foreign_keys = ON")
        if not table_exists(conn, "products"):
            execute_init_sql(conn)
        conn.execute("BEGIN")
        ensure_schema(conn)
        if migration_applied(conn, target) and not force:
            conn.rollback()
            return backup_path
        seed_defaults(conn)
        conn.execute(
            """
            INSERT INTO schema_migrations (version, note)
            VALUES (?, 'portfolio home schema')
            ON CONFLICT(version) DO UPDATE
                SET applied_at = CURRENT_TIMESTAMP,
                    note = excluded.note
            """,
            (target,),
        )
        conn.commit()
        return backup_path
    except Exception:
        conn.rollback()
        raise
    finally:
        conn.close()


def main() -> int:
    parser = argparse.ArgumentParser(description="执行 SQLite 结构迁移")
    parser.add_argument("--db", help="SQLite 数据库路径，默认读取 config/app.json")
    parser.add_argument("--target", default=MIGRATION_VERSION, help="目标迁移版本")
    parser.add_argument("--force", action="store_true", help="重复执行当前迁移")
    args = parser.parse_args()

    db_path = resolve_db_path(args.db)
    backup_path = migrate(db_path, args.target, args.force)
    if backup_path:
        print(f"backup {backup_path}")
    print(f"migrated {db_path} -> {args.target}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
