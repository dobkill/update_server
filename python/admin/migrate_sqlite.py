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


MIGRATION_VERSION = "20260607_portfolio_projects"
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

    if table_exists(conn, "releases") and not column_exists(conn, "releases", "html_path"):
        conn.execute("ALTER TABLE releases ADD COLUMN html_path TEXT")

    if (
        table_exists(conn, "releases")
        and column_exists(conn, "releases", "html_path")
        and column_exists(conn, "releases", "vue_path")
    ):
        conn.execute(
            """
            UPDATE releases
               SET html_path = vue_path
             WHERE (html_path IS NULL OR html_path = '')
               AND vue_path IS NOT NULL
               AND vue_path <> ''
            """
        )

    if table_exists(conn, "releases") and column_exists(conn, "releases", "html_path"):
        conn.execute(
            """
            UPDATE releases
               SET html_path = substr(html_path, 1, length(html_path) - 4) || '.html'
             WHERE html_path LIKE '%.vue'
            """
        )
        conn.execute(
            """
            UPDATE releases
               SET html_path = replace(html_path, '/vue/', '/html/')
             WHERE html_path LIKE '%/vue/%'
            """
        )

    conn.execute(
        """
        CREATE TABLE IF NOT EXISTS site_profile (
            id INTEGER PRIMARY KEY CHECK(id = 1),
            owner_name TEXT NOT NULL DEFAULT 'LIANG Y.',
            site_name TEXT NOT NULL DEFAULT 'Personal Software Lab',
            subtitle TEXT NOT NULL DEFAULT 'Self-built apps, tools, systems, and experiments.',
            hero_label TEXT NOT NULL DEFAULT 'DEVELOPER & BUILDER',
            hero_title TEXT NOT NULL DEFAULT 'Personal Software Lab',
            hero_description TEXT NOT NULL DEFAULT 'A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity.',
            github_url TEXT NOT NULL DEFAULT '',
            email TEXT NOT NULL DEFAULT '',
            resume_url TEXT NOT NULL DEFAULT '',
            linkedin_url TEXT NOT NULL DEFAULT '',
            twitter_url TEXT NOT NULL DEFAULT '',
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
        )
        """
    )

    for column, definition in [
        ("owner_name", "TEXT NOT NULL DEFAULT 'LIANG Y.'"),
        ("hero_label", "TEXT NOT NULL DEFAULT 'DEVELOPER & BUILDER'"),
        ("hero_title", "TEXT NOT NULL DEFAULT 'Personal Software Lab'"),
        (
            "hero_description",
            "TEXT NOT NULL DEFAULT 'A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity.'",
        ),
        ("resume_url", "TEXT NOT NULL DEFAULT ''"),
        ("linkedin_url", "TEXT NOT NULL DEFAULT ''"),
        ("twitter_url", "TEXT NOT NULL DEFAULT ''"),
    ]:
        if not column_exists(conn, "site_profile", column):
            conn.execute(f"ALTER TABLE site_profile ADD COLUMN {column} {definition}")

    conn.execute(
        """
        CREATE TABLE IF NOT EXISTS portfolio_projects (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            product_id INTEGER,
            slug TEXT NOT NULL UNIQUE,
            name TEXT NOT NULL,
            category TEXT NOT NULL,
            description TEXT NOT NULL DEFAULT '',
            long_description TEXT NOT NULL DEFAULT '',
            featured INTEGER NOT NULL DEFAULT 0,
            status TEXT NOT NULL DEFAULT 'Live',
            year TEXT NOT NULL DEFAULT '',
            platform TEXT NOT NULL DEFAULT '',
            role TEXT NOT NULL DEFAULT '',
            project_type TEXT NOT NULL DEFAULT '',
            cover_image_url TEXT NOT NULL DEFAULT '',
            hero_image_url TEXT NOT NULL DEFAULT '',
            tech_stack_json TEXT NOT NULL DEFAULT '[]',
            features_json TEXT NOT NULL DEFAULT '[]',
            screenshots_json TEXT NOT NULL DEFAULT '[]',
            architecture_json TEXT NOT NULL DEFAULT '[]',
            challenge TEXT NOT NULL DEFAULT '',
            solution TEXT NOT NULL DEFAULT '',
            result TEXT NOT NULL DEFAULT '',
            links_json TEXT NOT NULL DEFAULT '{}',
            visibility TEXT NOT NULL DEFAULT 'public',
            sort_order INTEGER NOT NULL DEFAULT 100,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(product_id) REFERENCES products(id),
            CHECK(featured IN (0, 1)),
            CHECK(visibility IN ('public', 'hidden'))
        )
        """
    )
    conn.execute(
        """
        CREATE INDEX IF NOT EXISTS idx_portfolio_projects_public_order
        ON portfolio_projects(visibility, sort_order, id)
        """
    )
    conn.execute(
        """
        CREATE INDEX IF NOT EXISTS idx_portfolio_projects_category
        ON portfolio_projects(category, visibility, sort_order)
        """
    )


def seed_defaults(conn: sqlite3.Connection) -> None:
    conn.execute(
        """
        INSERT INTO site_profile (
            id, owner_name, site_name, subtitle, hero_label, hero_title, hero_description,
            github_url, email, resume_url, linkedin_url, twitter_url
        )
        VALUES (
            1, 'LIANG Y.', 'Personal Software Lab',
            'Self-built apps, tools, systems, and experiments.',
            'DEVELOPER & BUILDER', 'Personal Software Lab',
            'A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity.',
            '', '', '', '', ''
        )
        ON CONFLICT(id) DO NOTHING
        """
    )
    conn.execute(
        """
        UPDATE site_profile
           SET owner_name = 'LIANG Y.',
               site_name = 'Personal Software Lab',
               subtitle = 'Self-built apps, tools, systems, and experiments.',
               hero_label = 'DEVELOPER & BUILDER',
               hero_title = 'Personal Software Lab',
               hero_description = 'A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity.'
         WHERE id = 1
           AND site_name = 'YXX Works'
           AND subtitle = '产品、插件与创作实验'
        """
    )
    conn.execute(
        """
        DELETE FROM portfolio_projects
         WHERE product_id IS NULL
           AND slug IN (
               'insightboard', 'autoreport', 'datacanvas', 'promptflow', 'taskpilot',
               'codesnip', 'serverlens', 'notestream', 'datapulse', 'shipgate'
           )
        """
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
            conn.commit()
            return backup_path
        seed_defaults(conn)
        conn.execute(
            """
            INSERT INTO schema_migrations (version, note)
            VALUES (?, 'portfolio project schema')
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
