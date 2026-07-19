#!/usr/bin/env python3
"""Showcase 配置读取 — 从 config/app.json 加载应用配置。"""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

_PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent
_CONFIG_PATH = _PROJECT_ROOT / "config" / "app.json"

_cache: dict | None = None


def project_root() -> Path:
    return _PROJECT_ROOT


def load_app_config(force_reload: bool = False) -> dict:
    global _cache
    if _cache is not None and not force_reload:
        return _cache

    if not _CONFIG_PATH.exists():
        raise FileNotFoundError(f"配置文件不存在: {_CONFIG_PATH}")

    with open(_CONFIG_PATH, "r", encoding="utf-8") as f:
        _cache = json.load(f)
    return _cache


def get(key: str, default: Any = None) -> Any:
    return load_app_config().get(key, default)


def get_assets_dir() -> Path:
    raw = get("assets_dir", "./data/assets")
    path = Path(raw)
    return path if path.is_absolute() else _PROJECT_ROOT / path


def get_database_path() -> Path:
    raw = get("database_path", "./data/database/showcase.db")
    path = Path(raw)
    return path if path.is_absolute() else _PROJECT_ROOT / path
