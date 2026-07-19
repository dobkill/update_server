#!/usr/bin/env python3
"""Showcase TOML 配置加载。"""

from __future__ import annotations

import tomllib
from pathlib import Path
from typing import Any


def load_toml(path: Path) -> dict[str, Any]:
    if not path.exists():
        raise FileNotFoundError(f"配置文件不存在: {path}")
    with open(path, "rb") as f:
        return tomllib.load(f)
