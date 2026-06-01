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

//! C FFI for URI utility functions

use crate::utils::uri_utils::UriUtils;

/// Format URI by removing query string. Caller must free with DataShareBufferFree.
#[no_mangle]
pub extern "C" fn DataShareUriUtilsFormatUri(
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
    let result = UriUtils::format_uri(uri_str);
    let result_bytes = result.into_bytes();
    let len = result_bytes.len();
    let ptr = Box::into_raw(result_bytes.into_boxed_slice()) as *mut u8;
    unsafe {
        *out_buf = ptr;
        *out_len = len as u32;
    }
    0
}

/// Parse string to u32. Returns 0 on success, -1 on failure.
#[no_mangle]
pub extern "C" fn DataShareUriUtilsStrtoul(
    s: *const u8,
    s_len: u32,
    out_success: *mut bool,
    out_value: *mut u32,
) -> i32 {
    if s.is_null() || out_success.is_null() || out_value.is_null() {
        return -1;
    }
    let s_slice = unsafe { std::slice::from_raw_parts(s, s_len as usize) };
    let s_str = match std::str::from_utf8(s_slice) {
        Ok(s) => s,
        Err(_) => {
            unsafe {
                *out_success = false;
                *out_value = 0;
            }
            return 0;
        }
    };
    let (success, value) = UriUtils::strtoul(s_str);
    unsafe {
        *out_success = success;
        *out_value = value;
    }
    0
}

/// Get user from URI query parameters.
#[no_mangle]
pub extern "C" fn DataShareUriUtilsGetUserFromUri(
    uri: *const u8,
    uri_len: u32,
    out_success: *mut bool,
    out_user: *mut i32,
) -> i32 {
    if uri.is_null() || out_success.is_null() || out_user.is_null() {
        return -1;
    }
    let uri_slice = unsafe { std::slice::from_raw_parts(uri, uri_len as usize) };
    let uri_str = match std::str::from_utf8(uri_slice) {
        Ok(s) => s,
        Err(_) => {
            unsafe {
                *out_success = false;
                *out_user = -1;
            }
            return 0;
        }
    };
    let (success, user) = UriUtils::get_user_from_uri(uri_str);
    unsafe {
        *out_success = success;
        *out_user = user;
    }
    0
}

/// Extract first path segment from URI. Caller must free with DataShareBufferFree.
///
/// # Safety
/// - `uri` must point to a valid UTF-8 byte buffer of length `uri_len`.
/// - `out_buf` and `out_len` must be valid, non-null pointers.
#[no_mangle]
pub unsafe extern "C" fn DataShareUriUtilsExtractFirstPathSegment(
    uri: *const u8,
    uri_len: u32,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if uri.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let uri_slice = std::slice::from_raw_parts(uri, uri_len as usize);
    let uri_str = match std::str::from_utf8(uri_slice) {
        Ok(s) => s,
        Err(_) => return -1,
    };
    let result = UriUtils::extract_first_path_segment(uri_str);
    let result_bytes = result.into_bytes();
    let len = result_bytes.len();
    let ptr = Box::into_raw(result_bytes.into_boxed_slice()) as *mut u8;
    *out_buf = ptr;
    *out_len = len as u32;
    0
}

/// Get query parameters from URI as serialized key-value pairs.
///
/// Returns key-value pairs as a null-terminated serialized buffer:
/// "key1\0val1\0key2\0val2\0..." with `out_count` indicating the number of pairs.
/// Caller must free the buffer via `DataShareBufferFree`.
///
/// # Safety
/// - `uri` must point to a valid UTF-8 byte buffer of length `uri_len`.
/// - All output pointers must be valid and non-null.
#[no_mangle]
pub unsafe extern "C" fn DataShareUriUtilsGetQueryParams(
    uri: *const u8,
    uri_len: u32,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
    out_count: *mut u32,
) -> i32 {
    if uri.is_null() || out_buf.is_null() || out_len.is_null() || out_count.is_null() {
        return -1;
    }
    let uri_slice = std::slice::from_raw_parts(uri, uri_len as usize);
    let uri_str = match std::str::from_utf8(uri_slice) {
        Ok(s) => s,
        Err(_) => {
            *out_buf = std::ptr::null_mut();
            *out_len = 0;
            *out_count = 0;
            return 0;
        }
    };
    let params = UriUtils::get_query_params(uri_str);
    let count = params.len() as u32;

    if count == 0 {
        *out_buf = std::ptr::null_mut();
        *out_len = 0;
        *out_count = 0;
        return 0;
    }

    // Serialize as "key1\0val1\0key2\0val2\0..."
    let mut buf = Vec::new();
    for (key, val) in &params {
        buf.extend_from_slice(key.as_bytes());
        buf.push(0);
        buf.extend_from_slice(val.as_bytes());
        buf.push(0);
    }

    let len = buf.len();
    let ptr = Box::into_raw(buf.into_boxed_slice()) as *mut u8;
    *out_buf = ptr;
    *out_len = len as u32;
    *out_count = count;
    0
}

/// Get system ability ID from URI.
///
/// # Safety
/// - `uri` must point to a valid UTF-8 byte buffer of length `uri_len`.
/// - `out_success` and `out_said` must be valid, non-null pointers.
#[no_mangle]
pub unsafe extern "C" fn DataShareUriUtilsGetSystemAbilityId(
    uri: *const u8,
    uri_len: u32,
    out_success: *mut bool,
    out_said: *mut i32,
) -> i32 {
    if uri.is_null() || out_success.is_null() || out_said.is_null() {
        return -1;
    }
    let uri_slice = std::slice::from_raw_parts(uri, uri_len as usize);
    let uri_str = match std::str::from_utf8(uri_slice) {
        Ok(s) => s,
        Err(_) => {
            *out_success = false;
            *out_said = -1;
            return 0;
        }
    };
    let (success, said) = UriUtils::get_system_ability_id(uri_str);
    *out_success = success;
    *out_said = said;
    0
}
