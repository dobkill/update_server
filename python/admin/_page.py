#!/usr/bin/env python3

"""页面运维操作核心业务逻辑。"""

from __future__ import annotations

import json
import shutil
from pathlib import Path

from lib.config import get as config_get

ALLOWED_BLOCKS = {
    "HeroBlock",
    "FeatureGridBlock",
    "RichTextBlock",
    "ImageTextBlock",
    "ScreenshotGalleryBlock",
    "DownloadPanelBlock",
    "TimelineBlock",
    "FaqBlock",
    "FooterCtaBlock",
}

_PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent


def page_publish(params: dict) -> dict:
    """页面发布校验与部署。

    params:
        manifest: str           必填
        assets: str             可选
        target: str             可选，默认 staging (staging/active)
    """
    manifest_path = Path(params["manifest"])
    if not manifest_path.exists():
        if not (_PROJECT_ROOT / manifest_path).exists():
            return {"section": "page_publish", "action": "error", "reason": f"Manifest 不存在: {manifest_path}"}
        manifest_path = _PROJECT_ROOT / manifest_path

    payload = json.loads(manifest_path.read_text(encoding="utf-8"))

    # 校验
    required_keys = {"page_key", "route", "title", "blocks"}
    missing = required_keys - payload.keys()
    if missing:
        return {"section": "page_publish", "action": "error", "reason": f"Manifest 缺少字段: {sorted(missing)}"}

    if not str(payload["route"]).startswith("/"):
        return {"section": "page_publish", "action": "error", "reason": "route 必须以 '/' 开头"}

    for block in payload["blocks"]:
        block_type = block.get("type", "")
        if block_type not in ALLOWED_BLOCKS:
            return {"section": "page_publish", "action": "error", "reason": f"不支持的块类型: {block_type}"}

    target = params.get("target", "staging")
    target_dir_key = "page_staging_dir" if target == "staging" else "page_active_dir"
    target_base = config_get(target_dir_key)
    if not target_base:
        return {"section": "page_publish", "action": "error", "reason": f"config/app.json 中缺少 {target_dir_key}"}

    target_dir = _PROJECT_ROOT / target_base
    target_dir.mkdir(parents=True, exist_ok=True)

    page_key = payload["page_key"]
    dest_manifest = target_dir / f"{page_key}.json"
    shutil.copy2(manifest_path, dest_manifest)

    # 复制资源目录
    assets_str = params.get("assets", "")
    assets_dir = ""
    if assets_str:
        assets_src = Path(assets_str)
        if not assets_src.is_absolute():
            assets_src = _PROJECT_ROOT / assets_src
        if assets_src.exists():
            assets_dest = target_dir / f"{page_key}_assets"
            if assets_dest.exists():
                shutil.rmtree(assets_dest)
            shutil.copytree(assets_src, assets_dest)
            assets_dir = str(assets_dest)

    return {
        "section": "page_publish",
        "action": "created",
        "page_key": page_key,
        "route": payload["route"],
        "title": payload["title"],
        "target": target,
        "manifest_path": str(dest_manifest),
        "assets_dir": assets_dir,
        "block_count": len(payload["blocks"]),
    }


def page_offline(params: dict) -> dict:
    """页面上线/下线。

    params:
        page_key: str           必填
        action: str             必填 (online/offline)
    """
    page_key = params["page_key"]
    action = params["action"]

    active_dir_key = config_get("page_active_dir", "data/pages/active")
    offline_dir_key = config_get("page_offline_dir", "data/pages/offline")

    active_dir = _PROJECT_ROOT / active_dir_key
    offline_dir = _PROJECT_ROOT / offline_dir_key
    active_dir.mkdir(parents=True, exist_ok=True)
    offline_dir.mkdir(parents=True, exist_ok=True)

    manifest_name = f"{page_key}.json"
    assets_name = f"{page_key}_assets"

    if action == "offline":
        src_manifest = active_dir / manifest_name
        src_assets = active_dir / assets_name
        if not src_manifest.exists():
            return {"section": "page_offline", "action": "error", "reason": f"活跃页面不存在: {src_manifest}"}
        shutil.move(str(src_manifest), str(offline_dir / manifest_name))
        if src_assets.exists():
            shutil.move(str(src_assets), str(offline_dir / assets_name))
        return {"section": "page_offline", "action": "completed", "page_key": page_key, "status": "offline"}
    else:
        # online
        src_manifest = offline_dir / manifest_name
        src_assets = offline_dir / assets_name
        if not src_manifest.exists():
            return {"section": "page_offline", "action": "error", "reason": f"离线页面不存在: {src_manifest}"}
        shutil.move(str(src_manifest), str(active_dir / manifest_name))
        if src_assets.exists():
            shutil.move(str(src_assets), str(active_dir / assets_name))
        return {"section": "page_online", "action": "completed", "page_key": page_key, "status": "online"}


def page_rollback(params: dict) -> dict:
    """页面回滚。

    params:
        page_key: str           必填
    """
    page_key = params["page_key"]

    active_dir_key = config_get("page_active_dir", "data/pages/active")
    history_dir_key = config_get("page_history_dir", "data/pages/history")

    active_dir = _PROJECT_ROOT / active_dir_key
    history_dir = _PROJECT_ROOT / history_dir_key
    active_dir.mkdir(parents=True, exist_ok=True)
    history_dir.mkdir(parents=True, exist_ok=True)

    manifest_name = f"{page_key}.json"
    assets_name = f"{page_key}_assets"

    current_manifest = active_dir / manifest_name
    history_manifest = history_dir / manifest_name

    if not history_manifest.exists():
        return {"section": "page_rollback", "action": "error", "reason": f"无历史版本可回滚: {history_manifest}"}

    # 备份当前版本
    backup_manifest = active_dir / f"{page_key}.json.bak"
    backup_assets = active_dir / f"{page_key}_assets.bak"
    if current_manifest.exists():
        shutil.copy2(current_manifest, backup_manifest)
    if (active_dir / assets_name).exists():
        if backup_assets.exists():
            shutil.rmtree(backup_assets)
        shutil.copytree(active_dir / assets_name, backup_assets)

    # 恢复历史版本
    shutil.copy2(history_manifest, current_manifest)
    if (history_dir / assets_name).exists():
        if (active_dir / assets_name).exists():
            shutil.rmtree(active_dir / assets_name)
        shutil.copytree(history_dir / assets_name, active_dir / assets_name)

    return {
        "section": "page_rollback",
        "action": "completed",
        "page_key": page_key,
        "status": "rolled_back",
        "backup_manifest": str(backup_manifest),
    }
