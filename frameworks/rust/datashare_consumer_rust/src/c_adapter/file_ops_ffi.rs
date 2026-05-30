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

//! FFI functions for DataShareHelper file operations.

use super::types_ffi::{c_str_to_rust, rust_str_to_c, CResultPair, DataShareHelperHandle};
use crate::helper::datashare_helper::DataShareHelper;

const DATA_SHARE_ERROR: i32 = -1;

/// Open a file. Returns file descriptor.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperOpenFile(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    mode_ptr: *const u8,
    mode_len: u32,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mode = match c_str_to_rust(mode_ptr, mode_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    (*handle).open_file(uri, mode)
}

/// Open a file with error code. Returns fd, writes error code to out_err_code.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperOpenFileWithErrCode(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    mode_ptr: *const u8,
    mode_len: u32,
    out_err_code: *mut i32,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mode = match c_str_to_rust(mode_ptr, mode_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let (fd, err) = (*handle).open_file_with_err_code(uri, mode);
    if !out_err_code.is_null() {
        *out_err_code = err;
    }
    fd
}

/// Open a raw file. Returns file descriptor.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperOpenRawFile(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    mode_ptr: *const u8,
    mode_len: u32,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mode = match c_str_to_rust(mode_ptr, mode_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    (*handle).open_raw_file(uri, mode)
}

/// Get MIME type for a URI. Writes result to out_buf.
/// Returns true if the buffer was large enough, false otherwise.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperGetType(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    out_buf: *mut u8,
    buf_len: u32,
    out_len: *mut u32,
) -> bool {
    if handle.is_null() {
        return false;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return false,
    };
    let result = (*handle).get_type(uri);
    rust_str_to_c(&result, out_buf, buf_len, out_len)
}

/// Get file types matching a MIME filter.
/// Writes results as a concatenated null-separated string to out_buf.
/// out_count receives the number of types found.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperGetFileTypes(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    filter_ptr: *const u8,
    filter_len: u32,
    out_buf: *mut u8,
    buf_len: u32,
    out_total_len: *mut u32,
    out_count: *mut u32,
) -> bool {
    if handle.is_null() {
        return false;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return false,
    };
    let filter = match c_str_to_rust(filter_ptr, filter_len) {
        Some(s) => s,
        None => return false,
    };
    let types = (*handle).get_file_types(uri, filter);
    if !out_count.is_null() {
        *out_count = types.len() as u32;
    }
    // Concatenate with null separators
    let total: usize = types.iter().map(|s| s.len() + 1).sum();
    if !out_total_len.is_null() {
        *out_total_len = total as u32;
    }
    if out_buf.is_null() || (buf_len as usize) < total {
        return false;
    }
    let mut offset = 0;
    for t in &types {
        std::ptr::copy_nonoverlapping(t.as_ptr(), out_buf.add(offset), t.len());
        offset += t.len();
        *out_buf.add(offset) = 0; // null separator
        offset += 1;
    }
    true
}

/// Normalize a URI. Writes result to out_buf.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperNormalizeUri(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    out_buf: *mut u8,
    buf_len: u32,
    out_len: *mut u32,
) -> bool {
    if handle.is_null() {
        return false;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return false,
    };
    let result = (*handle).normalize_uri(uri);
    rust_str_to_c(&result, out_buf, buf_len, out_len)
}

/// Denormalize a URI. Writes result to out_buf.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperDenormalizeUri(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    out_buf: *mut u8,
    buf_len: u32,
    out_len: *mut u32,
) -> bool {
    if handle.is_null() {
        return false;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return false,
    };
    let result = (*handle).denormalize_uri(uri);
    rust_str_to_c(&result, out_buf, buf_len, out_len)
}
