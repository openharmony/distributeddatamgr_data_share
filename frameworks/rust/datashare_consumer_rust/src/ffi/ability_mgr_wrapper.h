/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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

#ifndef DATASHARE_ABILITY_MGR_WRAPPER_H
#define DATASHARE_ABILITY_MGR_WRAPPER_H

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

int32_t DataShareAbilityMgrConnect(const char* uriPtr, uint32_t uriLen,
    void* connectRemote, void* callerToken);
int32_t DataShareAbilityMgrDisconnect(void* connectRemote);

/// Callback types for AbilityConnectionStub bridge.
typedef void (*DataShareOnConnectCallback)(void* context, void* remoteObject, int32_t resultCode);
typedef void (*DataShareOnDisconnectCallback)(void* context, int32_t resultCode);

/// Callbacks bundle for DataShareConnectionConnectExt.
typedef struct {
    DataShareOnConnectCallback on_connect;
    DataShareOnDisconnectCallback on_disconnect;
} DataShareConnectionCallbacks;

/// Create an AbilityConnectionStub that calls back into Rust.
/// Returns a handle (IRemoteObject*) for the connection stub, or nullptr on failure.
void* DataShareConnectionConnectExt(
    const char* uriPtr, uint32_t uriLen,
    void* callerToken, void* context,
    const DataShareConnectionCallbacks* callbacks);

/// Disconnect and release the connection stub created by ConnectExt.
void DataShareConnectionDisconnect(void* connectionHandle);

/// Call IRemoteObject::SendRequest with raw MessageParcel pointers.
/// Returns the IPC error code directly (0 = success).
int32_t DataShareRemoteObjSendRequest(
    void* remote, uint32_t code, void* data, void* reply);

#ifdef __cplusplus
}
#endif

#endif // DATASHARE_ABILITY_MGR_WRAPPER_H
