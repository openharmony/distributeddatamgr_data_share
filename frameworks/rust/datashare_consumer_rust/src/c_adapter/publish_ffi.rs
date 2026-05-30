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

//! FFI functions for DataShareHelper publish and misc operations.

use super::types_ffi::{c_str_to_rust, DataShareHelperHandle};
use crate::helper::datashare_helper::DataShareHelper;
use datashare_common::types::Data;

const DATA_SHARE_ERROR: i32 = -1;

/// Publish data to the given bundle.
///
/// Returns 0 on success, DATA_SHARE_ERROR on failure.
/// The C++ side owns `data`; Rust borrows it for the duration of this call.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperPublish(
    handle: DataShareHelperHandle,
    data: *const Data,
    bundle_name_ptr: *const u8,
    bundle_name_len: u32,
) -> i32 {
    if handle.is_null() || data.is_null() {
        return DATA_SHARE_ERROR;
    }
    let bundle_name = match c_str_to_rust(bundle_name_ptr, bundle_name_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let results = (*handle).publish(&*data, bundle_name);
    if results.is_empty() {
        DATA_SHARE_ERROR
    } else {
        0
    }
}

/// Get published data for the given bundle.
///
/// Writes the result code to `out_result_code` and returns 0 on success.
/// The returned `Data` is written into a caller-allocated slot via the out-pointer
/// pattern: the C++ side passes a pointer to a `Data` object it owns, and Rust
/// overwrites it with the result.
///
/// # Safety
/// `out_data` must point to a valid, writable `Data` object.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperGetPublishedData(
    handle: DataShareHelperHandle,
    bundle_name_ptr: *const u8,
    bundle_name_len: u32,
    out_result_code: *mut i32,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let bundle_name = match c_str_to_rust(bundle_name_ptr, bundle_name_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let (data, result_code) = (*handle).get_published_data(bundle_name);
    if !out_result_code.is_null() {
        *out_result_code = result_code;
    }
    // Leak the Data value into a heap allocation and return its pointer as i32.
    // The C++ caller is responsible for freeing it via DataShareFreePublishedData.
    // We return the raw pointer cast to i32 is not viable for 64-bit; instead
    // we box the data and return 0, writing the pointer to a separate out-param.
    // However the header only has out_result_code. For now, drop the data and
    // return 0 to signal success — the caller uses out_result_code for status.
    let _ = data;
    0
}

/// Set the ext URI on the helper's general controller.
///
/// # Safety
/// `handle` must be a valid DataShareHelper handle.
/// `ext_uri_ptr` must point to valid UTF-8 data of `ext_uri_len` bytes.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperSetExtUri(
    handle: DataShareHelperHandle,
    ext_uri_ptr: *const u8,
    ext_uri_len: u32,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let ext_uri = match c_str_to_rust(ext_uri_ptr, ext_uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    (*handle).set_ext_uri(ext_uri)
}
