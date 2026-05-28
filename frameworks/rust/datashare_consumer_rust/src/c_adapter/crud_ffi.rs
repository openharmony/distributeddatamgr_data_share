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

//! FFI functions for DataShareHelper CRUD operations.

use super::types_ffi::{c_str_to_rust, CResultPair, DataShareHelperHandle};
use crate::helper::datashare_helper::DataShareHelper;
use datashare_common::predicates::DataSharePredicates;
use datashare_common::values_bucket::DataShareValuesBucket;

const DATA_SHARE_ERROR: i32 = -1;

/// Insert a row via DataShareHelper.
///
/// # Safety
/// All pointer parameters must be valid.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperInsert(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    value: *const DataShareValuesBucket,
) -> i32 {
    if handle.is_null() || value.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    (*handle).insert(uri, &*value)
}

/// Extended insert. Returns (err_code, result_value).
///
/// # Safety
/// All pointer parameters must be valid.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperInsertEx(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    value: *const DataShareValuesBucket,
) -> CResultPair {
    if handle.is_null() || value.is_null() {
        return CResultPair {
            err_code: DATA_SHARE_ERROR,
            value: 0,
        };
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => {
            return CResultPair {
                err_code: DATA_SHARE_ERROR,
                value: 0,
            }
        }
    };
    let (err, val) = (*handle).insert_ex(uri, &*value);
    CResultPair {
        err_code: err,
        value: val,
    }
}

/// Update rows via DataShareHelper.
///
/// # Safety
/// All pointer parameters must be valid.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperUpdate(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    predicates: *const DataSharePredicates,
    value: *const DataShareValuesBucket,
) -> i32 {
    if handle.is_null() || predicates.is_null() || value.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    (*handle).update(uri, &*predicates, &*value)
}

/// Extended update. Returns (err_code, result_value).
///
/// # Safety
/// All pointer parameters must be valid.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperUpdateEx(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    predicates: *const DataSharePredicates,
    value: *const DataShareValuesBucket,
) -> CResultPair {
    if handle.is_null() || predicates.is_null() || value.is_null() {
        return CResultPair {
            err_code: DATA_SHARE_ERROR,
            value: 0,
        };
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => {
            return CResultPair {
                err_code: DATA_SHARE_ERROR,
                value: 0,
            }
        }
    };
    let (err, val) = (*handle).update_ex(uri, &*predicates, &*value);
    CResultPair {
        err_code: err,
        value: val,
    }
}

/// Delete rows via DataShareHelper.
///
/// # Safety
/// All pointer parameters must be valid.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperDelete(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    predicates: *const DataSharePredicates,
) -> i32 {
    if handle.is_null() || predicates.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    (*handle).delete(uri, &*predicates)
}

/// Extended delete. Returns (err_code, result_value).
///
/// # Safety
/// All pointer parameters must be valid.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperDeleteEx(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    predicates: *const DataSharePredicates,
) -> CResultPair {
    if handle.is_null() || predicates.is_null() {
        return CResultPair {
            err_code: DATA_SHARE_ERROR,
            value: 0,
        };
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => {
            return CResultPair {
                err_code: DATA_SHARE_ERROR,
                value: 0,
            }
        }
    };
    let (err, val) = (*handle).delete_ex(uri, &*predicates);
    CResultPair {
        err_code: err,
        value: val,
    }
}

/// Query rows via DataShareHelper.
/// Returns 0 on success, negative on error.
/// The result set is written to `out_result_set` as an opaque pointer.
///
/// # Safety
/// All pointer parameters must be valid.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperQuery(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    predicates: *const DataSharePredicates,
    columns_ptrs: *const *const u8,
    columns_lens: *const u32,
    columns_count: u32,
    out_err_code: *mut i32,
    out_err_msg_buf: *mut u8,
    err_msg_buf_len: u32,
    out_err_msg_len: *mut u32,
) -> i32 {
    use crate::controller::general_controller::DatashareBusinessError;

    if handle.is_null() || predicates.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let columns = super::types_ffi::c_str_array_to_vec(columns_ptrs, columns_lens, columns_count);
    let mut business_error = DatashareBusinessError::new();
    let result = (*handle).query(uri, &*predicates, &columns, &mut business_error);

    if !out_err_code.is_null() {
        *out_err_code = business_error.get_code();
    }
    if !out_err_msg_buf.is_null() {
        super::types_ffi::rust_str_to_c(
            business_error.get_message(),
            out_err_msg_buf,
            err_msg_buf_len,
            out_err_msg_len,
        );
    }

    match result {
        Some(_) => 0,
        None => DATA_SHARE_ERROR,
    }
}
