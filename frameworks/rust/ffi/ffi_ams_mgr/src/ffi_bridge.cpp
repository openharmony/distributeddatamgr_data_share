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

#include "ffi_ability_mgr_bridge.h"
#include "wrapper.rs.h"

#include <cstdint>
#include <dlfcn.h>

// 前向声明已有的 extern "C" 函数
extern "C" {
int32_t DataShareAbilityMgrConnect(
    const char* uriPtr, uint32_t uriLen, void* connectRemote, void* callerToken);
int32_t DataShareAbilityMgrDisconnect(void* connectRemote);

struct DataShareConnectionCallbacks {
    void (*on_connect)(void*, void*, int32_t);
    void (*on_disconnect)(void*, int32_t);
};

void* DataShareConnectionConnectExt(
    const char* uriPtr, uint32_t uriLen, void* callerToken, void* context,
    const DataShareConnectionCallbacks* callbacks);
void DataShareConnectionDisconnect(void* connectionHandle);
}

namespace OHOS {
namespace DataShare {
namespace FfiAbilityMgr {

// ========== dlopen fallback 机制 ==========

static void* LoadConsumerSym(const char* name)
{
    static void* handle = nullptr;
    if (!handle) {
#ifdef __LP64__
        handle = dlopen("/system/lib64/platformsdk/libdatashare_consumer.z.so", RTLD_NOW);
#else
        handle = dlopen("/system/lib/platformsdk/libdatashare_consumer.z.so", RTLD_NOW);
#endif
    }
    if (!handle) {
        return nullptr;
    }
    return dlsym(handle, name);
}

// ========== ConnectionHandle 回调转发 ==========

// C 回调函数，context 指向 ConnectionHandle
static void OnConnectCCallback(void* context, void* remoteObject, int32_t resultCode)
{
    auto* handle = static_cast<ConnectionHandle*>(context);
    if (handle) {
        handle->NotifyConnect(remoteObject, resultCode);
    }
}

static void OnDisconnectCCallback(void* context, int32_t resultCode)
{
    auto* handle = static_cast<ConnectionHandle*>(context);
    if (handle) {
        handle->NotifyDisconnect(resultCode);
    }
}

// ========== ConnectionHandle 实现 ==========

ConnectionHandle::ConnectionHandle(rust::Box<AbilityMgrCallback> callback, void* rawHandle)
    : callback_(std::move(callback)), rawHandle_(rawHandle) {}

ConnectionHandle::~ConnectionHandle()
{
    if (rawHandle_) {
        // 尝试直接调用
        typedef void (*DisconnectFn)(void*);
        static DisconnectFn realFn = nullptr;
        if (!realFn) {
            realFn = reinterpret_cast<DisconnectFn>(
                LoadConsumerSym("DataShareConnectionDisconnect"));
        }
        if (realFn) {
            realFn(rawHandle_);
        } else {
            DataShareConnectionDisconnect(rawHandle_);
        }
        rawHandle_ = nullptr;
    }
}

void ConnectionHandle::NotifyConnect(void* remoteObject, int32_t resultCode)
{
    callback_->on_connect(reinterpret_cast<uint64_t>(remoteObject), resultCode);
}

void ConnectionHandle::NotifyDisconnect(int32_t resultCode)
{
    callback_->on_disconnect(resultCode);
}

// ========== 桥接函数实现 ==========

int32_t AbilityMgrConnect(rust::Str uri, uint64_t connectRemote, uint64_t callerToken)
{
    return DataShareAbilityMgrConnect(
        uri.data(), static_cast<uint32_t>(uri.size()),
        reinterpret_cast<void*>(connectRemote),
        reinterpret_cast<void*>(callerToken));
}

int32_t AbilityMgrDisconnect(uint64_t connectRemote)
{
    return DataShareAbilityMgrDisconnect(reinterpret_cast<void*>(connectRemote));
}

std::unique_ptr<ConnectionHandle> ConnectionConnectExt(
    rust::Str uri, uint64_t callerToken, rust::Box<AbilityMgrCallback> callback)
{
    // 创建 ConnectionHandle（先不设置 rawHandle）
    auto handle = std::make_unique<ConnectionHandle>(std::move(callback), nullptr);

    DataShareConnectionCallbacks cbs = { OnConnectCCallback, OnDisconnectCCallback };

    // 尝试通过 dlopen 获取真实符号
    typedef void* (*ConnectExtFn)(const char*, uint32_t, void*, void*,
        const DataShareConnectionCallbacks*);
    static ConnectExtFn realFn = nullptr;
    if (!realFn) {
        realFn = reinterpret_cast<ConnectExtFn>(
            LoadConsumerSym("DataShareConnectionConnectExt"));
    }

    void* rawHandle = nullptr;
    if (realFn) {
        rawHandle = realFn(
            uri.data(), static_cast<uint32_t>(uri.size()),
            reinterpret_cast<void*>(callerToken),
            static_cast<void*>(handle.get()), &cbs);
    } else {
        rawHandle = DataShareConnectionConnectExt(
            uri.data(), static_cast<uint32_t>(uri.size()),
            reinterpret_cast<void*>(callerToken),
            static_cast<void*>(handle.get()), &cbs);
    }

    if (!rawHandle) {
        return nullptr;
    }

    // 设置 rawHandle（ConnectionHandle 构造时传了 nullptr，现在补上）
    handle->rawHandle_ = rawHandle;
    return handle;
}

void ConnectionDisconnect(const ConnectionHandle& handle)
{
    // const_cast 因为 CXX 传 &self 为 const ref，但断开需要修改内部状态
    auto* mutableHandle = const_cast<ConnectionHandle*>(&handle);
    if (mutableHandle->RawHandle()) {
        typedef void (*DisconnectFn)(void*);
        static DisconnectFn realFn = nullptr;
        if (!realFn) {
            realFn = reinterpret_cast<DisconnectFn>(
                LoadConsumerSym("DataShareConnectionDisconnect"));
        }
        if (realFn) {
            realFn(mutableHandle->RawHandle());
        } else {
            DataShareConnectionDisconnect(mutableHandle->RawHandle());
        }
        mutableHandle->rawHandle_ = nullptr;
    }
}

} // namespace FfiAbilityMgr
} // namespace DataShare
} // namespace OHOS
