# Web 展示与动态页面模块

## 当前状态

`web` 是 Vue 前端模块，当前后端直接服务 `web/dist`。浏览器访问 `/` 会返回 `web/dist/index.html`，未知前端路径由 Drogon default handler 回退到同一个 HTML 文件。

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

## 数据来源与降级

1. 优先请求后端 API。
2. API 不可用或业务数据不存在时，前端可回退到 `web/src/mock/*.json`。
3. 当前空数据库下，后端会返回空产品列表或业务 `404`，前端仍可展示本地 mock 数据。

## 当前边界

1. `web/dist` 是已构建产物，后端不在启动时自动执行 npm build。
2. 版本详情中的下载地址来自后端返回的 `page.page_data.download.download_url`。
3. 当前未实现单独下载接口，下载 URL 暂为数据库中的文件路径。
