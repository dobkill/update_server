#!/usr/bin/env python3

"""清理过期临时文件核心业务逻辑。"""

from __future__ import annotations

import time
from pathlib import Path


def clean_temp_files(params: dict) -> dict:
    """清理过期临时文件。

    params:
        directory: str          必填
        older_than_hours: int   可选，默认 24
    """
    directory = Path(params["directory"])
    older_than_hours = params.get("older_than_hours", 24)

    if not directory.exists():
        return {
            "section": "cleanup",
            "action": "skipped",
            "reason": f"目录不存在: {directory}",
            "removed": 0,
        }

    cutoff = time.time() - older_than_hours * 3600
    removed = 0
    for path in directory.rglob("*"):
        if not path.is_file():
            continue
        if path.stat().st_mtime < cutoff:
            path.unlink()
            removed += 1

    return {
        "section": "cleanup",
        "action": "completed",
        "directory": str(directory),
        "removed": removed,
        "older_than_hours": older_than_hours,
    }
