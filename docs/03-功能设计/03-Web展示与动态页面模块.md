# Web 展示与动态页面模块

## 静态站点

`web` 是 Vue 前端模块。后端服务 `web/dist`，浏览器访问 `/` 返回 `web/dist/index.html`。未知前端路径由 Drogon default handler 回退到同一个 HTML 文件。

`/data/...` 静态资源映射到项目 `data/` 目录：

```http
GET /data/releases/Daily/v1.0.0/vue/image.png
GET /data/packages/Daily/v1.0.0/Daily-v1.0.0.zip
```

## 首页数据

首页请求：

```http
GET /api/v1/portfolio-home
```

前端方法：

```ts
fetchPortfolioHome()
```

首页类型：

1. `SiteProfile`
2. `PortfolioProduct`
3. `RecentUpdate`
4. `FutureDirection`
5. `PortfolioHomeData`

`PortfolioHomeData` 字段：

1. `profile`
2. `recommendations`
3. `products`
4. `recent_updates`
5. `future_directions`

## 首页组件

文件：

```text
web/src/pages/ProductCatalogPage.vue
```

输入：

```text
home: PortfolioHomeData
```

页面结构：

1. Hero 首屏
2. 最近更新
3. 全部作品
4. 未来方向
5. Footer

真实作品来自 `home.products`。准备中和计划中卡片由组件内静态数组生成，不进入数据库。

## 顶部导航

文件：

```text
web/src/shell/AppShell.vue
```

导航项：

1. `作品`
2. `GitHub`

`GitHub` 链接来自 `profile.github_url`，为空时不渲染。

## 版本详情

版本详情请求：

```http
GET /api/v1/products/{product_code}/Document?version=latest&channel=stable
GET /api/v1/products/{product_code}/document?version=latest&channel=stable
```

前端方法：

```ts
fetchReleaseDetail()
```

后端返回结构匹配 `ProductReleaseDetail`。`page.page_data` 驱动区块渲染。

版本页面清单：

```text
data/releases/<product_code>/<version>/vue/page.json
```

Daily 页面清单：

```text
data/releases/Daily/v1.0.0/vue/page.json
```

支持区块：

1. `HeroBlock`
2. `FeatureGridBlock`
3. `RichTextBlock`
4. `ImageTextBlock`
5. `ScreenshotGalleryBlock`
6. `DownloadPanelBlock`
7. `TimelineBlock`
8. `FaqBlock`
9. `FooterCtaBlock`

## mock 数据

首页兜底文件：

```text
web/src/mock/portfolioHome.json
```

该文件只包含 Daily、站点资料、最近更新和未来方向。

版本详情兜底文件：

```text
web/src/mock/releases.json
```

产品列表兼容兜底文件：

```text
web/src/mock/products.json
```

该文件只包含 Daily。

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
