# Action 分工详情

Action 层负责把 Storage 查询结果组装成 API 响应，不直接处理 HTTP 路由注册。

## ProjectsAction

文件：

```text
include/action/ProjectsAction.h
src/action/ProjectsAction/ListProjects.cc
src/action/ProjectsAction/GetProjectDetail.cc
```

职责：

1. `ListProjects()` 返回 `/api/v1/projects`。
2. `GetProjectDetail(slug)` 返回 `/api/v1/projects/{slug}`。
3. 响应中统一包含 `profile`，让首页和详情页都能独立渲染导航和站点资料。

依赖 Storage 方法：

1. `getSiteProfile()`
2. `getPortfolioProjectList()`
3. `getFeaturedPortfolioProjects()`
4. `getPortfolioProjectDetail(slug)`

## DocumentAction

文件：

```text
include/action/DocumentAction.h
src/action/DocumentAction/getDocumnet.cc
src/action/DocumentAction/getReleases.cc
```

职责：

1. `GetDocument(product_code, version, channel)` 返回软件发布文档。
2. `GetListReleases(product_code)` 返回版本列表。

接口：

```http
GET /api/v1/products/{product_code}/document
GET /api/v1/products/{product_code}/releases
```

`/document` 只保留小写路径。

## CheckUpdateAction

文件：

```text
include/action/CheckUpdateAction.h
src/action/CheckUpdataAction/checkupdate.cc
```

职责：

1. 根据产品、客户端版本、平台、架构、包类型和渠道判断是否有更新。
2. 返回全量包或增量包信息。
3. 返回数据结构升级信息。

接口：

```http
GET /api/v1/products/{product_code}/check-update/{version}
```

## ConvertDataTaskAction

文件：

```text
include/action/ConvertDataTaskAction.h
src/action/ConvertDataTaskAction/createTask.cc
src/action/ConvertDataTaskAction/getTaskStatus.cc
src/action/ConvertDataTaskAction/getTaskResult.cc
src/action/ConvertDataTaskAction/runt_task.cc
```

职责：

1. 创建数据转换任务。
2. 执行转换脚本。
3. 查询任务状态。
4. 查询任务结果。

接口：

```http
POST /api/v1/products/{product_code}/convert-tasks
GET /api/v1/products/{product_code}/convert-tasks/{task_id}/status
GET /api/v1/products/{product_code}/convert-tasks/{task_id}/result
```
