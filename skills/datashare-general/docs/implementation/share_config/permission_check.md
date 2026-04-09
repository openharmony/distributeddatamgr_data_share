# 应用间配置共享 - 权限校验实现

> **说明**：本文档详细描述配置共享特性的 allowList 权限校验机制，包括校验点、校验规则和数据结构。

---

## 权限校验概述

allowList 是配置共享的核心权限控制机制，服务端在以下四个关键位置进行访问校验：

| 校验点 | 位置 | 校验时机 |
|--------|------|---------|
| 获取配置 | `PublishedProxyData::Query()` | 查询配置数据时 |
| 发布配置 | `PublishedProxyData::Upsert()` | 新增/修改配置时 |
| 删除配置 | `PublishedProxyData::Delete()` | 删除配置时 |
| 订阅通知 | `ProxyDataSubscriberManager::Emit()` | 配置变更通知分发时 |

---

## 核心校验函数

**位置**：`datamgr_service/services/distributeddataservice/service/data_share/common/proxy_data_manager.cpp`

```cpp
bool PublishedProxyData::VerifyPermission(const BundleInfo &callerBundleInfo, const ProxyDataNode &data)
{
    // 1. 发布者自身访问：通过 tokenId 匹配
    if (callerBundleInfo.tokenId == data.tokenId) {
        return true;
    }

    // 2. allowList 检查：遍历检查 appIdentifier
    for (const auto &item : data.proxyData.allowList) {
        if (callerBundleInfo.appIdentifier == item) {
            return true;
        }
    }
    return false;
}
```

**校验逻辑**：
1. 优先检查 `tokenId` 是否匹配（发布者自身访问）
2. 遍历 `allowList` 检查 `appIdentifier` 是否在授权列表中
3. 满足任一条件即返回 `true`

---

## 校验点 1：获取配置（Query）

**位置**：`PublishedProxyData::Query()`

```cpp
int32_t PublishedProxyData::Query(const std::string &uri, const BundleInfo &callerBundleInfo,
    DataShareProxyData &proxyData)
{
    // ... 数据库查询 ...

    DataShareProxyData tempProxyData(data.proxyData.uri, data.proxyData.value, data.proxyData.allowList);
    // 非发布者不返回 allowList
    if (callerBundleInfo.tokenId != data.tokenId) {
        tempProxyData.allowList_ = {};
    }
    // 权限校验失败返回 NO_PERMISSION
    if (!VerifyPermission(callerBundleInfo, data)) {
        return NO_PERMISSION;
    }

    proxyData = tempProxyData;
    return SUCCESS;
}
```

**校验规则**：
- 发布者（tokenId 匹配）：返回完整配置（包含 allowList）
- allowList 中的应用：返回配置值（allowList 为空）
- 无权限应用：返回 `NO_PERMISSION`

---

## 校验点 2：发布配置（Upsert）

**位置**：`PublishedProxyData::Upsert()`

```cpp
int32_t PublishedProxyData::Upsert(const DataShareProxyData &proxyData, const BundleInfo &callerBundleInfo,
    DataShareObserver::ChangeType &type)
{
    // ... 数据校验 ...

    // 新增配置：检查 URI 中的 bundleName 是否与调用者匹配
    if (queryResult.empty()) {
        type = DataShareObserver::ChangeType::INSERT;
        return InsertProxyData(delegate, callerBundleInfo.bundleName, ...);
    } else {
        // 修改配置：检查 tokenId 是否匹配（必须是发布者才能修改）
        ProxyDataNode oldData;
        ProxyDataNode::Unmarshall(queryResult, oldData);
        if (callerBundleInfo.tokenId != oldData.tokenId) {
            ZLOGE("only allow to modify the proxyData of self bundle");
            return NO_PERMISSION;
        }
        // ... 更新操作 ...
    }
}
```

### InsertProxyData 中的 BundleName 校验

```cpp
std::string modifyBundle;
URIUtils::GetBundleNameFromProxyURI(proxyData.uri_, modifyBundle);
if (modifyBundle != bundleName) {
    ZLOGE("only allowed to publish the proxyData of self bundle");
    return NO_PERMISSION;
}
```

**校验规则**：
- **新增配置**：URI 中的 `bundleName` 必须与调用者 `bundleName` 匹配
- **修改配置**：`tokenId` 必须与存储的发布者 `tokenId` 匹配
- 校验失败返回 `NO_PERMISSION`

---

## 校验点 3：删除配置（Delete）

**位置**：`PublishedProxyData::Delete()`

```cpp
int32_t PublishedProxyData::Delete(const std::string &uri, const BundleInfo &callerBundleInfo,
    DataShareProxyData &oldProxyData, DataShareObserver::ChangeType &type)
{
    // 1. URI 中的 bundleName 校验
    std::string bundleName;
    URIUtils::GetBundleNameFromProxyURI(uri, bundleName);
    if (callerBundleInfo.bundleName != bundleName) {
        return NO_PERMISSION;
    }

    // ... 查询配置 ...

    // 2. tokenId 校验（必须是发布者）
    ProxyDataNode oldData;
    ProxyDataNode::Unmarshall(queryResult, oldData);
    if (callerBundleInfo.tokenId != oldData.tokenId) {
        return NO_PERMISSION;
    }
    // ... 执行删除 ...
}
```

**校验规则**：
- **bundleName 校验**：URI 中的 `bundleName` 必须与调用者 `bundleName` 匹配
- **tokenId 校验**：`tokenId` 必须与存储的发布者 `tokenId` 匹配
- 双重校验确保只有配置发布者才能删除配置

---

## 校验点 4：订阅通知（Emit）

**位置**：`ProxyDataSubscriberManager::Emit()`

```cpp
void ProxyDataSubscriberManager::Emit(const std::vector<ProxyDataKey> &keys,
    const std::map<DataShareObserver::ChangeType, std::vector<DataShareProxyData>> &datas,
    const int32_t &userId)
{
    // ... 遍历配置变更 ...

    std::for_each(observers.begin(), observers.end(),
        [&callbacks, data, userId, bundleName, &type, this](const auto &obs) {
            // allowList 检查 或 发布者自身
            if ((CheckAllowList(data.allowList_, obs.callerAppIdentifier) ||
                bundleName == obs.bundleName) && obs.userId == userId) {
                callbacks[obs.observer].emplace_back(type, data.uri_, data.value_);
            } else {
                ZLOGE("no permission to receive notification");
            }
        });
}
```

### CheckAllowList 实现

```cpp
bool ProxyDataSubscriberManager::CheckAllowList(const std::vector<std::string> &allowList,
    const std::string &callerAppIdentifier)
{
    for (const auto &item : allowList) {
        if (callerAppIdentifier == item || item == ALLOW_ALL) {
            return true;
        }
    }
    return false;
}
```

**校验规则**：
- `appIdentifier` 在 `allowList` 中：发送通知
- `bundleName` 匹配（发布者自身）：发送通知
- 支持 `ALLOW_ALL` 通配符
- 需同时满足 `userId` 匹配（不支持跨用户）

---

## 权限校验规则总结

| 操作 | 校验规则 | 失败返回 |
|------|---------|---------|
| **发布（Insert）** | URI 中 bundleName == 调用者 bundleName | NO_PERMISSION |
| **发布（Update）** | tokenId == 发布者 tokenId | NO_PERMISSION |
| **删除** | bundleName 匹配 + tokenId 匹配 | NO_PERMISSION |
| **获取配置** | tokenId 匹配 OR appIdentifier 在 allowList 中 | NO_PERMISSION |
| **获取 allowList** | 仅发布者可见 | 返回空列表 |
| **订阅通知** | appIdentifier 在 allowList 中 OR bundleName 匹配 + userId 匹配 | 不发送通知 |

---

## 关键数据结构

### BundleInfo（调用者信息）

```cpp
struct BundleInfo {
    std::string bundleName;      // 应用包名
    std::string appIdentifier;   // 应用唯一标识
    uint32_t tokenId;            // 安全令牌（标识发布者）
    int32_t userId;              // 用户 ID
};
```

### ProxyDataNode（存储节点）

```cpp
class ProxyDataNode : public VersionData {
    SerialDataShareProxyData proxyData;  // 序列化后的配置数据
    int32_t userId;                      // 用户 ID
    uint32_t tokenId;                    // 发布者 tokenId
};
```

### DataShareProxyData（配置数据）

```cpp
struct DataShareProxyData {
    std::string uri_;                    // 配置 URI
    ValueType value_;                    // 配置值
    std::vector<std::string> allowList_; // 允许列表
};
```

---

## 相关文件

| 文件 | 路径 | 说明 |
|------|------|------|
| 权限校验主逻辑 | `datamgr_service/.../service/data_share/common/proxy_data_manager.cpp` | Query/Upsert/Delete 校验 |
| 订阅通知校验 | `datamgr_service/.../service/data_share/subscriber_managers/proxy_data_subscriber_manager.cpp` | Emit 通知分发校验 |
| 服务端接口 | `datamgr_service/.../service/data_share/data_share_service_impl.cpp` | 调用入口 |

---

**相关文档**：
- [配置共享特性](../../share_config.md) - ArkTS 接口和配置说明
- [C++ 接口实现](cpp_interfaces.md) - C++ 层接口详解
- [配置数据存储](storage_structure.md) - KvDB 存储结构
