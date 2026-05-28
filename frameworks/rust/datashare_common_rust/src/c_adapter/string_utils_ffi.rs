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

//! C FFI for string utility functions

use crate::utils::string_utils::StringUtils;

/// Anonymize a string. Caller must free the returned buffer with DataShareBufferFree.
/// Returns 0 on success, -1 on failure.
#[no_mangle]
pub extern "C" fn DataShareStringUtilsAnonymous(
    name: *const u8,
    name_len: u32,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if name.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let name_slice = unsafe { std::slice::from_raw_parts(name, name_len as usize) };
    let name_str = match std::str::from_utf8(name_slice) {
        Ok(s) => s,
        Err(_) => return -1,
    };
    let result = StringUtils::anonymous(name_str);
    let result_bytes = result.into_bytes();
    let len = result_bytes.len();
    let ptr = Box::into_raw(result_bytes.into_boxed_slice()) as *mut u8;
    unsafe {
        *out_buf = ptr;
        *out_len = len as u32;
    }
    0
}

/// Change/anonymize a string. Caller must free the returned buffer with DataShareBufferFree.
#[no_mangle]
pub extern "C" fn DataShareStringUtilsChange(
    name: *const u8,
    name_len: u32,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if name.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let name_slice = unsafe { std::slice::from_raw_parts(name, name_len as usize) };
    let name_str = match std::str::from_utf8(name_slice) {
        Ok(s) => s,
        Err(_) => return -1,
    };
    let result = StringUtils::change(name_str);
    let result_bytes = result.into_bytes();
    let len = result_bytes.len();
    let ptr = Box::into_raw(result_bytes.into_boxed_slice()) as *mut u8;
    unsafe {
        *out_buf = ptr;
        *out_len = len as u32;
    }
    0
}

/// Remove query string from URI. Caller must free the returned buffer with DataShareBufferFree.
#[no_mangle]
pub extern "C" fn DataShareStringUtilsRemoveFromQuery(
    uri: *const u8,
    uri_len: u32,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if uri.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let uri_slice = unsafe { std::slice::from_raw_parts(uri, uri_len as usize) };
    let uri_str = match std::str::from_utf8(uri_slice) {
        Ok(s) => s,
        Err(_) => return -1,
    };
    let result = StringUtils::remove_from_query(uri_str);
    let result_bytes = result.into_bytes();
    let len = result_bytes.len();
    let ptr = Box::into_raw(result_bytes.into_boxed_slice()) as *mut u8;
    unsafe {
        *out_buf = ptr;
        *out_len = len as u32;
    }
    0
}

/// Get random number between min and max (inclusive).
#[no_mangle]
pub extern "C" fn DataShareStringUtilsGetRandomNumber(min: i32, max: i32) -> i32 {
    StringUtils::get_random_number(min, max)
}
