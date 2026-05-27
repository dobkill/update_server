# Web 展示与版本详情页面模块

## 1. 目标

`web` 模块当前只实现两类页面：

1. 默认无参页面：展示所有小项目，支持搜索。
2. 小项目详情页：支持“默认 latest”或“指定版本号”两种访问方式。

设计目标：

1. 前端只保留一套通用 Vue 运行时。
2. 列表页结构固定，只换数据。
3. 详情页结构由版本文档 JSON 组合生成。
4. 页面依赖的数据优先从 API 获取，接口不可用时回退到本地 JSON。
5. 不兼容任何旧的 `/releases/latest` 页面请求方式。

## 2. 当前实现内容

双页面模型依赖以下正式接口：

1. `GET /api/v1/products` 驱动产品列表页。
2. `GET /api/v1/products/{product_code}/Document` 驱动版本详情页。
3. 列表页搜索基于已加载的小项目列表在前端过滤。
4. 详情页把 `page.page_data` 组装成统一的区块页面。
5. 本地 `web/src/mock/*.json` 作为接口失败时的后备数据。

## 3. 页面与 URL 规则

### 3.1 默认产品列表页

访问路径：

```text
/
```

页面行为：

1. 无参数直接进入产品列表页。
2. 固定展示搜索框、结果数和小项目卡片列表。
3. 卡片点击后跳到对应小项目的默认详情页。

### 3.2 小项目详情页

访问路径：

```text
/{product_code}
/{product_code}/{version}
```

说明：

1. `/{product_code}` 默认请求 `/api/v1/products/{product_code}/Document?version=latest`。
2. `/{product_code}/{version}` 用于查看指定版本详情，请求 `/api/v1/products/{product_code}/Document?version={version}`。
3. `channel` 缺省时，由服务端按 `products.default_channel` 解析；前端也可显式使用查询参数指定渠道，例如 `/{product_code}?channel=beta`。
4. 详情页顶部展示产品编码、请求版本、实际版本、渠道、发布时间和数据来源。

## 4. 协作方式

### 4.1 产品列表页

```text
用户访问 /
-> web 请求 /api/v1/products
-> service 返回产品列表
-> web 固定结构渲染搜索框和项目卡片
-> 用户在前端做本地搜索过滤
```

如果接口失败：

```text
web 读取本地 products.json
-> 继续渲染同一套列表页结构
-> 在页面上明确标注当前数据来源为 mock
```

### 4.2 小项目详情页

```text
用户访问 /{product_code} 或 /{product_code}/{version}
-> web 解析 product_code / version / channel
-> 若 version 不存在，则按 latest 语义发起请求
-> web 请求 /api/v1/products/{product_code}/Document?version=...&channel=...
-> service 校验参数并调用 action
-> action 解析 requested_version，并校验 channel 下是否存在目标版本
-> storage 读取按 product + version 共享的版本详情 JSON 和资源元数据
-> service 返回 requested_version / resolved_version / page.page_data
-> web 把 page.page_data 组装成区块页面
```

如果接口失败：

```text
web 读取本地 releases.json
-> 继续按同样的数据结构拼装详情页
-> 在页面上明确标注当前数据来源为 mock
```

## 5. 页面数据模型

### 5.1 产品列表页数据

列表页使用 `GET /api/v1/products` 返回的列表，当前前端关心以下字段：

| 字段 | 说明 |
| --- | --- |
| `product_code` | 小项目唯一编码 |
| `name` | 小项目名称 |
| `summary` | 小项目简介 |
| `cover_image_url` | 封面图地址，可为空 |
| `latest_version` | 该产品默认渠道的最新版本，前端可展示 |
| `updated_at` | 最近更新时间，前端可展示 |

### 5.2 版本详情页主模型

```json
{
  "product_code": "project-manage",
  "requested_version": "latest",
  "resolved_version": "2.3.1",
  "channel": "stable",
  "title": "Project Manage 2.3.1",
  "published_at": "2026-05-23T10:00:00Z",
  "release_notes_summary": "修复导出卡死问题，优化首页加载速度。",
  "page": {
    "vue_entry_url": "/public/releases/project-manage/2.3.1/entry.js",
    "page_data": {
      "hero": {
        "title": "Project Manage 2.3.1",
        "subtitle": "本次版本聚焦导出稳定性和下载链路统一化。"
      },
      "download": {
        "version": "2.3.1",
        "platform": "windows",
        "arch": "x64",
        "channel": "stable",
        "download_url": "/api/v1/products/project-manage/releases/2.3.1/download?platform=windows&arch=x64&channel=stable"
      }
    }
  }
}
```

### 5.3 `page_data` 到区块的组装规则

当前前端按固定顺序组合区块：

1. `hero` -> `HeroBlock`
2. `summary` -> `RichTextBlock`
3. `highlights` -> `FeatureGridBlock`
4. `timeline` -> `TimelineBlock`
5. `download` -> `DownloadPanelBlock`
6. `faq` -> `FaqBlock`
7. `footer_cta` -> `FooterCtaBlock`

补充规则：

1. 如果 `page_data.blocks` 已经存在且非空，则直接使用该区块数组。
2. 下载区块中的 `download.download_url` 必须由 API 直接返回，前端不再自行拼接下载地址。
3. 下载链接中应已经包含 `platform / arch / channel` 参数。
4. 同一 `product_code + version` 的页面内容跨渠道共享；切换 `channel` 只会影响 latest 解析结果和下载信息，不改变页面主体内容。

## 6. 存储与本地 JSON

版本详情页需要的数据应提前保存在可发布的数据文件中。

当前前端为了脱离后端调试，内置了两类 mock：

```text
web/src/mock/
├── products.json
└── releases.json
```

用途：

1. `products.json` 模拟产品列表接口结果。
2. `releases.json` 模拟不同产品、不同版本的详情文档数据。
3. 与正式 API 字段结构保持一致，便于后续平滑切换。
4. 即使在正式环境中，只要接口请求失败，也允许回退到本地 mock，但页面必须明确标注当前数据来源。

## 7. 资源访问原则

物理文件放在 `data/`，但浏览器不直接访问磁盘路径。

对外统一使用公共 URL，例如：

1. `/public/releases/project-manage/2.3.1/entry.js`
2. `/public/releases/project-manage/2.3.1/assets/banner.webp`
3. `/api/v1/products/project-manage/releases/2.3.1/download?platform=windows&arch=x64&channel=stable`

## 8. 设计约束

1. 列表页结构固定，不为不同产品单独写模板。
2. 详情页差异通过 `page.page_data` 控制，不直接写死在页面组件里。
3. 不允许注入任意 HTML，页面内容必须可序列化。
4. 前端可以扩展区块组件，但 API 协议应尽量稳定。
5. Web 只调用新版 `Document` 接口，不保留任何旧详情接口分支。
