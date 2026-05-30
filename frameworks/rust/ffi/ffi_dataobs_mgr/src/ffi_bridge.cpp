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

#include "ffi_dataobs_mgr_bridge.h"
#include "wrapper.rs.h"

// 前向声明已有的 extern "C" 函数（定义在 dataobs_mgr_wrapper.cpp 中）
extern "C" {
int32_t DataShareDataObsMgrRegisterObserver(const char *uriPtr, uint32_t uriLen, void *observerRemote);
int32_t DataShareDataObsMgrUnregisterObserver(const char *uriPtr, uint32_t uriLen, void *observerRemote);
int32_t DataShareDataObsMgrNotifyChange(const char *uriPtr, uint32_t uriLen);

int32_t DataShareDataObsMgrRegisterObserverExt(
    const char *uriPtr, uint32_t uriLen, void *observerRemote, bool isDescendants);
int32_t DataShareDataObsMgrUnregisterObserverExt(const char *uriPtr, uint32_t uriLen, void *observerRemote);
int32_t DataShareDataObsMgrNotifyChangeExt(const uint8_t *changeInfoData, uint32_t changeInfoLen);

int32_t DataShareDataObsMgrRegisterObserverExtWithOption(
    const char *uriPtr, uint32_t uriLen, void *observerRemote, bool isDescendants, bool isSystem);
int32_t DataShareDataObsMgrUnregisterObserverExtWithOption(
    const char *uriPtr, uint32_t uriLen, void *observerRemote, bool isSystem);
int32_t DataShareDataObsMgrNotifyChangeExtWithOption(
    const uint8_t *changeInfoData, uint32_t changeInfoLen, bool isSystem);
}

namespace OHOS {
namespace DataShare {
namespace FfiDataObsMgr {

int32_t RegisterObserver(rust::Str uri, uint64_t observerId)
{
    return DataShareDataObsMgrRegisterObserver(
        uri.data(), static_cast<uint32_t>(uri.size()), reinterpret_cast<void *>(observerId));
}

int32_t UnregisterObserver(rust::Str uri, uint64_t observerId)
{
    return DataShareDataObsMgrUnregisterObserver(
        uri.data(), static_cast<uint32_t>(uri.size()), reinterpret_cast<void *>(observerId));
}

int32_t NotifyChange(rust::Str uri)
{
    return DataShareDataObsMgrNotifyChange(uri.data(), static_cast<uint32_t>(uri.size()));
}

int32_t RegisterObserverExt(rust::Str uri, uint64_t observerId, bool isDescendants)
{
    return DataShareDataObsMgrRegisterObserverExt(
        uri.data(), static_cast<uint32_t>(uri.size()), reinterpret_cast<void *>(observerId), isDescendants);
}

int32_t UnregisterObserverExt(rust::Str uri, uint64_t observerId)
{
    return DataShareDataObsMgrUnregisterObserverExt(
        uri.data(), static_cast<uint32_t>(uri.size()), reinterpret_cast<void *>(observerId));
}

int32_t NotifyChangeExt(rust::Slice<const uint8_t> changeInfo)
{
    return DataShareDataObsMgrNotifyChangeExt(changeInfo.data(), static_cast<uint32_t>(changeInfo.size()));
}

int32_t RegisterObserverExtWithOption(rust::Str uri, uint64_t observerId, bool isDescendants, bool isSystem)
{
    return DataShareDataObsMgrRegisterObserverExtWithOption(uri.data(), static_cast<uint32_t>(uri.size()),
        reinterpret_cast<void *>(observerId), isDescendants, isSystem);
}

int32_t UnregisterObserverExtWithOption(rust::Str uri, uint64_t observerId, bool isSystem)
{
    return DataShareDataObsMgrUnregisterObserverExtWithOption(
        uri.data(), static_cast<uint32_t>(uri.size()), reinterpret_cast<void *>(observerId), isSystem);
}

int32_t NotifyChangeExtWithOption(rust::Slice<const uint8_t> changeInfo, bool isSystem)
{
    return DataShareDataObsMgrNotifyChangeExtWithOption(
        changeInfo.data(), static_cast<uint32_t>(changeInfo.size()), isSystem);
}

} // namespace FfiDataObsMgr
} // namespace DataShare
} // namespace OHOS
