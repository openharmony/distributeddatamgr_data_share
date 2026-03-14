/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
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

### 4. DataShare Service (服务端服务)

**位置**：`distributeddatamgr_datamgr_service/services/distributeddataservice/service/data_share/`

**运行环境**：运行于 `ddms` 进程（分布式数据管理服务进程）

**职责**：
- 提供 DataShare Silent/Non-Silent 模式支持
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

---

**相关文档**：
- [代码结构](code_structure.md) - 各代码仓的目录结构和核心文件
- [问题定位](troubleshooting.md) - 错误码速查和日志分析方法
- [代码仓清单](../repositories.md) - 完整的依赖仓列表
*/