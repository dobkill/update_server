#!/usr/bin/env python3
"""Showcase 管理员 CLI。

命令：
  init-db                 初始化数据库（执行迁移）
  set-site <toml>         写入站点资料
  add-project <toml>      新增/更新项目
  add-page <toml>         新增/更新项目页面（toml 中含 project_slug）
  add-asset <toml>        登记资源文件（toml 中含 project_slug, source, asset_key）
  apply <toml>            应用整站配置（site + projects + pages + assets）
  list-projects           列出所有项目
  export <json>           导出站点/项目/页面为 JSON
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from lib.db import get_connection, query_all
from lib.toml_loader import load_toml
from admin._asset import add_asset
from admin._page import upsert_page
from admin._project import upsert_project
from admin._site import upsert_site_profile

PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent


def cmd_init_db(_args: list[str]) -> int:
    get_connection()
    print("数据库已就绪")
    return 0


def cmd_set_site(args: list[str]) -> int:
    if not args:
        print("用法: set-site <toml>", file=sys.stderr)
        return 1
    config = load_toml(Path(args[0]))
    if "site_profile" not in config:
        print("配置缺少 [site_profile]", file=sys.stderr)
        return 1
    upsert_site_profile(config["site_profile"])
    print("站点资料已写入")
    return 0


def _apply_project(project_data: dict, base_dir: Path) -> int:
    project_id = upsert_project(project_data)
    project_slug = str(project_data["slug"])

    for asset_data in project_data.get("assets", []):
        source = Path(asset_data["source"])
        if not source.is_absolute():
            source = base_dir / source
        add_asset(project_slug, source, asset_data["asset_key"], project_id=project_id)

    for page_data in project_data.get("pages", []):
        if "html_asset_key" not in page_data and "html" in page_data:
            html_source = Path(page_data["html"])
            if not html_source.is_absolute():
                html_source = base_dir / html_source
            asset_key = f"projects/{project_slug}/pages/{page_data['slug']}.html"
            add_asset(project_slug, html_source, asset_key, project_id=project_id)
            page_data["html_asset_key"] = asset_key
        upsert_page(project_slug, page_data)

    return project_id


def cmd_add_project(args: list[str]) -> int:
    if not args:
        print("用法: add-project <toml>", file=sys.stderr)
        return 1
    config_path = Path(args[0])
    config = load_toml(config_path)
    if "project" not in config:
        print("配置缺少 [project]", file=sys.stderr)
        return 1
    project_id = _apply_project(config["project"], config_path.parent)
    print(f"项目已写入: id={project_id}")
    return 0


def cmd_add_page(args: list[str]) -> int:
    if not args:
        print("用法: add-page <toml>", file=sys.stderr)
        return 1
    config_path = Path(args[0])
    config = load_toml(config_path)
    if "page" not in config or "project_slug" not in config:
        print("配置缺少 [page] 或 project_slug", file=sys.stderr)
        return 1
    project_slug = config["project_slug"]
    page_id = upsert_page(project_slug, config["page"])
    print(f"页面已写入: id={page_id}")
    return 0


def cmd_add_asset(args: list[str]) -> int:
    if not args:
        print("用法: add-asset <toml>", file=sys.stderr)
        return 1
    config_path = Path(args[0])
    config = load_toml(config_path)
    if "asset" not in config or "project_slug" not in config:
        print("配置缺少 [asset] 或 project_slug", file=sys.stderr)
        return 1
    asset_cfg = config["asset"]
    source = Path(asset_cfg["source"])
    if not source.is_absolute():
        source = config_path.parent / source
    info = add_asset(config["project_slug"], source, asset_cfg["asset_key"])
    print(f"资源已登记: {info}")
    return 0


def cmd_apply(args: list[str]) -> int:
    if not args:
        print("用法: apply <toml>", file=sys.stderr)
        return 1
    config_path = Path(args[0])
    config = load_toml(config_path)

    if "site_profile" in config:
        upsert_site_profile(config["site_profile"])
        print("站点资料已写入")

    for project_data in config.get("projects", []):
        project_id = _apply_project(project_data, config_path.parent)
        print(f"项目已写入: slug={project_data['slug']} id={project_id}")

    return 0


def cmd_list_projects(_args: list[str]) -> int:
    rows = query_all(
        "SELECT slug, name, category, status, featured, sort_order FROM projects ORDER BY sort_order, id"
    )
    for row in rows:
        print(f"{row['slug']}\t{row['name']}\t{row['category']}\t{row['status']}\t{row['sort_order']}")
    return 0


def cmd_export(args: list[str]) -> int:
    if not args:
        print("用法: export <json>", file=sys.stderr)
        return 1

    conn = get_connection()
    site = conn.execute("SELECT * FROM site_profile WHERE id = 1").fetchone()
    projects = [
        dict(row) for row in conn.execute(
            "SELECT * FROM projects ORDER BY sort_order, id"
        ).fetchall()
    ]
    for project in projects:
        pages = [
            dict(row) for row in conn.execute(
                "SELECT * FROM project_pages WHERE project_id = ? ORDER BY sort_order, id",
                (project["id"],),
            ).fetchall()
        ]
        project["pages"] = pages

    data = {
        "site_profile": dict(site) if site else {},
        "projects": projects,
    }

    out_path = Path(args[0])
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with open(out_path, "w", encoding="utf-8") as f:
        json.dump(data, f, ensure_ascii=False, indent=2)
    print(f"已导出: {out_path}")
    return 0


COMMANDS = {
    "init-db": cmd_init_db,
    "set-site": cmd_set_site,
    "add-project": cmd_add_project,
    "add-page": cmd_add_page,
    "add-asset": cmd_add_asset,
    "apply": cmd_apply,
    "list-projects": cmd_list_projects,
    "export": cmd_export,
}


def main() -> int:
    if len(sys.argv) < 2 or sys.argv[1] in {"-h", "--help"}:
        print(__doc__)
        return 0

    command = sys.argv[1]
    handler = COMMANDS.get(command)
    if handler is None:
        print(f"未知命令: {command}", file=sys.stderr)
        print(__doc__, file=sys.stderr)
        return 1

    try:
        return handler(sys.argv[2:])
    except Exception as exc:
        print(f"执行失败: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
