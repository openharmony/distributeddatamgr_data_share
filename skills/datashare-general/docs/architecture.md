# Datashare 模块架构文档

> **说明**：文档中的文件路径格式为 `<仓名>/路径`，表示该文件位于对应代码仓中的相对路径。
> 详细代码结构见 [code_structure.md](code_structure.md)。

---

## 两种访问模式

Datashare 支持两种数据访问模式：静默访问和非静默访问。

### 静默访问 (Silent Access)

静默访问是一种无需创建 ExtensionAbility 的数据共享方式。

**特点**：
- 无需实现 DataShareExtensionAbility
- 由 DataShare Service 直接管理数据
- 适用于简单的数据共享场景

**数据流**：
```
应用 → DataShareHelper → DataShare Service → RDB
```

### 非静默访问 (Non-Silent Access)

非静默访问通过 DataShareExtensionAbility 实现。

**特点**：
- 需要实现并配置 DataShareExtensionAbility
- 数据提供方可以自定义业务逻辑
- 支持更细粒度的权限控制
- 适用于需要复杂业务逻辑的场景

**数据流**：
```
应用 A (DataShareHelper) → IPC → 应用 B (DataShareExtAbility) → RDB
```

**两种模式对比**：

| 特性 | 静默访问 | 非静默访问 |
|------|---------|-----------|
| 是否需要 ExtensionAbility | 否 | 是 |
| 业务逻辑自定义 | 否 | 是 |
| 权限控制粒度 | 粗粒度 | 细粒度 |
| 适用场景 | 简单共享 | 复杂业务 |
| IPC 跳转次数 | 1 次 | 2 次 |

---

## 核心组件详解

### 1. DataShareHelper (消费者)

**位置**：`distributeddatamgr_data_share/frameworks/native/consumer/`

**职责**：
- 提供数据访问接口（增删改查）
- 静默访问：与 DataShare Service 通信
- 非静默访问：通过 IPC 与 DataShareExtAbility 通信
- 管理数据观察者（Observer）

**关键接口**：`Insert`、`Delete`、`Update`、`Query`、`RegisterObserver`、`UnregisterObserver`

### 2. DataShareExtAbility (提供者)

**位置**：`distributeddatamgr_data_share/frameworks/native/provider/`

**职责**：
- 实现数据提供方的业务逻辑
- 响应客户端的增删改查请求
- 管理数据变更通知

**关键接口**：`Insert`、`Delete`、`Update`、`Query`（虚函数，可重写）

### 3. ResultSet (结果集)

**位置**：`distributeddatamgr_data_share/interfaces/inner_api/common/include/basic/result_set.h`

**职责**：
- 存储查询结果
- 通过共享内存实现，避免 IPC 大数据传输

**关键特性**：
- 支持共享内存，减少 IPC 开销
- 支持多种数据类型（int, double, string, blob 等）
- 支持随机访问和遍历

### 4. DataProxyHandle (配置共享句柄)

**位置**：`distributeddatamgr_data_share/frameworks/native/consumer/`

**职责**：
- 提供应用间配置共享的操作接口
- 支持配置的发布、删除、获取
- 支持配置变更事件的订阅和取消订阅
- 基于 allowList 的访问控制

**关键接口**：
- `createDataProxyHandle()` - 创建配置共享句柄
- `publish()` - 发布或修改配置项
- `delete()` - 删除配置项
- `get()` - 获取配置项
- `on('dataChange')` - 订阅配置变更
- `off('dataChange')` - 取消订阅配置变更

**使用场景**：
- 跨应用配置同步（如主题、语言等）
- 系统全局配置发布
- 应用间共享参数配置

**URI 格式**：
```
datashareproxy://{bundleName}/{path}
```

### 5. DataShare Service (服务端服务)

**位置**：`distributeddatamgr_datamgr_service/services/distributeddataservice/service/data_share/`

**运行环境**：运行于 `ddms` 进程（分布式数据管理服务进程）

**职责**：
- 提供 DataShare Silent/Non-Silent 模式支持
- 提供配置共享服务端支持（DataProxyHandle 后端）
- 处理权限验证

**关键组件**：
- `IDataShareService` - 服务接口
- `DataShareServiceImpl` - 服务实现

---

## 关键设计模式

### 1. 策略模式 (Strategy Pattern)

在服务端用于处理不同场景的数据访问策略：

**策略结构**：
- `LoadConfigCommonStrategy` - 通用配置加载策略
- `PermissionStrategy` - 权限验证策略
- `CrossPermissionStrategy` - 跨权限策略

**策略接口**：`Strategy::operator()`

### 2. 观察者模式 (Observer Pattern)

用于数据变更通知，通过 DataObs 机制实现。

**核心组件**：
- `IDataAbilityObserver` - 观察者接口，定义 `OnChange()` 回调
- `DataObsManager` - 数据观察管理器（位于 `ability_ability_runtime` 仓）

**DataObs 核心接口**：`RegisterObserver`、`UnregisterObserver`、`NotifyChange`、`NotifyChangeExt`

**观察者模式流程**：
```
1. DataShareHelper.RegisterObserver() 注册观察者
2. DataObsManager.RegisterObserver() 记录到 DataObsService
3. 数据变更时，调用 DataObsManager.NotifyChange()
4. DataObsService 通过 IPC 通知所有观察者
5. 观察者的 OnChange() 回调被触发
```

详见 [code_structure.md](code_structure.md#ability_ability_runtimedataobs)。

### 3. 配置共享架构 (Share-Config Architecture)

配置共享通过 DataProxyHandle 实现应用间配置数据的安全共享。

**核心特性**：
- **静态配置**：应用安装时通过 `shared_config.json` 预置配置
- **动态配置**：运行时通过 `publish/delete` 接口动态管理配置
- **访问控制**：基于 `allowList` 的细粒度权限控制
- **变更通知**：支持 `on/off` 订阅配置变更事件

**数据流**：
```
配置发布方                          配置访问方
    ↓                                   ↓
DataProxyHandle ← IPC → DataShare Service ←→ 配置存储
    ↓                                   ↓
发布/删除/修改                      获取/订阅变更
```

**权限校验流程**：
```
1. 访问方调用 get() 获取配置
2. DataShare Service 检查 allowList
3. 验证通过则返回配置值
4. 验证失败则返回 NO_PERMISSION
```

详见 [share_config.md](share_config.md)、[C++ 接口实现](implementation/config_data/cpp_interfaces.md) 和 [权限校验](implementation/config_data/permission_check.md)。

---

**相关文档**：
- [代码结构](code_structure.md) - 各代码仓的目录结构和核心文件
- [问题定位](troubleshooting.md) - 错误码速查和日志分析方法
- [配置共享](share_config.md) - 应用间配置共享特性
  - [C++ 接口实现](implementation/config_data/cpp_interfaces.md) - 配置共享 C++ 层接口详解
  - [权限校验](implementation/config_data/permission_check.md) - allowList 权限校验逻辑
  - [配置数据存储](implementation/config_data/storage_structure.md) - KvDB 存储结构
  - [静态配置加载](implementation/config_data/static_config_loading.md) - 事件监听和配置加载机制
- [代码仓清单](../repositories.md) - 完整的依赖仓列表