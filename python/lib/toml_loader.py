#!/usr/bin/env python3

"""TOML 配置文件加载 — 兼容 Python 3.11+ 内置 tomllib 和 3.9+ 第三方 tomli。"""

from __future__ import annotations

import sys
from pathlib import Path

if sys.version_info >= (3, 11):
    import tomllib
else:
    try:
        import tomli as tomllib  # type: ignore[no-redef]
    except ImportError:
        tomllib = None  # type: ignore[assignment]


def load_toml(path: str | Path) -> dict:
    """加载 TOML 文件并返回字典。

    Raises:
        RuntimeError: Python < 3.11 且未安装 tomli
        FileNotFoundError: 文件不存在
    """
    if tomllib is None:
        raise RuntimeError(
            "Python < 3.11 需要安装 tomli：pip install tomli"
        )

    file_path = Path(path)
    if not file_path.exists():
        raise FileNotFoundError(f"TOML 文件不存在: {file_path}")

    with open(file_path, "rb") as f:
        return tomllib.load(f)


def validate_deploy_config(config: dict) -> list[str]:
    """校验 deploy config 的合法性，返回警告/错误消息列表（空列表表示全部通过）。"""
    messages: list[str] = []

    # 校验 product
    for i, p in enumerate(config.get("product", [])):
        if not p.get("name"):
            messages.append(f"product[{i}]: 缺少 name")
        if not p.get("code"):
            messages.append(f"product[{i}]: 缺少 code")
        status = p.get("status", "active")
        if status not in ("active", "disabled", "archived"):
            messages.append(f"product[{i}]: status='{status}' 不合法 (active/disabled/archived)")

    profile = config.get("site_profile")
    if profile is not None:
        if "site_name" in profile and not profile.get("site_name"):
            messages.append("site_profile: site_name 不能为空")
        if "subtitle" in profile and not profile.get("subtitle"):
            messages.append("site_profile: subtitle 不能为空")

    for i, item in enumerate(config.get("portfolio_project", [])):
        if not item.get("slug"):
            messages.append(f"portfolio_project[{i}]: 缺少 slug")
        if not item.get("name"):
            messages.append(f"portfolio_project[{i}]: 缺少 name")
        if not item.get("category"):
            messages.append(f"portfolio_project[{i}]: 缺少 category")
        visibility = item.get("visibility", "public")
        if visibility not in ("public", "hidden"):
            messages.append(f"portfolio_project[{i}]: visibility='{visibility}' 不合法")

    # 校验 release
    for i, r in enumerate(config.get("release", [])):
        if not r.get("product_code"):
            messages.append(f"release[{i}]: 缺少 product_code")
        if not r.get("version"):
            messages.append(f"release[{i}]: 缺少 version")
        status = r.get("status", "draft")
        if status not in ("draft", "active", "deprecated", "yanked"):
            messages.append(f"release[{i}]: status='{status}' 不合法")

        for j, a in enumerate(r.get("asset", [])):
            if not a.get("file_path"):
                messages.append(f"release[{i}].asset[{j}]: 缺少 file_path")
            platform = a.get("platform", "windows")
            if platform not in ("windows", "linux", "macos", "obsidian"):
                messages.append(f"release[{i}].asset[{j}]: platform='{platform}' 不合法")
            arch = a.get("arch", "x64")
            if arch not in ("x64", "arm64", "x86", "any"):
                messages.append(f"release[{i}].asset[{j}]: arch='{arch}' 不合法")

        for j, ch in enumerate(r.get("channel", [])):
            if not ch.get("channel"):
                messages.append(f"release[{i}].channel[{j}]: 缺少 channel")
            channel_val = ch.get("channel", "")
            if channel_val not in ("stable", "beta", "dev", ""):
                messages.append(f"release[{i}].channel[{j}]: channel='{channel_val}' 不合法")

    # 校验 update_package
    for i, u in enumerate(config.get("update_package", [])):
        if not u.get("product_code"):
            messages.append(f"update_package[{i}]: 缺少 product_code")
        if not u.get("source_version"):
            messages.append(f"update_package[{i}]: 缺少 source_version")
        if not u.get("target_version"):
            messages.append(f"update_package[{i}]: 缺少 target_version")
        if not u.get("file_path"):
            messages.append(f"update_package[{i}]: 缺少 file_path")

    # 校验 convert_rule
    for i, c in enumerate(config.get("convert_rule", [])):
        if not c.get("product_code"):
            messages.append(f"convert_rule[{i}]: 缺少 product_code")
        if not c.get("source_schema"):
            messages.append(f"convert_rule[{i}]: 缺少 source_schema")
        if not c.get("target_schema"):
            messages.append(f"convert_rule[{i}]: 缺少 target_schema")
        if not c.get("script_path"):
            messages.append(f"convert_rule[{i}]: 缺少 script_path")

    # 校验 page_publish
    pp = config.get("page_publish")
    if pp is not None:
        if not pp.get("manifest"):
            messages.append("page_publish: 缺少 manifest")

    # 校验 page_offline
    po = config.get("page_offline")
    if po is not None:
        if not po.get("page_key"):
            messages.append("page_offline: 缺少 page_key")
        action = po.get("action", "")
        if action not in ("online", "offline", ""):
            messages.append(f"page_offline: action='{action}' 不合法")

    # 校验 page_rollback
    pr = config.get("page_rollback")
    if pr is not None:
        if not pr.get("page_key"):
            messages.append("page_rollback: 缺少 page_key")

    return messages
