# Action 分工详情

## Action 列表

1. `ProductsAction`
2. `DocumentAction`
3. `CheckUpdateAction`
4. `ConvertDataTaskAction`

## ProductsAction

入口：

```cpp
json GetPortfolioHome();
json ListProducts();
```

`GetPortfolioHome()` 返回 `/api/v1/portfolio-home`：

1. `profile`
2. `recommendations`
3. `products`
4. `recent_updates`
5. `future_directions`

`ListProducts()` 返回 `/api/v1/products`：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "items": []
  }
}
```

内部方法：

1. `GetRecommendations()`
2. `GetFutureDirections()`
3. `GetSiteProfile()`
4. `GetRecentUpdates()`

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
4. 支持 `/document` 和 `/Document` 两个路径。

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
2. 使用 `Tools::canUpgrade()` 比较版本。
3. 返回全量包路径、大小和摘要。
4. 根据 `data_schema_version` 返回数据转换标记。

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
2. 根据 `from_version` 和 `to_version` 查询 `convert_rules`。
3. 创建 `convert_tasks` 记录。
4. 将任务放入内存队列。
5. worker 调用 `python/convert/convert_data.py`。
6. 回写任务状态、输出路径和输出 SHA256。

任务状态：

1. `pending`
2. `running`
3. `success`
4. `failed`
5. `canceled`
