# Web 展示与动态页面模块

## 当前状态

`web` 是 Vue 前端模块，当前后端直接服务 `web/dist`。浏览器访问 `/` 会返回 `web/dist/index.html`，未知前端路径由 Drogon default handler 回退到同一个 HTML 文件。

后端同时支持 `/data/...` 静态资源访问，用于发布页截图、图标、插件包等文件。例如：

```http
GET /data/releases/Daily/v1.0.0/vue/image.png
GET /data/packages/Daily/v1.0.0/Daily-v1.0.0.zip
```

## 页面数据接口

产品列表：

```http
GET /api/v1/products
```

版本详情：

```http
GET /api/v1/products/{product_code}/Document?version=latest&channel=stable
```

后端也支持推荐小写路径：

```http
GET /api/v1/products/{product_code}/document?version=latest&channel=stable
```

## 前端期望结构

`web/src/runtime/api.ts` 中定义了接口结构：

1. `ProductSummary`
2. `ProductReleaseDetail`
3. `ReleasePageData`
4. `PageBlock`

后端 `DocumentAction` 返回的 `data` 会兼容 `ProductReleaseDetail`，其中 `page.page_data` 用于驱动区块渲染。

如果存在以下文件，后端会优先读取它作为版本详情页数据：

```text
data/releases/<product_code>/<version>/vue/page.json
```

Daily 示例：

```text
data/releases/Daily/v1.0.0/vue/page.json
```

`page.json` 可以直接提供 `blocks` 数组。当前支持的区块：

1. `HeroBlock`
2. `FeatureGridBlock`
3. `RichTextBlock`
4. `ImageTextBlock`
5. `ScreenshotGalleryBlock`
6. `DownloadPanelBlock`
7. `TimelineBlock`
8. `FaqBlock`
9. `FooterCtaBlock`

其中 `ScreenshotGalleryBlock` 用于展示 Daily 这类版本目录中的多张截图。

## 数据来源与降级

1. 优先请求后端 API。
2. API 不可用或业务数据不存在时，前端可回退到 `web/src/mock/*.json`。
3. 当前空数据库下，后端会返回空产品列表或业务 `404`，前端仍可展示本地 mock 数据。

## 当前边界

1. `web/dist` 是已构建产物，后端不在启动时自动执行 npm build。
2. 版本详情中的下载地址来自后端返回的 `page.page_data.download.download_url`。
3. 当前未实现单独下载接口，下载 URL 暂为数据库中的文件路径。
4. `releases.vue_path` 当前作为页面源码或入口路径展示；实际运行时优先消费同目录 `page.json`，不会在浏览器中直接编译 `.vue` 单文件组件。
