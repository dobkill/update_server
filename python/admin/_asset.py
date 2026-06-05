#!/usr/bin/env python3

"""全量安装包操作核心业务逻辑。"""

from __future__ import annotations

import hashlib
from pathlib import Path

from lib.db import execute, get_product_id_by_code, get_release_id, query_one

_PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent


def _compute_sha256(file_path: Path) -> str:
    h = hashlib.sha256()
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(8192), b""):
            h.update(chunk)
    return h.hexdigest()


def _compute_md5(file_path: Path) -> str:
    h = hashlib.md5()
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(8192), b""):
            h.update(chunk)
    return h.hexdigest()


def add_asset(params: dict) -> dict:
    """添加全量安装包。

    params:
        product_code: str       必填
        version: str            必填
        file_path: str          必填
        platform: str           可选，默认 windows
        arch: str               可选，默认 x64
        package_type: str       可选，默认 portable
        file_size: int          可选
        auto_size: bool         可选
        sha256: str             可选
        md5: str                可选
        auto_hash: bool         可选
        status: str             可选，默认 active
    """
    product_code = params["product_code"]
    version = params["version"]
    file_path_str = params["file_path"]
    platform = params.get("platform", "windows")
    arch = params.get("arch", "x64")
    package_type = params.get("package_type", "portable")
    status = params.get("status", "active")

    # 查找产品 ID 和版本 ID
    try:
        product_id = get_product_id_by_code(product_code)
        release_id = get_release_id(product_id, version)
    except ValueError as e:
        return {"section": "release.asset", "action": "error", "reason": str(e)}

    # 校验不重复
    existing = query_one(
        "SELECT id FROM release_assets WHERE release_id = ? AND platform = ? AND arch = ? AND package_type = ?",
        (release_id, platform, arch, package_type),
    )
    if existing is not None:
        return {
            "section": "release.asset",
            "action": "skipped",
            "reason": f"安装包已存在 (id={existing['id']})",
            "asset_id": existing["id"],
            "product_code": product_code,
            "version": version,
            "platform": platform,
            "arch": arch,
            "package_type": package_type,
        }

    # 解析文件路径
    file_path = Path(file_path_str)
    abs_file_path = file_path if file_path.is_absolute() else _PROJECT_ROOT / file_path

    # 文件大小
    file_size = params.get("file_size")
    auto_size = params.get("auto_size", False)
    if auto_size or file_size is None:
        if not abs_file_path.exists():
            return {"section": "release.asset", "action": "error", "reason": f"文件不存在: {abs_file_path}"}
        file_size = abs_file_path.stat().st_size

    # SHA256 / MD5
    sha256 = params.get("sha256")
    md5 = params.get("md5")
    auto_hash = params.get("auto_hash", False)
    if auto_hash:
        if not abs_file_path.exists():
            return {"section": "release.asset", "action": "error", "reason": f"文件不存在，无法计算哈希: {abs_file_path}"}
        if sha256 is None:
            sha256 = _compute_sha256(abs_file_path)
        if md5 is None:
            md5 = _compute_md5(abs_file_path)

    cur = execute(
        """
        INSERT INTO release_assets (product_id, release_id, platform, arch, package_type, file_path, file_size, sha256, md5, status)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        """,
        (product_id, release_id, platform, arch, package_type, file_path_str, file_size, sha256, md5, status),
    )

    asset_id = cur.lastrowid
    return {
        "section": "release.asset",
        "action": "created",
        "asset_id": asset_id,
        "product_code": product_code,
        "version": version,
        "platform": platform,
        "arch": arch,
        "package_type": package_type,
        "file_path": file_path_str,
        "file_size": file_size,
        "sha256": sha256,
        "md5": md5,
    }
