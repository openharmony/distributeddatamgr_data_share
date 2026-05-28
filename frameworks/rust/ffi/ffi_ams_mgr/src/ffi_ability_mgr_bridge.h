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

#ifndef DATASHARE_FFI_ABILITY_MGR_BRIDGE_H
#define DATASHARE_FFI_ABILITY_MGR_BRIDGE_H

#include "cxx.h"
#include <cstdint>
#include <memory>

namespace OHOS {
namespace DataShare {
namespace FfiAbilityMgr {

// Rust 回调类型的前向声明（CXX 生成）
struct AbilityMgrCallback;

/// C++ 不透明连接句柄。
class ConnectionHandle {
public:
    ConnectionHandle(rust::Box<AbilityMgrCallback> callback, void* rawHandle);
    ~ConnectionHandle();

    // 禁止拷贝
    ConnectionHandle(const ConnectionHandle&) = delete;
    ConnectionHandle& operator=(const ConnectionHandle&) = delete;

    void* RawHandle() const { return rawHandle_; }

    /// C 回调转发：连接建立。
    void NotifyConnect(void* remoteObject, int32_t resultCode);

    /// C 回调转发：连接断开。
    void NotifyDisconnect(int32_t resultCode);

private:
    rust::Box<AbilityMgrCallback> callback_;
    void* rawHandle_;

    // 允许自由函数访问 rawHandle_ 进行赋值
    friend std::unique_ptr<ConnectionHandle> ConnectionConnectExt(
        rust::Str uri, uint64_t callerToken, rust::Box<AbilityMgrCallback> callback);
    friend void ConnectionDisconnect(const ConnectionHandle& handle);
};

/// 通过 AbilityManager 连接 DataShare extension ability。
int32_t AbilityMgrConnect(rust::Str uri, uint64_t connectRemote, uint64_t callerToken);

/// 断开 AbilityManager 连接。
int32_t AbilityMgrDisconnect(uint64_t connectRemote);

/// 创建 AbilityConnectionStub 并发起连接。
std::unique_ptr<ConnectionHandle> ConnectionConnectExt(
    rust::Str uri, uint64_t callerToken, rust::Box<AbilityMgrCallback> callback);

/// 断开并释放连接句柄。
void ConnectionDisconnect(const ConnectionHandle& handle);

} // namespace FfiAbilityMgr
} // namespace DataShare
} // namespace OHOS

#endif // DATASHARE_FFI_ABILITY_MGR_BRIDGE_H
