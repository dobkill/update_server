#!/usr/bin/env python3

"""SQLite 数据库连接与读写封装。

- 从 config/app.json 读取 database_path
- 启用 WAL 模式和 foreign_keys
- 提供 execute / query_one / query_all 基础 CRUD
"""

from __future__ import annotations

import sqlite3
from pathlib import Path
from typing import Any

from lib.config import get as config_get

_conn: sqlite3.Connection | None = None


def get_db_path() -> Path:
    """从 config/app.json 读取数据库路径（相对于项目根目录）。"""
    raw = config_get("database_path")
    if not raw:
        raise RuntimeError("config/app.json 中缺少 database_path 配置")
    # 相对路径基于项目根目录解析
    project_root = Path(__file__).resolve().parent.parent.parent
    db_path = project_root / raw
    return db_path


def get_connection() -> sqlite3.Connection:
    """返回 sqlite3.Connection（启用 WAL、foreign_keys），单例复用。"""
    global _conn
    if _conn is not None:
        return _conn

    db_path = get_db_path()
    db_path.parent.mkdir(parents=True, exist_ok=True)

    _conn = sqlite3.connect(str(db_path))
    _conn.row_factory = sqlite3.Row
    _conn.execute("PRAGMA journal_mode = WAL")
    _conn.execute("PRAGMA foreign_keys = ON")
    return _conn


def execute(sql: str, params: tuple | None = None) -> sqlite3.Cursor:
    """执行写操作 SQL，自动 commit。"""
    conn = get_connection()
    cur = conn.execute(sql, params or ())
    conn.commit()
    return cur


def query_one(sql: str, params: tuple | None = None) -> dict | None:
    """查询单行，返回 dict 或 None。"""
    conn = get_connection()
    cur = conn.execute(sql, params or ())
    row = cur.fetchone()
    if row is None:
        return None
    return dict(row)


def query_all(sql: str, params: tuple | None = None) -> list[dict]:
    """查询多行，返回 dict 列表。"""
    conn = get_connection()
    cur = conn.execute(sql, params or ())
    return [dict(row) for row in cur.fetchall()]


def get_product_id_by_code(product_code: str) -> int:
    """根据产品编码查询产品 ID，不存在则抛出 ValueError。"""
    row = query_one("SELECT id FROM products WHERE code = ?", (product_code,))
    if row is None:
        raise ValueError(f"产品不存在: code={product_code}")
    return row["id"]


def get_release_id(product_id: int, version: str) -> int:
    """根据产品 ID 和版本号查询版本 ID，不存在则抛出 ValueError。"""
    row = query_one(
        "SELECT id FROM releases WHERE product_id = ? AND version = ?",
        (product_id, version),
    )
    if row is None:
        raise ValueError(f"版本不存在: product_id={product_id}, version={version}")
    return row["id"]