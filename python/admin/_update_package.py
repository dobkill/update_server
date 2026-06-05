#!/usr/bin/env python3

"""增量包操作核心业务逻辑。"""

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


def add_update_package(params: dict) -> dict:
    """添加增量包。

    params:
        product_code: str       必填
        source_version: str     必填
        target_version: str     必填
        file_path: str          必填
        platform: str           可选，默认 windows
        arch: str               可选，默认 x64
        package_type: str       可选，默认 portable
        update_type: str        可选，默认 incremental
        file_size: int          可选
        auto_size: bool         可选
        sha256: str             可选
        auto_hash: bool         可选
        status: str             可选，默认 active
    """
    product_code = params["product_code"]
    source_version = params["source_version"]
    target_version = params["target_version"]
    file_path_str = params["file_path"]
    platform = params.get("platform", "windows")
    arch = params.get("arch", "x64")
    package_type = params.get("package_type", "portable")
    update_type = params.get("update_type", "incremental")
    status = params.get("status", "active")

    # 查找产品 ID、源版本 ID 和目标版本 ID
    try:
        product_id = get_product_id_by_code(product_code)
        source_release_id = get_release_id(product_id, source_version)
        target_release_id = get_release_id(product_id, target_version)
    except ValueError as e:
        return {"section": "update_package", "action": "error", "reason": str(e)}

    # 校验不重复
    existing = query_one(
        """
        SELECT id FROM update_packages
        WHERE source_release_id = ? AND target_release_id = ? AND platform = ? AND arch = ? AND package_type = ?
        """,
        (source_release_id, target_release_id, platform, arch, package_type),
    )
    if existing is not None:
        return {
            "section": "update_package",
            "action": "skipped",
            "reason": f"增量包已存在 (id={existing['id']})",
            "package_id": existing["id"],
            "product_code": product_code,
            "source_version": source_version,
            "target_version": target_version,
        }

    # 解析文件路径
    file_path = Path(file_path_str)
    abs_file_path = file_path if file_path.is_absolute() else _PROJECT_ROOT / file_path

    # 文件大小
    file_size = params.get("file_size")
    auto_size = params.get("auto_size", False)
    if auto_size or file_size is None:
        if not abs_file_path.exists():
            return {"section": "update_package", "action": "error", "reason": f"文件不存在: {abs_file_path}"}
        file_size = abs_file_path.stat().st_size

    # SHA256
    sha256 = params.get("sha256")
    auto_hash = params.get("auto_hash", False)
    if auto_hash and sha256 is None:
        if not abs_file_path.exists():
            return {"section": "update_package", "action": "error", "reason": f"文件不存在，无法计算哈希: {abs_file_path}"}
        sha256 = _compute_sha256(abs_file_path)

    cur = execute(
        """
        INSERT INTO update_packages (product_id, source_release_id, target_release_id, platform, arch, package_type, update_type, file_path, file_size, sha256, status)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        """,
        (product_id, source_release_id, target_release_id, platform, arch, package_type, update_type, file_path_str, file_size, sha256, status),
    )

    package_id = cur.lastrowid
    return {
        "section": "update_package",
        "action": "created",
        "package_id": package_id,
        "product_code": product_code,
        "source_version": source_version,
        "target_version": target_version,
        "platform": platform,
        "arch": arch,
        "package_type": package_type,
        "update_type": update_type,
        "file_path": file_path_str,
        "file_size": file_size,
        "sha256": sha256,
    }
