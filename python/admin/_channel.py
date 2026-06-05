#!/usr/bin/env python3

"""渠道发布操作核心业务逻辑。"""

from __future__ import annotations

from lib.db import execute, get_product_id_by_code, get_release_id, query_one


def set_release_channel(params: dict) -> dict:
    """设置渠道发布。

    params:
        product_code: str       必填
        version: str            必填
        channel: str            必填 (stable/beta/dev)
        platform: str           可选，默认 windows
        arch: str               可选，默认 x64
        package_type: str       可选，默认 portable
        is_latest: int          可选，默认 1 (0/1)
        status: str             可选，默认 active
    """
    product_code = params["product_code"]
    version = params["version"]
    channel = params["channel"]
    platform = params.get("platform", "windows")
    arch = params.get("arch", "x64")
    package_type = params.get("package_type", "portable")
    is_latest = int(params.get("is_latest", 1))
    status = params.get("status", "active")

    # 查找产品 ID 和版本 ID
    try:
        product_id = get_product_id_by_code(product_code)
        release_id = get_release_id(product_id, version)
    except ValueError as e:
        return {"section": "release.channel", "action": "error", "reason": str(e)}

    # 检查是否已存在
    existing = query_one(
        """
        SELECT id, is_latest, status FROM release_channels
        WHERE release_id = ? AND channel = ? AND platform = ? AND arch = ? AND package_type = ?
        """,
        (release_id, channel, platform, arch, package_type),
    )

    if existing is not None:
        # 已存在 — 更新
        channel_id = existing["id"]
        if is_latest == 1:
            # 先将旧的 latest 置为 0
            execute(
                """
                UPDATE release_channels
                SET is_latest = 0
                WHERE product_id = ? AND channel = ? AND platform = ? AND arch = ? AND package_type = ?
                  AND is_latest = 1 AND status = 'active'
                """,
                (product_id, channel, platform, arch, package_type),
            )

        execute(
            """
            UPDATE release_channels
            SET is_latest = ?, status = ?
            WHERE id = ?
            """,
            (is_latest, status, channel_id),
        )
        action = "updated"
    else:
        # 不存在 — 新增
        if is_latest == 1:
            # 先将旧的 latest 置为 0
            execute(
                """
                UPDATE release_channels
                SET is_latest = 0
                WHERE product_id = ? AND channel = ? AND platform = ? AND arch = ? AND package_type = ?
                  AND is_latest = 1 AND status = 'active'
                """,
                (product_id, channel, platform, arch, package_type),
            )

        cur = execute(
            """
            INSERT INTO release_channels (product_id, release_id, channel, platform, arch, package_type, is_latest, status)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
            """,
            (product_id, release_id, channel, platform, arch, package_type, is_latest, status),
        )
        channel_id = cur.lastrowid
        action = "created"

    return {
        "section": "release.channel",
        "action": action,
        "channel_id": channel_id,
        "product_code": product_code,
        "version": version,
        "channel": channel,
        "platform": platform,
        "arch": arch,
        "package_type": package_type,
        "is_latest": is_latest,
    }
