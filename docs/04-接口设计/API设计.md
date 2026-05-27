# Service API 设计文档 v2

## 1. 文档说明

本文档定义 `service` 模块对外暴露的正式 API 协议。

当前版本明确采用以下规则：

1. 版本详情页面接口统一使用 `GET /api/v1/products/{product_code}/Document`。
2. 版本选择通过 Query 参数 `version` 表达，不再使用 `/releases/latest`。
3. `version` 为空、缺省或传入 `latest` 时，统一表示“按渠道解析当前最新版本”。
4. 不兼容任何旧接口、旧字段和旧页面查询方式。

`service` 模块只负责以下职责：

1. 协议入口定义。
2. 请求参数校验。
3. 统一响应格式。
4. HTTP 状态码与业务错误码映射。
5. 返回公共访问地址，不直接暴露 `data/` 真实磁盘路径。

以下职责不放在 `service` 内处理：

1. 版本选择策略。
2. 页面发布流程。
3. 任务调度。
4. 状态切换。
5. 包体、页面清单、图片和转换结果的实际生成逻辑。

当前阶段约束：

1. 所有业务接口统一挂载在 `/api/v1` 下。
2. 当前阶段不做鉴权、不做登录。
3. 当前阶段不实现增量更新、增量兜底和兼容逻辑。
4. 当前阶段版本历史列表默认返回全量，不分页。
5. 下载地址、页面入口地址、转换结果地址必须是公共访问地址。
6. 不直接返回 `data/...` 本地路径。
7. 发布、回滚、下线和清理不提供 HTTP API，只能由管理员在服务器本机人工执行 `python/` 脚本。

---

## 2. 通用约定

### 2.1 参数放置规则

| 参数类型 | 放置位置 | 示例 |
| --- | --- | --- |
| 资源唯一标识 | 默认放 Path；上传型接口可在 Form 中冗余一份做一致性校验 | `/products/{product_code}/releases/{version}` |
| 版本选择、筛选条件 | Query | `?version=latest&channel=stable` |
| 创建任务、提交动作的参数 | Body | `POST /update-check` 请求体 |
| 上传文件 | `multipart/form-data` | `file=<binary>` |
| 请求追踪 ID | Response Body | `request_id` |

### 2.2 公共字段

| 字段 | 说明 | 示例 |
| --- | --- | --- |
| `request_id` | 本次请求唯一标识，用于日志追踪 | `req_01JX8W6N4X8A6H5M9A2Q` |
| `product_code` | 产品唯一编码 | `project-manage` |
| `platform` | 平台 | `windows`、`linux`、`macos` |
| `arch` | 架构 | `x64`、`arm64` |
| `channel` | 发布渠道 | `stable`、`beta` |
| `version` | 版本号 | `2.3.1` |

### 2.3 通用成功响应

```json
{
  "code": 0,
  "message": "ok",
  "request_id": "req_01JX8W6N4X8A6H5M9A2Q",
  "data": {}
}
```

### 2.4 通用失败响应

```json
{
  "code": 4001,
  "message": "invalid parameter: product_code",
  "request_id": "req_01JX8W6N4X8A6H5M9A2Q"
}
```

### 2.5 HTTP 状态码约定

| HTTP 状态码 | 使用场景 |
| --- | --- |
| `200 OK` | 普通查询成功 |
| `202 Accepted` | 异步任务已创建，等待执行 |
| `302 Found` | 下载接口跳转到实际公共文件地址 |
| `400 Bad Request` | 参数错误 |
| `404 Not Found` | 产品、版本、包体、任务不存在 |
| `409 Conflict` | 当前状态不允许操作 |
| `500 Internal Server Error` | 服务端内部错误 |

### 2.6 业务错误码建议

| 业务错误码 | 说明 |
| --- | --- |
| `0` | 成功 |
| `4001` | 参数错误 |
| `4002` | 文件类型不支持 |
| `4003` | 文件大小超限 |
| `4041` | 产品不存在 |
| `4042` | 版本不存在 |
| `4043` | 包体不存在 |
| `4044` | 任务不存在 |
| `5001` | 数据库错误 |
| `5002` | 存储服务错误 |
| `5003` | 任务调度错误 |

---

## 3. API 总览

| 模块 | 方法 | 路径 | 说明 |
| --- | --- | --- | --- |
| 产品 | `GET` | `/api/v1/products` | 获取产品列表 |
| 版本 | `GET` | `/api/v1/products/{product_code}/releases` | 获取产品版本列表 |
| 文档 | `GET` | `/api/v1/products/{product_code}/Document` | 获取前端渲染所需的版本详情文档 |
| 更新 | `POST` | `/api/v1/products/{product_code}/update-check` | 检查客户端是否需要更新 |
| 下载 | `GET` | `/api/v1/products/{product_code}/releases/{version}/download` | 下载指定版本安装包 |
| 转换 | `POST` | `/api/v1/products/{product_code}/convert-tasks` | 创建数据转换任务 |
| 转换 | `GET` | `/api/v1/convert-tasks/{task_id}` | 查询转换任务状态和结果 |
| 运维 | `GET` | `/healthz` | 服务存活检查 |
| 运维 | `GET` | `/readyz` | 服务就绪检查 |

---

## 4. 产品接口

## 4.1 获取产品列表

### 接口

```http
GET /api/v1/products
```

### 用途

用于门户首页展示所有产品入口。

### 请求参数

无。

### 响应字段

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `items` | `array` | 产品列表 |
| `items[].product_code` | `string` | 产品唯一编码 |
| `items[].name` | `string` | 产品名称 |
| `items[].summary` | `string` | 产品简介 |
| `items[].cover_image_url` | `string` | 产品封面图公共地址 |
| `items[].latest_version` | `string` | 产品默认渠道对应的最新版本号 |
| `items[].updated_at` | `string` | 最近更新时间，ISO 8601 格式 |

### 成功响应

```json
{
  "code": 0,
  "message": "ok",
  "request_id": "req_01JX9004S9VN8B2Y9H9Q7Q1V3A",
  "data": {
    "items": [
      {
        "product_code": "project-manage",
        "name": "Project Manage",
        "summary": "项目管理工具，支持任务规划、进度跟踪和数据导出。",
        "cover_image_url": "/public/products/project-manage/cover.png",
        "latest_version": "2.3.1",
        "updated_at": "2026-05-23T10:00:00Z"
      }
    ]
  }
}
```

---

## 5. 版本与文档接口

## 5.1 获取产品版本列表

### 接口

```http
GET /api/v1/products/{product_code}/releases
```

### 用途

获取某个产品的全部版本历史。

当前阶段默认返回全量列表，不分页。

### Path 参数

| 参数 | 必填 | 说明 | 示例 |
| --- | --- | --- | --- |
| `product_code` | 是 | 产品唯一编码 | `project-manage` |

### Query 参数

| 参数 | 必填 | 说明 | 示例 |
| --- | --- | --- | --- |
| `channel` | 否 | 按渠道筛选版本 | `stable` |

### 响应字段

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `product_code` | `string` | 产品唯一编码 |
| `items` | `array` | 版本列表 |
| `items[].version` | `string` | 版本号 |
| `items[].channel` | `string` | 发布渠道 |
| `items[].published_at` | `string` | 发布时间，ISO 8601 格式 |
| `items[].release_notes_summary` | `string` | 发布说明摘要 |

### 成功响应

```json
{
  "code": 0,
  "message": "ok",
  "request_id": "req_01JX901EVT9E7SQ6QZ12W1H2RR",
  "data": {
    "product_code": "project-manage",
    "items": [
      {
        "version": "2.3.1",
        "channel": "stable",
        "published_at": "2026-05-23T10:00:00Z",
        "release_notes_summary": "修复导出卡死问题，优化首页加载速度。"
      },
      {
        "version": "2.3.0",
        "channel": "stable",
        "published_at": "2026-05-10T09:30:00Z",
        "release_notes_summary": "新增批量导出功能。"
      }
    ]
  }
}
```

## 5.2 获取版本详情文档

### 接口

```http
GET /api/v1/products/{product_code}/Document
```

### 用途

获取前端渲染版本详情页所需的正式文档数据。

该接口完全替代旧的 `/releases/latest` 与旧页面查询方式。

### Path 参数

| 参数 | 必填 | 说明 | 示例 |
| --- | --- | --- | --- |
| `product_code` | 是 | 产品唯一编码 | `project-manage` |

### Query 参数

| 参数 | 必填 | 说明 | 示例 |
| --- | --- | --- | --- |
| `version` | 否 | 目标版本号。缺省、空值或 `latest` 都表示按渠道解析当前最新版本 | `latest` |
| `channel` | 否 | 渠道。缺省时按 `products.default_channel` 处理；若指定渠道下不存在对应版本，直接返回 `404` | `stable` |

### 解析规则

1. `version` 缺省、空值或 `latest` 时，按 `product_code + channel` 解析最新版本。
2. `version` 为真实版本号时，必须在指定渠道下存在对应 `Release`。
3. 若显式传入 `channel`，但该渠道下不存在目标版本或不存在最新版本指针，返回 `404`。
4. 响应必须同时返回“请求版本语义”和“实际解析版本”。

### 响应字段

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `product_code` | `string` | 产品唯一编码 |
| `requested_version` | `string` | 请求语义中的版本值，统一规范化为真实版本号或 `latest` |
| `resolved_version` | `string` | 最终解析到的真实版本号 |
| `channel` | `string` | 实际使用的渠道 |
| `title` | `string` | 版本标题 |
| `published_at` | `string` | 发布时间 |
| `release_notes_summary` | `string` | 发布说明摘要 |
| `page` | `object` | 页面文档内容 |
| `page.vue_entry_url` | `string` | 发布后的前端入口脚本地址 |
| `page.page_data` | `object` | 页面区块数据 |

### 交互示例一：请求 latest

```bash
curl -X GET "https://example.com/api/v1/products/project-manage/Document?version=latest&channel=stable"
```

```json
{
  "code": 0,
  "message": "ok",
  "request_id": "req_01JX9032DB09QPT6D00Z2H58A5",
  "data": {
    "product_code": "project-manage",
    "requested_version": "latest",
    "resolved_version": "2.3.1",
    "channel": "stable",
    "title": "Project Manage 2.3.1",
    "published_at": "2026-05-23T10:00:00Z",
    "release_notes_summary": "修复导出卡死问题，优化首页加载速度。",
    "page": {
      "vue_entry_url": "/public/releases/project-manage/2.3.1/entry.js",
      "page_data": {
        "hero": {
          "title": "Project Manage 2.3.1",
          "subtitle": "更稳定的导出体验和更快的首页加载速度。"
        },
        "download": {
          "version": "2.3.1",
          "download_url": "/api/v1/products/project-manage/releases/2.3.1/download?platform=windows&arch=x64&channel=stable"
        }
      }
    }
  }
}
```

### 交互示例二：请求指定版本

```bash
curl -X GET "https://example.com/api/v1/products/project-manage/Document?version=2.3.0&channel=stable"
```

```json
{
  "code": 0,
  "message": "ok",
  "request_id": "req_01JX903B6VCYG56G3GBDJ49W42",
  "data": {
    "product_code": "project-manage",
    "requested_version": "2.3.0",
    "resolved_version": "2.3.0",
    "channel": "stable",
    "title": "Project Manage 2.3.0",
    "published_at": "2026-05-10T09:30:00Z",
    "release_notes_summary": "新增批量导出功能。",
    "page": {
      "vue_entry_url": "/public/releases/project-manage/2.3.0/entry.js",
      "page_data": {
        "download": {
          "version": "2.3.0",
          "download_url": "/api/v1/products/project-manage/releases/2.3.0/download?platform=windows&arch=x64&channel=stable"
        }
      }
    }
  }
}
```

### 版本不存在响应

```json
{
  "code": 4042,
  "message": "release not found: project-manage@2.3.0 stable",
  "request_id": "req_01JX903F9SKG1Z6KJBM0TQJCV0"
}
```

---

## 6. 更新接口

## 6.1 检查更新

### 接口

```http
POST /api/v1/products/{product_code}/update-check
```

### 用途

客户端调用该接口判断当前版本是否需要更新。

该接口只返回全量更新信息，不返回增量包、兜底包和兼容策略。

如果需要更新，服务端直接返回 `download.url`，客户端不需要自行拼接下载接口。

### Path 参数

| 参数 | 必填 | 说明 | 示例 |
| --- | --- | --- | --- |
| `product_code` | 是 | 产品唯一编码 | `project-manage` |

### Body 参数

| 字段 | 必填 | 类型 | 说明 |
| --- | --- | --- | --- |
| `client_version` | 是 | `string` | 当前客户端版本 |
| `target.platform` | 是 | `string` | 客户端平台 |
| `target.arch` | 是 | `string` | 客户端架构 |
| `target.channel` | 是 | `string` | 发布渠道 |

### 响应字段

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `product_code` | `string` | 产品唯一编码 |
| `client_version` | `string` | 当前客户端版本 |
| `need_update` | `boolean` | 是否需要更新 |
| `latest_version` | `string` | 最新版本 |
| `release_notes_summary` | `string` | 发布说明摘要 |
| `download` | `object|null` | 下载信息。无需更新时为 `null` |
| `download.url` | `string` | 服务直接返回的可用下载 URL，当前统一为 API 下载地址 |
| `download.file_name` | `string` | 推荐文件名 |
| `download.size_bytes` | `number` | 文件大小，单位字节 |
| `download.sha256` | `string` | 文件 SHA-256 校验值 |

### 成功响应

```json
{
  "code": 0,
  "message": "ok",
  "request_id": "req_01JX9045ZHZV3JH5A27FSR77E3",
  "data": {
    "product_code": "project-manage",
    "client_version": "2.3.0",
    "need_update": true,
    "latest_version": "2.3.1",
    "release_notes_summary": "修复导出卡死问题，优化首页加载速度。",
    "download": {
      "url": "/api/v1/products/project-manage/releases/2.3.1/download?platform=windows&arch=x64&channel=stable",
      "file_name": "project-manage-2.3.1-windows-x64.exe",
      "size_bytes": 158334976,
      "sha256": "1c7f2d7a8d6f4a6a7d91a3fb58f8c2a60f7dbf7e4bd7f65d89c0ef2ad9d2f1aa"
    }
  }
}
```

---

## 7. 下载接口

## 7.1 下载指定版本安装包

### 接口

```http
GET /api/v1/products/{product_code}/releases/{version}/download
```

### 用途

下载某个产品、某个版本、某个平台对应的安装包。

当前阶段只提供全量安装包下载。

### Path 参数

| 参数 | 必填 | 说明 | 示例 |
| --- | --- | --- | --- |
| `product_code` | 是 | 产品唯一编码 | `project-manage` |
| `version` | 是 | 版本号 | `2.3.1` |

### Query 参数

| 参数 | 必填 | 说明 | 示例 |
| --- | --- | --- | --- |
| `platform` | 是 | 包所属平台 | `windows` |
| `arch` | 是 | 包所属架构 | `x64` |
| `channel` | 是 | 包所属渠道 | `stable` |

### 响应方式

服务端返回 `302 Found`，跳转到公共文件地址。

### 交互示例

```bash
curl -I "https://example.com/api/v1/products/project-manage/releases/2.3.1/download?platform=windows&arch=x64&channel=stable"
```

```http
HTTP/1.1 302 Found
Location: /public/packages/project-manage/2.3.1/stable/windows/x64/project-manage-2.3.1-windows-x64.exe
Content-Type: text/plain
```

---

## 8. 数据转换接口

## 8.1 创建转换任务

### 接口

```http
POST /api/v1/products/{product_code}/convert-tasks
```

### 用途

上传文件并创建异步数据转换任务。

### Path 参数

| 参数 | 必填 | 说明 | 示例 |
| --- | --- | --- | --- |
| `product_code` | 是 | 产品唯一编码 | `project-manage` |

### 请求方式

```http
Content-Type: multipart/form-data
```

### Form 字段

| 字段 | 必填 | 说明 | 示例 |
| --- | --- | --- | --- |
| `product_code` | 是 | 产品唯一编码，必须与 Path 中的 `product_code` 一致 | `project-manage` |
| `source_version` | 是 | 上传数据当前所属版本 | `2.3.0` |
| `target_version` | 是 | 转换目标版本。支持真实版本号或 `latest` | `latest` |
| `file` | 是 | 待转换文件 | `backup.zip` |

### 成功响应

```json
{
  "code": 0,
  "message": "accepted",
  "request_id": "req_01JX906MD4MZJC8YYT8JC7Z9EM",
  "data": {
    "task_id": "task_01JX8Y0BTRM5Q7AS",
    "product_code": "project-manage",
    "status": "Pending",
    "estimated_wait_seconds": 45,
    "queue_position": 2,
    "expire_at": "2026-05-24T12:00:00Z"
  }
}
```

## 8.2 查询转换任务

### 接口

```http
GET /api/v1/convert-tasks/{task_id}
```

### 用途

查询转换任务状态。

如果任务已成功，响应中直接返回转换结果下载信息。

### 成功响应

```json
{
  "code": 0,
  "message": "ok",
  "request_id": "req_01JX908PK3NN9E06CD0RNBYEC2",
  "data": {
    "task_id": "task_01JX8Y0BTRM5Q7AS",
    "product_code": "project-manage",
    "status": "Success",
    "progress": 100,
    "error_message": null,
    "result": {
      "download_url": "/public/convert-results/task_01JX8Y0BTRM5Q7AS/result.zip",
      "file_name": "project-manage-converted.zip",
      "expire_at": "2026-05-24T12:00:00Z"
    }
  }
}
```

---

## 9. 运维接口

## 9.1 服务存活检查

```http
GET /healthz
```

## 9.2 服务就绪检查

```http
GET /readyz
```

---

## 10. 前端推荐调用流程

## 10.1 客户端检查更新流程

1. 客户端启动后调用 `POST /api/v1/products/{product_code}/update-check`。
2. 如果 `need_update = false`，不处理。
3. 如果 `need_update = true`，直接使用响应中的 `download.url` 下载全量安装包。

## 10.2 Web 版本详情页渲染流程

1. 首页进入产品详情时，调用：

```http
GET /api/v1/products/{product_code}/Document?version=latest&channel=stable
```

2. 查看指定版本时，调用：

```http
GET /api/v1/products/{product_code}/Document?version=2.3.0&channel=stable
```

3. 前端读取 `requested_version / resolved_version / page.vue_entry_url / page.page_data`。
4. 前端使用 `page.page_data` 渲染页面，并直接使用 `download.download_url` 作为下载入口。
5. 如果未显式传入 `channel`，服务端按产品的 `default_channel` 解析。

## 10.3 数据转换流程

1. 用户上传文件，前端创建转换任务：`POST /api/v1/products/{product_code}/convert-tasks`。
2. 服务端返回 `task_id`。
3. 前端轮询 `GET /api/v1/convert-tasks/{task_id}`。
4. 若任务成功，直接使用 `result.download_url` 下载结果文件。
