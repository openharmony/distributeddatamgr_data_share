# AGENTS.md

本文件是 AI Agent 处理本仓库任务时的轻量入口。先读本文件，再按任务类型加载 skills 文档。

# 阅读策略

不要一开始就读取 `docs/` 下的所有文件。

默认只读本文件。涉及需求设计或代码开发时，最多按需加载：

1. 如果影响范围不清楚，读取 `docs/datashare_general/architecture.md` 了解两种访问模式。
2. 如果涉及错误码分析，读取 `docs/datashare_general/troubleshooting.md`。
3. 规划验证时，见本文件"编译和测试方法"章节。

# 仓库定位

`data_share` 是 OpenHarmony 数据共享组件。在 OpenHarmony 源码树中的位置：

```text
//foundation/distributeddatamgr/data_share
```

组件元信息：

- 子系统：`distributeddatamgr`
- 部件：`data_share`
- 主要能力面：跨应用数据共享、静默访问、非静默访问（DataShareExtAbility）、数据订阅通知、ResultSet 共享内存结果集。

基于 IPC 的跨应用数据共享框架，为应用提供数据共享能力。主要实现语言是 C++，通过 NAPI 暴露到 ArkTS/JavaScript，同时包含 ETS/ANI 实现、Cangjie FFI、Rust FFI 和测试代码。

# 核心架构

## 两种访问模式

| 模式 | 特点 | URI 格式 | 数据流 |
|------|------|----------|--------|
| 静默访问 | 无需 ExtensionAbility，由 DataShare Service 直接管理数据 | `datashareproxy://<bundleName>/<dataKey>` | 应用 → DataShareHelper → DataShare Service → RDB |
| 非静默访问 | 需 DataShareExtAbility，支持自定义业务逻辑和细粒度权限控制 | `datashare://<bundleName>/<extAbility>/<dataKey>` | 应用 → DataShareHelper → IPC → DataShareExtAbility → RDB |

## 依赖服务

- **DataShare Service**：运行于 `ddms` 进程（distributeddatamgr_datamgr_service 仓）
- **DataObs Service**：数据观察者通知（ability_ability_runtime 仓）

# 代码地图

## 代码分层

```
data_share/
├ frameworks/                  # 实现代码
│   ├── native/               # C++ 原生实现
│   │   ├── common/           # 公共代码（ResultSet、Predicate、URI 工具等）
│   │   ├── consumer/         # 消费端（DataShareHelper）
│   │   ├── provider/         # 提供端（DataShareExtAbility、Stub）
│   │   ├── proxy/            # 数据代理（订阅管理、Service Proxy）
│   │   ├── permission/       # 权限验证
│   │   └── dfx/              # DFX（故障/雷达报告）
│   ├── js/napi/              # JS NAPI 绑定
│   ├── js/ani/               # JS ANI 绑定
│   ├── ets/ani/              # ETS ANI 实现
│   ├── cj/ffi/               # Cangjie FFI
│   └── rust/ffi/             # Rust FFI 绑定
│
├ interfaces/inner_api/        # 内部 API
│   ├── common/               # 公共接口（errno、ResultSet、Predicate、ValueBucket）
│   ├── consumer/             # 消费端接口（DataShareHelper、DataProxyHandle）
│   ├── provider/             # 提供端接口（ResultSetBridge）
│   └── permission/           # 权限接口
│
└ test/                        # 测试代码
    ├── native/unittest/      # C++ 单元测试
    ├── js/data_share/        # JS 测试
    ├── ets/                  # ETS 测试
    └ fuzztest/               # Fuzz 测试
```

**分层原则**：

- `interfaces/inner_api/`：模块间接口，内部使用，**NEVER** 破坏兼容。
- `frameworks/`：实现层，不影响原有功能可重构。
- 修改 `frameworks` 层代码 **MUST** 评估对 `interfaces` 层影响。

## 任务路由表

| 任务类型 | 主要改动路径 | MUST 检查的上层 | 对应测试 |
|----------|--------------|----------------|----------|
| Consumer 核心逻辑 | `frameworks/native/consumer/` | `frameworks/js/napi/`、`frameworks/ets/ani/` | `test/native/unittest/mediadatashare_test/` |
| Provider 核心逻辑 | `frameworks/native/provider/` | `frameworks/js/napi/datashare_ext_ability/` | `test/native/unittest/mediadatashare_test/` |
| 公共组件（ResultSet/Predicate） | `frameworks/native/common/` | `interfaces/inner_api/common/` | `test/native/unittest/` |
| JS/NAPI 接口 | `frameworks/js/napi/` | 无上层 | `test/js/data_share/` |
| ETS/ANI 接口 | `frameworks/ets/ani/` | 无上层 | `test/ets/` |
| Rust FFI | `frameworks/rust/ffi/` | 无上层 | `frameworks/rust/ffi/*/tests/` |
| 权限验证 | `frameworks/native/permission/` | `frameworks/native/consumer/` | `test/native/unittest/mediadatashare_test/src/permission_test.cpp` |

# 知识路由

## 触发式文档加载

| 触发词/路径模式 | 加载文档 | 加载后 MUST 检查 |
|-----------------|----------|------------------|
| `错误码` / `errno` / `E_OK` / `datashare_errno.h` / 修改返回值 | `docs/datashare_general/troubleshooting.md` | 存量接口返回码不变；新增错误码仅用于新增接口 |
| `静默访问` / `非静默访问` / `Silent` / `datashareproxy` | `docs/datashare_general/architecture.md` | 区分两种模式的数据流路径 |
| `DataObs` / `Observer` / `订阅` / `通知` | `docs/datashare_general/architecture.md`（Observer Pattern） | DataObs 依赖 ability_ability_runtime 仓 |
| `URI` / `datashare://` / `datashareproxy://` | `docs/datashare_general/architecture.md` | URI 格式决定访问模式 |

# 专家经验

## 核心原则

1. 修改存量接口 **MUST** 评估对上层 NAPI/ANI/FFI 绑定的影响。
2. 新增外部依赖 **MUST** 检查是否已通过动态加载方式实现。
3. 用户说"调研一下" **MUST NOT** 直接实现 — 先调研再决定。
4. Claim "done" **MUST** 有测试验证 — 所改模块对应 UT 跑通且断言全过才算 done。
5. 接口层 API **NEVER** 破坏向后兼容。

## 编码规范

| 维度 | 约定 |
|------|------|
| 不可变性 | 接口层 API（inner_api）**NEVER** 破坏向后兼容 |
| 错误处理 | **MUST** 使用 `datashare_errno.h` 标准错误码；**NEVER** 让存量接口返回新错误码 |
| 日志 | DataShare Domain: `0xD001651`；DataObs Domain: `0xD001312` |
| 工具类复用 | **MUST** 优先使用 `kv_store:datamgr_common` 工具类（ITypesUtil 等），**NEVER** 自行实现已有同等能力 |
| 定义复用 | 新增映射表、函数前 **MUST** 先检查 `frameworks/native/common/` 是否存在可复用定义 |

Commit 信息 **MUST** 包含 `Co-Authored-By: Agent`。

## 已知陷阱

- **MUST** 考虑 SA 进程（ddms）未启动场景，做容错处理。
- **MUST** 考虑 DataObs 服务未就绪场景（E_DATA_OBS_NOT_READY）。
- 跨进程访问通过 IPC，**NEVER** 忽略 IPC 载荷限制。

# 编译和测试方法

> **MUST**：修改类任务完成后 MUST 执行以下验证步骤。

## 修改类任务验证步骤（MUST 执行）

1. **格式化**（提交前必跑）：
   ```bash
   ${OHOS_ROOT}/prebuilts/clang/ohos/linux-x86_64/llvm/bin/clang-format --style=file -i <修改的文件>
   ```

2. **编译构建**：
   ```bash
   ./build.sh --product-name rk3568 --build-target data_share
   ```

3. **单元测试**：
   ```bash
   ./build.sh --product-name rk3568 --build-target foundation/distributeddatamgr/data_share/test/unittest/native:unittest
   ```

> Claim "done" MUST 有以上 3 步验证结果。

# 技能文档索引

详细文档见 `docs/datashare_general/` 和 `docs/skills/`：

| 文档 | 路径 | 适用场景 |
|------|------|----------|
| 架构详解 | `docs/datashare_general/architecture.md` | 两种访问模式、Observer Pattern |
| 代码结构 | `docs/datashare_general/code_structure.md` | 核心文件位置、跨仓依赖 |
| 构建指南 | `docs/datashare_general/build_guide.md` | 编译命令详解 |
| 问题定位 | `docs/datashare_general/troubleshooting.md` | 错误码速查、日志分析 |
| 技能定义 | `docs/skills/SKILL.md` | Skill 入口、代码仓管理原则 |
| 代码仓清单 | `docs/skills/repositories.md` | 依赖仓列表 |