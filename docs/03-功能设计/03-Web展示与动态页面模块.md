# Web 展示与动态页面模块

## 定位

`web` 是个人软件作品集站点，展示自研软件、工具、系统和实验项目。Web 展示模型独立于软件更新模型，数据来自 `portfolio_projects`，不再复用 `products/releases` 拼装首页。

## 静态站点

`web` 是 Vite + TypeScript 前端模块。后端服务 `web/dist`，浏览器访问 `/` 返回 `web/dist/index.html`。未知前端路径由 Drogon default handler 回退到同一个 HTML 文件。

`/data/...` 静态资源映射到项目 `data/` 目录：

```http
GET /data/releases/Daily/v1.0.0/html/总览.png
GET /data/packages/Daily/v1.0.0/Daily-v1.0.0.zip
```

## 页面

| 路径 | 用途 | 数据接口 |
| --- | --- | --- |
| `/` | 作品集首页，展示 Hero、Featured Projects、All Projects | `GET /api/v1/projects` |
| `/projects/{slug}` | 项目案例详情页 | `GET /api/v1/projects/{slug}` |

## 首页数据

首页请求：

```http
GET /api/v1/projects
```

前端方法：

```ts
fetchProjects()
```

首页类型：

1. `SiteProfile`
2. `PortfolioProject`
3. `ProjectListData`

`ProjectListData` 字段：

1. `profile`
2. `featured`
3. `items`
4. `filters`

## 首页渲染

文件：

```text
web/src/runtime/htmlRenderer.ts
```

输入：

```text
renderCatalogPage(data, visibleProjects, selectedFilter, searchTerm)
```

页面结构：

1. 紧凑 Hero
2. Featured Project
3. All Projects
4. Footer

交互：

1. 筛选标签按 `category` 过滤。
2. 搜索框按项目名、简介和技术栈过滤。
3. 项目卡片跳转到 `/projects/{slug}`。

## 项目详情

详情请求：

```http
GET /api/v1/projects/{slug}
```

前端方法：

```ts
fetchProjectDetail(slug)
```

详情页结构：

1. 浅色产品案例 Hero
2. 项目信息条
3. Overview + Core Features
4. Screenshots 真实截图
5. Tech Stack
6. Architecture
7. Challenge / Solution / Result

## 顶部导航

导航数据来自 `site_profile`：

1. `ownerName`
2. `githubUrl`
3. `email`
4. `resumeUrl`

## 构建

```bash
cd web
npm run typecheck
npm run build
```

构建产物写入：

```text
web/dist
```
