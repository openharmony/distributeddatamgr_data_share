# 应用间配置共享 - C++ 接口实现

> **说明**：本文档详细描述配置共享特性的 C++ 层接口实现，用于开发时快速查找代码位置。

---

## 客户端接口（Consumer）

**头文件位置**：`data_share/interfaces/inner_api/consumer/include/dataproxy_handle.h`

**实现位置**：`data_share/frameworks/native/consumer/src/dataproxy_handle.cpp`

### 核心接口

| 接口名 | 返回值 | 参数 | 说明 |
|--------|--------|------|------|
| `Create()` | `std::pair<int, std::shared_ptr<DataProxyHandle>>` | 无 | 创建 DataProxyHandle 实例 |
| `PublishProxyData()` | `std::vector<DataProxyResult>` | `proxyData`, `proxyConfig` | 发布/修改配置项 |
| `DeleteProxyData()` | `std::vector<DataProxyResult>` | `uris`, `proxyConfig` | 删除配置项 |
| `GetProxyData()` | `std::vector<DataProxyGetResult>` | `uris`, `proxyConfig` | 获取配置项（静态方法） |
| `SubscribeProxyData()` | `std::vector<DataProxyResult>` | `uris`, `callback` | 订阅配置变更 |
| `UnsubscribeProxyData()` | `std::vector<DataProxyResult>` | `uris` | 取消订阅配置变更 |

### 接口签名详解

```cpp
// 创建句柄
static std::pair<int, std::shared_ptr<DataProxyHandle>> Create();

// 发布配置
std::vector<DataProxyResult> PublishProxyData(
    const std::vector<DataShareProxyData> &proxyData,
    const DataProxyConfig &proxyConfig);

// 删除配置
std::vector<DataProxyResult> DeleteProxyData(
    const std::vector<std::string> &uris,
    const DataProxyConfig &proxyConfig);

// 获取配置（静态方法）
static std::vector<DataProxyGetResult> GetProxyData(
    const std::vector<std::string> uris,
    const DataProxyConfig &proxyConfig);

// 订阅变更
std::vector<DataProxyResult> SubscribeProxyData(
    const std::vector<std::string> &uris,
    const std::function<void(const std::vector<DataProxyChangeInfo> &changeNode)> &callback);

// 取消订阅
std::vector<DataProxyResult> UnsubscribeProxyData(
    const std::vector<std::string> &uris);
```

---

## 服务端接口（Service）

**接口定义**：`datamgr_service/services/distributeddataservice/service/data_share/idata_share_service.h`

**实现位置**：`datamgr_service/services/distributeddataservice/service/data_share/data_share_service_impl.cpp`

### IDL 接口定义

```cpp
// IDataShareService 接口定义
class IDataShareService {
public:
    virtual std::vector<DataProxyResult> PublishProxyData(
        const std::vector<DataShareProxyData> &proxyData,
        const DataProxyConfig &proxyConfig) = 0;

    virtual std::vector<DataProxyResult> DeleteProxyData(
        const std::vector<std::string> &uris,
        const DataProxyConfig &proxyConfig) = 0;

    virtual std::vector<DataProxyGetResult> GetProxyData(
        const std::vector<std::string> &uris,
        const DataProxyConfig &proxyConfig) = 0;

    virtual std::vector<DataProxyResult> SubscribeProxyData(
        const std::vector<std::string> &uris,
        const sptr<IRemoteObject> &callback) = 0;

    virtual std::vector<DataProxyResult> UnsubscribeProxyData(
        const std::vector<std::string> &uris,
        const sptr<IRemoteObject> &callback) = 0;
};
```

### 服务端实现位置

| 接口 | 实现文件 |
|------|---------|
| `PublishProxyData` | `data_share_service_impl.cpp` |
| `DeleteProxyData` | `data_share_service_impl.cpp` |
| `GetProxyData` | `data_share_service_impl.cpp` |
| `SubscribeProxyData` | `data_share_service_impl.cpp` |
| `UnsubscribeProxyData` | `data_share_service_impl.cpp` |

---

## 调用链路

### 发布配置流程

```
ArkTS (createDataProxyHandle().publish)
    ↓
NAPI (napi_dataproxy_handle.cpp) 或 ANI (dataproxy_handle_ani.cpp)
    ↓
Consumer (dataproxy_handle.cpp::PublishProxyData)
    ↓
Proxy (data_share_service_proxy.cpp)
    ↓ IPC
Service (data_share_service_impl.cpp::PublishProxyData)
    ↓
配置存储/通知
```

### 订阅配置变更流程

```
ArkTS (on('dataChange'))
    ↓
NAPI (napi_subscriber_manager.cpp) 或 ANI (ani_subscriber_manager.cpp)
    ↓
Consumer (dataproxy_handle.cpp::SubscribeProxyData)
    ↓
Proxy (proxy_data_subscriber_manager.cpp)
    ↓ IPC
Service (data_share_service_impl.cpp::SubscribeProxyData)
    ↓
注册观察者到 DataObs
```

---

## 相关文件清单

### 客户端（Consumer）

| 文件 | 路径 | 说明 |
|------|------|------|
| 头文件 | `interfaces/inner_api/consumer/include/dataproxy_handle.h` | 接口定义 |
| 实现 | `frameworks/native/consumer/src/dataproxy_handle.cpp` | 核心实现 |
| NAPI | `frameworks/js/napi/dataShare/src/napi_dataproxy_handle.cpp` | JS 绑定 |
| ANI | `frameworks/ets/ani/src/cxx/dataproxy_handle_ani.cpp` | ArkTS 绑定 |

### 服务端（Service）

| 文件 | 路径 | 说明 |
|------|------|------|
| 接口定义 | `services/distributeddataservice/service/data_share/idata_share_service.h` | IDL 接口 |
| 实现 | `services/distributeddataservice/service/data_share/data_share_service_impl.cpp` | 服务实现 |
| Proxy | `frameworks/native/proxy/src/data_share_service_proxy.cpp` | IPC 代理 |

### 订阅管理

| 文件 | 路径 | 说明 |
|------|------|------|
| 消费者订阅 | `frameworks/native/proxy/src/proxy_data_subscriber_manager.cpp` | 订阅管理 |
| ANI 订阅 | `frameworks/ets/ani/src/cxx/ani_subscriber_manager.cpp` | ANI 订阅管理 |
| NAPI 订阅 | `frameworks/js/napi/observer/src/napi_subscriber_manager.cpp` | NAPI 订阅管理 |

### 测试

| 文件 | 路径 |
|------|------|
| 单元测试 | `test/unittest/native/consumer/src/dataproxy_handle_test.cpp` |

---

## 关键常量定义

```cpp
// 最大配置数量
constexpr size_t MAX_PROXY_CONFIG_COUNT = 32;

// URI 最大长度
constexpr size_t MAX_URI_LENGTH = 256;

// value 最大长度
constexpr size_t MAX_VALUE_LENGTH = 4096;

// allowList 最大数量
constexpr size_t MAX_ALLOW_LIST_SIZE = 256;
```

---

## 相关文档

| 文档 | 路径 | 说明 |
|------|------|------|
| [配置共享特性](../../share_config.md) | ArkTS 接口和配置说明 |
| [权限校验实现](permission_check.md) | allowList 权限校验逻辑详解 |
| [配置数据存储](storage_structure.md) | KvDB 存储结构和数据格式 |
| [架构说明](../../architecture.md) | DataShare 整体架构 |
