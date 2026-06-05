# Python 模块说明

本目录包含后端调用的数据转换脚本，以及管理员用于写入 SQLite 的配置驱动工具。

## 目录结构

```text
python/
├── README.md
├── convert/
│   ├── __init__.py
│   ├── convert_data.py          # C++ worker 默认调用入口
│   └── <product_code>/          # 产品专属转换脚本，可选
├── admin/
│   ├── admin.py                 # 统一入口，交互式选择 TOML 配置
│   ├── _product.py              # 产品写入
│   ├── _release.py              # 版本写入
│   ├── _asset.py                # 全量包写入与 hash/size 计算
│   ├── _channel.py              # 渠道 latest 指针
│   ├── _update_package.py       # 增量包写入
│   ├── _convert_rule.py         # 数据转换规则写入
│   ├── _page.py                 # 页面清单校验与运维
│   └── _cleanup.py              # 临时文件清理
└── lib/
    ├── __init__.py
    ├── db.py
    ├── config.py
    ├── toml_loader.py
    └── operations.py
```

## 管理员入口

推荐直接运行：

```bash
python python/admin/admin.py
```

`admin.py` 会列出 `config/*.toml` 配置文件，选择后默认先预演。确认无误后再次运行，并在提示中选择写入数据库。

自动化场景不使用命令行参数，改用环境变量：

```bash
ADMIN_CONFIG=config/daily_release.toml ADMIN_DRY_RUN=0 python python/admin/admin.py
```

## Daily 示例

`config/daily_release.toml` 已注册 Daily 插件：

1. `products.code = Daily`
2. `releases.version = v1.0.0`
3. `releases.data_schema_version = v1.0.0`
4. `release_assets.platform = obsidian`
5. `release_assets.arch = any`
6. `release_assets.package_type = plugin`

插件包位于：

```text
data/packages/Daily/v1.0.0/Daily-v1.0.0.zip
```

## 与 C++ 后端的协调

1. `config/app.json` 中的 `convert_script` 指向 `./python/convert/convert_data.py`。
2. `convert_rules.script_path` 可指向 `python/convert/<product_code>/convert_xxx.py`。
3. 管理工具直接写 SQLite，使用 WAL 模式，可与 C++ 并发读写。
