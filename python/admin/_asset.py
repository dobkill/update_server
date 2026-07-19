#!/usr/bin/env python3
"""资源文件复制与登记。"""

from __future__ import annotations

import hashlib
import mimetypes
import shutil
from pathlib import Path

from lib.config import get_assets_dir
from lib.db import execute, get_project_id, query_one


def _sha256(path: Path) -> str:
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(65536), b""):
            h.update(chunk)
    return h.hexdigest()


def _kind_from_content_type(content_type: str) -> str:
    if content_type.startswith("image/"):
        return "image"
    if content_type == "text/html":
        return "html"
    if content_type.startswith("video/"):
        return "video"
    return "file"


def add_asset(
    project_slug: str,
    source_file: Path,
    asset_key: str,
    *,
    project_id: int | None = None,
) -> dict:
    if not source_file.exists() or not source_file.is_file():
        raise FileNotFoundError(f"源文件不存在: {source_file}")

    asset_key = asset_key.strip().lstrip("/")
    if not asset_key:
        raise ValueError("asset_key 不能为空")

    if project_id is None:
        project_id = get_project_id(project_slug)

    assets_root = get_assets_dir()
    target_path = assets_root / asset_key
    target_path.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source_file, target_path)

    sha = _sha256(target_path)
    size = target_path.stat().st_size
    content_type = mimetypes.guess_type(str(source_file))[0] or "application/octet-stream"

    existing = query_one("SELECT id FROM project_assets WHERE asset_key = ?", (asset_key,))
    if existing:
        execute(
            "UPDATE project_assets SET project_id = ?, original_name = ?, content_type = ?, "
            "file_size = ?, sha256 = ?, kind = ? WHERE asset_key = ?",
            (project_id, source_file.name, content_type, size, sha,
             _kind_from_content_type(content_type), asset_key),
        )
        asset_id = existing["id"]
    else:
        cur = execute(
            "INSERT INTO project_assets "
            "(project_id, asset_key, original_name, content_type, file_size, sha256, kind) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)",
            (project_id, asset_key, source_file.name, content_type, size, sha,
             _kind_from_content_type(content_type)),
        )
        asset_id = int(cur.lastrowid)

    return {
        "id": asset_id,
        "asset_key": asset_key,
        "url": f"/assets/{asset_key}",
        "sha256": sha,
        "file_size": size,
        "content_type": content_type,
    }
