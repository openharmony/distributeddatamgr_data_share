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

//! Common FFI type helpers for DataShare consumer C adapter.

use std::ffi::c_void;
use std::slice;

use crate::helper::datashare_helper_impl::DataShareHelperImpl;

/// Opaque handle to a DataShareHelperImpl instance.
pub type DataShareHelperHandle = *mut DataShareHelperImpl;

/// Convert a C string (ptr + len) to a Rust &str.
/// Returns None if ptr is null.
///
/// # Safety
/// Caller must ensure ptr points to valid UTF-8 data of at least `len` bytes.
pub(crate) unsafe fn c_str_to_rust(ptr: *const u8, len: u32) -> Option<&'static str> {
    if ptr.is_null() {
        return None;
    }
    let bytes = slice::from_raw_parts(ptr, len as usize);
    std::str::from_utf8(bytes).ok()
}

/// Convert a C string array (array of ptr+len pairs) to Vec<String>.
///
/// # Safety
/// Caller must ensure all pointers are valid.
pub(crate) unsafe fn c_str_array_to_vec(
    ptrs: *const *const u8,
    lens: *const u32,
    count: u32,
) -> Vec<String> {
    if ptrs.is_null() || lens.is_null() || count == 0 {
        return Vec::new();
    }
    let ptrs_slice = slice::from_raw_parts(ptrs, count as usize);
    let lens_slice = slice::from_raw_parts(lens, count as usize);
    let mut result = Vec::with_capacity(count as usize);
    for i in 0..count as usize {
        if let Some(s) = c_str_to_rust(ptrs_slice[i], lens_slice[i]) {
            result.push(s.to_string());
        }
    }
    result
}

/// Write a Rust string into a C-allocated buffer.
/// Returns the number of bytes written, or the required size if buffer is too small.
///
/// # Safety
/// Caller must ensure out_buf points to a buffer of at least `buf_len` bytes.
pub(crate) unsafe fn rust_str_to_c(
    s: &str,
    out_buf: *mut u8,
    buf_len: u32,
    out_len: *mut u32,
) -> bool {
    if !out_len.is_null() {
        *out_len = s.len() as u32;
    }
    if out_buf.is_null() || (buf_len as usize) < s.len() {
        return false;
    }
    std::ptr::copy_nonoverlapping(s.as_ptr(), out_buf, s.len());
    true
}

/// C-compatible pair of (i32, i32) for Ex-variant return values.
#[repr(C)]
pub struct CResultPair {
    pub err_code: i32,
    pub value: i32,
}

/// C-compatible operation result.
#[repr(C)]
pub struct COperationResult {
    pub uri_ptr: *const u8,
    pub uri_len: u32,
    pub result: i32,
}

/// C-compatible operation result array (caller-owned).
#[repr(C)]
pub struct COperationResultArray {
    pub data: *mut COperationResult,
    pub count: u32,
}

/// Set the Rust-side DataShareServiceProxy system flag.
///
/// Must be called before Rust FFI functions that route through
/// DataShareServiceProxy (e.g., notify_change on silent path,
/// del_query_template). The C++ DataShareServiceProxy::SetSystem()
/// only affects the C++ flag and has no effect on Rust IPC calls.
#[no_mangle]
pub extern "C" fn DataShareServiceProxySetSystem(is_system: bool) {
    crate::proxy::service_proxy::DataShareServiceProxy::set_system(is_system);
}

/// Clear the Rust-side DataShareServiceProxy system flag.
#[no_mangle]
pub extern "C" fn DataShareServiceProxyCleanSystem() {
    crate::proxy::service_proxy::DataShareServiceProxy::clean_system();
}

/// Free an operation result array allocated by Rust.
#[no_mangle]
pub extern "C" fn DataShareFreeOperationResults(arr: *mut COperationResultArray) {
    if arr.is_null() {
        return;
    }
    unsafe {
        let arr = &mut *arr;
        if !arr.data.is_null() && arr.count > 0 {
            let results = Vec::from_raw_parts(arr.data, arr.count as usize, arr.count as usize);
            for r in &results {
                if !r.uri_ptr.is_null() && r.uri_len > 0 {
                    let _ = String::from_raw_parts(
                        r.uri_ptr as *mut u8,
                        r.uri_len as usize,
                        r.uri_len as usize,
                    );
                }
            }
            // Vec drop handles deallocation
        }
        arr.data = std::ptr::null_mut();
        arr.count = 0;
    }
}
