# 需求1
data/releases/Daily 是一个插件，v1.0.0 版本已经出来了，data版本是 v1.0.0
vue 文件是介绍 这个项目的页面，但是我还没写出如何介绍，你来帮我写个vue界面进行展示
（Daily 是 obsidian 的插件，用来管理每日任务 和 项目 规划进度的 插件 ，在vue 文件夹下 有很多图片，分别展示了 不同的页面）
（vue 是展示 当前版本 页面）

# 需求2
data/releases/Daily 是一个插件，v1.0.0 版本已经出来了，data版本是 v1.0.0
帮我将 Daily 插件如何 将这个项目 写入sqlite  ？


# 需求3
优化 web 模块的页面：当前UI太丑
要办证 web模块 能够兼容显示 data/releases/Daily 的vue 页面


# python 模块 的 python/admin 目录
    这个是给管理员用的，用来便捷的将 新软件，插件  写入 sqlite 数据库。但是当前的python模块的设计为觉得非常臃肿 不方便，python模块你可以重构一下（禁止兼容老版本的设计，只能采用最优设计）
    python/admin 要方便管理员操作（禁止命令行传参方式，这非常不方便）
    admin下可以任意创建文件夹，依据模块功能分类 分开存放，依据最优设计

---

# 实现记录

## 2026-06-05

1. Daily v1.0.0 已注册到 SQLite：
   - 产品编码：`Daily`
   - 插件版本：`v1.0.0`
   - 数据版本：`v1.0.0`
   - stable latest：`release_channels.is_latest = 1`
2. Daily 插件包已生成：
   - `data/packages/Daily/v1.0.0/Daily-v1.0.0.zip`
3. Daily 展示页已补齐：
   - 页面清单：`data/releases/Daily/v1.0.0/vue/page.json`
   - Vue 源文件：`data/releases/Daily/v1.0.0/vue/DailyReleasePage.vue`
   - 截图画廊使用 `ScreenshotGalleryBlock`
4. Web 模块已优化：
   - 默认产品/插件列表页改为更清爽的发布目录界面
   - 版本详情页改为紧凑元信息 + 动态区块
   - 支持 `/data/...` 静态资源和插件包访问
   - 支持读取 `data/releases/<product_code>/<version>/vue/page.json`
5. Python 管理工具已重构：
   - 保留 `python/admin/admin.py` 统一入口
   - 移除旧的单独命令行参数脚本
   - 管理员通过交互式选择 TOML 配置执行
   - 自动化通过 `ADMIN_CONFIG` 和 `ADMIN_DRY_RUN` 环境变量执行
