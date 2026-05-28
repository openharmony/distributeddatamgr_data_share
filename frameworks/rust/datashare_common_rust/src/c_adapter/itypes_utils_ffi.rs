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

//! C FFI for ITypesUtil binary buffer serialization.
//!
//! Provides functions to serialize/deserialize DataSharePredicates and
//! DataShareValuesBucket vectors to/from binary buffers. These replace
//! the C++ MarshalPredicatesToBuffer/UnmarshalPredicatesToBuffer and
//! MarshalValuesBucketVecToBuffer/UnmarshalValuesBucketVecToBuffer.
//!
//! Design: Operations (which have complex variant types) are passed as
//! pre-serialized buffers across the FFI boundary. Simple fields
//! (where_clause, where_args, order, mode) are passed directly.

use std::io::Write as IoWrite;

use super::serialization_ffi::ValuesBucketVec;
use crate::ipc::itypes_utils::{read_string, read_string_vec, write_basic, write_string};
use crate::types::MAX_IPC_SIZE;
use crate::value_object::{DataShareValue, DataShareValueType};
use crate::values_bucket::DataShareValuesBucket;

// =====================================================================
// Predicates: Marshal from parts
// =====================================================================

/// Assemble a predicates binary buffer from pre-serialized operations and
/// simple fields. The `ops_buf` must already be in the correct binary
/// format (serialized by C++ MarshalOperationItemVecToBuffer or Rust).
///
/// This function concatenates: ops_buf + where_clause + where_args + order + mode.
///
/// Returns 0 on success, -1 on failure.
/// Caller must free the output buffer via `DataShareBufferFree`.
///
/// # Safety
///
/// - `ops_buf` must point to `ops_len` valid bytes (or be null when `ops_len == 0`).
/// - `where_clause` must point to `where_clause_len` valid UTF-8 bytes (or be null).
/// - `where_args` and `where_args_lens` must each point to `where_args_count` elements.
/// - Each `where_args[i]` must point to `where_args_lens[i]` valid UTF-8 bytes.
/// - `order` must point to `order_len` valid UTF-8 bytes (or be null).
/// - `out_buf` and `out_size` must be valid, non-null pointers.
#[no_mangle]
pub unsafe extern "C" fn DataShareITypesMarshalPredicatesFromParts(
    ops_buf: *const u8,
    ops_len: u32,
    where_clause: *const u8,
    where_clause_len: u32,
    where_args: *const *const u8,
    where_args_lens: *const u32,
    where_args_count: u32,
    order: *const u8,
    order_len: u32,
    mode: i16,
    out_buf: *mut *mut u8,
    out_size: *mut u32,
) -> i32 {
    if out_buf.is_null() || out_size.is_null() {
        return -1;
    }
    // ops_buf can be null if ops_len == 0 (empty operations)
    if ops_buf.is_null() && ops_len > 0 {
        return -1;
    }

    let mut buf = Vec::with_capacity(ops_len as usize + 256);

    // 1. Copy pre-serialized operations as-is
    if ops_len > 0 {
        let ops = std::slice::from_raw_parts(ops_buf, ops_len as usize);
        if buf.write_all(ops).is_err() {
            return -1;
        }
    }

    // 2. Serialize where_clause
    let wc = if where_clause.is_null() {
        ""
    } else {
        std::str::from_utf8_unchecked(std::slice::from_raw_parts(
            where_clause,
            where_clause_len as usize,
        ))
    };
    if !write_string(&mut buf, wc) {
        return -1;
    }

    // 3. Serialize where_args
    let args_count = where_args_count as usize;
    if !write_basic(&mut buf, &args_count) {
        return -1;
    }
    if args_count > 0 && !where_args.is_null() && !where_args_lens.is_null() {
        let ptrs = std::slice::from_raw_parts(where_args, args_count);
        let lens = std::slice::from_raw_parts(where_args_lens, args_count);
        for i in 0..args_count {
            let s = if ptrs[i].is_null() {
                ""
            } else {
                std::str::from_utf8_unchecked(std::slice::from_raw_parts(ptrs[i], lens[i] as usize))
            };
            if !write_string(&mut buf, s) {
                return -1;
            }
        }
    }

    // 4. Serialize order
    let ord = if order.is_null() {
        ""
    } else {
        std::str::from_utf8_unchecked(std::slice::from_raw_parts(order, order_len as usize))
    };
    if !write_string(&mut buf, ord) {
        return -1;
    }

    // 5. Serialize mode
    if !write_basic(&mut buf, &mode) {
        return -1;
    }

    if buf.len() > MAX_IPC_SIZE {
        return -1;
    }

    let len = buf.len() as u32;
    let boxed = buf.into_boxed_slice();
    let ptr = Box::into_raw(boxed) as *mut u8;
    *out_buf = ptr;
    *out_size = len;
    0
}

// =====================================================================
// Predicates: Unmarshal to parts
// =====================================================================

/// Opaque handle for unmarshal result parts.
pub struct PredicatesParts {
    pub ops_buf: Vec<u8>,
    pub where_clause: String,
    pub where_args: Vec<String>,
    pub order: String,
    pub mode: i16,
}

/// Disassemble a predicates binary buffer into parts.
///
/// The operations portion is returned as raw bytes (for C++ to deserialize
/// via UnmarshalOperationItemVecToBuffer). Simple fields are extracted.
///
/// Returns a PredicatesParts handle, or null on failure.
/// Caller must free via `DataSharePredicatesPartsFree`.
///
/// # Safety
///
/// - `buf` must point to `size` valid bytes (or be null, which returns null).
#[no_mangle]
pub unsafe extern "C" fn DataShareITypesUnmarshalPredicatesToParts(
    buf: *const u8,
    size: u32,
) -> *mut PredicatesParts {
    if buf.is_null() || size == 0 || size as usize > MAX_IPC_SIZE {
        return std::ptr::null_mut();
    }
    let data = std::slice::from_raw_parts(buf, size as usize);
    match unmarshal_to_parts(data) {
        Some(parts) => Box::into_raw(Box::new(parts)),
        None => std::ptr::null_mut(),
    }
}

fn unmarshal_to_parts(data: &[u8]) -> Option<PredicatesParts> {
    use crate::ipc::itypes_utils::read_basic;
    use std::io::Cursor;

    let mut cursor = Cursor::new(data);

    // Read operations count
    let ops_count: usize = read_basic(&mut cursor)?;

    // Skip over the serialized operation items to find where simple fields begin
    skip_operations_in_buffer(&mut cursor, ops_count)?;
    let ops_end = cursor.position() as usize;

    // Operations buffer = data[0..ops_end] (includes the count prefix)
    let ops_buf = data[..ops_end].to_vec();

    // Now read simple fields from ops_end
    let mut cursor2 = Cursor::new(&data[ops_end..]);
    let where_clause = read_string(&mut cursor2)?;
    let where_args = read_string_vec(&mut cursor2)?;
    let order = read_string(&mut cursor2)?;
    let mode: i16 = read_basic(&mut cursor2)?;

    Some(PredicatesParts {
        ops_buf,
        where_clause,
        where_args,
        order,
        mode,
    })
}

/// Skip over serialized operations in a buffer without fully parsing them.
/// This reads the binary format to advance the cursor past all operation items.
fn skip_operations_in_buffer<R: std::io::Read>(r: &mut R, count: usize) -> Option<()> {
    use crate::ipc::itypes_utils::read_basic;

    // Write the count first (already read by caller, but we need to account for it)
    // Actually, the caller already read the count. We need to skip `count` operation items.
    for _ in 0..count {
        // operation: i32
        let _op: i32 = read_basic(r)?;
        // singleParams: vec of SingleValue
        skip_single_value_vec(r)?;
        // multiParams: vec of MultiValue
        skip_multi_value_vec(r)?;
    }
    Some(())
}

fn skip_single_value_vec<R: std::io::Read>(r: &mut R) -> Option<()> {
    use crate::ipc::itypes_utils::read_basic;
    let len: usize = read_basic(r)?;
    for _ in 0..len {
        skip_single_value(r)?;
    }
    Some(())
}

fn skip_single_value<R: std::io::Read>(r: &mut R) -> Option<()> {
    use crate::ipc::itypes_utils::read_basic;
    let type_id: u8 = read_basic(r)?;
    match type_id {
        0 => {} // null
        1 => {
            let _: i32 = read_basic(r)?;
        } // int
        2 => {
            let _: f64 = read_basic(r)?;
        } // double
        3 => {
            let _ = read_string(r)?;
        } // string
        4 => {
            let _: bool = read_basic(r)?;
        } // bool
        5 => {
            let _: i64 = read_basic(r)?;
        } // long
        _ => return None,
    }
    Some(())
}

fn skip_multi_value_vec<R: std::io::Read>(r: &mut R) -> Option<()> {
    use crate::ipc::itypes_utils::read_basic;
    let len: usize = read_basic(r)?;
    for _ in 0..len {
        skip_multi_value(r)?;
    }
    Some(())
}

fn skip_multi_value<R: std::io::Read>(r: &mut R) -> Option<()> {
    use crate::ipc::itypes_utils::read_basic;
    let type_id: u8 = read_basic(r)?;
    match type_id {
        0 => {} // null
        1 => {
            // int vec
            let count: usize = read_basic(r)?;
            skip_bytes(r, count * std::mem::size_of::<i32>())?;
        }
        2 => {
            // long vec
            let count: usize = read_basic(r)?;
            skip_bytes(r, count * std::mem::size_of::<i64>())?;
        }
        3 => {
            // double vec
            let count: usize = read_basic(r)?;
            skip_bytes(r, count * std::mem::size_of::<f64>())?;
        }
        4 => {
            // string vec
            let count: usize = read_basic(r)?;
            for _ in 0..count {
                let _ = read_string(r)?;
            }
        }
        _ => return None,
    }
    Some(())
}

fn skip_bytes<R: std::io::Read>(r: &mut R, n: usize) -> Option<()> {
    let mut buf = vec![0u8; n];
    r.read_exact(&mut buf).ok()?;
    Some(())
}

// =====================================================================
// PredicatesParts accessors
// =====================================================================

/// Get the operations buffer from PredicatesParts.
///
/// # Safety
///
/// - `parts` must be a valid pointer returned by `DataShareITypesUnmarshalPredicatesToParts`.
/// - `out_buf` and `out_len` must be valid, non-null pointers.
#[no_mangle]
pub unsafe extern "C" fn DataSharePredicatesPartsGetOpsBuf(
    parts: *const PredicatesParts,
    out_buf: *mut *const u8,
    out_len: *mut u32,
) -> i32 {
    if parts.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    *out_buf = (*parts).ops_buf.as_ptr();
    *out_len = (*parts).ops_buf.len() as u32;
    0
}

/// Get the where_clause from PredicatesParts.
///
/// # Safety
///
/// - `parts` must be a valid pointer returned by `DataShareITypesUnmarshalPredicatesToParts`.
/// - `out_buf` and `out_len` must be valid, non-null pointers.
#[no_mangle]
pub unsafe extern "C" fn DataSharePredicatesPartsGetWhereClause(
    parts: *const PredicatesParts,
    out_buf: *mut *const u8,
    out_len: *mut u32,
) -> i32 {
    if parts.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    *out_buf = (*parts).where_clause.as_ptr();
    *out_len = (*parts).where_clause.len() as u32;
    0
}

/// Get the where_args count from PredicatesParts.
///
/// # Safety
///
/// - `parts` must be a valid pointer returned by `DataShareITypesUnmarshalPredicatesToParts`
///   (or null, which returns 0).
#[no_mangle]
pub unsafe extern "C" fn DataSharePredicatesPartsGetWhereArgsCount(
    parts: *const PredicatesParts,
) -> u32 {
    if parts.is_null() {
        return 0;
    }
    (*parts).where_args.len() as u32
}

/// Get a where_arg at index from PredicatesParts.
///
/// # Safety
///
/// - `parts` must be a valid pointer returned by `DataShareITypesUnmarshalPredicatesToParts`.
/// - `out_buf` and `out_len` must be valid, non-null pointers.
/// - `index` must be less than the count returned by `DataSharePredicatesPartsGetWhereArgsCount`.
#[no_mangle]
pub unsafe extern "C" fn DataSharePredicatesPartsGetWhereArg(
    parts: *const PredicatesParts,
    index: u32,
    out_buf: *mut *const u8,
    out_len: *mut u32,
) -> i32 {
    if parts.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let args = &(*parts).where_args;
    if index as usize >= args.len() {
        return -1;
    }
    let arg = &args[index as usize];
    *out_buf = arg.as_ptr();
    *out_len = arg.len() as u32;
    0
}

/// Get the order from PredicatesParts.
///
/// # Safety
///
/// - `parts` must be a valid pointer returned by `DataShareITypesUnmarshalPredicatesToParts`.
/// - `out_buf` and `out_len` must be valid, non-null pointers.
#[no_mangle]
pub unsafe extern "C" fn DataSharePredicatesPartsGetOrder(
    parts: *const PredicatesParts,
    out_buf: *mut *const u8,
    out_len: *mut u32,
) -> i32 {
    if parts.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    *out_buf = (*parts).order.as_ptr();
    *out_len = (*parts).order.len() as u32;
    0
}

/// Get the mode from PredicatesParts.
///
/// # Safety
///
/// - `parts` must be a valid pointer returned by `DataShareITypesUnmarshalPredicatesToParts`
///   (or null, which returns 0).
#[no_mangle]
pub unsafe extern "C" fn DataSharePredicatesPartsGetMode(parts: *const PredicatesParts) -> i16 {
    if parts.is_null() {
        return 0;
    }
    (*parts).mode
}

/// Free a PredicatesParts handle.
///
/// # Safety
///
/// - `parts` must be a pointer returned by `DataShareITypesUnmarshalPredicatesToParts`
///   (or null, which is a no-op). Must not be freed twice.
#[no_mangle]
pub unsafe extern "C" fn DataSharePredicatesPartsFree(parts: *mut PredicatesParts) {
    if !parts.is_null() {
        let _ = Box::from_raw(parts);
    }
}

// =====================================================================
// ValuesBucketVec element access (for C++ unmarshal direction)
// =====================================================================

/// Get a borrowed reference to a ValuesBucket at index.
/// The returned pointer is valid as long as the ValuesBucketVec is alive.
/// Do NOT free the returned pointer.
///
/// # Safety
///
/// - `vec` must be a valid pointer to a `ValuesBucketVec` (or null, which returns null).
/// - `index` must be less than the vec length.
#[no_mangle]
pub unsafe extern "C" fn DataShareValuesBucketVecGetAt(
    vec: *const ValuesBucketVec,
    index: u32,
) -> *const DataShareValuesBucket {
    if vec.is_null() {
        return std::ptr::null();
    }
    let inner = &(*vec).inner;
    if index as usize >= inner.len() {
        return std::ptr::null();
    }
    &inner[index as usize] as *const DataShareValuesBucket
}

/// Get the number of key-value pairs in a ValuesBucket.
///
/// # Safety
///
/// - `bucket` must be a valid pointer to a `DataShareValuesBucket` (or null, which returns 0).
#[no_mangle]
pub unsafe extern "C" fn DataShareValuesBucketGetKeyCount(
    bucket: *const DataShareValuesBucket,
) -> u32 {
    if bucket.is_null() {
        return 0;
    }
    (*bucket).size() as u32
}

/// Get the key at a given index (iteration order).
/// Returns 0 on success, -1 on failure.
/// out_buf/out_len point to the key string (borrowed, valid while bucket lives).
///
/// # Safety
///
/// - `bucket` must be a valid pointer to a `DataShareValuesBucket`.
/// - `out_buf` and `out_len` must be valid, non-null pointers.
#[no_mangle]
pub unsafe extern "C" fn DataShareValuesBucketGetKeyAt(
    bucket: *const DataShareValuesBucket,
    index: u32,
    out_buf: *mut *const u8,
    out_len: *mut u32,
) -> i32 {
    if bucket.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let map = (*bucket).get_all();
    if let Some((key, _)) = map.iter().nth(index as usize) {
        *out_buf = key.as_ptr();
        *out_len = key.len() as u32;
        0
    } else {
        -1
    }
}

/// Get the value type for a given key.
/// Returns the DataShareValueType as i32, or -1 if not found.
///
/// # Safety
///
/// - `bucket` must be a valid pointer to a `DataShareValuesBucket`.
/// - `key` must point to `key_len` valid UTF-8 bytes.
#[no_mangle]
pub unsafe extern "C" fn DataShareValuesBucketGetValueType(
    bucket: *const DataShareValuesBucket,
    key: *const u8,
    key_len: u32,
) -> i32 {
    if bucket.is_null() || key.is_null() {
        return -1;
    }
    let k = std::str::from_utf8_unchecked(std::slice::from_raw_parts(key, key_len as usize));
    match (*bucket).get(k) {
        Some(DataShareValue::Null) => DataShareValueType::Null as i32,
        Some(DataShareValue::Int(_)) => DataShareValueType::Int as i32,
        Some(DataShareValue::Double(_)) => DataShareValueType::Double as i32,
        Some(DataShareValue::String(_)) => DataShareValueType::String as i32,
        Some(DataShareValue::Bool(_)) => DataShareValueType::Bool as i32,
        Some(DataShareValue::Blob(_)) => DataShareValueType::Blob as i32,
        None => -1,
    }
}

/// Get an int64 value for a given key.
///
/// # Safety
///
/// - `bucket` must be a valid pointer to a `DataShareValuesBucket`.
/// - `key` must point to `key_len` valid UTF-8 bytes.
/// - `out_val` must be a valid, non-null pointer.
#[no_mangle]
pub unsafe extern "C" fn DataShareValuesBucketGetInt(
    bucket: *const DataShareValuesBucket,
    key: *const u8,
    key_len: u32,
    out_val: *mut i64,
) -> i32 {
    if bucket.is_null() || key.is_null() || out_val.is_null() {
        return -1;
    }
    let k = std::str::from_utf8_unchecked(std::slice::from_raw_parts(key, key_len as usize));
    match (*bucket).get(k) {
        Some(DataShareValue::Int(v)) => {
            *out_val = *v;
            0
        }
        _ => -1,
    }
}

/// Get a double value for a given key.
///
/// # Safety
///
/// - `bucket` must be a valid pointer to a `DataShareValuesBucket`.
/// - `key` must point to `key_len` valid UTF-8 bytes.
/// - `out_val` must be a valid, non-null pointer.
#[no_mangle]
pub unsafe extern "C" fn DataShareValuesBucketGetDouble(
    bucket: *const DataShareValuesBucket,
    key: *const u8,
    key_len: u32,
    out_val: *mut f64,
) -> i32 {
    if bucket.is_null() || key.is_null() || out_val.is_null() {
        return -1;
    }
    let k = std::str::from_utf8_unchecked(std::slice::from_raw_parts(key, key_len as usize));
    match (*bucket).get(k) {
        Some(DataShareValue::Double(v)) => {
            *out_val = *v;
            0
        }
        _ => -1,
    }
}

/// Get a string value for a given key (borrowed pointer).
///
/// # Safety
///
/// - `bucket` must be a valid pointer to a `DataShareValuesBucket`.
/// - `key` must point to `key_len` valid UTF-8 bytes.
/// - `out_buf` and `out_len` must be valid, non-null pointers.
#[no_mangle]
pub unsafe extern "C" fn DataShareValuesBucketGetString(
    bucket: *const DataShareValuesBucket,
    key: *const u8,
    key_len: u32,
    out_buf: *mut *const u8,
    out_len: *mut u32,
) -> i32 {
    if bucket.is_null() || key.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let k = std::str::from_utf8_unchecked(std::slice::from_raw_parts(key, key_len as usize));
    match (*bucket).get(k) {
        Some(DataShareValue::String(v)) => {
            *out_buf = v.as_ptr();
            *out_len = v.len() as u32;
            0
        }
        _ => -1,
    }
}

/// Get a bool value for a given key.
///
/// # Safety
///
/// - `bucket` must be a valid pointer to a `DataShareValuesBucket`.
/// - `key` must point to `key_len` valid UTF-8 bytes.
/// - `out_val` must be a valid, non-null pointer.
#[no_mangle]
pub unsafe extern "C" fn DataShareValuesBucketGetBool(
    bucket: *const DataShareValuesBucket,
    key: *const u8,
    key_len: u32,
    out_val: *mut bool,
) -> i32 {
    if bucket.is_null() || key.is_null() || out_val.is_null() {
        return -1;
    }
    let k = std::str::from_utf8_unchecked(std::slice::from_raw_parts(key, key_len as usize));
    match (*bucket).get(k) {
        Some(DataShareValue::Bool(v)) => {
            *out_val = *v;
            0
        }
        _ => -1,
    }
}

/// Get a blob value for a given key (borrowed pointer).
///
/// # Safety
///
/// - `bucket` must be a valid pointer to a `DataShareValuesBucket`.
/// - `key` must point to `key_len` valid UTF-8 bytes.
/// - `out_buf` and `out_len` must be valid, non-null pointers.
#[no_mangle]
pub unsafe extern "C" fn DataShareValuesBucketGetBlob(
    bucket: *const DataShareValuesBucket,
    key: *const u8,
    key_len: u32,
    out_buf: *mut *const u8,
    out_len: *mut u32,
) -> i32 {
    if bucket.is_null() || key.is_null() || out_buf.is_null() || out_len.is_null() {
        return -1;
    }
    let k = std::str::from_utf8_unchecked(std::slice::from_raw_parts(key, key_len as usize));
    match (*bucket).get(k) {
        Some(DataShareValue::Blob(v)) => {
            *out_buf = v.as_ptr();
            *out_len = v.len() as u32;
            0
        }
        _ => -1,
    }
}
