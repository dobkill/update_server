# personal-project-showcase

C++/Drogon 个人作品展示站，提供站点首页、项目列表、项目详情和项目专属 HTML 展示页。

## 定位

- 单机展示站点，不支持多用户、不提供软件更新服务。
- 后端只读取 `data/database/showcase.db` 和 `data/assets`。
- 内容管理通过 Python CLI 写入数据库和资源目录。

## Build

```bash
cmake -S . -B build
cmake --build build -j

cd web
npm install
npm run build
```

## Run

```bash
./build/software_update_platform
```

默认监听 `http://0.0.0.0:8080`。

## 初始化数据

```bash
python python/admin/admin.py init-db
python python/admin/admin.py apply config/showcase.toml
```

## 接口

```http
GET /api/v1/site
GET /api/v1/home
GET /api/v1/projects
GET /api/v1/projects/{slug}
GET /api/v1/projects/{slug}/pages
GET /assets/{asset_key}
GET /projects/{slug}/pages/{page_slug}
```

更多说明见 `docs/`。
