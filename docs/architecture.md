# 系统架构

## 定位

个人作品展示站。C++/Drogon 后端 + SQLite + Vite 前端，单机部署；内容由 Python CLI 写入数据库与资源目录，后端只读。

## 运行时组件

```text
Browser
  -> Drogon (src/bin/main.cpp)
     -> web/dist                首页与项目详情 SPA
     -> /api/v1/*               展示 API（ApiRoutes）
     -> /assets/{asset_key}     受控静态资源
     -> /projects/{slug}/pages/{page_slug}  项目专属 HTML 页
```

Drogon 监听 `0.0.0.0:8080`（见 `config/drogon_config.json`）。非 `/api/`、`/assets/` 的请求回退到 `web/dist/index.html`，由前端路由处理。

## 源码模块

| 路径 | 命名空间 / 目标 | 职责 |
| --- | --- | --- |
| `src/bin/main.cpp` | — | 入口：定位 `config/drogon_config.json` 与 `web/dist`，初始化 `Config::AppConfig` 与 `Router::ApiRoutes`，注册默认处理器并 `app().run()` |
| `src/config` | `Config::AppConfig`（`showcase_config`） | 读取 `config/app.json`，提供数据库名、资源目录、SQL 文件、SQLite 忙等待超时等参数 |
| `src/storage` | `Storage::Storage_SQL`（`showcase_storage`） | 打开 SQLite、执行迁移、运行展示查询、解析资源与页面路径 |
| `src/service` | `Router::ApiRoutes`（`showcase_service`） | 注册 HTTP 路由，统一封装成功/错误 JSON 响应 |
| `python/admin` | — | 内容管理 CLI，直接写 SQLite 与 `data/assets` |
| `python/lib` | — | 配置读取、SQLite 连接、TOML 加载 |
| `web` | — | Vite + TypeScript SPA，运行时调用 `/api/v1/*` |

依赖关系：`bin -> service -> storage -> config`，均链接 Drogon、nlohmann_json；storage 链接 sqlite3。

## 数据边界

```text
site_profile      站点资料（单行）
projects          展示项目
project_pages     项目专属 HTML 页（html_asset_key 指向资源）
project_assets    资源登记（asset_key 唯一）
```

## 资源访问

- 数据库只保存 `asset_key`，不保存磁盘绝对路径。
- 资源文件位于 `data/assets/`（`config/app.json` 的 `assets_dir`）。
- 前端通过 `/assets/{asset_key}` 访问；后端 `Storage_SQL::resolveAssetPath` 校验路径为安全相对路径（不含 `..`、`.`、非绝对），再拼接 `assets_dir` 返回文件。
- 项目专属 HTML 页通过 `/projects/{slug}/pages/{page_slug}` 访问；后端按 `(slug, page_slug)` 查 `project_pages.html_asset_key`，再走相同的资源解析逻辑返回 HTML。

## 配置文件

| 文件 | 作用 |
| --- | --- |
| `config/app.json` | 应用配置（数据库名、目录、SQL 文件、超时） |
| `config/drogon_config.json` | Drogon 运行配置（线程数、上传路径、监听端口等） |
