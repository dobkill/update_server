# 开发计划与 MVP

## 已完成的设计收敛

1. Web 展示从旧首页聚合模型切换到独立作品集模型。
2. 新增 `portfolio_projects` 表。
3. 新增 `ProjectsAction`。
4. 新增 `/api/v1/projects` 和 `/api/v1/projects/{slug}`。
5. 删除旧 Web 兼容接口 `/api/v1/portfolio-home` 和 `/api/v1/products`。
6. 删除大写 `/api/v1/products/{product_code}/Document` 入口。
7. 前端删除 mock fallback，改为 API 驱动。
8. 管理脚本改为 `portfolio_project` 配置块。

## MVP 验收

1. `cmake --build build` 通过。
2. `cd web && npm run typecheck && npm run build` 通过。
3. `GET /api/v1/projects` 返回站点资料、Featured 项目和真实作品集项目。
4. `GET /api/v1/projects/daily` 返回完整项目详情和真实截图对象。
5. `/` 可以渲染深色作品集首页。
6. `/projects/daily` 可以渲染浅色项目案例页。
7. 更新检查、版本文档和转换任务接口保持可用。
