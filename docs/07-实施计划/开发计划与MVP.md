# 开发计划与 MVP

## 已完成

1. CMake 构建 `software_update_platform`。
2. Drogon 启动、静态站点、API 路由、SPA fallback。
3. 首页聚合接口 `/api/v1/portfolio-home`。
4. 产品列表兼容接口 `/api/v1/products`。
5. 版本详情、版本列表、更新检查、数据转换任务接口。
6. SQLite 初始化和首页结构补齐。
7. Web 首页只展示 Daily 真实作品。
8. Web 首页准备中和计划中卡片由前端静态生成。
9. Python 管理入口写入产品、站点资料、未来方向、推荐项目、版本、包、渠道。
10. Python 数据库迁移、导出、导入脚本。

## 验证标准

1. `cmake -S . -B build` 成功。
2. `cmake --build build -j2` 成功。
3. `cd web && npm run typecheck` 成功。
4. `cd web && npm run build` 成功。
5. `python -m py_compile` 覆盖 `python/admin/*.py` 成功。
6. `GET /api/v1/portfolio-home` 返回 `profile`、`recommendations`、`products`、`recent_updates`、`future_directions`。
7. `GET /api/v1/products` 返回 `data.items`。
8. 首页不渲染非真实上线作品。
