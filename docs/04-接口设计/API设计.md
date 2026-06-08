# API 设计

接口统一挂载在 `/api/v1` 下，响应体为 JSON。

```json
{
  "code": 0,
  "message": "ok",
  "data": {}
}
```

`code = 0` 表示业务成功。业务错误使用 JSON `code` 表达，并同步设置合理的 HTTP 状态码。

## API 总览

| 方法 | 路径 | Action | 用途 |
| --- | --- | --- | --- |
| `GET` | `/api/v1/projects` | `ProjectsAction::ListProjects()` | 作品集首页项目列表 |
| `GET` | `/api/v1/projects/{slug}` | `ProjectsAction::GetProjectDetail()` | 作品集项目详情 |
| `GET` | `/api/v1/products/{product_code}/document` | `DocumentAction::GetDocument()` | 软件发布文档 |
| `GET` | `/api/v1/products/{product_code}/releases` | `DocumentAction::GetListReleases()` | 软件版本列表 |
| `GET` | `/api/v1/products/{product_code}/check-update/{version}` | `CheckUpdateAction::CheckUpdate()` | 客户端更新检查 |
| `POST` | `/api/v1/products/{product_code}/convert-tasks` | `ConvertDataTaskAction::createTask()` | 创建数据转换任务 |
| `GET` | `/api/v1/products/{product_code}/convert-tasks/{task_id}/status` | `ConvertDataTaskAction::getTaskStatus()` | 查询转换任务状态 |
| `GET` | `/api/v1/products/{product_code}/convert-tasks/{task_id}/result` | `ConvertDataTaskAction::getTaskResult()` | 查询转换任务结果 |

Web 作品集只使用 `/api/v1/projects` 和 `/api/v1/projects/{slug}`。软件更新能力仍使用 `/api/v1/products/...` 下的发布、更新检查和转换任务接口。

## 作品集项目列表

```http
GET /api/v1/projects
```

成功响应：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "profile": {
      "ownerName": "Xiang Y.",
      "siteName": "Personal Software Lab",
      "subtitle": "Self-built apps, tools, systems, and experiments.",
      "heroLabel": "DEVELOPER & BUILDER",
      "heroTitle": "Personal Software Lab",
      "heroDescription": "A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity.",
      "githubUrl": "",
      "email": "",
      "resumeUrl": "",
      "linkedinUrl": "",
      "twitterUrl": ""
    },
    "featured": [],
    "items": [],
    "filters": ["All", "Tool"]
  }
}
```

`data.items` 和 `data.featured` 中的项目字段：

1. `slug`
2. `name`
3. `category`
4. `description`
5. `longDescription`
6. `featured`
7. `status`
8. `year`
9. `platform`
10. `role`
11. `type`
12. `coverImageUrl`
13. `heroImageUrl`
14. `techStack`
15. `features`
16. `screenshots`
17. `architecture`
18. `challenge`
19. `solution`
20. `result`
21. `links`
22. `sortOrder`
23. `productCode`

`screenshots` 为对象数组，每项包含 `title`、`desc` 和 `image`。`image` 使用可直接访问的静态资源路径，例如 `/data/releases/Daily/v1.0.0/html/总览.png`。

## 作品集项目详情

```http
GET /api/v1/projects/{slug}
```

成功响应：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "profile": {},
    "project": {
      "slug": "daily",
      "name": "Daily",
      "category": "Tool",
      "description": "An Obsidian workflow plugin for daily tasks, quick capture, and project planning progress.",
      "longDescription": "Daily is a personal knowledge workflow plugin that connects daily tasks, quick notes, project progress, and lightweight review pages inside Obsidian.",
      "featured": true,
      "status": "Live",
      "year": "2026",
      "platform": "Obsidian",
      "role": "Developer & Designer",
      "type": "Plugin",
      "coverImageUrl": "/data/releases/Daily/v1.0.0/html/总览.png",
      "heroImageUrl": "/data/releases/Daily/v1.0.0/html/总览.png",
      "techStack": ["TypeScript", "Obsidian API", "Markdown", "SQLite"],
      "features": ["任务总览", "项目进度", "今日任务"],
      "screenshots": [
        {
          "title": "总览页面",
          "desc": "任务总览和项目进度两个 Tab，一屏掌握全局。",
          "image": "/data/releases/Daily/v1.0.0/html/总览.png"
        }
      ],
      "architecture": [],
      "challenge": "",
      "solution": "",
      "result": "",
      "links": {
        "liveDemo": "",
        "github": "https://github.com/dobkill/obsidian-daily"
      }
    }
  }
}
```

未找到项目：

```json
{
  "code": 404,
  "message": "project not found",
  "data": {
    "slug": "missing"
  }
}
```

## 发布文档

```http
GET /api/v1/products/{product_code}/document?version=latest&channel=stable
```

Query 参数：

| 参数 | 默认值 | 说明 |
| --- | --- | --- |
| `version` | `latest` | 版本；`latest` 按 `release_channels` 解析 |
| `channel` | `stable` | 发布渠道 |

成功响应：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "product_code": "Daily",
    "requested_version": "latest",
    "resolved_version": "v1.0.0",
    "channel": "stable",
    "title": "Daily v1.0.0",
    "published_at": "2026-06-05T09:00:00Z",
    "release_notes_summary": "Daily v1.0.0 首个可用版本。",
    "page": {
      "html_entry_url": "/data/releases/Daily/v1.0.0/html/DailyReleasePage.html",
      "page_data": {}
    }
  }
}
```

## 版本列表

```http
GET /api/v1/products/{product_code}/releases
```

成功响应：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "items": []
  }
}
```

## 更新检查

```http
GET /api/v1/products/{product_code}/check-update/{version}?platform=windows&arch=x64&package_type=portable&data_schema_version=1
```

成功响应包含：

1. `product_code`
2. `current_version`
3. `latest_version`
4. `has_update`
5. `package`
6. `data_upgrade`
7. `data_schema_version`

## 数据转换任务

创建任务：

```http
POST /api/v1/products/{product_code}/convert-tasks?from_version=1&to_version=2
Content-Type: application/json

{"demo": true}
```

创建成功：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "task_id": "demo_1_2_...",
    "status": "pending",
    "cost_time": 5
  }
}
```

查询状态：

```http
GET /api/v1/products/{product_code}/convert-tasks/{task_id}/status
```

查询结果：

```http
GET /api/v1/products/{product_code}/convert-tasks/{task_id}/result
```

任务完成后结果响应包含 `output_file_path` 和 `download_url`。
