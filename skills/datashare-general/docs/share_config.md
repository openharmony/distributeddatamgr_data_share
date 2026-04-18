# 应用间配置共享（Share-Config）特性

> **说明**：本文档基于 API version 20+，仅支持 Stage 模型。

---

## 特性概述

应用间配置共享（Share-Config）是 DataShare 提供的应用间配置数据共享能力，通过集中管理公共配置信息，在不同应用间共享配置，提升协作效率。

### 核心能力

- **配置发布**：应用可发布配置供其他授权应用访问
- **配置访问**：授权应用可获取其他应用的配置数据
- **变更通知**：支持订阅配置变更事件，实时感知配置变化
- **访问控制**：基于 allowList 的细粒度权限控制
- **静态配置**：应用安装时预置默认配置（不依赖应用启动）
- **动态配置**：运行时动态增删改配置

### 架构角色

| 角色 | 说明 | 职责 |
|------|------|------|
| **配置发布方** | 数据提供方 | 发布共享配置、维护 allowList、更新配置值 |
| **配置访问方** | 数据消费方 | 获取配置、订阅配置变更 |

---

## URI 格式规范

应用间配置共享使用统一的 URI 格式：

```
datashareproxy://{bundleName}/{path}
```

| 组成部分 | 说明 | 约束 |
|----------|------|------|
| `datashareproxy://` | 固定协议头 | 必须 |
| `{bundleName}` | 配置发布方应用的 bundleName | 必须 |
| `{path}` | 配置路径标识 | 同一应用内不允许重复 |

**约束**：
- URI 最大长度：256 字节
- path 可随意填写，但同一应用内不允许重复
- uri 如果配置的bundleName与配置发布方应用的bundleName必须相同，不同则配置不生效

**示例**：
```
datashareproxy://com.example.app1/config1
datashareproxy://com.example.app1/settings/theme
datashareproxy://com.system.settings/global/brightness
```

---

## 核心数据结构

### ProxyData（共享配置数据结构）

配置发布方发布配置时使用的数据结构。

| 字段 | 类型 | 必填 | 说明 | 约束 |
|------|------|------|------|------|
| `uri` | string | 是 | 共享配置的全局唯一标识 | 最大 256 字节 |
| `value` | ValueType | 否 | 共享配置的值 | 最大 4096 字节，默认为空字符串 |
| `allowList` | string[] | 否 | 允许订阅和读取的应用列表 | 最大 256 个，单个 appIdentifier 最大 128 字节 |

**示例**：
```typescript
const configData: dataShare.ProxyData[] = [{
  uri: 'datashareproxy://com.example.app1/config1',
  value: 'Value1',
  allowList: ['6917573629901742292', '6917573298752100864']
}];
```

### DataProxyChangeInfo（配置变更通知数据结构）

配置变更时通知访问方的数据结构。

| 字段 | 类型 | 说明 |
|------|------|------|
| `type` | ChangeType | 数据变更类型（INSERT/DELETE/UPDATE） |
| `uri` | string | 变化的 URI |
| `value` | ValueType | 变更后的配置值 |

### DataProxyResult（操作结果数据结构）

批量操作的返回结果。

| 字段 | 类型 | 说明 |
|------|------|------|
| `uri` | string | 被操作的 URI |
| `result` | DataProxyErrorCode | 操作结果错误码 |

### DataProxyGetResult（获取操作结果数据结构）

获取配置操作的返回结果。

| 字段 | 类型 | 说明 |
|------|------|------|
| `uri` | string | 被操作的 URI |
| `result` | DataProxyErrorCode | 操作结果错误码 |
| `value` | ValueType \| undefined | 配置值（仅发布者或 allowList 中应用可获取） |
| `allowList` | string[] \| undefined | 允许列表（仅发布者可获取） |

---

## 接口说明

### 公共接口

#### createDataProxyHandle()

创建数据代理操作句柄。

```typescript
createDataProxyHandle(): Promise<DataProxyHandle>
```

**系统能力**：SystemCapability.DistributedDataManager.DataShare.Consumer

**示例**：
```typescript
import { dataShare } from '@kit.ArkData';

const dataProxyHandle = await dataShare.createDataProxyHandle();
```

---

### 配置发布方接口

#### publish()

发布或修改配置项。

```typescript
publish(data: ProxyData[], config: DataProxyConfig): Promise<DataProxyResult[]>
```

**约束**：
- 只有发布者才能更新配置
- 每个应用最多发布 32 个配置（静态 + 动态总和）
- 数组最大长度 32

**示例**：
```typescript
const newConfigData: dataShare.ProxyData[] = [{
  uri: 'datashareproxy://com.example.app1/config1',
  value: 'Value1',
  allowList: ['6917573629901742292']
}];
const config: dataShare.DataProxyConfig = {
  type: dataShare.DataProxyType.SHARED_CONFIG
};
await dataProxyHandle.publish(newConfigData, config);
```

#### delete()

删除配置项。

```typescript
delete(uris: string[], config: DataProxyConfig): Promise<DataProxyResult[]>
```

**约束**：
- 只有配置发布方能删除
- 数组最大长度 32

---

### 配置访问方接口

#### get()

获取配置项信息。

```typescript
get(uris: string[], config: DataProxyConfig): Promise<DataProxyGetResult[]>
```

**约束**：
- 只有发布者或 allowList 中指定的应用可以访问
- 非发布者只能获取 value，无法获取 allowList

#### on('dataChange')

订阅配置变更事件。

```typescript
on(event: 'dataChange', uris: string[], config: DataProxyConfig,
   callback: AsyncCallback<DataProxyChangeInfo[]>): DataProxyResult[]
```

**约束**：
- 不支持跨用户订阅
- 不支持订阅未发布的配置
- 权限收回后不再通知

**示例**：
```typescript
const urisToWatch: string[] = ['datashareproxy://com.example.app1/config1'];
const config: dataShare.DataProxyConfig = {
  type: dataShare.DataProxyType.SHARED_CONFIG
};
const callback = (err, changes) => {
  changes.forEach(change => {
    console.info(`Change: ${change.type}, URI: ${change.uri}, Value: ${change.value}`);
  });
};
dataProxyHandle.on('dataChange', urisToWatch, config, callback);
```

#### off('dataChange')

取消订阅配置变更事件。

```typescript
off(event: 'dataChange', uris: string[], config: DataProxyConfig,
    callback?: AsyncCallback<DataProxyChangeInfo[]>): DataProxyResult[]
```

---

## 配置方式

### 静态配置

应用包安装时提供的默认共享配置项，不依赖应用启动即生效。

**触发时机**：

通过 `SysEventSubscriber` 监听 BMS 的公共事件触发配置加载：

| 事件 | Action | 触发时机 |
|------|--------|---------|
| 应用安装 | `COMMON_EVENT_PACKAGE_ADDED` | 应用安装完成后 |
| 应用更新 | `COMMON_EVENT_PACKAGE_CHANGED` | 应用更新完成后 |
| 应用卸载 | `COMMON_EVENT_PACKAGE_REMOVED` | 应用卸载完成后 |

**处理逻辑**：
- **应用安装** → `ProxyDataManager::OnAppInstall()` - 读取并加载静态配置到 KvDB
- **应用更新** → `ProxyDataManager::OnAppUpdate()` - 先删除旧配置，再加载新配置
- **应用卸载** → `ProxyDataManager::OnAppUninstall()` - 自动删除该应用的所有配置

**配置步骤**：

1. **创建共享配置文件**

   在 `resources/base/profile/` 目录下创建配置文件（如 `shared_config.json`）：

   ```json5
   {
     "crossAppSharedConfig": [
       {
         "uri": "datashareproxy://com.example.example/key1",
         "value": "SHARED_CONFIG_DEMO1",
         "allowList": ["6917573629901742292"]
       },
       {
         "uri": "datashareproxy://com.example.example/key2",
         "value": "SHARED_CONFIG_DEMO2",
         "allowList": ["6917573298752100864"]
       }
     ]
   }
   ```

2. **配置 module.json5**

   在 module.json5 中添加 crossAppSharedConfig 字段：

   ```json5
   {
     "module": {
       "crossAppSharedConfig": "$profile:shared_config"
     }
   }
   ```

3. **构建安装**

   应用安装后，静态配置自动加载到服务端 KvDB 数据库。

**实现细节**：详见 [静态配置加载机制](implementation/share_config/static_config_loading.md)。

---

### 动态配置

运行时通过接口动态增删改配置项。

**发布配置**：
```typescript
function publishSharedConfig() {
  const dataProxyHandle = await dataShare.createDataProxyHandle();
  const newConfigData: dataShare.ProxyData[] = [{
    uri: 'datashareproxy://com.samples.shareconfig/config1',
    value: 'Value1',
    allowList: ['appIdentifier1', 'appIdentifier2']
  }];
  const config = { type: dataShare.DataProxyType.SHARED_CONFIG };
  await dataProxyHandle.publish(newConfigData, config);
}
```

**删除配置**：
```typescript
function deleteSharedConfig() {
  const urisToDelete = [
    'datashareproxy://com.samples.shareconfig/config1',
    'datashareproxy://com.samples.shareconfig/config2'
  ];
  const config = { type: dataShare.DataProxyType.SHARED_CONFIG };
  await dataProxyHandle.delete(urisToDelete, config);
}
```

---

## 错误码

### DataProxyErrorCode

| 错误码 | 值 | 说明 |
|--------|-----|------|
| SUCCESS | 0 | 操作成功 |
| URI_NOT_EXIST | 1 | URI 不存在或取消未订阅的 URI |
| NO_PERMISSION | 2 | 无权限执行操作 |
| OVER_LIMIT | 3 | 配置数量超过 32 个上限 |

### 通用错误码

| 错误码 ID | 错误信息 |
|-----------|----------|
| 15700000 | Inner error（服务未就绪或异常重启） |
| 15700014 | 参数格式错误或值范围无效 |

---

## 约束与限制

| 项目 | 限制 |
|------|------|
| 单应用最大配置数 | 32 个（静态 + 动态总和） |
| URI 最大长度 | 256 字节 |
| value 最大长度 | 4096 字节 |
| allowList 最大长度 | 256 个应用 |
| appIdentifier 最大长度 | 128 字节 |
| 单次批量操作最大数量 | 32 个 |
| 跨用户支持 | 不支持 |

---

## 核心代码路径

### 客户端（Consumer）

| 模块 | 文件路径 | 说明 |
|------|---------|------|
| ArkTS 接口 | `data_share/frameworks/ets/ani/` | ArkTS 接口实现 |
| NAPI 绑定 | `data_share/frameworks/js/napi/dataShare/src/` | JS NAPI 绑定 |
| DataProxyHandle | `data_share/frameworks/native/consumer/include/datashare_helper.h` | C++ 实现 |

### 服务端（Service）

| 模块 | 文件路径 | 说明 |
|------|---------|------|
| DataShare Service | `datamgr_service/services/distributeddataservice/service/data_share/` | 服务实现 |

---

## 权限控制机制

### allowList 访问控制

- **发布配置时**：可指定 allowList，只有列表中应用可访问
- **获取配置时**：仅发布者或 allowList 中应用可获取
- **订阅通知时**：仅 allowList 中应用可订阅
- **空 allowList**：表示只有发布者自身可访问

### 获取 appIdentifier

使用 `getBundleInfoForSelf` 接口获取当前应用的 appIdentifier：

```typescript
import { bundleManager } from '@kit.AbilityKit';

const bundleInfo = await bundleManager.getBundleInfoForSelf(0);
const appIdentifier = bundleInfo.appIdentifier;
```

---

## 调试与排查

### 日志 Domain

- DataShare: `0xD001651`
- DataObs（变更通知）: `0xD001312`

### 常见问题

**Q1: 配置发布失败**
- 检查 URI 格式是否正确
- 检查配置数量是否超过 32 个
- 检查 service 是否就绪

**Q2: 获取配置失败（NO_PERMISSION）**
- 确认应用是否在 allowList 中
- 确认 URI 格式正确
- 确认配置已发布

**Q3: 订阅通知不生效**
- 确认配置已发布
- 确认应用在 allowList 中
- 确认非跨用户场景

---

## 相关文档

- [开发参考](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/database/share-config.md) - ArkTS 开发者指南
- [应用间配置共享ArkTS接口](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkdata/js-apis-data-dataShare.md#datasharecreatedataproxyhandle20) - ArkTS接口
- [架构说明](architecture.md) - DataShare 整体架构
- [错误码](errorcode-datashare.md) - 数据共享错误码速查

---

## 典型使用场景

### 场景 1：跨应用主题同步

应用 A 发布主题配置，应用 B 和 C 订阅变化：

```typescript
// 应用 A - 发布配置
const themeConfig = [{
  uri: 'datashareproxy://com.example.appA/theme',
  value: 'dark',
  allowList: ['appB_id', 'appC_id']
}];
const config: dataShare.DataProxyConfig = {
      type: dataShare.DataProxyType.SHARED_CONFIG,
};
await dataProxyHandle.publish(themeConfig, config);

// 应用 B - 订阅变化
dataProxyHandle.on('dataChange', [themeUri], config, (err, changes) => {
  console.info(`Theme changed to: ${changes[0].value}`);
});
```

### 场景 2：应用私有配置

应用发布私有配置，仅自身可访问（allowList 为空）：

```typescript
// 应用 - 发布私有配置
const privateConfig = [{
  uri: 'datashareproxy://com.example.app/private/token',
  value: 'secret_value',
  allowList: []  // 空列表，仅发布者自身可访问
}];
await dataProxyHandle.publish(privateConfig, config);
```

**使用说明**：
- `allowList` 为空或省略：仅配置发布者自身可访问
- 适用于应用内跨模块共享配置、临时参数传递等场景

---
