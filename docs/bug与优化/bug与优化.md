# Web 首页作品集优化执行记录

参考图：`docs/bug与优化/image1.png`

## 实现结果

首页已改为个人作品集首页。页面只把 `Daily` 渲染为真实已发布作品，准备中和计划中卡片由前端静态生成，不写入数据库。

首页结构：

1. 顶部导航
2. Hero 首屏
3. 最近更新
4. 全部作品
5. 未来方向
6. Footer

## 后端接口

首页使用聚合接口：

```http
GET /api/v1/portfolio-home
```

Action：

```cpp
ProductsAction::GetPortfolioHome()
```

响应结构：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "profile": {},
    "recommendations": [],
    "products": [],
    "recent_updates": [],
    "future_directions": []
  }
}
```

兼容接口：

```http
GET /api/v1/products
```

该接口由 `ProductsAction::ListProducts()` 返回真实产品列表。

## 数据来源

| 页面数据 | 来源 |
| --- | --- |
| 站点名称、副标题、站点 GitHub、邮箱 | `site_profile` |
| 真实作品 | `products` |
| 产品 GitHub | `products.github_url` |
| 主推项目 | `recommendations` 关联 `products` |
| 最近更新 | `products + releases + release_channels` |
| 未来方向 | `future_directions` |
| 准备中和计划中卡片 | `web/src/pages/ProductCatalogPage.vue` 静态数组 |

## 数据库结构

`migrations/002.init.sql` 包含以下首页相关结构：

1. `schema_migrations`
2. `product_careers`
3. `recommendations`
4. `site_profile`
5. `future_directions`
6. `products.github_url`

`recommendations` 和 `future_directions` 都包含 `sort_order`。

`Storage_SQL::ensurePortfolioSchema()` 在服务启动时补齐首页相关表和字段：

1. `schema_migrations`
2. `products.github_url`
3. `product_careers`
4. `recommendations.sort_order`
5. `site_profile`
6. `future_directions.sort_order`

## Python 脚本

结构迁移：

```bash
python python/admin/migrate_sqlite.py --db ./data/database/update_platform.db
```

导出 SQLite：

```bash
python python/admin/export_sqlite_json.py \
  --db ./data/database/update_platform.db \
  --out ./data/backups/update_platform.json
```

导入 SQLite：

```bash
python python/admin/import_sqlite_json.py \
  --db ./data/database/update_platform.db \
  --in ./data/backups/update_platform.json \
  --mode upsert
```

日常入库入口：

```bash
ADMIN_CONFIG=config/daily_release.toml ADMIN_DRY_RUN=0 python python/admin/admin.py
```

`config/daily_release.toml` 包含：

1. `product`
2. `site_profile`
3. `future_direction`
4. `recommendation`
5. `release`
6. `release.asset`
7. `release.channel`

## Web 实现

首页请求：

```ts
fetchPortfolioHome()
```

前端类型：

1. `SiteProfile`
2. `PortfolioProduct`
3. `RecentUpdate`
4. `FutureDirection`
5. `PortfolioHomeData`

首页组件：

```text
web/src/pages/ProductCatalogPage.vue
```

组件输入：

```text
home: PortfolioHomeData
```

首页保留：

1. `Daily` 主推项目卡片
2. `Daily` 最近更新卡片
3. 垂直更新时间线
4. `Daily` 真实作品卡片
5. 三张准备中/计划中占位卡
6. 三张未来方向卡片
7. 站点 GitHub 和邮件链接

首页输出范围：

1. Hero 首屏
2. 最近更新
3. 全部作品
4. 未来方向
5. Footer

mock 兜底文件：

```text
web/src/mock/portfolioHome.json
```

该文件只包含 Daily、站点资料、最近更新和未来方向。

## 验收项

数据库：

1. `products` 存在 `github_url`。
2. `site_profile` 存在 `id = 1`。
3. `future_directions` 存在 active 数据。
4. `recommendations` 只引用真实产品。
5. 迁移脚本重复执行不破坏已有数据。

接口：

1. `GET /api/v1/portfolio-home` 返回 `profile` 对象。
2. `GET /api/v1/portfolio-home` 返回 `recommendations`、`products`、`recent_updates`、`future_directions` 数组。
3. `GET /api/v1/products` 返回 `data.items`。
4. 所有首页字段使用 snake_case。

Web：

1. 顶部导航只显示 `作品` 和已配置的 `GitHub`。
2. Hero 左侧标题为 `个人作品集`。
3. Hero 右侧展示 Daily 主推项目。
4. 最近更新展示 Daily 和时间线。
5. 全部作品中只有 Daily 是真实已发布作品。
6. Footer 只显示版权、已配置的 GitHub、已配置的邮件。

构建：

```bash
cmake -S . -B build
cmake --build build -j2
cd web && npm run typecheck && npm run build
python -m py_compile python/admin/export_sqlite_json.py python/admin/import_sqlite_json.py python/admin/migrate_sqlite.py
```
