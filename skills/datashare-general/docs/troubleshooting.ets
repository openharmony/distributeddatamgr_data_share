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
# Datashare 问题定位指南

## 错误码速查

完整错误码定义在 `distributeddatamgr_data_share/interfaces/inner_api/common/include/datashare_errno.h`。

所有错误码基于 `E_BASE = 1000` 进行编号。

## 错误码分类

### 成功错误码

| 错误码 | 值 | 说明 |
|--------|-----|------|
| E_OK | 0 | 成功 |

### 一般异常（1001-1019）

| 错误码 | 值 | 说明 | 可能原因 | 解决方案 |
|--------|-----|------|----------|----------|
| E_ERROR | 1001 | 一般异常 | 未指定的错误 | 查看日志获取详细错误信息 |
| E_REGISTERED_REPEATED | 1002 | 重复注册 | 重复注册观察者 | 先注销再注册 |
| E_UNREGISTERED_EMPTY | 1003 | 注销空对象 | 注销不存在的观察者 | 检查是否正确注册 |

### 参数校验错误（1007-1041）

| 错误码 | 值 | 说明 | 可能原因 | 解决方案 |
|--------|-----|------|----------|----------|
| E_INVALID_STATEMENT | 1007 | 无效语句 | 谓词语句格式错误 | 检查谓词语法 |
| E_INVALID_COLUMN_INDEX | 1008 | 无效列索引 | 列索引越界 | 检查列索引范围 |
| E_INVALID_OBJECT_TYPE | 1020 | 无效对象类型 | 对象类型不匹配 | 检查数据类型 |
| E_INVALID_PARCEL | 1042 | 无效 Parcel | IPC 序列化失败 | 检查参数合法性 |
| E_INVALID_USER_ID | 1068 | 无效用户 ID | 用户 ID 格式错误 | 检查用户 ID 格式 |
| E_DATASHARE_INVALID_URI | 1078 | 无效 URI | URI 格式错误 | 检查 URI 格式 |
| E_EMPTY_URI | 1081 | URI 为空 | URI 参数为空 | 提供有效 URI |
| E_FIELD_ILLEGAL | 1087 | 字段非法 | 字段值不合法 | 检查字段值 |
| E_FIELD_INVALID | 1088 | 字段无效 | 字段不存在或无效 | 检查字段定义 |

### 资源和状态错误（1045-1062）

| 错误码 | 值 | 说明 | 可能原因 | 解决方案 |
|--------|-----|------|----------|----------|
| E_VERSION_NOT_NEWER | 1045 | 版本不新 | 版本过低 | 升级版本 |
| E_TEMPLATE_NOT_EXIST | 1046 | 模板不存在 | 模板未定义 | 检查模板名称 |
| E_SUBSCRIBER_NOT_EXIST | 1047 | 订阅者不存在 | 订阅者未注册 | 检查订阅者 |
| E_URI_NOT_EXIST | 1048 | URI 不存在 | URI 未注册 | 检查 URI 注册 |
| E_BUNDLE_NAME_NOT_EXIST | 1049 | Bundle 名不存在 | Bundle 未安装 | 安装应用 |
| E_BMS_NOT_READY | 1050 | BMS 未就绪 | Bundle 管理服务未启动 | 等待服务启动 |
| E_METADATA_NOT_EXISTS | 1051 | 元数据不存在 | 元数据未初始化 | 初始化元数据 |
| E_SILENT_PROXY_DISABLE | 1052 | 静默代理禁用 | 静默模式未启用 | 启用静默模式 |
| E_TOKEN_EMPTY | 1053 | Token 为空 | 权限 Token 缺失 | 申请权限 |
| E_EXT_URI_INVALID | 1054 | 扩展 URI 无效 | Extension URI 错误 | 检查 URI 配置 |
| E_DATA_SHARE_NOT_READY | 1055 | DataShare 未就绪 | 服务未启动 | 等待服务启动 |
| E_DB_ERROR | 1056 | 数据库错误 | SQLite 错误 | 查看数据库日志 |
| E_DATA_SUPPLIER_ERROR | 1057 | 数据提供者错误 | 提供者异常 | 检查提供者实现 |
| E_RESULTSET_BUSY | 1061 | ResultSet 忙 | 结果集被占用 | 等待释放 |
| E_APPINDEX_INVALID | 1062 | 无效 AppIndex | AppIndex 错误 | 检查 AppIndex |

### 序列化和通信错误（1058-1067）

| 错误码 | 值 | 说明 | 可能原因 | 解决方案 |
|--------|-----|------|----------|----------|
| E_MARSHAL_ERROR | 1058 | 序列化错误 | 对象序列化失败 | 检查对象结构 |
| E_UNMARSHAL_ERROR | 1059 | 反序列化错误 | 数据反序列化失败 | 检查数据格式 |
| E_WRITE_TO_PARCE_ERROR | 1060 | 写入 Parcel 错误 | IPC 写入失败 | 检查 IPC 参数 |
| E_PROVIDER_NOT_CONNECTED | 1066 | Provider 未连接 | 连接未建立 | 建立连接 |
| E_PROVIDER_CONN_NULL | 1067 | 连接为空 | 连接对象为空 | 检查连接初始化 |

### 生命周期和状态错误（1063-1077）

| 错误码 | 值 | 说明 | 可能原因 | 解决方案 |
|--------|-----|------|----------|----------|
| E_NULL_OBSERVER | 1063 | 空 Observer | Observer 为空指针 | 检查 Observer 初始化 |
| E_HELPER_DIED | 1064 | Helper 已释放 | Helper 已被销毁 | 重新创建 Helper |
| E_DATA_OBS_NOT_READY | 1065 | DataObs 未就绪 | DataObs 服务未启动 | 等待服务启动 |
| E_NULL_OBSERVER_CLIENT | 1084 | 空 Observer 客户端 | 客户端 Observer 为空 | 检查 Observer |
| E_NOT_SYSTEM_APP | 1069 | 非系统应用 | 缺少系统权限 | 申请系统权限 |
| E_NOT_HAP | 1075 | 非 HAP | 应用类型错误 | 检查应用类型 |
| E_GET_BUNDLEINFO_FAILED | 1076 | 获取 Bundle 信息失败 | Bundle 信息查询失败 | 检查 Bundle 状态 |
| E_NOT_DATASHARE_EXTENSION | 1077 | 非 DataShare 扩展 | 扩展类型错误 | 检查扩展配置 |

### 权限和安全错误（1079-1083）

| 错误码 | 值 | 说明 | 可能原因 | 解决方案 |
|--------|-----|------|----------|----------|
| E_VERIFY_FAILED | 1079 | 验证失败 | 安全验证失败 | 检查签名 |
| E_DATASHARE_PERMISSION_DENIED | 1080 | 权限拒绝 | 缺少 DataShare 权限 | 申请权限 |
| E_NOT_IN_TRUSTS | 1082 | 不在信任列表 | 应用不受信任 | 添加到信任列表 |
| E_GET_CALLER_NAME_FAILED | 1083 | 获取调用者失败 | 无法获取调用者信息 | 检查 IPC 调用 |

### 异步和超时错误（1070-1074）

| 错误码 | 值 | 说明 | 可能原因 | 解决方案 |
|--------|-----|------|----------|----------|
| E_REGISTER_ERROR | 1070 | 注册错误 | 观察者注册失败 | 检查注册条件 |
| E_NOTIFYCHANGE_ERROR | 1071 | 通知错误 | 变更通知失败 | 检查通知机制 |
| E_TIMEOUT_ERROR | 1072 | 超时错误 | 操作超时 | 增加超时时间 |
| E_ERROR_OVER_LIMIT_TASK | 1073 | 超出限制 | 任务数超限 | 减少并发 |
| E_EXECUTOR_POOL_IS_NULL | 1074 | 线程池为空 | 线程池未初始化 | 初始化线程池 |

### 其他错误

| 错误码 | 值 | 说明 | 可能原因 | 解决方案 |
|--------|-----|------|----------|----------|
| E_UNIMPLEMENT | 1085 | 未实现 | 虚函数未实现 | 实现接口 |
| E_DATASHARE_TYPE | 1086 | 类型错误 | DataShare 类型错误 | 检查类型配置 |

## 常见错误场景分析

### 场景 1：E_DATA_SHARE_NOT_READY (1055)

**现象**：调用 DataShare 接口时返回此错误

**可能原因**：
1. DataShare 服务未启动
2. 系统启动过程中服务尚未就绪

**解决方案**：
订阅公共事件 `DATA_SHARE_READY`，收到事件后再调用 DataShare 接口。

### 场景 2：E_DATASHARE_PERMISSION_DENIED (1080)

**现象**：访问其他应用数据时被拒绝

**可能原因**：
1. 未申请 DataShare 权限
2. 权限配置错误

**解决方案**：
1. 根据数据提供方在 module.json5 中的配置权限，数据获取方申请对应权限

### 场景 3：E_PROVIDER_NOT_CONNECTED (1066)

**现象**：无法连接到数据提供者

**可能原因**：
1. 提供者应用未安装
2. 提供者服务未启动
3. IPC 连接失败

**解决方案**：
1. 安装提供者应用
2. 检查提供者配置
3. 查看 IPC 日志

### 场景 4：E_DATASHARE_INVALID_URI (1078)

**现象**：URI 格式错误

**可能原因**：
1. URI 格式不符合规范
2. URI 缺少必要部分

**解决方案**：
1. 使用标准格式：`datashareproxy://<bundleName>/<dataKey>`（静默访问）或 `datashare://<bundleName>/<extensionAbilityName>/<dataKey>`（非静默访问）
2. 检查 URI 各组成部分

## 错误日志分析方法

### 1. 分析日志文件

用户通常直接提供日志文件，分析步骤如下：

```bash
# 查看 DataShare 相关日志 (domain: 0xD001651)
cat log.txt | grep "0xD001651"

# 查看 DataObs 相关日志 (domain: 0xD001312)
cat log.txt | grep "0xD001312"
```

### 2. 定位错误码

在 DataShare/DataObs 日志中搜索错误码：
```bash
# DataShare 错误码
cat log.txt | grep "0xD001651" | grep -E "E_[A-Z_]+"

# DataObs 错误码
cat log.txt | grep "0xD001312" | grep -E "E_[A-Z_]+"
```

找到错误码后，查阅上方的**错误码分类**表格。

### 3. 代码定位

1. 根据日志标签或错误类型确定涉及的代码仓
2. 查阅 [code_structure.md](code_structure.md) 找到核心文件路径
3. 直接阅读源代码理解逻辑
*/