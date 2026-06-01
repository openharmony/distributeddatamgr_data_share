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

//! C FFI adapter for IPC serialization utilities (ITypesUtil + Serializable).
//!
//! Provides C-compatible functions for binary buffer serialization of
//! DataSharePredicates and DataShareValuesBucket arrays, and JSON
//! serialization for the Serializable framework.

use crate::ipc::itypes_utils;
use crate::ipc::serializable::{self, JsonValue};
use crate::predicates::predicates::DataSharePredicates;
use crate::values_bucket::DataShareValuesBucket;

// =====================================================================
// Binary buffer predicates serialization
// =====================================================================

/// Serialize DataSharePredicates to a binary buffer.
///
/// On success, writes the buffer pointer to `out_buf` and size to `out_size`.
/// The caller must free the buffer via `DataShareBufferFree`.
/// Returns 0 on success, -1 on failure.
#[no_mangle]
pub extern "C" fn DataShareMarshalPredicatesToBuffer(
    predicates: *const DataSharePredicates,
    out_buf: *mut *mut u8,
    out_size: *mut u32,
) -> i32 {
    if predicates.is_null() || out_buf.is_null() || out_size.is_null() {
        return -1;
    }
    unsafe {
        match itypes_utils::marshal_predicates_to_buffer(&*predicates) {
            Some(buf) => {
                let len = buf.len() as u32;
                let boxed = buf.into_boxed_slice();
                let ptr = Box::into_raw(boxed) as *mut u8;
                *out_buf = ptr;
                *out_size = len;
                0
            }
            None => -1,
        }
    }
}

/// Deserialize DataSharePredicates from a binary buffer.
///
/// Returns a pointer to the deserialized predicates, or null on failure.
/// The caller must free the result via `DataSharePredicatesFree`.
#[no_mangle]
pub extern "C" fn DataShareUnmarshalPredicatesFromBuffer(
    buf: *const u8,
    size: u32,
) -> *mut DataSharePredicates {
    if buf.is_null() || size == 0 {
        return std::ptr::null_mut();
    }
    unsafe {
        let data = std::slice::from_raw_parts(buf, size as usize);
        match itypes_utils::unmarshal_predicates_from_buffer(data) {
            Some(pred) => Box::into_raw(Box::new(pred)),
            None => std::ptr::null_mut(),
        }
    }
}

// =====================================================================
// Binary buffer ValuesBucket vector serialization
// =====================================================================

/// Opaque handle for a vector of DataShareValuesBucket.
pub struct ValuesBucketVec {
    pub(crate) inner: Vec<DataShareValuesBucket>,
}

/// Create a new empty ValuesBucketVec.
#[no_mangle]
pub extern "C" fn DataShareValuesBucketVecNew() -> *mut ValuesBucketVec {
    Box::into_raw(Box::new(ValuesBucketVec { inner: Vec::new() }))
}

/// Push a DataShareValuesBucket into the vector (transfers ownership).
#[no_mangle]
pub extern "C" fn DataShareValuesBucketVecPush(
    vec: *mut ValuesBucketVec,
    bucket: *mut DataShareValuesBucket,
) -> i32 {
    if vec.is_null() || bucket.is_null() {
        return -1;
    }
    unsafe {
        let b = *Box::from_raw(bucket);
        (*vec).inner.push(b);
        0
    }
}

/// Get the number of elements in the vector.
#[no_mangle]
pub extern "C" fn DataShareValuesBucketVecSize(vec: *const ValuesBucketVec) -> u32 {
    if vec.is_null() {
        return 0;
    }
    unsafe { (*vec).inner.len() as u32 }
}

/// Free a ValuesBucketVec.
#[no_mangle]
pub extern "C" fn DataShareValuesBucketVecFree(vec: *mut ValuesBucketVec) {
    if !vec.is_null() {
        unsafe {
            let _ = Box::from_raw(vec);
        }
    }
}

/// Serialize a ValuesBucketVec to a binary buffer.
///
/// On success, writes the buffer pointer to `out_buf` and size to `out_size`.
/// The caller must free the buffer via `DataShareBufferFree`.
/// Returns 0 on success, -1 on failure.
#[no_mangle]
pub extern "C" fn DataShareMarshalValuesBucketVecToBuffer(
    vec: *const ValuesBucketVec,
    out_buf: *mut *mut u8,
    out_size: *mut u32,
) -> i32 {
    if vec.is_null() || out_buf.is_null() || out_size.is_null() {
        return -1;
    }
    unsafe {
        match itypes_utils::marshal_values_bucket_vec_to_buffer(&(*vec).inner) {
            Some(buf) => {
                let len = buf.len() as u32;
                let boxed = buf.into_boxed_slice();
                let ptr = Box::into_raw(boxed) as *mut u8;
                *out_buf = ptr;
                *out_size = len;
                0
            }
            None => -1,
        }
    }
}

/// Deserialize a ValuesBucketVec from a binary buffer.
///
/// Returns a pointer to the deserialized vector, or null on failure.
/// The caller must free the result via `DataShareValuesBucketVecFree`.
#[no_mangle]
pub extern "C" fn DataShareUnmarshalValuesBucketVecFromBuffer(
    buf: *const u8,
    size: u32,
) -> *mut ValuesBucketVec {
    if buf.is_null() || size == 0 {
        return std::ptr::null_mut();
    }
    unsafe {
        let data = std::slice::from_raw_parts(buf, size as usize);
        match itypes_utils::unmarshal_values_bucket_vec_from_buffer(data) {
            Some(buckets) => Box::into_raw(Box::new(ValuesBucketVec { inner: buckets })),
            None => std::ptr::null_mut(),
        }
    }
}

// =====================================================================
// JSON serialization (Serializable framework)
// =====================================================================

type OpaqueJsonValue = JsonValue;

/// Parse a JSON string into a JsonValue.
///
/// Returns a pointer to the parsed JsonValue, or null on failure.
/// The caller must free the result via `DataShareJsonValueFree`.
#[no_mangle]
pub extern "C" fn DataShareJsonParse(json_str: *const u8, json_len: u32) -> *mut OpaqueJsonValue {
    if json_str.is_null() {
        return std::ptr::null_mut();
    }
    unsafe {
        let s =
            std::str::from_utf8_unchecked(std::slice::from_raw_parts(json_str, json_len as usize));
        let val = serializable::to_json(s);
        if val.is_null() {
            return std::ptr::null_mut();
        }
        Box::into_raw(Box::new(val))
    }
}

/// Serialize a JsonValue to a JSON string.
///
/// On success, writes the string pointer to `out_buf` and length to `out_len`.
/// The caller must free the buffer via `DataShareBufferFree`.
/// Returns 0 on success, -1 on failure.
#[no_mangle]
pub extern "C" fn DataShareJsonDump(
    json: *const OpaqueJsonValue,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if json.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    unsafe {
        let s = (*json).dump();
        let bytes = s.into_bytes();
        let len = bytes.len() as u32;
        let boxed = bytes.into_boxed_slice();
        let ptr = Box::into_raw(boxed) as *mut u8;
        *out_buf = ptr;
        *out_len = len;
        0
    }
}

/// Check if a string is valid JSON.
#[no_mangle]
pub extern "C" fn DataShareIsJson(json_str: *const u8, json_len: u32) -> bool {
    if json_str.is_null() {
        return false;
    }
    unsafe {
        let s =
            std::str::from_utf8_unchecked(std::slice::from_raw_parts(json_str, json_len as usize));
        serializable::is_json(s)
    }
}

/// Free a JsonValue.
#[no_mangle]
pub extern "C" fn DataShareJsonValueFree(json: *mut OpaqueJsonValue) {
    if !json.is_null() {
        unsafe {
            let _ = Box::from_raw(json);
        }
    }
}

// =====================================================================
// JSON GetValue FFI (C++ Serializable::GetValue bridge)
// =====================================================================

/// Helper: convert raw name pointer to &str.
unsafe fn name_to_str(name: *const u8, name_len: u32) -> &'static str {
    if name.is_null() || name_len == 0 {
        ""
    } else {
        std::str::from_utf8_unchecked(std::slice::from_raw_parts(name, name_len as usize))
    }
}

/// Helper: write a string result to out_buf/out_len. Returns 0 on success, -1 on failure.
unsafe fn write_string_result(s: &str, out_buf: *mut *mut u8, out_len: *mut u32) -> i32 {
    let bytes = s.as_bytes().to_vec();
    let len = bytes.len() as u32;
    let boxed = bytes.into_boxed_slice();
    let ptr = Box::into_raw(boxed) as *mut u8;
    *out_buf = ptr;
    *out_len = len;
    0
}

/// Get a string value from a JsonValue by name.
/// Returns 0 on success, -1 on failure.
///
/// # Safety
///
/// - `json` must be a valid pointer returned by `DataShareJsonParse`.
/// - `name` must point to valid UTF-8 of `name_len` bytes, or be null.
/// - `out_buf` and `out_len` must be valid for writes.
/// - The caller must free `*out_buf` via `DataShareBufferFree`.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonGetStringValue(
    json: *const OpaqueJsonValue,
    name: *const u8,
    name_len: u32,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if json.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let n = name_to_str(name, name_len);
    match serializable::get_string(&*json, n) {
        Some(s) => write_string_result(&s, out_buf, out_len),
        None => -1,
    }
}

/// Get an i32 value from a JsonValue by name.
///
/// # Safety
///
/// - `json` must be a valid pointer returned by `DataShareJsonParse`.
/// - `name` must point to valid UTF-8 of `name_len` bytes, or be null.
/// - `out_value` must be valid for writes.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonGetI32Value(
    json: *const OpaqueJsonValue,
    name: *const u8,
    name_len: u32,
    out_value: *mut i32,
) -> i32 {
    if json.is_null() || out_value.is_null() {
        return -1;
    }
    let n = name_to_str(name, name_len);
    match serializable::get_i32(&*json, n) {
        Some(v) => {
            *out_value = v;
            0
        }
        None => -1,
    }
}

/// Get a u32 value from a JsonValue by name.
///
/// # Safety
///
/// - `json` must be a valid pointer returned by `DataShareJsonParse`.
/// - `name` must point to valid UTF-8 of `name_len` bytes, or be null.
/// - `out_value` must be valid for writes.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonGetU32Value(
    json: *const OpaqueJsonValue,
    name: *const u8,
    name_len: u32,
    out_value: *mut u32,
) -> i32 {
    if json.is_null() || out_value.is_null() {
        return -1;
    }
    let n = name_to_str(name, name_len);
    match serializable::get_u32(&*json, n) {
        Some(v) => {
            *out_value = v;
            0
        }
        None => -1,
    }
}

/// Get an i64 value from a JsonValue by name.
///
/// # Safety
///
/// - `json` must be a valid pointer returned by `DataShareJsonParse`.
/// - `name` must point to valid UTF-8 of `name_len` bytes, or be null.
/// - `out_value` must be valid for writes.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonGetI64Value(
    json: *const OpaqueJsonValue,
    name: *const u8,
    name_len: u32,
    out_value: *mut i64,
) -> i32 {
    if json.is_null() || out_value.is_null() {
        return -1;
    }
    let n = name_to_str(name, name_len);
    match serializable::get_i64(&*json, n) {
        Some(v) => {
            *out_value = v;
            0
        }
        None => -1,
    }
}

/// Get a u64 value from a JsonValue by name.
///
/// # Safety
///
/// - `json` must be a valid pointer returned by `DataShareJsonParse`.
/// - `name` must point to valid UTF-8 of `name_len` bytes, or be null.
/// - `out_value` must be valid for writes.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonGetU64Value(
    json: *const OpaqueJsonValue,
    name: *const u8,
    name_len: u32,
    out_value: *mut u64,
) -> i32 {
    if json.is_null() || out_value.is_null() {
        return -1;
    }
    let n = name_to_str(name, name_len);
    match serializable::get_u64(&*json, n) {
        Some(v) => {
            *out_value = v;
            0
        }
        None => -1,
    }
}

/// Get a u16 value from a JsonValue by name.
///
/// # Safety
///
/// - `json` must be a valid pointer returned by `DataShareJsonParse`.
/// - `name` must point to valid UTF-8 of `name_len` bytes, or be null.
/// - `out_value` must be valid for writes.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonGetU16Value(
    json: *const OpaqueJsonValue,
    name: *const u8,
    name_len: u32,
    out_value: *mut u16,
) -> i32 {
    if json.is_null() || out_value.is_null() {
        return -1;
    }
    let n = name_to_str(name, name_len);
    match serializable::get_u16(&*json, n) {
        Some(v) => {
            *out_value = v;
            0
        }
        None => -1,
    }
}

/// Get a bool value from a JsonValue by name.
/// Matches C++ behavior: accepts bool or unsigned int (0=false, non-0=true).
///
/// # Safety
///
/// - `json` must be a valid pointer returned by `DataShareJsonParse`.
/// - `name` must point to valid UTF-8 of `name_len` bytes, or be null.
/// - `out_value` must be valid for writes.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonGetBoolValue(
    json: *const OpaqueJsonValue,
    name: *const u8,
    name_len: u32,
    out_value: *mut bool,
) -> i32 {
    if json.is_null() || out_value.is_null() {
        return -1;
    }
    let n = name_to_str(name, name_len);
    match serializable::get_bool(&*json, n) {
        Some(v) => {
            *out_value = v;
            0
        }
        None => -1,
    }
}

/// Get a blob (Vec<u8>) value from a JsonValue by name.
/// Returns 0 on success, -1 on failure.
///
/// # Safety
///
/// - `json` must be a valid pointer returned by `DataShareJsonParse`.
/// - `name` must point to valid UTF-8 of `name_len` bytes, or be null.
/// - `out_buf` and `out_len` must be valid for writes.
/// - The caller must free `*out_buf` via `DataShareBufferFree`.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonGetBlobValue(
    json: *const OpaqueJsonValue,
    name: *const u8,
    name_len: u32,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if json.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let n = name_to_str(name, name_len);
    match serializable::get_blob(&*json, n) {
        Some(blob) => {
            let len = blob.len() as u32;
            let boxed = blob.into_boxed_slice();
            let ptr = Box::into_raw(boxed) as *mut u8;
            *out_buf = ptr;
            *out_len = len;
            0
        }
        None => -1,
    }
}

// =====================================================================
// JSON SetValue FFI (C++ Serializable::SetValue bridge)
// Each produces a JSON string representation of the value.
// =====================================================================

/// Helper: dump a JsonValue to an allocated buffer.
unsafe fn dump_json_value(val: &JsonValue, out_buf: *mut *mut u8, out_len: *mut u32) -> i32 {
    let s = val.dump();
    write_string_result(&s, out_buf, out_len)
}

/// Create a JSON string value: string → "\"value\""
///
/// # Safety
///
/// - `value` must point to valid UTF-8 of `value_len` bytes, or be null.
/// - `out_buf` and `out_len` must be valid for writes.
/// - The caller must free `*out_buf` via `DataShareBufferFree`.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonFromString(
    value: *const u8,
    value_len: u32,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let s = if value.is_null() || value_len == 0 {
        String::new()
    } else {
        std::str::from_utf8_unchecked(std::slice::from_raw_parts(value, value_len as usize))
            .to_string()
    };
    let val = JsonValue::String(s);
    dump_json_value(&val, out_buf, out_len)
}

/// Create a JSON i32 value: 42 → "42"
///
/// # Safety
///
/// - `out_buf` and `out_len` must be valid for writes.
/// - The caller must free `*out_buf` via `DataShareBufferFree`.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonFromI32(
    value: i32,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let val = JsonValue::Int(value as i64);
    dump_json_value(&val, out_buf, out_len)
}

/// Create a JSON u32 value.
///
/// # Safety
///
/// - `out_buf` and `out_len` must be valid for writes.
/// - The caller must free `*out_buf` via `DataShareBufferFree`.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonFromU32(
    value: u32,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let val = JsonValue::Uint(value as u64);
    dump_json_value(&val, out_buf, out_len)
}

/// Create a JSON i64 value.
///
/// # Safety
///
/// - `out_buf` and `out_len` must be valid for writes.
/// - The caller must free `*out_buf` via `DataShareBufferFree`.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonFromI64(
    value: i64,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let val = JsonValue::Int(value);
    dump_json_value(&val, out_buf, out_len)
}

/// Create a JSON f64 value.
///
/// # Safety
///
/// - `out_buf` and `out_len` must be valid for writes.
/// - The caller must free `*out_buf` via `DataShareBufferFree`.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonFromDouble(
    value: f64,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let val = JsonValue::Float(value);
    dump_json_value(&val, out_buf, out_len)
}

/// Create a JSON u64 value.
///
/// # Safety
///
/// - `out_buf` and `out_len` must be valid for writes.
/// - The caller must free `*out_buf` via `DataShareBufferFree`.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonFromU64(
    value: u64,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let val = JsonValue::Uint(value);
    dump_json_value(&val, out_buf, out_len)
}

/// Create a JSON u16 value.
///
/// # Safety
///
/// - `out_buf` and `out_len` must be valid for writes.
/// - The caller must free `*out_buf` via `DataShareBufferFree`.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonFromU16(
    value: u16,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let val = JsonValue::Uint(value as u64);
    dump_json_value(&val, out_buf, out_len)
}

/// Create a JSON blob value: [1,2,3] from raw bytes.
///
/// # Safety
///
/// - `data` must point to `data_len` bytes, or be null.
/// - `out_buf` and `out_len` must be valid for writes.
/// - The caller must free `*out_buf` via `DataShareBufferFree`.
#[no_mangle]
pub unsafe extern "C" fn DataShareJsonFromBlob(
    data: *const u8,
    data_len: u32,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let arr = if data.is_null() || data_len == 0 {
        Vec::new()
    } else {
        std::slice::from_raw_parts(data, data_len as usize)
            .iter()
            .map(|b| JsonValue::Uint(*b as u64))
            .collect()
    };
    let val = JsonValue::Array(arr);
    dump_json_value(&val, out_buf, out_len)
}

// =====================================================================
// Buffer management
// =====================================================================

/// Free a buffer allocated by Rust serialization functions.
///
/// Must be called for buffers returned by:
/// - `DataShareMarshalPredicatesToBuffer`
/// - `DataShareMarshalValuesBucketVecToBuffer`
/// - `DataShareJsonDump`
#[no_mangle]
pub extern "C" fn DataShareBufferFree(buf: *mut u8, size: u32) {
    if !buf.is_null() && size > 0 {
        unsafe {
            let slice = std::slice::from_raw_parts_mut(buf, size as usize);
            let _ = Box::from_raw(slice as *mut [u8]);
        }
    }
}
