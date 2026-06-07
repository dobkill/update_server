# API 设计

## 响应结构

接口统一挂载在 `/api/v1` 下，响应体为 JSON。

```json
{
  "code": 0,
  "message": "ok",
  "data": {}
}
```

`code = 0` 表示业务成功。业务错误使用 JSON `code` 表达。

## API 总览

| 方法 | 路径 | Action |
| --- | --- | --- |
| `GET` | `/api/v1/portfolio-home` | `ProductsAction::GetPortfolioHome()` |
| `GET` | `/api/v1/products` | `ProductsAction::ListProducts()` |
| `GET` | `/api/v1/products/{product_code}/document` | `DocumentAction::GetDocument()` |
| `GET` | `/api/v1/products/{product_code}/Document` | `DocumentAction::GetDocument()` |
| `GET` | `/api/v1/products/{product_code}/releases` | `DocumentAction::GetListReleases()` |
| `GET` | `/api/v1/products/{product_code}/check-update/{version}` | `CheckUpdateAction::CheckUpdate()` |
| `POST` | `/api/v1/products/{product_code}/convert-tasks` | `ConvertDataTaskAction::createTask()` |
| `GET` | `/api/v1/products/{product_code}/convert-tasks/{task_id}/status` | `ConvertDataTaskAction::getTaskStatus()` |
| `GET` | `/api/v1/products/{product_code}/convert-tasks/{task_id}/result` | `ConvertDataTaskAction::getTaskResult()` |

## 首页聚合

```http
GET /api/v1/portfolio-home
```

成功响应：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "profile": {
      "site_name": "YXX Works",
      "subtitle": "产品、插件与创作实验",
      "github_url": "",
      "email": ""
    },
    "recommendations": [],
    "products": [],
    "recent_updates": [],
    "future_directions": []
  }
}
```

`data.profile` 字段：

1. `site_name`
2. `subtitle`
3. `github_url`
4. `email`

`data.recommendations` 和 `data.products` 字段：

1. `product_code`
2. `name`
3. `summary`
4. `cover_image_url`
5. `github_url`
6. `latest_version`
7. `published_at`
8. `updated_at`
9. `tag`
10. `status_label`

`data.recent_updates` 字段：

1. `product_code`
2. `name`
3. `version`
4. `published_at`
5. `title`
6. `description`

`data.future_directions` 字段：

1. `title`
2. `comment`
3. `icon_path`

## 产品列表

```http
GET /api/v1/products
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

`items` 字段：

1. `product_code`
2. `name`
3. `summary`
4. `cover_image_url`
5. `github_url`
6. `latest_version`
7. `updated_at`
8. `tag`
9. `status_label`

## 版本详情文档

```http
GET /api/v1/products/{product_code}/document?version=latest&channel=stable
GET /api/v1/products/{product_code}/Document?version=latest&channel=stable
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
      "vue_entry_url": "/data/releases/Daily/v1.0.0/vue/DailyReleasePage.html",
      "page_data": {}
    }
  }
}
```

未找到版本：

```json
{
  "code": 404,
  "message": "release document not found",
  "data": {
    "product_code": "Daily",
    "requested_version": "latest"
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

Path 参数：

| 参数 | 说明 |
| --- | --- |
| `product_code` | 产品编码 |
| `version` | 客户端软件版本 |

Query 参数：

| 参数 | 默认值 | 说明 |
| --- | --- | --- |
| `platform` | `windows` | 平台 |
| `arch` | `x64` | 架构 |
| `package_type` | `portable` | 包类型 |
| `data_schema_version` | 空 | 客户端数据结构版本 |

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

## 静态资源

`/data/...` 映射到项目 `data/` 目录下的真实文件，并拒绝路径穿越。

```http
GET /data/releases/Daily/v1.0.0/vue/image.png
GET /data/packages/Daily/v1.0.0/Daily-v1.0.0.zip
```
