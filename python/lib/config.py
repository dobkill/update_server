#!/usr/bin/env python3

"""配置读取 — 从 config/app.json 加载应用配置。"""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

_PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent  # updata_server/
_CONFIG_PATH = _PROJECT_ROOT / "config" / "app.json"

_cache: dict | None = None


def load_app_config(force_reload: bool = False) -> dict:
    """读取 config/app.json，返回字典。结果会缓存，除非 force_reload=True。"""
    global _cache
    if _cache is not None and not force_reload:
        return _cache

    if not _CONFIG_PATH.exists():
        raise FileNotFoundError(f"配置文件不存在: {_CONFIG_PATH}")

    with open(_CONFIG_PATH, "r", encoding="utf-8") as f:
        _cache = json.load(f)
    return _cache


def get(key: str, default: Any = None) -> Any:
    """获取单个配置项。"""
    cfg = load_app_config()
    return cfg.get(key, default)