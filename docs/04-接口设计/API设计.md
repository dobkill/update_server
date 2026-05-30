# API 设计

## 通用说明

当前 HTTP 服务由 `src/service/ApiRoutes.cc` 及各 `register_*_routes.cc` 注册。接口统一挂载在 `/api/v1` 下，响应体为 JSON。

通用响应结构：

```json
{
  "code": 0,
  "message": "ok",
  "data": {}
}
```

说明：

1. `code = 0` 表示业务成功。
2. 当前业务错误通常仍返回 HTTP 200，并在 JSON 中返回业务 `code`，例如 `404`。
3. 数据库为空时，产品列表返回空数组，详情和更新检查返回业务 `404`。

## API 总览

| 方法 | 路径 | Action |
| --- | --- | --- |
| `GET` | `/api/v1/products` | `ProductsAction::ListProducts()` |
| `GET` | `/api/v1/products/{product_code}/document` | `DocumentAction::GetDocument()` |
| `GET` | `/api/v1/products/{product_code}/Document` | `DocumentAction::GetDocument()`，兼容当前 web 构建产物 |
| `GET` | `/api/v1/products/{product_code}/releases` | `DocumentAction::GetListReleases()` |
| `GET` | `/api/v1/products/{product_code}/check-update/{version}` | `CheckUpdateAction::CheckUpdate()` |
| `POST` | `/api/v1/products/{product_code}/convert-tasks` | `ConvertDataTaskAction::createTask()` |
| `GET` | `/api/v1/products/{product_code}/convert-tasks/{task_id}/status` | `ConvertDataTaskAction::getTaskStatus()` |
| `GET` | `/api/v1/products/{product_code}/convert-tasks/{task_id}/result` | `ConvertDataTaskAction::getTaskResult()` |

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

`items` 中的字段来自 `products` 和当前 stable latest 指针：

1. `product_code`
2. `name`
3. `summary`
4. `cover_image_url`
5. `latest_version`
6. `updated_at`

## 版本详情文档

```http
GET /api/v1/products/{product_code}/document?version=latest&channel=stable
GET /api/v1/products/{product_code}/Document?version=latest&channel=stable
```

Query 参数：

| 参数 | 默认值 | 说明 |
| --- | --- | --- |
| `version` | `latest` | 指定版本；`latest` 会按 `release_channels` 解析 |
| `channel` | `stable` | 发布渠道 |

成功响应 `data` 结构兼容 `web/src/runtime/api.ts` 中的 `ProductReleaseDetail`：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "product_code": "demo",
    "requested_version": "latest",
    "resolved_version": "1.0.0",
    "channel": "stable",
    "title": "Demo 1.0.0",
    "published_at": "2026-05-30 00:00:00",
    "release_notes_summary": "release note",
    "page": {
      "vue_entry_url": "",
      "page_data": {}
    }
  }
}
```

未找到版本时：

```json
{
  "code": 404,
  "message": "release document not found",
  "data": {
    "product_code": "demo",
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
| `version` | 客户端当前软件版本 |

Query 参数：

| 参数 | 默认值 | 说明 |
| --- | --- | --- |
| `platform` | `windows` | 平台 |
| `arch` | `x64` | 架构 |
| `package_type` | `portable` | 包类型 |
| `data_schema_version` | 空 | 客户端当前数据结构版本，用于判断是否需要数据转换 |

成功响应示例：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "product_code": "demo",
    "current_version": "1.0.0",
    "latest_version": "1.1.0",
    "has_update": true,
    "package": {
      "type": "full",
      "url": "./data/packages/demo/1.1.0/demo.zip",
      "file_size": 1024,
      "sha256": "..."
    },
    "data_upgrade": false,
    "data_schema_version": "1"
  }
}
```

## 数据转换任务

创建任务：

```http
POST /api/v1/products/{product_code}/convert-tasks?from_version=1&to_version=2
Content-Type: application/json

{"demo": true}
```

说明：

1. 请求 body 会保存到 `config/app.json` 的 `upload_task_dir`。
2. `from_version` 和 `to_version` 当前对应 `convert_rules` 的 `source_data_schema_version` 与 `target_data_schema_version`。
3. 创建成功后任务进入 `pending`，worker 会调用 `python/convert/convert_data.py`。

成功响应：

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
