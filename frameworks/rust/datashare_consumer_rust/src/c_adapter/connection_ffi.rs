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

//! FFI functions for DataShareConnection lifecycle management.
//!
//! Corresponds to C++ `datashare_connection.cpp` `#ifdef DATASHARE_USE_RUST_IMPL` path.
//! C++ holds a `void* rustConnectionInner_` which is an `Arc<DataShareConnection>`.

use std::ffi::c_void;
use std::sync::Arc;

use super::types_ffi::c_str_to_rust;
use crate::connection::datashare_connection::DataShareConnection;

const DATA_SHARE_ERROR: i32 = -1;

/// Create a Rust DataShareConnection. Returns 0 on success.
///
/// # Safety
/// All pointer parameters must be valid for the specified lengths.
#[no_mangle]
pub unsafe extern "C" fn DataShareConnectionCreate(
    uri_ptr: *const u8,
    uri_len: u32,
    token: u64,
    wait_time: i32,
    out_handle: *mut *mut c_void,
) -> i32 {
    if out_handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let token_raw = token as *mut c_void;
    let remote_token = if token != 0 {
        ipc::remote::RemoteObj::from_ciremote(token as *mut _)
    } else {
        None
    };
    let conn = DataShareConnection::new_with_token_raw(
        uri.to_string(),
        remote_token,
        token_raw,
        wait_time,
    );
    *out_handle = Arc::into_raw(Arc::new(conn)) as *mut c_void;
    0
}

/// Destroy a Rust DataShareConnection and decrement its Arc refcount.
///
/// # Safety
/// `handle` must be a valid pointer returned by `DataShareConnectionCreate`,
/// or null (in which case this is a no-op).
#[no_mangle]
pub unsafe extern "C" fn DataShareConnectionDestroy(handle: *mut c_void) {
    if !handle.is_null() {
        drop(Arc::from_raw(handle as *const DataShareConnection));
    }
}

/// Connect to the extension (blocking) and return the proxy's raw IRemoteObject*.
/// Returns nullptr on failure or timeout.
///
/// # Safety
/// `handle` must be a valid pointer returned by `DataShareConnectionCreate`.
#[no_mangle]
pub unsafe extern "C" fn DataShareConnectionConnect(handle: *mut c_void) -> *mut c_void {
    if handle.is_null() {
        return std::ptr::null_mut();
    }
    let conn = &*(handle as *const DataShareConnection);
    match conn.get_data_share_proxy() {
        Some(_) => conn.get_proxy_remote_raw(),
        None => std::ptr::null_mut(),
    }
}

/// Get the current proxy's raw IRemoteObject* without triggering a connect.
/// Returns nullptr if no proxy is connected.
///
/// # Safety
/// `handle` must be a valid pointer returned by `DataShareConnectionCreate`.
#[no_mangle]
pub unsafe extern "C" fn DataShareConnectionGetProxyRemote(handle: *mut c_void) -> *mut c_void {
    if handle.is_null() {
        return std::ptr::null_mut();
    }
    let conn = &*(handle as *const DataShareConnection);
    conn.get_proxy_remote_raw()
}

/// Mark the connection as invalid.
///
/// # Safety
/// `handle` must be a valid pointer returned by `DataShareConnectionCreate`.
#[no_mangle]
pub unsafe extern "C" fn DataShareConnectionSetInvalid(handle: *mut c_void) {
    if handle.is_null() {
        return;
    }
    let conn = &*(handle as *const DataShareConnection);
    conn.set_connect_invalid();
}

/// Disconnect from the extension.
///
/// # Safety
/// `handle` must be a valid pointer returned by `DataShareConnectionCreate`.
#[no_mangle]
pub unsafe extern "C" fn DataShareConnectionDisconnect(handle: *mut c_void) {
    if handle.is_null() {
        return;
    }
    let conn = &*(handle as *const DataShareConnection);
    conn.disconnect();
}

/// Store observer info for re-registration on reconnect.
///
/// # Safety
/// All pointer parameters must be valid for the specified lengths.
#[no_mangle]
pub unsafe extern "C" fn DataShareConnectionUpdateObserverMap(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    observer_id: u64,
    is_descendants: bool,
) {
    if handle.is_null() {
        return;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return,
    };
    let conn = &*(handle as *const DataShareConnection);
    conn.update_observer_exts_provider_map(uri, observer_id, is_descendants);
}

/// Remove observer tracking info.
///
/// # Safety
/// All pointer parameters must be valid for the specified lengths.
#[no_mangle]
pub unsafe extern "C" fn DataShareConnectionDeleteObserverMap(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    observer_id: u64,
) {
    if handle.is_null() {
        return;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return,
    };
    let conn = &*(handle as *const DataShareConnection);
    conn.delete_observer_exts_provider_map(uri, observer_id);
}

/// Clone an Arc reference from a connection handle.
/// Used by controller FFI to share the connection with Rust controllers.
/// The caller must eventually call `DataShareConnectionDestroy` on the returned handle.
///
/// # Safety
/// `handle` must be a valid pointer returned by `DataShareConnectionCreate`.
pub unsafe fn connection_arc_clone(handle: *mut c_void) -> Option<Arc<DataShareConnection>> {
    if handle.is_null() {
        return None;
    }
    Arc::increment_strong_count(handle as *const DataShareConnection);
    Some(Arc::from_raw(handle as *const DataShareConnection))
}
