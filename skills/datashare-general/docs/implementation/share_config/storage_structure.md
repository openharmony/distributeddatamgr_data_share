# 应用间配置共享 - 配置数据存储结构

> **说明**：本文档描述配置共享特性在服务端的存储结构设计，包括 KvDB 表结构、数据格式和存储路径。

---

## 存储概述

配置共享数据存储在服务端的 **KvDB**（键值数据库）中，由 `KvDBDelegate` 统一管理。

**核心特性**：
- 使用 KvDB 的 `PROXYDATA_TABLE` 表存储所有配置数据
- 支持版本控制（基于 `VersionData` 基类）
- 数据序列化使用 `DistributedData::Serializable` 框架
- 支持自动备份和恢复

---

## 数据库表结构

### PROXYDATA_TABLE

配置数据存储在单一的 KvDB 表中，表名为 `PROXYDATA_TABLE`。

| 字段 | 类型 | 说明 |
|------|------|------|
| Key | `std::string` | 由 `Id(uri, userId)` 或 `Id(tokenId, userId)` 生成 |
| Value | `std::string` | 序列化后的数据（`ProxyDataNode` 或 `ProxyDataList`） |

**Key 生成规则**：
```cpp
// 配置数据 Key：Id(uri, userId)
std::string filter = Id(uri, callerBundleInfo.userId);

// 配置列表 Key：Id(tokenId, userId)
std::string listFilter = Id(std::to_string(tokenId), user);
```

---

## 数据结构

### ProxyDataNode（配置节点）

存储单个配置项的完整信息。

```cpp
class ProxyDataNode : public VersionData {
    SerialDataShareProxyData proxyData;  // 序列化后的配置数据
    int32_t userId;                      // 用户 ID
    uint32_t tokenId;                    // 发布者 tokenId
};
```

**序列化字段**：
```cpp
bool ProxyDataNode::Marshal(json &node) const
{
    bool ret = SetValue(node[GET_NAME(proxyData)], proxyData);
    ret = ret && SetValue(node[GET_NAME(userId)], userId);
    ret = ret && SetValue(node[GET_NAME(tokenId)], tokenId);
    return ret && VersionData::Marshal(node);
}
```

### ProxyDataListNode（配置列表节点）

存储单个应用发布的所有配置 URI 列表。

```cpp
class ProxyDataListNode : public VersionData {
    std::vector<std::string> uris;       // 配置 URI 列表
    int32_t userId;                      // 用户 ID
    uint32_t tokenId;                    // 发布者 tokenId
};
```

### SerialDataShareProxyData（序列化配置数据）

```cpp
struct SerialDataShareProxyData {
    std::string uri;                     // 配置 URI
    ValueType value;                     // 配置值（支持多种类型）
    std::vector<std::string> allowList;  // 允许列表
};
```

---

## 存储操作流程

### 静态配置加载

静态配置通过监听 BMS 的公共事件触发加载，详细说明见 [静态配置加载机制](static_config_loading.md)。

**简述**：
- `SysEventSubscriber` 监听 `COMMON_EVENT_PACKAGE_ADDED/CHANGED/REMOVED` 事件
- 事件触发后调用 `ProxyDataManager::OnAppInstall/Update/Uninstall()`
- 通过 `DataShareProfileConfig::GetCrossAppSharedConfig()` 解析配置文件
- 最终调用 `PublishedProxyData::Upsert()` 写入 KvDB

**事件处理流程**：
```cpp
// SysEventSubscriber 订阅事件
installCallbacks_ = {
    { COMMON_EVENT_PACKAGE_ADDED,   &SysEventSubscriber::OnAppInstall },
    { COMMON_EVENT_PACKAGE_CHANGED, &SysEventSubscriber::OnAppUpdate },
    { COMMON_EVENT_PACKAGE_REMOVED, &SysEventSubscriber::OnAppUninstall }
};

// 仅当应用包含静态配置时才加载
if (isCrossAppSharedConfig) {
    ProxyDataManager::GetInstance().OnAppInstall/Update/Uninstall(...);
}
```

### 插入配置（Insert）

**位置**：`ProxyDataManager::OnAppInstall/OnAppUpdate/OnAppUninstall()`

静态配置在应用安装时自动加载到 KvDB，无需应用启动。

#### 应用安装（OnAppInstall）

```cpp
void ProxyDataManager::OnAppInstall(const std::string &bundleName, int32_t user, int32_t index, uint32_t tokenId)
{
    // 1. 从 BMS 获取应用配置信息
    std::vector<DataShareProxyData> proxyDatas;
    if (!GetCrossAppSharedConfig(bundleName, user, index, proxyDatas)) {
        ZLOGE("GetCrossAppSharedConfig after install failed");
        return;
    }

    // 2. 遍历静态配置，插入数据库
    DataShareObserver::ChangeType type;
    std::for_each(proxyDatas.begin(), proxyDatas.end(),
        [user, bundleName, callerBundleInfo, &type](const DataShareProxyData proxyData) {
            PublishedProxyData::Upsert(proxyData, callerBundleInfo, type);
        });
}
```

#### 应用更新（OnAppUpdate）

```cpp
void ProxyDataManager::OnAppUpdate(const std::string &bundleName, int32_t user, int32_t index, uint32_t tokenId)
{
    // 1. 获取新的静态配置
    std::vector<DataShareProxyData> proxyDatas;
    GetCrossAppSharedConfig(bundleName, user, index, proxyDatas);

    // 2. 删除旧的动态配置
    std::vector<std::string> uris;
    ProxyDataList::Query(tokenId, user, uris);
    for (const auto &uri : uris) {
        PublishedProxyData::Delete(uri, callerBundleInfo, oldProxyData, type);
    }

    // 3. 重新插入新的静态配置
    std::for_each(proxyDatas.begin(), proxyDatas.end(),
        [user, bundleName, callerBundleInfo, &type](const DataShareProxyData proxyData) {
            PublishedProxyData::Upsert(proxyData, callerBundleInfo, type);
        });
}
```

#### 应用卸载（OnAppUninstall）

```cpp
void ProxyDataManager::OnAppUninstall(const std::string &bundleName, int32_t user, int32_t index, uint32_t tokenId)
{
    // 删除该应用的所有配置
    std::vector<std::string> uris;
    ProxyDataList::Query(tokenId, user, uris);
    for (const auto &uri : uris) {
        PublishedProxyData::Delete(uri, callerBundleInfo, proxyData, type);
    }
}
```

### 配置文件解析

**位置**：`DataShareProfileConfig::GetCrossAppSharedConfig()`

配置文件解析负责读取并解析应用的静态配置文件，详细说明见 [静态配置加载机制](static_config_loading.md#配置文件格式)。

```cpp
std::pair<int, std::vector<SerialDataShareProxyData>>
DataShareProfileConfig::GetCrossAppSharedConfig(
    const std::string &resource, const std::string &resPath, const std::string &hapPath)
{
    // 1. 初始化资源管理器
    std::shared_ptr<ResourceManager> resMgr = InitResMgr(resourcePath);

    // 2. 从资源管理器读取配置文件内容
    std::string info = GetResFromResMgr(resource, *resMgr, hapPath);

    // 3. 反序列化 JSON 为 ProxyDataProfileInfo
    ProxyDataProfileInfo profileInfo;
    if (!profileInfo.Unmarshall(info)) {
        return std::make_pair(ERROR, serialProxyDatas);
    }

    // 4. 返回解析后的配置数据
    return std::make_pair(SUCCESS, profileInfo.dataShareProxyDatas);
}
```

**配置文件格式**（`shared_config.json`）：
```json
{
  "crossAppSharedConfig": [
    {
      "uri": "datashareproxy://com.example.app/config1",
      "value": "SHARED_CONFIG_DEMO1",
      "allowList": ["6917573629901742292"]
    }
  ]
}
```

### 插入配置（Insert）

**位置**：`PublishedProxyData::InsertProxyData()`

```cpp
int32_t PublishedProxyData::InsertProxyData(std::shared_ptr<KvDBDelegate> kvDelegate,
    const std::string &bundleName, const int32_t &user, const uint32_t &tokenId,
    const DataShareProxyData &proxyData)
{
    // 1. 查询配置列表
    ProxyDataListNode proxyDataList;
    std::string listFilter = Id(std::to_string(tokenId), user);
    std::string listQueryResult;
    kvDelegate->Get(KvDBDelegate::PROXYDATA_TABLE, listFilter, "{}", listQueryResult);

    // 2. 反序列化列表
    if (!listQueryResult.empty()) {
        ProxyDataListNode::Unmarshall(listQueryResult, proxyDataList);
    }

    // 3. 检查配置数量上限（32 个）
    if (proxyDataList.uris.size() >= PROXY_DATA_MAX_COUNT) {
        return OVER_LIMIT;
    }

    // 4. 添加 URI 到列表
    auto it = std::find(proxyDataList.uris.begin(), proxyDataList.uris.end(), proxyData.uri_);
    if (it == proxyDataList.uris.end()) {
        proxyDataList.uris.emplace_back(proxyData.uri_);
    }

    // 5. 序列化配置数据
    SerialDataShareProxyData serialProxyData(proxyData.uri_, proxyData.value_, proxyData.allowList_);

    // 6. 写入数据库（配置数据）
    auto ret = kvDelegate->Upsert(KvDBDelegate::PROXYDATA_TABLE,
        PublishedProxyData(ProxyDataNode(serialProxyData, user, tokenId)));

    // 7. 更新配置列表
    auto value = ProxyDataList(ProxyDataListNode(proxyDataList.uris, user, tokenId));
    ret = kvDelegate->Upsert(KvDBDelegate::PROXYDATA_TABLE, value);

    return SUCCESS;
}
```

### 查询配置（Query）

**位置**：`PublishedProxyData::Query()`

```cpp
int32_t PublishedProxyData::Query(const std::string &uri, const BundleInfo &callerBundleInfo,
    DataShareProxyData &proxyData)
{
    auto delegate = KvDBDelegate::GetInstance();

    // 1. 根据 Key 查询
    std::string filter = Id(uri, callerBundleInfo.userId);
    std::string queryResult;
    delegate->Get(KvDBDelegate::PROXYDATA_TABLE, filter, "{}", queryResult);

    if (queryResult.empty()) {
        return URI_NOT_EXIST;
    }

    // 2. 反序列化数据
    ProxyDataNode data;
    ProxyDataNode::Unmarshall(queryResult, data);

    // 3. 非发布者不返回 allowList
    DataShareProxyData tempProxyData(data.proxyData.uri, data.proxyData.value, data.proxyData.allowList);
    if (callerBundleInfo.tokenId != data.tokenId) {
        tempProxyData.allowList_ = {};
    }

    // 4. 权限校验
    if (!VerifyPermission(callerBundleInfo, data)) {
        return NO_PERMISSION;
    }

    proxyData = tempProxyData;
    return SUCCESS;
}
```

### 删除配置（Delete）

**位置**：`PublishedProxyData::Delete()`

```cpp
int32_t PublishedProxyData::Delete(const std::string &uri, const BundleInfo &callerBundleInfo,
    DataShareProxyData &oldProxyData, DataShareObserver::ChangeType &type)
{
    auto delegate = KvDBDelegate::GetInstance();

    // 1. 查询配置数据
    std::string queryResult;
    delegate->Get(KvDBDelegate::PROXYDATA_TABLE, Id(uri, callerBundleInfo.userId), "{}", queryResult);

    // 2. 反序列化
    ProxyDataNode oldData;
    ProxyDataNode::Unmarshall(queryResult, oldData);

    // 3. 删除配置记录
    auto ret = delegate->Delete(KvDBDelegate::PROXYDATA_TABLE, Id(uri, callerBundleInfo.userId));

    // 4. 更新配置列表
    std::vector<std::string> uris;
    ProxyDataList::Query(callerBundleInfo.tokenId, callerBundleInfo.userId, uris);
    auto it = std::find(uris.begin(), uris.end(), uri);
    if (it != uris.end()) {
        uris.erase(it);
    }
    ret = delegate->Upsert(KvDBDelegate::PROXYDATA_TABLE,
        ProxyDataList(ProxyDataListNode(uris, callerBundleInfo.userId, callerBundleInfo.tokenId)));

    oldProxyData = DataShareProxyData(oldData.proxyData.uri, oldData.proxyData.value, oldData.proxyData.allowList);
    type = DataShareObserver::ChangeType::DELETE;
    return SUCCESS;
}
```

### 查询配置列表（Query List）

**位置**：`ProxyDataList::Query()`

```cpp
int32_t ProxyDataList::Query(uint32_t tokenId, int32_t userId, std::vector<std::string> &proxyDataList)
{
    auto delegate = KvDBDelegate::GetInstance();

    // 1. 根据 tokenId 查询配置列表
    std::string filter = Id(std::to_string(tokenId), userId);
    std::string queryResult;
    delegate->Get(KvDBDelegate::PROXYDATA_TABLE, filter, "{}", queryResult);

    if (queryResult.empty()) {
        return -1;
    }

    // 2. 反序列化列表
    ProxyDataListNode data;
    ProxyDataListNode::Unmarshall(queryResult, data);

    proxyDataList = std::move(data.uris);
    return proxyDataList.size();
}
```

---

## 存储路径

**KvDB 文件路径**：
```
/data/service/el1/public/database/distributeddata/kvdb/dataShare.db
```

**获取方式**：通过 `KvDBDelegate::GetInstance()` 获取单例实例

---

## 常量定义

| 常量 | 值 | 说明 |
|------|-----|------|
| `PROXY_DATA_MAX_COUNT` | 32 | 单应用最大配置数量 |
| `VALUE_MAX_SIZE` | 4096 | 配置值最大字节数 |
| `URI_MAX_SIZE` | 256 | URI 最大字节数 |
| `ALLOW_LIST_MAX_COUNT` | 256 | allowList 最大应用数量 |
| `APPIDENTIFIER_MAX_SIZE` | 128 | appIdentifier 最大字节数 |

---

## 相关组件

### KvDBDelegate

**位置**：`datamgr_service/services/distributeddataservice/service/data_share/common/kv_delegate.h`

```cpp
class KvDelegate final : public KvDBDelegate {
    // 核心接口
    std::pair<int32_t, int32_t> Upsert(const std::string &collectionName, const KvData &value);
    std::pair<int32_t, int32_t> Delete(const std::string &collectionName, const std::string &filter);
    int32_t Get(const std::string &collectionName, const Id &id, std::string &value);
    int32_t Get(const std::string &collectionName, const std::string &filter,
                const std::string &projection, std::string &result);
};
```

### VersionData（版本基类）

支持版本控制的数据基类，用于乐观锁和变更检测。

```cpp
class VersionData {
    int version;  // 版本号
};
```

---

## 相关文件

| 文件 | 路径 | 说明 |
|------|------|------|
| KvDB Delegate | `datamgr_service/.../service/data_share/common/kv_delegate.h` | KvDB 操作代理 |
| ProxyDataManager | `datamgr_service/.../service/data_share/common/proxy_data_manager.h/.cpp` | 配置数据管理 |
| SubscriberManager | `datamgr_service/.../service/data_share/subscriber_managers/proxy_data_subscriber_manager.h/.cpp` | 订阅管理 |
| DataShareProfileConfig | `datamgr_service/.../service/data_share/data_share_profile_config.h/.cpp` | 配置文件解析 |

---

## 附录：配置加载流程

```
应用安装/更新事件
    ↓
ProxyDataManager::OnAppInstall/Update()
    ↓
GetCrossAppSharedConfig()
    ↓
读取 module.json5 中的 crossAppSharedConfig
    ↓
DataShareProfileConfig.GetCrossAppSharedConfig()
    ↓
解析 $profile:shared_config 文件
    ↓
反序列化 JSON → SerialDataShareProxyData[]
    ↓
PublishedProxyData::Upsert() → KvDB
```

**详细的事件监听机制见**：[静态配置加载机制](static_config_loading.md)

**相关配置**：

1. **module.json5 配置**：
```json5
{
  "module": {
    "crossAppSharedConfig": "$profile:shared_config"
  }
}
```

2. **shared_config.json**（位于 `resources/base/profile/`）：
```json
{
  "crossAppSharedConfig": [
    {
      "uri": "datashareproxy://com.example.app/key1",
      "value": "value1",
      "allowList": ["appIdentifier1"]
    }
  ]
}
```

---

**相关文档**：
- [配置共享特性](../../share_config.md) - ArkTS 接口和配置说明
- [C++ 接口实现](cpp_interfaces.md) - C++ 层接口详解
- [权限校验实现](permission_check.md) - allowList 权限校验逻辑
