# Python 模块说明

本目录是展示站的 Python 管理后台 CLI，直接写入 SQLite 与 `data/assets`。

## 目录结构

```text
python/
├── README.md
├── admin/
│   ├── admin.py        # CLI 入口
│   ├── _site.py        # 站点资料
│   ├── _project.py     # 项目写入
│   ├── _page.py        # 项目页面写入
│   └── _asset.py       # 资源文件复制与登记
└── lib/
    ├── __init__.py
    ├── config.py        # 读取 config/app.json
    ├── db.py            # SQLite 连接与迁移
    └── toml_loader.py   # TOML 加载
```

## 命令

```bash
python python/admin/admin.py init-db
python python/admin/admin.py set-site config/site.toml
python python/admin/admin.py add-project config/projects/daily.toml
python python/admin/admin.py add-page config/pages/daily_overview.toml
python python/admin/admin.py add-asset config/assets/daily_cover.toml
python python/admin/admin.py apply config/showcase.toml
python python/admin/admin.py list-projects
python python/admin/admin.py export data/export.json
```

`apply` 可以一次性写入站点、项目、页面和资源，适合整站导入。

## 配置示例

整站配置 `config/showcase.toml`：

```toml
[site_profile]
owner_name = "Xiang Y."
site_name = "Personal Software Lab"

[[projects]]
slug = "daily"
name = "Daily"
category = "Obsidian Plugin"
summary = "Daily workflow plugin."
long_description = "A personal knowledge workflow plugin."
featured = true
sort_order = 10

[[projects.pages]]
slug = "overview"
title = "Overview"
html = "./pages/overview.html"

[[projects.assets]]
source = "./assets/cover.png"
asset_key = "projects/daily/cover.png"
```

## 与 C++ 后端的协调

- Python CLI 直接写 SQLite 和 `data/assets`，使用 WAL 模式，可与 C++ 服务并发读。
- C++ 后端只读取数据库和资源目录，不修改内容。
- 资源通过 `/assets/{asset_key}` 受控访问，不再暴露整个 `data/`。
