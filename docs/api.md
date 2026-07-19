# API 设计

接口统一挂载在 `/api/v1` 下，响应体为 JSON。

成功：

```json
{
  "success": true,
  "data": {}
}
```

失败：

```json
{
  "success": false,
  "error": {
    "code": "PROJECT_NOT_FOUND",
    "message": "project not found",
    "details": {}
  }
}
```

错误码：`STORAGE_ERROR`（500）、`PROJECT_NOT_FOUND`（404）、`ASSET_NOT_FOUND`（404）、`PROJECT_PAGE_NOT_FOUND`（404）。`/api/` 与 `/assets/` 下未匹配的路由由 `main.cpp` 的默认处理器返回 404 JSON。

## 接口总览

| 方法 | 路径 | 处理 | 说明 |
| --- | --- | --- | --- |
| `GET` | `/api/v1/site` | `Storage_SQL::getSiteProfile` | 站点资料 |
| `GET` | `/api/v1/home` | `Storage_SQL::getHomeData` | 首页聚合数据 |
| `GET` | `/api/v1/projects` | `Storage_SQL::getProjectList` | 公开项目列表 |
| `GET` | `/api/v1/projects/{slug}` | `Storage_SQL::getProjectDetail` | 项目详情（含 `pages`） |
| `GET` | `/api/v1/projects/{slug}/pages` | `Storage_SQL::getProjectPages` | 项目页面列表 |
| `GET` | `/assets/{asset_key}` | `Storage_SQL::resolveAssetPath` | 受控静态资源（正则 `^/assets/(.+)$`） |
| `GET` | `/projects/{slug}/pages/{page_slug}` | `Storage_SQL::resolveProjectPagePath` | 项目专属 HTML 页 |

所有项目查询均过滤 `visibility = 'public'`，`slug` 比较使用 `COLLATE NOCASE`，排序按 `sort_order, id`。

## 站点资料

```http
GET /api/v1/site
```

返回 `site_profile` 单行（别名后的驼峰字段）。空库时返回内置默认值。

## 首页聚合

```http
GET /api/v1/home
```

返回对象同时包含以下字段（兼容前端不同命名）：

- `site` / `profile`：站点资料
- `featuredProjects` / `featured`：`featured = 1` 的项目
- `projects` / `items`：全部公开项目
- `categories` / `filters`：分类列表，首项恒为 `"All"`

## 项目列表

```http
GET /api/v1/projects
```

返回 `{"items": [...]}`。每个项目由 `normalizeProject` 处理：`featured` 转布尔，`description`/`longDescription`/`type`/`coverImageUrl`/`heroImageUrl` 等字段重命名，各 `_json` 字段解析为对象/数组，`asset_key` 自动加上 `/assets/` 前缀。

## 项目详情

```http
GET /api/v1/projects/daily
```

返回 `{"project": {...}}`，其中 `project` 同列表项的归一化结构，并附带 `pages` 列表。

## 项目页面列表

```http
GET /api/v1/projects/daily/pages
```

返回 `{"items": [...]}`。每项包含 `slug`、`title`、`summary`、`sortOrder`、`htmlUrl`（`/projects/{slug}/pages/{page_slug}`）。

## 项目专属 HTML 页

```http
GET /projects/daily/pages/overview
```

直接返回 HTML 文件（`Content-Type: text/html`），文件由数据库登记的 `html_asset_key` 解析得到。
