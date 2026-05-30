# 开发计划与 MVP

## 已完成

1. CMake 可构建 `software_update_platform`。
2. `main.cpp` 已接入配置、静态 web、API 路由和 SPA fallback。
3. `service` 已注册当前核心 API。
4. `action` 已形成产品、文档、更新检查、数据转换任务四类入口。
5. `storage` 已接入 sqlite3 prepared statement，并能执行 `migrations/002.init.sql`。
6. `ConvertDataTaskAction` 已能创建 worker、入队任务、调用转换脚本并回写状态。
7. `config/drogon_config.json` 已移除未使用的 Drogon `db_clients`，SQLite 统一由 `Storage_SQL` 管理。

## 当前 MVP 验证标准

1. `cmake --build build -j 2` 成功。
2. `./build/software_update_platform` 可启动。
3. `GET /api/v1/products` 返回 `{"code":0,"data":{"items":[]}}` 或实际产品列表。
4. `GET /` 返回 `web/dist/index.html`。
5. 空数据库下详情/更新检查返回业务 `404`，服务不崩溃。

## 下一阶段建议

1. 补充 seed/demo 数据，方便端到端演示产品列表、版本详情和更新检查。
2. 将 `Storage_SQL` 继续拆分为 Product/Release/Task repository，降低单类复杂度。
3. 为转换任务补 multipart 上传、任务恢复和失败原因记录。
4. 增加下载接口或签名 URL，避免直接暴露服务器文件路径。
5. 增加自动化测试：迁移 SQL、storage 查询、Action 返回结构、核心 API smoke test。
6. 逐步修正历史拼写目录，例如 `CheckUpdataAction`，但重命名前需确认构建脚本和引用路径。
