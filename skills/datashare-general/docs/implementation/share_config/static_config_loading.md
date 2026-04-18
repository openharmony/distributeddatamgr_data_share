# 应用间配置共享 - 静态配置加载机制

> **说明**：本文档描述配置共享特性的静态配置加载机制，包括事件监听、触发时机和加载流程。

---

## 概述

静态配置通过监听 BMS（Bundle Manager Service）的公共事件触发加载，无需应用启动即可将配置数据加载到服务端 KvDB 数据库中。

**核心组件**：
- `SysEventSubscriber` - 系统事件监听器
- `ProxyDataManager` - 配置数据管理器
- `DataShareProfileConfig` - 配置文件解析器

---

## 事件监听机制

### 事件订阅

**位置**：`datamgr_service/services/distributeddataservice/service/data_share/sys_event_subscriber.cpp`

`SysEventSubscriber` 订阅以下 BMS 公共事件：

| 事件 | Action | 常量名 | 触发时机 |
|------|--------|--------|---------|
| 应用安装 | `eventfwk.event PACKAGE_ADDED` | `COMMON_EVENT_PACKAGE_ADDED` | 应用安装完成后 |
| 应用更新 | `eventfwk.event PACKAGE_CHANGED` | `COMMON_EVENT_PACKAGE_CHANGED` | 应用更新完成后 |
| 应用卸载 | `eventfwk.event PACKAGE_REMOVED` | `COMMON_EVENT_PACKAGE_REMOVED` | 应用卸载完成后 |
| BMS 就绪 | `eventfwk.event BUNDLE_SCAN_FINISHED` | `COMMON_EVENT_BUNDLE_SCAN_FINISHED` | 系统启动 BMS 扫描完成后 |

```cpp
SysEventSubscriber::SysEventSubscriber(const CommonEventSubscribeInfo& info,
    std::shared_ptr<ExecutorPool> executors)
    : CommonEventSubscriber(info), executors_(executors)
{
    callbacks_ = {
        { COMMON_EVENT_BUNDLE_SCAN_FINISHED, &SysEventSubscriber::OnBMSReady }
    };
    installCallbacks_ = {
        { COMMON_EVENT_PACKAGE_ADDED,   &SysEventSubscriber::OnAppInstall },
        { COMMON_EVENT_PACKAGE_CHANGED, &SysEventSubscriber::OnAppUpdate },
        { COMMON_EVENT_PACKAGE_REMOVED, &SysEventSubscriber::OnAppUninstall }
    };
}
```

### 事件接收与处理

**位置**：`SysEventSubscriber::OnReceiveEvent()`

```cpp
void SysEventSubscriber::OnReceiveEvent(const CommonEventData& event)
{
    EventFwk::Want want = event.GetWant();
    std::string action = want.GetAction();

    // 处理安装/更新/卸载事件
    auto installEvent = installCallbacks_.find(action);
    if (installEvent != installCallbacks_.end()) {
        // 1. 解析事件参数
        std::string bundleName = want.GetElement().GetBundleName();
        int32_t userId = want.GetIntParam(USER_ID, -1);
        int32_t appIndex = want.GetIntParam(APP_INDEX, 0);
        uint32_t tokenId = static_cast<uint32_t>(want.GetIntParam(ACCESS_TOKEN_ID, 0));

        // 2. 应用更新时 tokenId 为 0，需要通过 HapTokenID 获取
        if (tokenId == 0) {
            tokenId = AccessTokenKit::GetHapTokenID(userId, bundleName, appIndex);
        }

        // 3. 判断是否包含静态配置
        bool isCrossAppSharedConfig = want.GetBoolParam(CROSS_APP_SHARED_CONFIG, false);

        // 4. 调用对应处理函数
        (this->*(installEvent->second))(bundleName, userId, appIndex, tokenId, isCrossAppSharedConfig);
    }
}
```

### 事件参数说明

| 参数 | 类型 | 来源 | 说明 |
|------|------|------|------|
| `bundleName` | `std::string` | `want.GetElement().GetBundleName()` | 应用包名 |
| `userId` | `int32_t` | `want.GetIntParam(USER_ID)` | 用户 ID |
| `appIndex` | `int32_t` | `want.GetIntParam(APP_INDEX)` | 应用索引 |
| `tokenId` | `uint32_t` | `want.GetIntParam(ACCESS_TOKEN_ID)` | 访问令牌 ID（多用于权限校验） |
| `isCrossAppSharedConfig` | `bool` | `want.GetBoolParam(CROSS_APP_SHARED_CONFIG)` | 是否包跨应用配置共享静态配置 |

**注意**：应用更新时，事件中的 `tokenId` 为 0，需要通过 `AccessTokenKit::GetHapTokenID()` 重新获取。

---

## 事件处理逻辑

### 应用安装（OnAppInstall）

**位置**：`SysEventSubscriber::OnAppInstall()`

```cpp
void SysEventSubscriber::OnAppInstall(const std::string &bundleName,
    int32_t userId, int32_t appIndex, uint32_t tokenId, bool isCrossAppSharedConfig)
{
    ZLOGI("%{public}s installed, userId: %{public}d, appIndex: %{public}d, tokenId: %{public}d",
        bundleName.c_str(), userId, appIndex, tokenId);

    // 仅当应用包含静态配置时才加载
    if (isCrossAppSharedConfig) {
        ProxyDataManager::GetInstance().OnAppInstall(bundleName, userId, appIndex, tokenId);
    }
}
```

**处理逻辑**：
1. 记录安装日志
2. 检查 `isCrossAppSharedConfig` 标志
3. 调用 `ProxyDataManager::OnAppInstall()` 加载静态配置

### 应用更新（OnAppUpdate）

**位置**：`SysEventSubscriber::OnAppUpdate()`

```cpp
void SysEventSubscriber::OnAppUpdate(const std::string &bundleName,
    int32_t userId, int32_t appIndex, uint32_t tokenId, bool isCrossAppSharedConfig)
{
    ZLOGI("%{public}s updated, userId: %{public}d, appIndex: %{public}d, tokenId: %{public}d",
        bundleName.c_str(), userId, appIndex, tokenId);

    // 1. 先删除旧的缓存数据
    BundleMgrProxy::GetInstance()->Delete(bundleName, userId, appIndex);

    // 2. 重新加载新的静态配置
    if (isCrossAppSharedConfig) {
        ProxyDataManager::GetInstance().OnAppUpdate(bundleName, userId, appIndex, tokenId);
    }
}
```

**处理逻辑**：
1. 记录更新日志
2. 删除旧的 Bundle 缓存数据
3. 检查 `isCrossAppSharedConfig` 标志
4. 调用 `ProxyDataManager::OnAppUpdate()` 先删除旧配置，再加载新配置

### 应用卸载（OnAppUninstall）

**位置**：`SysEventSubscriber::OnAppUninstall()`

```cpp
void SysEventSubscriber::OnAppUninstall(const std::string &bundleName,
    int32_t userId, int32_t appIndex, uint32_t tokenId, bool isCrossAppSharedConfig)
{
    ZLOGI("%{public}s uninstalled, userId: %{public}d, appIndex: %{public}d, tokenId: %{public}d",
        bundleName.c_str(), userId, appIndex, tokenId);

    // 1. 删除缓存数据
    BundleMgrProxy::GetInstance()->Delete(bundleName, userId, appIndex);

    // 2. 删除该应用的所有配置数据（无论是否包含静态配置）
    ProxyDataManager::GetInstance().OnAppUninstall(bundleName, userId, appIndex, tokenId);
}
```

**处理逻辑**：
1. 记录卸载日志
2. 删除 Bundle 缓存数据
3. 调用 `ProxyDataManager::OnAppUninstall()` 删除该应用的所有配置数据

### BMS 就绪（OnBMSReady）

**位置**：`SysEventSubscriber::OnBMSReady()`

```cpp
void SysEventSubscriber::OnBMSReady()
{
    // 1. 通知 DataShare 服务就绪
    NotifyDataShareReady();

    // 2. 异步更新启动信息
    auto executors = executors_;
    if (executors == nullptr) {
        ZLOGE("executors is null.");
        return;
    }
    executors->Execute([]() {
        DataShareServiceImpl::UpdateLaunchInfo();
    });
}
```

**处理逻辑**：
1. 发送 `DATA_SHARE_READY` 粘性事件
2. 异步执行 `DataShareServiceImpl::UpdateLaunchInfo()` 更新启动信息

---

## 配置加载流程

### 完整流程图

```
┌─────────────────────────────────────────────────────────────┐
│                     BMS 事件监听                             │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│              SysEventSubscriber::OnReceiveEvent()           │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ 解析事件参数                                          │   │
│  │ • bundleName  • userId  • appIndex                   │   │
│  │ • tokenId (更新时需重新获取)                           │   │
│  │ • isCrossAppSharedConfig                             │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    事件类型判断                              │
│  ┌────────────────┐  ┌────────────────┐  ┌───────────────┐  │
│  │ PACKAGE_ADDED  │  │PACKAGE_CHANGED │  │PACKAGE_REMOVED│  │
│  └────────┬───────┘  └────────┬───────┘  └───────┬───────┘  │
│           ↓                   ↓                   ↓          │
│  ┌─────────────────────────────────────────────────────────┐│
│  │           isCrossAppSharedConfig 判断                    ││
│  └─────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│              ProxyDataManager 处理                           │
│  ┌────────────┐  ┌────────────┐  ┌────────────────────┐    │
│  │OnAppInstall│  │OnAppUpdate │  │OnAppUninstall      │    │
│  │加载静态配置 │  │先删后加载   │  │删除所有配置        │    │
│  └─────┬──────┘  └─────┬──────┘  └──────────┬─────────┘    │
└────────┼───────────────┼─────────────────────┼──────────────┘
         │               │                     │
         └───────────────┴─────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│              DataShareProfileConfig                         │
│  GetCrossAppSharedConfig()                                  │
│  • 读取 module.json5 中的 crossAppSharedConfig字段           │
│  • 解析 $profile:shared_config 文件                          │
│  • 反序列化 JSON → SerialDataShareProxyData[]               │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│              PublishedProxyData::Upsert()                   │
│  • 写入 KvDB 数据库                                           │
│  • 更新配置列表                                              │
└─────────────────────────────────────────────────────────────┘
```

### 静态配置文件解析

**位置**：`DataShareProfileConfig::GetCrossAppSharedConfig()`

```cpp
std::pair<int, std::vector<SerialDataShareProxyData>>
DataShareProfileConfig::GetCrossAppSharedConfig(
    const std::string &resource, const std::string &resPath, const std::string &hapPath)
{
    // 1. 初始化资源管理器
    std::shared_ptr<ResourceManager> resMgr = InitResMgr(resourcePath);
    if (resMgr == nullptr) {
        return std::make_pair(ERROR, serialProxyDatas);
    }

    // 2. 从资源管理器读取配置文件内容
    std::string info = GetResFromResMgr(resource, *resMgr, hapPath);
    if (info.empty()) {
        return std::make_pair(NOT_FOUND, serialProxyDatas);
    }

    // 3. 反序列化 JSON 为 ProxyDataProfileInfo
    ProxyDataProfileInfo profileInfo;
    if (!profileInfo.Unmarshall(info)) {
        return std::make_pair(ERROR, serialProxyDatas);
    }

    // 4. 返回解析后的配置数据
    return std::make_pair(SUCCESS, profileInfo.dataShareProxyDatas);
}
```

---

## 配置文件格式

### module.json5

```json5
{
  "module": {
    "crossAppSharedConfig": "$profile:shared_config"
  }
}
```

**说明**：
- `crossAppSharedConfig` 指定静态配置文件路径
- 格式为 `$profile:<文件名>`，不含 `.json` 后缀

### shared_config.json

位于 `resources/base/profile/` 目录下：

```json
{
  "crossAppSharedConfig": [
    {
      "uri": "datashareproxy://com.example.app/config1",
      "value": "SHARED_CONFIG_DEMO1",
      "allowList": ["6917573629901742292"]
    },
    {
      "uri": "datashareproxy://com.example.app/config2",
      "value": "SHARED_CONFIG_DEMO2",
      "allowList": ["6917573298752100864"]
    }
  ]
}
```

**字段说明**：
- `uri`: 配置的唯一标识，格式为 `datashareproxy://<bundleName>/<path>`
- `value`: 配置值（字符串类型）
- `allowList`: 允许访问该配置的应用 appIdentifier 列表

---

## 关键特性

### 1. 无需应用启动

静态配置在应用安装时由 BMS 事件触发加载，不需要应用进程启动。

### 2. 自动清理

应用卸载时，自动删除该应用的所有配置数据（包括静态配置和动态配置）。

### 3. 更新同步

应用更新时，先删除旧配置，再加载新配置。

### 4. 条件加载

仅当 `isCrossAppSharedConfig = true` 时才执行加载，避免无静态配置的应用触发无效操作。

---

## 相关文件

| 文件 | 路径 | 说明 |
|------|------|------|
| SysEventSubscriber | `datamgr_service/.../service/data_share/sys_event_subscriber.h/.cpp` | 系统事件监听器 |
| ProxyDataManager | `datamgr_service/.../service/data_share/common/proxy_data_manager.h/.cpp` | 配置数据管理 |
| DataShareProfileConfig | `datamgr_service/.../service/data_share/data_share_profile_config.h/.cpp` | 配置文件解析 |

---

**相关文档**：
- [配置共享特性](../../share_config.md) - ArkTS 接口和配置说明
- [C++ 接口实现](cpp_interfaces.md) - C++ 层接口详解
- [权限校验实现](permission_check.md) - allowList 权限校验逻辑
- [配置数据存储](storage_structure.md) - KvDB 存储结构
