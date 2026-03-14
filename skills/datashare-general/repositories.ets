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
# Datashare 模块代码仓清单

## 核心代码仓

| 序号 | 仓名 | GitCode 地址 | 功能描述 |
|------|------|-------------|---------|
| 1 | distributeddatamgr_data_share | https://gitcode.com/openharmony/distributeddatamgr_data_share | DataShare 客户端和 Provider 端实现 |
| 2 | distributeddatamgr_datamgr_service | https://gitcode.com/openharmony/distributeddatamgr_datamgr_service | 分布式数据管理服务（包含 DataShare Service） |
| 3 | ability_ability_runtime | https://gitcode.com/openharmony/ability_ability_runtime | DataObs（数据观察者），提供数据订阅和变更通知能力 |

完整仓清单和依赖关系见 [代码结构详解](docs/code_structure.md)。

---

## 依赖代码仓

| 分类 | 仓名 | GitCode 地址 |
|------|------|-------------|
| **存储相关** | kv_store | https://gitcode.com/openharmony/distributeddatamgr_kv_store |
|  | relational_store | https://gitcode.com/openharmony/distributeddatamgr_relational_store |
| **通信依赖** | dsoftbus | https://gitcode.com/openharmony/communication_dsoftbus |
|  | device_manager | https://gitcode.com/openharmony/distributedhardware_device_manager |
|  | netmanager_base | https://gitcode.com/openharmony/communication_netmanager_base |
| **安全依赖** | access_token | https://gitcode.com/openharmony/security_access_token |
|  | device_auth | https://gitcode.com/openharmony/security_device_auth |
|  | huks | https://gitcode.com/openharmony/security_huks |
| **日志和调试** | hilog | https://gitcode.com/openharmony/hiviewdfx_hilog |
|  | hisysevent | https://gitcode.com/openharmony/hiviewdfx_hisysevent |
|  | hicollie | https://gitcode.com/openharmony/hiviewdfx_hicollie |
| **系统服务** | samgr | https://gitcode.com/openharmony/systemabilitymgr_samgr |
|  | safwk | https://gitcode.com/openharmony/systemabilitymgr_safwk |
|  | common_event_service | https://gitcode.com/openharmony/notification_common_event_service |

---

## 拉取指南

### 按需拉取原则

| 场景 | 推荐拉取的仓 |
|------|-------------|
| 错误码/日志分析 | 核心仓 |
| 新功能开发 | 核心仓 |
| 涉及 RDB/KV 存储 | 核心仓 + relational_store/kv_store |

### 拉取命令

```bash
# 拉取核心仓
git clone https://gitcode.com/openharmony/distributeddatamgr_data_share.git
git clone https://gitcode.com/openharmony/distributeddatamgr_datamgr_service.git
git clone https://gitcode.com/openharmony/ability_ability_runtime.git

# 拉取官方文档仓
git clone https://gitcode.com/openharmony/docs.git
```

### 更新命令

```bash
cd <repo_name>
git pull origin master
```
*/