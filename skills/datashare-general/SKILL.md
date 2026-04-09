---
name: datashare-general
description: 本技能专为 OpenHarmony Datashare 模块代码分析和问题定位设计。
---

# Datashare 模块代码 Skill

## 技能描述

本技能专为 OpenHarmony Datashare 模块代码分析和问题定位设计。Datashare 是 OpenHarmony 的数据共享模块，支持同个设备上不同应用之间的数据共享。

本技能提供代码仓清单、架构文档和问题定位指南，帮助 agent 高效定位问题和辅助开发。

**核心能力**：
- Datashare 客户端/服务端实现
- DataObs 数据观察者（数据订阅和变更通知）
- 应用间配置共享（Share-Config）：支持应用间配置数据的发布、访问和变更通知

## 代码仓管理原则

**核心原则**：按需拉取、自动管理

| 场景 | 操作 |
|------|------|
| **首次使用** | 拉取核心仓 |
| **代码仓不存在** | 拉取对应仓 |
| **需要最新代码** | 更新对应仓 |
| **用户提到代码变更** | 更新对应仓 |
| **分析错误日志但代码不匹配** | 更新核心仓 |

agent 应检查代码仓是否存在，根据问题需要决定拉取哪些仓，不是每次都拉取所有仓。

## 核心代码仓

| 仓名 | GitCode 地址 | 说明 |
|------|-------------|------|
| distributeddatamgr_data_share | https://gitcode.com/openharmony/distributeddatamgr_data_share | DataShare 客户端实现 + Provider 端（DataShareExtAbility） |
| distributeddatamgr_datamgr_service | https://gitcode.com/openharmony/distributeddatamgr_datamgr_service | 分布式数据管理服务（包含 DataShare Service） |
| ability_ability_runtime | https://gitcode.com/openharmony/ability_ability_runtime | DataObs（数据观察者），提供数据订阅和变更通知能力 |

完整仓清单见 [repositories.md](repositories.md)。

## 架构概览

### 静默访问架构

```
应用 (DataShareHelper)
    ↓
DataShare Service
    ↓
RDB(数据存储)
```

### 非静默访问架构

```
应用 A (DataShareHelper)
    ↓ IPC
应用 B (DataShareExtAbility)
    ↓
RDB(数据存储)
```

详细架构见 [docs/architecture.md](docs/architecture.md)。

## 静默访问 vs 非静默访问

Datashare 支持两种数据访问模式：

### 静默访问 (Silent Access)

**特点**：
- 无需实现 DataShareExtensionAbility
- 由 DataShare Service 直接管理数据
- 适用于简单的数据共享场景

**数据流**：
```
应用 → DataShareHelper → DataShare Service → RDB(数据存储)
```

### 非静默访问 (Non-Silent Access)

非静默访问通过 DataShareExtensionAbility 实现。

**特点**：
- 需要实现并配置 DataShareExtensionAbility
- 数据提供方可以自定义业务逻辑
- 支持更细粒度的权限控制

**数据流**：
```
应用 A (DataShareHelper) → IPC → 应用 B (DataShareExtAbility) → RDB(数据存储)
```

详细说明见 [docs/architecture.md](docs/architecture.md#两种访问模式)。

## 适用场景

- **错误码分析**：根据错误码/错误日志定位问题
- **新功能开发**：在 Datashare 模块上添加新功能
- **代码阅读**：理解 Datashare 模块的架构和实现
- **问题排查**：分析数据共享相关的 IPC 通信、权限、数据库等问题

## 核心参考

### 核心接口文件

**说明**：以下为仓内相对路径，根据仓名在本地对应仓库中查找。

| 接口 | 仓名 | 文件路径 |
|------|------|---------|
| DataShareHelper | distributeddatamgr_data_share | `frameworks/native/consumer/include/datashare_helper.h` |
| DataShareExtAbility | distributeddatamgr_data_share | `frameworks/native/provider/include/datashare_ext_ability.h` |
| 错误码 | distributeddatamgr_data_share | `interfaces/inner_api/common/include/datashare_errno.h` |
| ResultSet | distributeddatamgr_data_share | `interfaces/inner_api/common/include/basic/result_set.h` |
| DataObsManager | ability_ability_runtime | `interfaces/inner_api/dataobs_manager/include/dataobs_mgr_client.h` |
| IDataAbilityObserver | ability_ability_runtime | `interfaces/inner_api/dataobs_manager/include/data_ability_observer_interface.h` |
| DataProxyHandle (配置共享) | distributeddatamgr_data_share | `frameworks/native/consumer/include/datashare_helper.h` |

### URI 格式

```
静默访问：datashareproxy://<bundleName>/<dataKey>
非静默访问：datashare://<bundleName>/<extensionAbilityName>/<dataKey>
配置共享：datashareproxy://<bundleName>/<path>
```

### 日志 Domain

DataShare: `0xD001651`
DataObs: `0xD001312`

日志分析方法见 [docs/troubleshooting.md](docs/troubleshooting.md#错误日志分析方法)。

### 构建命令

构建命令见 [docs/build_guide.md](docs/build_guide.md)。

## 快速参考

| 文档 | 路径 |
|------|------|
| 配置共享特性 | [docs/share_config.md](docs/share_config.md) |
| └─ C++ 接口实现 | [docs/implementation/config_data/cpp_interfaces.md](docs/implementation/config_data/cpp_interfaces.md) |
| └─ 权限校验 | [docs/implementation/config_data/permission_check.md](docs/implementation/config_data/permission_check.md) |
| └─ 数据存储 | [docs/implementation/config_data/storage_structure.md](docs/implementation/config_data/storage_structure.md) |
| └─ 静态配置加载 | [docs/implementation/config_data/static_config_loading.md](docs/implementation/config_data/static_config_loading.md) |
| 问题定位 | [docs/troubleshooting.md](docs/troubleshooting.md) |
| 架构详解 | [docs/architecture.md](docs/architecture.md) |
| 代码结构 | [docs/code_structure.md](docs/code_structure.md) |
| 构建指南 | [docs/build_guide.md](docs/build_guide.md) |
| 代码仓清单 | [repositories.md](repositories.md) |

## 官方文档

### 在线文档

- [DataShare 总览](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/database/data-share-overview.md)
- [非静默连接（ExtensionAbility）](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/database/share-data-by-datashareextensionability-sys.md)
- [静默连接](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/database/share-data-by-silent-access-sys.md)
- [应用间配置共享](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/database/share-config.md)
- [ArkTS 接口文档](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkdata/js-apis-data-dataShare-sys.md)

### 本地文档

拉取 docs 仓后，文档位于：`docs/zh-cn/application-dev/database/`

```bash
git clone https://gitcode.com/openharmony/docs.git
```

问题定位指南见 [docs/troubleshooting.md](docs/troubleshooting.md)。