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

//! FFI functions for extension-specific DataShareHelper operations.
//!
//! These operations are only available on the provider (non-silent) path:
//! - InsertExt (string result)
//! - ExecuteBatch
//! - RegisterObserverExtProvider / UnregisterObserverExtProvider
//! - NotifyChangeExtProvider
//! - UserDefineFunc

use std::ffi::c_void;

use super::types_ffi::{c_str_array_to_vec, c_str_to_rust, rust_str_to_c, DataShareHelperHandle};
use datashare_common::observer::{ChangeInfo, ChangeType};
use datashare_common::values_bucket::DataShareValuesBucket;

const DATA_SHARE_ERROR: i32 = -1;

/// Extension insert — returns error code and writes string result to buffer.
///
/// # Safety
/// All pointer parameters must be valid.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperInsertExtStr(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    value: *const DataShareValuesBucket,
    out_buf: *mut u8,
    buf_len: u32,
    out_len: *mut u32,
) -> i32 {
    if handle.is_null() || value.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let (err_code, result_str) = (*handle).insert_ext_str(uri, &*value);
    if !out_buf.is_null() {
        rust_str_to_c(&result_str, out_buf, buf_len, out_len);
    } else if !out_len.is_null() {
        *out_len = result_str.len() as u32;
    }
    err_code
}

/// Register an observer at the provider level (non-silent only).
///
/// # Safety
/// `handle` must be a valid DataShareHelper handle.
/// `observer_id` is the raw pointer to the C++ ObserverImpl (IDataAbilityObserver).
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperRegisterObserverExtProvider(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    observer_id: u64,
    is_descendants: bool,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    (*handle).register_observer_ext_provider(uri, observer_id, is_descendants)
}

/// Unregister an observer at the provider level (non-silent only).
///
/// # Safety
/// `handle` must be a valid DataShareHelper handle.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperUnregisterObserverExtProvider(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    observer_id: u64,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    (*handle).unregister_observer_ext_provider(uri, observer_id)
}

/// Notify change at the provider level (non-silent only).
///
/// Takes ChangeInfo fields individually:
/// - change_type: 0=Insert, 1=Delete, 2=Update, 3=Other
/// - uris: array of URI strings
/// - data_ptr/data_len: raw data payload
///
/// # Safety
/// All pointer parameters must be valid.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperNotifyChangeExtProvider(
    handle: DataShareHelperHandle,
    change_type: i32,
    uris_ptrs: *const *const u8,
    uris_lens: *const u32,
    uris_count: u32,
    data_ptr: *const u8,
    data_len: u32,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uris = c_str_array_to_vec(uris_ptrs, uris_lens, uris_count);
    let data = if !data_ptr.is_null() && data_len > 0 {
        std::slice::from_raw_parts(data_ptr, data_len as usize).to_vec()
    } else {
        Vec::new()
    };
    let change_info = ChangeInfo {
        change_type: ChangeType::from_u32(change_type as u32),
        uris,
        data,
        value_buckets: Vec::new(),
    };
    (*handle).notify_change_ext_provider(&change_info)
}

/// User-defined function dispatch via raw MessageParcel pointer.
///
/// Takes a raw C++ MessageParcel* for input data and output reply.
/// Returns 0 on success, negative on error.
///
/// # Safety
/// `data_parcel` and `reply_parcel` must be valid C++ MessageParcel pointers.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperUserDefineFunc(
    handle: DataShareHelperHandle,
    data_parcel: *mut c_void,
    reply_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || data_parcel.is_null() || reply_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    // Convert raw C++ MessageParcel* to Rust MsgParcel
    // The ipc_rust crate provides MsgParcel::from_ptr for this
    let mut data = ipc::parcel::MsgParcel::from_ptr(data_parcel as *mut _);
    match (*handle).user_define_func(&mut data) {
        Some(mut reply) => {
            let mut reply_dst = ipc::parcel::MsgParcel::from_ptr(reply_parcel as *mut _);
            let _ = reply.set_read_position(0);
            let readable = reply.readable();
            if readable > 0 {
                if let Ok(buf) = reply.read_buffer(readable) {
                    let _ = reply_dst.write_buffer(&buf);
                }
            }
            std::mem::forget(reply_dst);
            0
        }
        None => DATA_SHARE_ERROR,
    }
}

/// Execute a batch of operations using pre-serialized binary blob.
///
/// The C++ side calls `MarshalOperationStatementVecToBuffer` to get the blob,
/// then passes the raw bytes here. The Rust side writes them directly to the
/// IPC parcel in the format expected by `CmdExecuteBatch` on the stub.
///
/// # Safety
/// All pointer parameters must be valid.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperExecuteBatchBlob(
    handle: DataShareHelperHandle,
    blob_ptr: *const u8,
    blob_len: u32,
) -> i32 {
    if handle.is_null() || blob_ptr.is_null() || blob_len == 0 {
        return DATA_SHARE_ERROR;
    }
    let blob = std::slice::from_raw_parts(blob_ptr, blob_len as usize);
    (*handle).execute_batch_blob(blob)
}

/// Query with result set bridge.
///
/// Performs a query and writes the result set's remote object into the
/// caller-provided MessageParcel (outParcel). The C++ side can then read
/// the IRemoteObject from the parcel to create a DataShareResultSet.
///
/// # Safety
/// All pointer parameters must be valid. `out_parcel` must be a valid C++ MessageParcel*.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperQueryWithResultSet(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    predicates: *const datashare_common::predicates::DataSharePredicates,
    columns_ptrs: *const *const u8,
    columns_lens: *const u32,
    columns_count: u32,
    timeout: i32,
    out_parcel: *mut c_void,
    out_err_code: *mut i32,
    out_err_msg_buf: *mut u8,
    err_msg_buf_len: u32,
    out_err_msg_len: *mut u32,
) -> i32 {
    use crate::controller::general_controller::DatashareBusinessError;
    use crate::helper::datashare_helper::DataShareHelper;
    use datashare_common::types::DataShareOption;

    if handle.is_null() || predicates.is_null() || out_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let columns = c_str_array_to_vec(columns_ptrs, columns_lens, columns_count);
    let mut business_error = DatashareBusinessError::new();

    let result = if timeout > 0 {
        let option = DataShareOption {
            timeout: timeout as u32,
        };
        (*handle).query_with_option(uri, &*predicates, &columns, &option, &mut business_error)
    } else {
        (*handle).query(uri, &*predicates, &columns, &mut business_error)
    };

    if !out_err_code.is_null() {
        *out_err_code = business_error.get_code();
    }
    if !out_err_msg_buf.is_null() {
        rust_str_to_c(
            business_error.get_message(),
            out_err_msg_buf,
            err_msg_buf_len,
            out_err_msg_len,
        );
    }

    match result {
        Some(proxy) => {
            // Write the result set's remote object into the caller's parcel
            let mut parcel = ipc::parcel::MsgParcel::from_ptr(out_parcel as *mut _);
            if parcel.write_remote(proxy.remote().clone()).is_ok() {
                std::mem::forget(parcel); // Don't drop — C++ owns the parcel
                0
            } else {
                std::mem::forget(parcel);
                DATA_SHARE_ERROR
            }
        }
        None => DATA_SHARE_ERROR,
    }
}
