# Action 分工详情

## 当前 Action 列表

1. `ProductsAction`
2. `DocumentAction`
3. `CheckUpdateAction`
4. `ConvertDataTaskAction`

当前代码没有 `DownloadAction`。下载信息暂由 `DocumentAction` 和 `CheckUpdateAction` 从 `release_assets` 返回路径字段，后续如需要真实下载鉴权、断点续传或签名 URL，再单独新增下载 action。

## ProductsAction

入口：

```cpp
json ListProducts();
```

职责：

1. 调用 `Storage_SQL::getALLProducts()`。
2. 返回 `/api/v1/products` 所需的 `{code,message,data.items}`。

## DocumentAction

入口：

```cpp
json GetDocument(const std::string &product_code, const std::string &version, const std::string &channel);
json GetListReleases(const std::string &product_code);
```

职责：

1. 查询版本详情页数据。
2. 支持 `version=latest`，由 storage 根据 `release_channels` 解析。
3. 返回版本列表。
4. 兼容 web 当前调用的 `/Document` 大写路径和推荐的 `/document` 小写路径。

## CheckUpdateAction

入口：

```cpp
json CheckUpdate(
    const std::string &product_code,
    const std::string &cur_version,
    const std::string &platform,
    const std::string &arch,
    const std::string &package_type,
    const std::string &cur_data_schema_version);
```

职责：

1. 查询 stable latest release。
2. 使用 `Tools::canUpgrade()` 判断软件版本是否需要升级。
3. 返回全量包路径、大小和摘要。
4. 根据 `data_schema_version` 判断是否需要数据转换。

## ConvertDataTaskAction

入口：

```cpp
json createTask(
    const std::string &product_code,
    const std::string &from_version,
    const std::string &to_version,
    const std::string &file_path);

json getTaskStatus(const std::string &taskId);
json getTaskResult(const std::string &taskId);
```

职责：

1. 根据 `product_code` 查询产品。
2. 根据 `from_version/to_version` 查询 `convert_rules`。
3. 创建 `convert_tasks` 记录。
4. 将任务放入内存队列。
5. worker 调用 `python/convert/convert_data.py`。
6. 回写任务状态、输出路径和输出 SHA256。

当前任务状态：

1. `pending`
2. `running`
3. `success`
4. `failed`
5. `canceled`
