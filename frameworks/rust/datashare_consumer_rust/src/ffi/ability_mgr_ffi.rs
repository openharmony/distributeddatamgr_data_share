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

//! FFI declarations for AbilityMgrProxy C++ wrapper.
//!
//! These functions are implemented in `ability_mgr_wrapper.cpp` and provide
//! a thin C interface to the C++ `AbilityMgrProxy` class and
//! an AbilityConnectionStub bridge for connect/disconnect callbacks.

use std::ffi::c_void;

/// Callback when ability connection is established.
pub type OnConnectCallback =
    unsafe extern "C" fn(context: *mut c_void, remote_object: *mut c_void, result_code: i32);

/// Callback when ability connection is lost.
pub type OnDisconnectCallback = unsafe extern "C" fn(context: *mut c_void, result_code: i32);

/// Callbacks bundle for DataShareConnectionConnectExt.
#[repr(C)]
pub struct DataShareConnectionCallbacks {
    pub on_connect: OnConnectCallback,
    pub on_disconnect: OnDisconnectCallback,
}

/// Observer registration flags for DataShareProxyRegisterObserverExtProvider.
pub const DATASHARE_OBSERVER_FLAG_DESCENDANTS: u32 = 1;
pub const DATASHARE_OBSERVER_FLAG_RECONNECT: u32 = 2;

extern "C" {
    pub fn DataShareAbilityMgrConnect(
        uri_ptr: *const u8,
        uri_len: u32,
        connect_remote: *mut c_void,
        caller_token: *mut c_void,
    ) -> i32;

    /// Disconnect from a DataShare extension ability.
    pub fn DataShareAbilityMgrDisconnect(connect_remote: *mut c_void) -> i32;

    /// Connect to a DataShare extension, creating an AbilityConnectionStub
    /// that bridges callbacks into Rust function pointers.
    ///
    /// Returns a connection handle (opaque pointer) on success, null on failure.
    /// The handle must be released via `DataShareConnectionDisconnect`.
    pub fn DataShareConnectionConnectExt(
        uri_ptr: *const u8,
        uri_len: u32,
        caller_token: *mut c_void,
        context: *mut c_void,
        callbacks: *const DataShareConnectionCallbacks,
    ) -> *mut c_void;

    /// Disconnect and release the connection stub created by `ConnectExt`.
    pub fn DataShareConnectionDisconnect(connection_handle: *mut c_void);

    /// Call IRemoteObject::SendRequest with raw MessageParcel pointers.
    /// Returns the IPC error code directly (0 = success).
    pub fn DataShareRemoteObjSendRequest(
        remote: *mut c_void,
        code: u32,
        data: *mut c_void,
        reply: *mut c_void,
    ) -> i32;

    /// Register a silent observer via DataObsMgrClient with DataObsOption(isSystem, true).
    /// observer_ptr is IDataAbilityObserver*.
    pub fn DataShareDataObsMgrRegisterObserverSilent(
        uri_ptr: *const u8,
        uri_len: u32,
        observer_ptr: *mut c_void,
    ) -> i32;

    /// Unregister a silent observer via DataObsMgrClient with DataObsOption(isSystem, true).
    /// observer_ptr is IDataAbilityObserver*.
    pub fn DataShareDataObsMgrUnregisterObserverSilent(
        uri_ptr: *const u8,
        uri_len: u32,
        observer_ptr: *mut c_void,
    ) -> i32;

    pub fn DataShareProxyRegisterObserver(
        proxy_remote: *mut c_void,
        uri_ptr: *const u8,
        uri_len: u32,
        observer_remote: *mut c_void,
    ) -> i32;

    pub fn DataShareProxyUnregisterObserver(
        proxy_remote: *mut c_void,
        uri_ptr: *const u8,
        uri_len: u32,
        observer_remote: *mut c_void,
    ) -> i32;

    pub fn DataShareProxyRegisterObserverExtProvider(
        proxy_remote: *mut c_void,
        uri_ptr: *const u8,
        uri_len: u32,
        observer_remote: *mut c_void,
        flags: u32,
    ) -> i32;

    pub fn DataShareProxyUnregisterObserverExtProvider(
        proxy_remote: *mut c_void,
        uri_ptr: *const u8,
        uri_len: u32,
        observer_remote: *mut c_void,
    ) -> i32;
}
