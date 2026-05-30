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

//! FFI functions for DataShareHelper lifecycle management.
//!
//! Corresponds to C++ factory methods in `datashare_helper.cpp`
//! and constructor/destructor in `datashare_helper_impl.cpp`.

use super::types_ffi::{c_str_to_rust, DataShareHelperHandle};
use crate::connection::DataShareManagerImpl;
use crate::helper::datashare_helper_impl::DataShareHelperImpl;

const DATA_SHARE_ERROR: i32 = -1;

/// Create a non-silent (provider path) DataShareHelper.
///
/// # Safety
/// All pointer parameters must be valid for the specified lengths.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperCreateProvider(
    uri_ptr: *const u8,
    uri_len: u32,
    token: u64,
    is_system: bool,
    out_handle: *mut DataShareHelperHandle,
) -> i32 {
    if out_handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let token_raw = token as *mut std::ffi::c_void;
    let remote_token = if token != 0 {
        ipc::remote::RemoteObj::from_ciremote(token as *mut _)
    } else {
        None
    };
    let helper = DataShareHelperImpl::new_provider(uri, remote_token, token_raw, is_system);
    *out_handle = Box::into_raw(Box::new(helper));
    0
}

/// Create a silent (service path) DataShareHelper.
///
/// # Safety
/// All pointer parameters must be valid for the specified lengths.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperCreateSilent(
    ext_uri_ptr: *const u8,
    ext_uri_len: u32,
    is_system: bool,
    out_handle: *mut DataShareHelperHandle,
) -> i32 {
    if out_handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let ext_uri = match c_str_to_rust(ext_uri_ptr, ext_uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let helper = DataShareHelperImpl::new_service(ext_uri, is_system);
    *out_handle = Box::into_raw(Box::new(helper));
    0
}

/// Destroy a DataShareHelper instance and free its memory.
///
/// # Safety
/// `handle` must be a valid pointer returned by `DataShareHelperCreateProvider`
/// or `DataShareHelperCreateSilent`, and must not be used after this call.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperDestroy(handle: DataShareHelperHandle) {
    if !handle.is_null() {
        let _ = Box::from_raw(handle);
    }
}

/// Release the DataShareHelper resources (keeps the handle alive but clears controllers).
///
/// # Safety
/// `handle` must be a valid DataShareHelper handle.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperRelease(handle: DataShareHelperHandle) -> bool {
    if handle.is_null() {
        return false;
    }
    use crate::helper::datashare_helper::DataShareHelper;
    (*handle).release()
}

/// Set silent switch for a URI.
///
/// # Safety
/// `uri_ptr` must point to valid UTF-8 data of `uri_len` bytes.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperSetSilentSwitch(
    uri_ptr: *const u8,
    uri_len: u32,
    enable: bool,
    is_system: bool,
) -> i32 {
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    crate::helper::datashare_helper::set_silent_switch(uri, enable, is_system)
}

/// Get silent proxy status for a URI.
///
/// # Safety
/// `uri_ptr` must point to valid UTF-8 data of `uri_len` bytes.
#[no_mangle]
pub unsafe extern "C" fn DataShareGetSilentProxyStatus(
    uri_ptr: *const u8,
    uri_len: u32,
    is_system: bool,
) -> i32 {
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    crate::helper::datashare_helper::get_silent_proxy_status(uri, is_system)
}

/// Inject an already-connected ext ability proxy into the Rust DataShareHelper.
/// Called by C++ after it establishes the ability connection for the non-silent path.
/// `remote_obj` is a raw CIRemoteObject* pointer to the ext ability's IRemoteObject.
///
/// # Safety
/// `handle` must be a valid DataShareHelper handle.
/// `remote_obj` must be a valid CIRemoteObject* pointer.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperSetExtProxy(
    handle: DataShareHelperHandle,
    remote_obj: *mut std::ffi::c_void,
) {
    if handle.is_null() || remote_obj.is_null() {
        return;
    }
    if let Some(remote) = ipc::remote::RemoteObj::from_ciremote(remote_obj as *mut _) {
        (*handle).set_ext_proxy(remote);
    }
}

/// Set bundle name on the DataShareManagerImpl singleton.
///
/// # Safety
/// `name_ptr` must point to valid UTF-8 data of `name_len` bytes.
#[no_mangle]
pub unsafe extern "C" fn DataShareManagerSetBundleName(name_ptr: *const u8, name_len: u32) {
    let name = match c_str_to_rust(name_ptr, name_len) {
        Some(s) => s,
        None => return,
    };
    DataShareManagerImpl::get_instance().set_bundle_name(name);
}

/// Check if the DataShare service proxy is available.
#[no_mangle]
pub unsafe extern "C" fn DataShareManagerIsServiceProxyReady() -> bool {
    DataShareManagerImpl::get_service_proxy().is_some()
}

/// Set call count on the DataShareManagerImpl singleton.
///
/// # Safety
/// Pointer parameters must be valid for the specified lengths.
#[no_mangle]
pub unsafe extern "C" fn DataShareManagerSetCallCount(
    func_ptr: *const u8,
    func_len: u32,
    uri_ptr: *const u8,
    uri_len: u32,
) -> bool {
    let func = match c_str_to_rust(func_ptr, func_len) {
        Some(s) => s,
        None => return true,
    };
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return true,
    };
    DataShareManagerImpl::get_instance().set_call_count(func, uri)
}

/// Transfer URI prefix: replace `origin_prefix` with `replaced_prefix` in `origin_uri`.
///
/// # Safety
/// All pointer parameters must be valid for the specified lengths.
/// `out_buf` must have at least `buf_len` bytes available.
#[no_mangle]
pub unsafe extern "C" fn DataShareTransferUriPrefix(
    origin_prefix_ptr: *const u8,
    origin_prefix_len: u32,
    replaced_prefix_ptr: *const u8,
    replaced_prefix_len: u32,
    origin_uri_ptr: *const u8,
    origin_uri_len: u32,
    out_buf: *mut u8,
    buf_len: u32,
    out_len: *mut u32,
) -> bool {
    let origin_prefix = match c_str_to_rust(origin_prefix_ptr, origin_prefix_len) {
        Some(s) => s,
        None => return false,
    };
    let replaced_prefix = match c_str_to_rust(replaced_prefix_ptr, replaced_prefix_len) {
        Some(s) => s,
        None => return false,
    };
    let origin_uri = match c_str_to_rust(origin_uri_ptr, origin_uri_len) {
        Some(s) => s,
        None => return false,
    };

    let result = if origin_uri.starts_with(origin_prefix) {
        format!("{}{}", replaced_prefix, &origin_uri[origin_prefix.len()..])
    } else {
        origin_uri.to_string()
    };

    if out_buf.is_null() || out_len.is_null() {
        return false;
    }
    let result_bytes = result.as_bytes();
    if result_bytes.len() > buf_len as usize {
        return false;
    }
    std::ptr::copy_nonoverlapping(result_bytes.as_ptr(), out_buf, result_bytes.len());
    *out_len = result_bytes.len() as u32;
    true
}

/// Check if a URI represents a proxy data share.
///
/// # Safety
/// `uri_ptr` must point to valid UTF-8 data of `uri_len` bytes.
#[no_mangle]
pub unsafe extern "C" fn DataShareIsProxy(uri_ptr: *const u8, uri_len: u32) -> bool {
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return false,
    };
    uri.contains("Proxy=true") || uri.starts_with("datashareproxy://")
}

/// Get the helper type: SILENT(0) or NON_SILENT(1).
///
/// # Safety
/// `handle` must be a valid DataShareHelper handle.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperGetHelperType(handle: DataShareHelperHandle) -> i32 {
    if handle.is_null() {
        return 0; // default SILENT
    }
    (*handle).get_helper_type()
}
