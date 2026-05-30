// Copyright (c) 2026 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DATASHARE_FFI_DATAOBS_MGR_BRIDGE_H
#define DATASHARE_FFI_DATAOBS_MGR_BRIDGE_H

#include "cxx.h"
#include <cstdint>

namespace OHOS {
namespace DataShare {
namespace FfiDataObsMgr {

// 基础观察者操作
int32_t RegisterObserver(rust::Str uri, uint64_t observerId);
int32_t UnregisterObserver(rust::Str uri, uint64_t observerId);
int32_t NotifyChange(rust::Str uri);

// 扩展观察者操作（支持 descendants 标志）
int32_t RegisterObserverExt(rust::Str uri, uint64_t observerId, bool isDescendants);
int32_t UnregisterObserverExt(rust::Str uri, uint64_t observerId);
int32_t NotifyChangeExt(rust::Slice<const uint8_t> changeInfo);

// 带 DataObsOption 的扩展观察者操作（支持 isSystem 标志）
int32_t RegisterObserverExtWithOption(
    rust::Str uri, uint64_t observerId, bool isDescendants, bool isSystem);
int32_t UnregisterObserverExtWithOption(
    rust::Str uri, uint64_t observerId, bool isSystem);
int32_t NotifyChangeExtWithOption(
    rust::Slice<const uint8_t> changeInfo, bool isSystem);

} // namespace FfiDataObsMgr
} // namespace DataShare
} // namespace OHOS

#endif // DATASHARE_FFI_DATAOBS_MGR_BRIDGE_H
