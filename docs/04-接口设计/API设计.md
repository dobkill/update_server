# Service API 设计文档 v2

## 1. 文档说明

本文档以当前 `src/service` 路由实现为准，描述 `service` 模块已经注册的 HTTP API。

当前代码中的接口统一挂载在 `/api/v1` 下，响应体统一为 JSON。

注意：

1. 文档中的路径、方法、参数名、大小写均以代码实现为准。
2. 当前代码里版本详情接口路径为小写 `document`，不是 `Document`。
3. 当前代码里更新检查接口是 `GET`，并通过 Path 传入版本号。
4. 当前代码里转换任务已经拆成 3 个接口：创建、查状态、查结果。
5. 当前代码里下载接口返回 JSON，不是 `302` 跳转。

---

## 2. 通用响应结构

当前 action 的返回值都会被 `service` 直接序列化为 JSON 响应体，整体结构约定如下：

```json
{
  "code": 0,
  "message": "ok",
  "data": {}
}
```

常见字段：

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `code` | `number` | 业务状态码，`0` 表示成功 |
| `message` | `string` | 结果描述 |
| `data` | `object` | 具体返回数据 |

---

## 3. API 总览

| 模块 | 方法 | 路径 | 说明 |
| --- | --- | --- | --- |
| 产品 | `GET` | `/api/v1/products` | 获取产品列表 |
| 更新 | `GET` | `/api/v1/products/{product_code}/check-update/{version}` | 检查指定产品、指定版本的更新信息 |
| 转换 | `POST` | `/api/v1/products/{product_code}/convert-tasks` | 创建转换任务 |
| 转换 | `GET` | `/api/v1/products/{product_code}/convert-tasks/{task_id}/status` | 查询转换任务状态 |
| 转换 | `GET` | `/api/v1/products/{product_code}/convert-tasks/{task_id}/result` | 查询转换任务结果 |
| 下载 | `GET` | `/api/v1/products/{product_code}/releases/{version}/download` | 获取下载信息 |
| 文档 | `GET` | `/api/v1/products/{product_code}/document` | 获取版本文档数据 |

---

## 4. 产品接口

### 4.1 获取产品列表

```http
GET /api/v1/products
```

请求参数：无。

成功响应示例：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "items": []
  }
}
```

---

## 5. 更新接口

### 5.1 检查更新

```http
GET /api/v1/products/{product_code}/check-update/{version}
```

Path 参数：

| 参数 | 必填 | 说明 |
| --- | --- | --- |
| `product_code` | 是 | 产品编码 |
| `version` | 是 | 当前版本号 |

成功响应示例：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "product_code": "project-manage",
    "version": "1.0.0",
    "need_update": false
  }
}
```

---

## 6. 转换任务接口

### 6.1 创建转换任务

```http
POST /api/v1/products/{product_code}/convert-tasks?from_version={from_version}&to_version={to_version}
```

Path 参数：

| 参数 | 必填 | 说明 |
| --- | --- | --- |
| `product_code` | 是 | 产品编码 |

Query 参数：

| 参数 | 必填 | 说明 |
| --- | --- | --- |
| `from_version` | 否 | 源版本号 |
| `to_version` | 否 | 目标版本号 |

说明：

1. 当前实现从 Query 读取 `from_version` 和 `to_version`。
2. 当前实现没有读取 Body，也没有处理文件上传。

成功响应示例：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "task_id": "task_1",
    "product_code": "project-manage",
    "from_version": "1.0.0",
    "to_version": "2.0.0",
    "status": "success",
    "progress": 100,
    "result": {
      "download_url": "",
      "file_name": "",
      "expire_at": ""
    }
  }
}
```

### 6.2 查询转换任务状态

```http
GET /api/v1/products/{product_code}/convert-tasks/{task_id}/status
```

Path 参数：

| 参数 | 必填 | 说明 |
| --- | --- | --- |
| `product_code` | 是 | 产品编码 |
| `task_id` | 是 | 任务 ID |

说明：

1. 当前代码虽然包含 `product_code` 路径段，但查询时实际只使用 `task_id`。
2. 当前返回体包含状态和进度信息。

成功响应示例：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "task_id": "task_1",
    "product_code": "project-manage",
    "from_version": "1.0.0",
    "to_version": "2.0.0",
    "status": "success",
    "progress": 100
  }
}
```

### 6.3 查询转换任务结果

```http
GET /api/v1/products/{product_code}/convert-tasks/{task_id}/result
```

Path 参数：

| 参数 | 必填 | 说明 |
| --- | --- | --- |
| `product_code` | 是 | 产品编码 |
| `task_id` | 是 | 任务 ID |

说明：

1. 当前代码虽然包含 `product_code` 路径段，但查询时实际只使用 `task_id`。
2. 当前返回体中的结果数据位于 `data.result`。

成功响应示例：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "task_id": "task_1",
    "product_code": "project-manage",
    "status": "success",
    "result": {
      "download_url": "",
      "file_name": "",
      "expire_at": ""
    }
  }
}
```

任务不存在示例：

```json
{
  "code": 4044,
  "message": "task not found",
  "data": {
    "task_id": "task_999"
  }
}
```

---

## 7. 下载接口

### 7.1 获取下载信息

```http
GET /api/v1/products/{product_code}/releases/{version}/download?mode={mode}&from_version={from_version}
```

Path 参数：

| 参数 | 必填 | 说明 |
| --- | --- | --- |
| `product_code` | 是 | 产品编码 |
| `version` | 是 | 目标版本号 |

Query 参数：

| 参数 | 必填 | 说明 |
| --- | --- | --- |
| `mode` | 否 | 下载模式 |
| `from_version` | 否 | 来源版本号 |

说明：

1. 当前实现读取的 Query 参数是 `mode` 和 `from_version`。
2. 当前实现不会读取 `platform`、`arch`、`channel`。
3. 当前实现返回 JSON，不是跳转响应。

成功响应示例：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "product_code": "project-manage",
    "mode": "full",
    "from_version": "1.0.0",
    "version": "2.0.0"
  }
}
```

---

## 8. 文档接口

### 8.1 获取版本文档

```http
GET /api/v1/products/{product_code}/document?version={version}
```

Path 参数：

| 参数 | 必填 | 说明 |
| --- | --- | --- |
| `product_code` | 是 | 产品编码 |

Query 参数：

| 参数 | 必填 | 说明 |
| --- | --- | --- |
| `version` | 否 | 版本号；为空时由 action 自行处理 |

说明：

1. 当前实现路径为小写 `document`。
2. 当前实现只读取 `version`，不读取 `channel`。

成功响应示例：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "product_code": "project-manage",
    "requested_version": "latest",
    "resolved_version": "latest",
    "channel": "stable",
    "title": "project-manage latest",
    "published_at": "",
    "release_notes_summary": "",
    "page": {
      "vue_entry_url": "",
      "page_data": {}
    }
  }
}
```

---

## 9. 一致性说明

本次已按 `src/service` 当前代码完成以下对齐：

1. 新增了 `status` 和 `result` 两个转换任务 API。
2. 将文档接口路径修正为小写 `document`。
3. 将更新检查接口修正为 `GET + Path version`。
4. 将下载接口参数修正为 `mode`、`from_version`。
5. 将创建转换任务的参数来源修正为 Query。
6. 去掉了当前代码中未注册的旧接口说明，避免文档继续与实现偏离。
