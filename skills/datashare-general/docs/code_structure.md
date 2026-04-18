# Datashare 代码结构详解

本文档详细描述各代码仓的目录结构、核心文件位置和能力说明。

---

## distributeddatamgr_data_share（客户端 + Provider 端）

**代码结构**：
```
data_share/
├── frameworks/
│   ├── js/napi/      # JS NAPI 绑定
│   └── native/
│       ├── common/   # 公共代码
│       ├── consumer/ # 消费者实现（DataShareHelper）
│       └── provider/ # 提供者实现（DataShareExtAbility）
├── interfaces/inner_api/
│   ├── common/       # 公共头文件（错误码、ResultSet 等）
│   ├── consumer/     # 消费者头文件
│   └── provider/     # 提供者头文件
└── test/             # 测试代码
```

**核心文件**：
| 文件 | 路径 | 说明 |
|------|------|------|
| DataShareHelper | `frameworks/native/consumer/include/datashare_helper.h` | 客户端帮助类，提供增删改查接口 |
| DataShareExtAbility | `frameworks/native/provider/include/datashare_ext_ability.h` | Provider 端基类 |
| 错误码 | `interfaces/inner_api/common/include/datashare_errno.h` | 错误码定义 |
| ResultSet | `interfaces/inner_api/common/include/basic/result_set.h` | 结果集接口 |
| DataProxyHandle | `frameworks/native/consumer/include/datashare_helper.h` | 配置共享句柄（API 20+） |

**核心能力**：
- **消费者（Consumer）**：提供 DataShareHelper，支持增删改查、观察者注册、文件操作等
- **提供者（Provider）**：提供 DataShareExtAbility，实现数据提供方的业务逻辑
- **ResultSet**：共享内存结果集，避免 IPC 大数据传输
- **配置共享（Share-Config）**：API 20+ 支持应用间配置数据共享（DataProxyHandle）

---

## distributeddatamgr_datamgr_service（服务端）

**代码结构**：
```
datamgr_service/
└── services/distributeddataservice/
    ├── adapter/   # 适配器层（Account、Communicator、DFX 等）
    ├── app/       # 应用层（Feature 管理系统）
    ├── framework/ # 框架层（AutoCache、MetaDataManager、EventCenter 等）
    └── service/   # 服务层
        ├── data_share/ # DataShare 功能
        ├── rdb/        # RDB 功能
        ├── kvdb/       # KVDB 功能
        └── ...
```

**核心文件**：
| 文件 | 路径 | 说明 |
|------|------|------|
| IDataShareService | `services/distributeddataservice/service/data_share/idata_share_service.h` | 服务接口 |
| DataShareServiceImpl | `services/distributeddataservice/service/data_share/data_share_service_impl.h` | 服务实现 |
| DataShareConfig | `services/distributeddataservice/service/config/include/model/datashare_config.h` | 配置模型定义 |

**核心能力**：
- **静默访问数据库操作**：直接管理数据，无需 ExtensionAbility
- **数据代理**：支持卡片等场景的数据代理访问
- **配置共享**：支持应用间配置数据的共享（DataProxyHandle 服务端支持）
- **权限校验**：URI 信任列表、allowList 访问控制

**构建命令**：
```bash
./build.sh --product-name <product> --gn-flags='--export-compile-commands' \
  --build-target foundation/distributeddatamgr/datamgr_service/services/distributeddataservice/service/data_share:data_share_service
./build.sh --product-name <product> --gn-flags='--export-compile-commands' \
  --build-target foundation/distributeddatamgr/datamgr_service/services/distributeddataservice/service/config:config_service
```

---

## ability_ability_runtime（DataObs）

**代码结构**：
```
ability_ability_runtime/
├── interfaces/inner_api/dataobs_manager/
│   ├── include/
│   │   ├── dataobs_mgr_client.h         # DataObs 客户端
│   │   ├── data_ability_observer_interface.h  # 观察者接口
│   │   └── ...
│   └── src/
│       └── dataobs_mgr_client.cpp
├── services/dataobsmgr/
│   ├── include/
│   │   ├── dataobs_mgr_service.h        # DataObs 服务
│   │   └── ...
│   └── src/
│       ├── dataobs_mgr_service.cpp
│       └── ...
└── test/
```

**核心文件**：
| 文件 | 路径 | 说明 |
|------|------|------|
| DataObsManager | `interfaces/inner_api/dataobs_manager/include/dataobs_mgr_client.h` | 客户端，提供注册/注销观察者、发布数据变更的能力 |
| IDataAbilityObserver | `interfaces/inner_api/dataobs_manager/include/data_ability_observer_interface.h` | 观察者接口，定义 OnChange() 回调 |
| DataObsService | `services/dataobsmgr/include/dataobs_mgr_service.h` | 服务端实现 |

**核心能力**：
- **数据订阅**：通过 `RegisterObserver` 注册观察者，订阅特定 URI 的数据变更
- **变更通知**：通过 `NotifyChange` 通知所有订阅该 URI 的观察者
- **跨进程通知**：支持跨应用的数据变更通知

**构建命令**：
```bash
./build.sh --product-name <product> --gn-flags='--export-compile-commands' \
  --build-target foundation/ability/ability_runtime/services/dataobsmgr:dataobsms
```

**与 Datashare 的关系**：
- DataShare 依赖 DataObs 实现数据变更通知
- DataShareHelper 通过 DataObsManager 注册观察者
- 数据变更时，DataShare 调用 DataObs 通知所有注册的观察者

---

## 配置共享（Share-Config）代码路径

### 客户端实现

| 模块 | 文件路径 | 说明 |
|------|---------|------|
| DataProxyHandle | `data_share/frameworks/ets/ani/src/cxx/dataproxy_handle_ani.cpp` | ArkTS 接口实现 |
| NAPI 绑定 | `data_share/frameworks/js/napi/dataShare/src/napi_dataproxy_handle.cpp` | JS NAPI 绑定 |

---

**相关文档**：
- [代码仓清单](../repositories.md) - 完整依赖仓列表（按需查阅）
- [配置共享特性](share_config.md) - Share-Config 详细说明