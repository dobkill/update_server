# 数据库设计

数据库为 SQLite，结构以 `migrations/001_init_showcase.sql` 为准。连接时设置 `PRAGMA foreign_keys = ON; journal_mode = WAL; synchronous = NORMAL`，并应用 `config/app.json` 中的 `sqlite_busy_timeout_ms`。

启动时若 `projects` 与 `site_profile` 表均已存在则跳过迁移；否则读取并执行 `migrations/001_init_showcase.sql`。数据库文件路径由 `config/app.json` 的 `database_path` 指定，默认 `./data/database/showcase.db`。

## 表结构

| 表 | 用途 |
| --- | --- |
| `schema_migrations` | 迁移版本记录（`version` 主键） |
| `site_profile` | 站点资料（单行，`id = 1`） |
| `projects` | 展示项目 |
| `project_pages` | 项目专属 HTML 页 |
| `project_assets` | 图片/HTML/附件等资源登记 |

## site_profile

单行表（`CHECK(id = 1)`），字段：`owner_name`、`site_name`、`subtitle`、`hero_label`、`hero_title`、`hero_description`、`github_url`、`email`、`resume_url`、`linkedin_url`、`twitter_url`，以及 `created_at`/`updated_at`。初始迁移插入一行默认值。

API 中字段别名见 `Storage_SQL::getSiteProfile`（如 `owner_name AS ownerName`）。

## projects

字段：`id`、`slug`（唯一）、`name`、`category`、`summary`、`description`、`status`、`year`、`role`、`platform`、`project_type`、`challenge`、`solution`、`result`、`cover_asset_key`、`hero_asset_key`、各 `_json` 字段、`featured`、`visibility`、`sort_order`、`created_at`、`updated_at`。

约束：

```text
featured   IN (0, 1)
visibility IN ('public', 'hidden')
status     IN ('active', 'archived', 'draft')
```

JSON 字段（存储为 TEXT，查询时解析）：

1. `tech_stack_json`（数组）
2. `features_json`（数组）
3. `screenshots_json`（数组，元素含 `image`）
4. `architecture_json`（数组）
5. `links_json`（对象）

索引：`idx_projects_public_order(visibility, sort_order, id)`、`idx_projects_category(category, visibility, sort_order)`。

## project_pages

字段：`id`、`project_id`、`slug`、`title`、`summary`、`html_asset_key`、`sort_order`、`visibility`、`created_at`、`updated_at`。

- 外键 `project_id -> projects(id) ON DELETE CASCADE`
- 唯一约束 `(project_id, slug)`
- `visibility IN ('public', 'hidden')`
- `html_asset_key` 指向 `project_assets.asset_key`，由后端解析为 `data/assets/` 下的 HTML 文件

索引：`idx_project_pages_project(project_id, visibility, sort_order)`。

## project_assets

字段：`id`、`project_id`（可空，外键级联删除）、`asset_key`（唯一）、`original_name`、`content_type`、`file_size`、`sha256`、`kind`、`created_at`。

`kind IN ('image', 'html', 'file', 'video')`。

索引：`idx_project_assets_project(project_id)`、`idx_project_assets_key(asset_key)`。

## 触发器

每张业务表均有 `trg_<table>_updated_at` 触发器：当 `UPDATE` 后 `updated_at` 未变时，自动刷新为 `CURRENT_TIMESTAMP`。

## 文件目录

```text
data/
├── database/showcase.db        SQLite 数据库（WAL 模式）
└── assets/                     资源文件根目录
    └── projects/{slug}/...     按 asset_key 组织
```
