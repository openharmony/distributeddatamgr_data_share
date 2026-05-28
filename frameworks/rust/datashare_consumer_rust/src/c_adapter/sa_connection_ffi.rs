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

//! FFI functions for DataShareSAConnection lifecycle management.
//!
//! Corresponds to C++ `datashare_sa_connection.cpp` `#ifdef DATASHARE_USE_RUST_IMPL` path.
//! C++ holds a `void* rustConnectionInner_` which is a `Box<DataShareSAConnection>`.

use std::any::Any;
use std::ffi::c_void;

use ipc::parcel::MsgParcel;
use ipc::remote::RemoteObj;
use samgr::manage::SystemAbilityManager;

use super::types_ffi::c_str_to_rust;
use crate::connection::datashare_sa_connection::{DataShareSAConnection, DATA_SHARE_ERROR};
use crate::connection::manager_impl::DataShareManagerImpl;

/// Create a Rust DataShareSAConnection. Returns 0 on success.
///
/// # Safety
/// All pointer parameters must be valid for the specified lengths.
#[no_mangle]
pub unsafe extern "C" fn DataShareSAConnectionCreate(
    uri_ptr: *const u8,
    uri_len: u32,
    sa_id: i32,
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
    let conn = DataShareSAConnection::new(uri.to_string(), sa_id, wait_time);
    *out_handle = Box::into_raw(Box::new(conn)) as *mut c_void;
    0
}

/// Destroy a Rust DataShareSAConnection and free its memory.
///
/// # Safety
/// `handle` must be a valid pointer returned by `DataShareSAConnectionCreate`,
/// or null (in which case this is a no-op).
#[no_mangle]
pub unsafe extern "C" fn DataShareSAConnectionDestroy(handle: *mut c_void) {
    if !handle.is_null() {
        drop(Box::from_raw(handle as *mut DataShareSAConnection));
    }
}

/// Connect if needed and write the proxy IRemoteObject into `out_parcel`.
/// Returns 0 on success, negative on failure.
///
/// # Safety
/// `handle` must be valid; `out_parcel` must be a valid C++ MessageParcel*.
#[no_mangle]
pub unsafe extern "C" fn DataShareSAConnectionGetProxyRemote(
    handle: *mut c_void,
    out_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || out_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let conn = &*(handle as *const DataShareSAConnection);
    let mut parcel = MsgParcel::from_ptr(out_parcel as *mut _);
    conn.write_proxy_remote(&mut parcel)
}

/// Notify Rust that the remote SA died: clears cached proxy.
///
/// # Safety
/// `handle` must be a valid pointer returned by `DataShareSAConnectionCreate`.
#[no_mangle]
pub unsafe extern "C" fn DataShareSAConnectionOnRemoteDied(handle: *mut c_void) {
    if handle.is_null() {
        return;
    }
    let conn = &*(handle as *const DataShareSAConnection);
    conn.on_remote_died();
}

const E_OK: i32 = 0;

/// Fetch interface info (descriptor + code) via DataShareServiceProxy.
/// Writes UTF-8 descriptor bytes to `out_desc_buf` (up to `buf_len`).
/// `*out_desc_len` receives actual byte length; `*out_code` receives IPC code.
/// Returns errCode (0 = E_OK).
///
/// # Safety
/// All pointer parameters must be valid. `out_desc_buf` must have capacity `buf_len`.
#[no_mangle]
pub unsafe extern "C" fn DataShareSAConnectionFetchInterfaceInfo(
    sa_id: i32,
    wait_time: u32,
    out_desc_buf: *mut u8,
    buf_len: u32,
    out_desc_len: *mut u32,
    out_code: *mut u32,
) -> i32 {
    if out_desc_buf.is_null() || out_desc_len.is_null() || out_code.is_null() {
        return DATA_SHARE_ERROR;
    }
    let svc = match DataShareManagerImpl::get_service_proxy() {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let (err_code, code, descriptor) = match svc.get_connection_interface_info(sa_id, wait_time) {
        Some(t) => t,
        None => return DATA_SHARE_ERROR,
    };
    if err_code != E_OK {
        return err_code;
    }
    let desc_bytes = descriptor.as_bytes();
    if desc_bytes.len() > buf_len as usize {
        return DATA_SHARE_ERROR;
    }
    std::ptr::copy_nonoverlapping(desc_bytes.as_ptr(), out_desc_buf, desc_bytes.len());
    *out_desc_len = desc_bytes.len() as u32;
    *out_code = code;
    E_OK
}

/// Check / load a system ability via samgr. Writes the SA's IRemoteObject
/// into `out_parcel`. Returns 0 on success.
///
/// # Safety
/// `out_parcel` must be a valid C++ MessageParcel*.
#[no_mangle]
pub unsafe extern "C" fn DataShareSAConnectionCheckAndLoadSA(
    sa_id: i32,
    wait_time: u32,
    out_parcel: *mut c_void,
) -> i32 {
    if out_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let remote = SystemAbilityManager::check_system_ability(sa_id)
        .or_else(|| SystemAbilityManager::load_system_ability(sa_id, wait_time as i32));
    let remote = match remote {
        Some(r) => r,
        None => return DATA_SHARE_ERROR,
    };
    let mut parcel = MsgParcel::from_ptr(out_parcel as *mut _);
    if parcel.write_remote(remote).is_err() {
        return DATA_SHARE_ERROR;
    }
    E_OK
}

/// Connect to an SA-hosted provider: load SA, send IPC with interface
/// token + URI, read the provider proxy remote. Writes the proxy
/// IRemoteObject into `out_parcel`. Returns 0 on success.
///
/// # Safety
/// All pointer parameters must be valid for the specified lengths.
/// `out_parcel` must be a valid C++ MessageParcel*.
#[no_mangle]
pub unsafe extern "C" fn DataShareSAConnectionConnectToProviderFFI(
    uri_ptr: *const u8,
    uri_len: u32,
    desc_ptr: *const u8,
    desc_len: u32,
    code: u32,
    sa_id: i32,
    wait_time: u32,
    out_parcel: *mut c_void,
) -> i32 {
    if out_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let descriptor = match c_str_to_rust(desc_ptr, desc_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };

    let remote = SystemAbilityManager::check_system_ability(sa_id)
        .or_else(|| SystemAbilityManager::load_system_ability(sa_id, wait_time as i32));
    let remote = match remote {
        Some(r) => r,
        None => return DATA_SHARE_ERROR,
    };

    let mut data = MsgParcel::new();
    if data.write_interface_token(descriptor).is_err() {
        return DATA_SHARE_ERROR;
    }
    if data.write(&uri.to_string()).is_err() {
        return DATA_SHARE_ERROR;
    }

    let mut reply = match remote.send_request(code, &mut data) {
        Ok(r) => r,
        Err(_) => return DATA_SHARE_ERROR,
    };

    let err_code: i32 = match reply.read() {
        Ok(v) => v,
        Err(_) => return DATA_SHARE_ERROR,
    };
    if err_code != E_OK {
        return err_code;
    }
    let proxy_remote: RemoteObj = match reply.read_remote() {
        Ok(r) => r,
        Err(_) => return DATA_SHARE_ERROR,
    };

    let mut parcel = MsgParcel::from_ptr(out_parcel as *mut _);
    if parcel.write_remote(proxy_remote).is_err() {
        return DATA_SHARE_ERROR;
    }
    E_OK
}

/// Death-recipient callback type for LinkToDeath FFI.
type SADeathCallback = unsafe extern "C" fn(*mut c_void);

/// Register a death recipient on a remote object (read from `remote_in_parcel`).
/// On death, invokes `cb(context)`. The recipient handle is stored in the Rust
/// connection instance (`handle`) to prevent premature drop.
/// Returns 0 on success.
///
/// # Safety
/// `handle` must be a valid `DataShareSAConnection` pointer.
/// `remote_in_parcel` must be a valid C++ MessageParcel* containing a remote object.
/// `context` must remain valid for the lifetime of the connection.
#[no_mangle]
pub unsafe extern "C" fn DataShareSAConnectionLinkToDeathFFI(
    handle: *mut c_void,
    remote_in_parcel: *mut c_void,
    cb: SADeathCallback,
    context: *mut c_void,
) -> i32 {
    if handle.is_null() || remote_in_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let conn = &*(handle as *const DataShareSAConnection);
    let mut parcel = MsgParcel::from_ptr(remote_in_parcel as *mut _);
    let remote = match parcel.read_remote() {
        Ok(r) => r,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let ctx_addr = context as usize;
    let handle_opt = remote.add_death_recipient(move |_| unsafe {
        cb(ctx_addr as *mut c_void);
    });
    conn.set_external_death_handle(handle_opt.map(|h| Box::new(h) as Box<dyn Any>));
    E_OK
}
