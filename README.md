# update_server

C++/Drogon 软件更新平台，当前已接入后端 API、SQLite 存储、数据转换任务和 `web/dist` 静态页面。

## Build

```bash
cmake --build build -j 2
```

## Run

```bash
./build/software_update_platform
```

默认监听 `http://127.0.0.1:8080`。

## Smoke Test

```bash
curl http://127.0.0.1:8080/api/v1/products
curl "http://127.0.0.1:8080/api/v1/products/demo/check-update/1.0.0?platform=windows&arch=x64&package_type=portable"
```

更多说明见 [docs/项目设计.md](docs/项目设计.md)。
