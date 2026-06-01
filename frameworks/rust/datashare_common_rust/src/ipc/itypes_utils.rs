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

//! IPC type serialization/deserialization utilities.
//!
//! Equivalent to C++ `datashare_itypes_utils.cpp` (1,241 lines).
//!
//! Two serialization paths:
//! 1. **Binary buffer path** — direct binary serialization for large payloads (>200KB).
//!    Used by `MarshalPredicates/UnmarshalPredicates` and
//!    `MarshalValuesBucketVec/UnmarshalValuesBucketVec`.
//! 2. **MessageParcel path** — IPC serialization via `ipc::parcel::Serialize/Deserialize`.
//!    Behind `#[cfg(feature = "ipc")]` until ipc_rust dependency is added.

use std::io::{Cursor, Read as IoRead, Write as IoWrite};

use crate::operation::{BackReference, OperationStatement};
use crate::predicates::operations::{OperationItem, OperationType, SingleValue};
use crate::predicates::DataSharePredicates;
use crate::types::MAX_IPC_SIZE;
use crate::value_object::{DataShareValue, DataShareValueType};
use crate::values_bucket::DataShareValuesBucket;

// =====================================================================
// Binary buffer serialization (self-contained, no ipc dependency)
// =====================================================================

/// Write a fixed-size value to the buffer in little-endian format.
pub(crate) fn write_basic<W: IoWrite, T: Copy>(w: &mut W, val: &T) -> bool {
    let bytes = unsafe {
        std::slice::from_raw_parts(val as *const T as *const u8, std::mem::size_of::<T>())
    };
    w.write_all(bytes).is_ok()
}

/// Read a fixed-size value from the buffer in little-endian format.
pub(crate) fn read_basic<R: IoRead, T: Copy + Default>(r: &mut R) -> Option<T> {
    let mut val = T::default();
    let bytes = unsafe {
        std::slice::from_raw_parts_mut(&mut val as *mut T as *mut u8, std::mem::size_of::<T>())
    };
    r.read_exact(bytes).ok()?;
    Some(val)
}

pub(crate) fn write_string<W: IoWrite>(w: &mut W, s: &str) -> bool {
    let len = s.len();
    if !write_basic(w, &len) {
        return false;
    }
    if len > 0 {
        return w.write_all(s.as_bytes()).is_ok();
    }
    true
}

pub(crate) fn read_string<R: IoRead>(r: &mut R) -> Option<String> {
    let len: usize = read_basic(r)?;
    if len > MAX_IPC_SIZE {
        return None;
    }
    if len == 0 {
        return Some(String::new());
    }
    let mut buf = vec![0u8; len];
    r.read_exact(&mut buf).ok()?;
    String::from_utf8(buf).ok()
}

fn write_string_vec<W: IoWrite>(w: &mut W, values: &[String]) -> bool {
    let len = values.len();
    if !write_basic(w, &len) {
        return false;
    }
    for s in values {
        if !write_string(w, s) {
            return false;
        }
    }
    true
}

pub(crate) fn read_string_vec<R: IoRead>(r: &mut R) -> Option<Vec<String>> {
    let len: usize = read_basic(r)?;
    let mut result = Vec::with_capacity(len.min(1024));
    for _ in 0..len {
        result.push(read_string(r)?);
    }
    Some(result)
}

fn write_i32_vec<W: IoWrite>(w: &mut W, values: &[i32]) -> bool {
    let len = values.len();
    if !write_basic(w, &len) {
        return false;
    }
    if len > 0 {
        let bytes = unsafe {
            std::slice::from_raw_parts(values.as_ptr() as *const u8, std::mem::size_of_val(values))
        };
        return w.write_all(bytes).is_ok();
    }
    true
}

fn read_i32_vec<R: IoRead>(r: &mut R) -> Option<Vec<i32>> {
    let len: usize = read_basic(r)?;
    if len > MAX_IPC_SIZE / std::mem::size_of::<i32>() {
        return None;
    }
    let mut result = vec![0i32; len];
    if len > 0 {
        let bytes = unsafe {
            std::slice::from_raw_parts_mut(
                result.as_mut_ptr() as *mut u8,
                std::mem::size_of_val(result.as_slice()),
            )
        };
        r.read_exact(bytes).ok()?;
    }
    Some(result)
}

fn write_i64_vec<W: IoWrite>(w: &mut W, values: &[i64]) -> bool {
    let len = values.len();
    if !write_basic(w, &len) {
        return false;
    }
    if len > 0 {
        let bytes = unsafe {
            std::slice::from_raw_parts(values.as_ptr() as *const u8, std::mem::size_of_val(values))
        };
        return w.write_all(bytes).is_ok();
    }
    true
}

fn read_i64_vec<R: IoRead>(r: &mut R) -> Option<Vec<i64>> {
    let len: usize = read_basic(r)?;
    if len > MAX_IPC_SIZE / std::mem::size_of::<i64>() {
        return None;
    }
    let mut result = vec![0i64; len];
    if len > 0 {
        let bytes = unsafe {
            std::slice::from_raw_parts_mut(
                result.as_mut_ptr() as *mut u8,
                std::mem::size_of_val(result.as_slice()),
            )
        };
        r.read_exact(bytes).ok()?;
    }
    Some(result)
}

fn write_f64_vec<W: IoWrite>(w: &mut W, values: &[f64]) -> bool {
    let len = values.len();
    if !write_basic(w, &len) {
        return false;
    }
    if len > 0 {
        let bytes = unsafe {
            std::slice::from_raw_parts(values.as_ptr() as *const u8, std::mem::size_of_val(values))
        };
        return w.write_all(bytes).is_ok();
    }
    true
}

fn read_f64_vec<R: IoRead>(r: &mut R) -> Option<Vec<f64>> {
    let len: usize = read_basic(r)?;
    if len > MAX_IPC_SIZE / std::mem::size_of::<f64>() {
        return None;
    }
    let mut result = vec![0f64; len];
    if len > 0 {
        let bytes = unsafe {
            std::slice::from_raw_parts_mut(
                result.as_mut_ptr() as *mut u8,
                std::mem::size_of_val(result.as_slice()),
            )
        };
        r.read_exact(bytes).ok()?;
    }
    Some(result)
}

fn write_u8_vec<W: IoWrite>(w: &mut W, values: &[u8]) -> bool {
    let len = values.len();
    if !write_basic(w, &len) {
        return false;
    }
    if len > 0 {
        return w.write_all(values).is_ok();
    }
    true
}

fn read_u8_vec<R: IoRead>(r: &mut R) -> Option<Vec<u8>> {
    let len: usize = read_basic(r)?;
    if len > MAX_IPC_SIZE {
        return None;
    }
    let mut result = vec![0u8; len];
    if len > 0 {
        r.read_exact(&mut result).ok()?;
    }
    Some(result)
}

// --- SingleValue serialization (predicates) ---

/// C++ DataSharePredicatesObjectType enum values
const SINGLE_TYPE_NULL: u8 = 0;
const SINGLE_TYPE_INT: u8 = 1;
const SINGLE_TYPE_DOUBLE: u8 = 2;
const SINGLE_TYPE_STRING: u8 = 3;
const SINGLE_TYPE_BOOL: u8 = 4;
const SINGLE_TYPE_LONG: u8 = 5;

/// Write a SingleValue (predicates parameter) to buffer.
/// C++ uses variant<monostate, int, double, string, bool, int64_t>.
fn write_single_value<W: IoWrite>(
    w: &mut W,
    value: &Option<crate::predicates::operations::SingleValue>,
) -> bool {
    match value {
        None => write_basic(w, &SINGLE_TYPE_NULL),
        Some(sv) => {
            match sv {
                crate::predicates::operations::SingleValue::Null => {
                    write_basic(w, &SINGLE_TYPE_NULL)
                }
                crate::predicates::operations::SingleValue::Int(v) => {
                    write_basic(w, &SINGLE_TYPE_INT) && write_basic(w, v)
                }
                crate::predicates::operations::SingleValue::Double(v) => {
                    write_basic(w, &SINGLE_TYPE_DOUBLE) && write_basic(w, v)
                }
                crate::predicates::operations::SingleValue::String(v) => {
                    write_basic(w, &SINGLE_TYPE_STRING) && write_string(w, v)
                }
                crate::predicates::operations::SingleValue::Bool(v) => {
                    write_basic(w, &SINGLE_TYPE_BOOL) && write_basic(w, v)
                }
                crate::predicates::operations::SingleValue::Long(v) => {
                    write_basic(w, &SINGLE_TYPE_LONG) && write_basic(w, v)
                }
                // Blob is not part of C++ predicates SingleValue; skip
                crate::predicates::operations::SingleValue::Blob(_) => {
                    write_basic(w, &SINGLE_TYPE_NULL)
                }
            }
        }
    }
}

fn read_single_value<R: IoRead>(r: &mut R) -> Option<crate::predicates::operations::SingleValue> {
    use crate::predicates::operations::SingleValue;
    let type_id: u8 = read_basic(r)?;
    match type_id {
        SINGLE_TYPE_NULL => Some(SingleValue::Null),
        SINGLE_TYPE_INT => {
            let v: i32 = read_basic(r)?;
            Some(SingleValue::Int(v))
        }
        SINGLE_TYPE_DOUBLE => {
            let v: f64 = read_basic(r)?;
            Some(SingleValue::Double(v))
        }
        SINGLE_TYPE_STRING => {
            let v = read_string(r)?;
            Some(SingleValue::String(v))
        }
        SINGLE_TYPE_BOOL => {
            let v: bool = read_basic(r)?;
            Some(SingleValue::Bool(v))
        }
        SINGLE_TYPE_LONG => {
            let v: i64 = read_basic(r)?;
            Some(SingleValue::Long(v))
        }
        _ => None,
    }
}

fn write_single_value_vec<W: IoWrite>(
    w: &mut W,
    values: &[crate::predicates::operations::SingleValue],
) -> bool {
    let len = values.len();
    if !write_basic(w, &len) {
        return false;
    }
    for v in values {
        if !write_single_value(w, &Some(v.clone())) {
            return false;
        }
    }
    true
}

fn read_single_value_vec<R: IoRead>(
    r: &mut R,
) -> Option<Vec<crate::predicates::operations::SingleValue>> {
    let len: usize = read_basic(r)?;
    let mut result = Vec::with_capacity(len.min(1024));
    for _ in 0..len {
        result.push(read_single_value(r)?);
    }
    Some(result)
}

// --- MultiValue serialization ---

/// C++ DataSharePredicatesObjectsType enum values (offset from null base)
const MULTI_TYPE_NULL: u8 = 0;
const MULTI_TYPE_INT_VEC: u8 = 1;
const MULTI_TYPE_LONG_VEC: u8 = 2;
const MULTI_TYPE_DOUBLE_VEC: u8 = 3;
const MULTI_TYPE_STRING_VEC: u8 = 4;

fn write_multi_value<W: IoWrite>(
    w: &mut W,
    value: &Option<Vec<crate::predicates::operations::SingleValue>>,
) -> bool {
    use crate::predicates::operations::SingleValue;
    match value {
        None => write_basic(w, &MULTI_TYPE_NULL),
        Some(values) => {
            if values.is_empty() {
                return write_basic(w, &MULTI_TYPE_NULL);
            }
            // Determine type from first element
            match &values[0] {
                SingleValue::Int(_) => {
                    if !write_basic(w, &MULTI_TYPE_INT_VEC) {
                        return false;
                    }
                    let ints: Vec<i32> = values
                        .iter()
                        .filter_map(|v| {
                            if let SingleValue::Int(i) = v {
                                Some(*i)
                            } else {
                                None
                            }
                        })
                        .collect();
                    write_i32_vec(w, &ints)
                }
                SingleValue::Long(_) => {
                    if !write_basic(w, &MULTI_TYPE_LONG_VEC) {
                        return false;
                    }
                    let longs: Vec<i64> = values
                        .iter()
                        .filter_map(|v| {
                            if let SingleValue::Long(l) = v {
                                Some(*l)
                            } else {
                                None
                            }
                        })
                        .collect();
                    write_i64_vec(w, &longs)
                }
                SingleValue::Double(_) => {
                    if !write_basic(w, &MULTI_TYPE_DOUBLE_VEC) {
                        return false;
                    }
                    let doubles: Vec<f64> = values
                        .iter()
                        .filter_map(|v| {
                            if let SingleValue::Double(d) = v {
                                Some(*d)
                            } else {
                                None
                            }
                        })
                        .collect();
                    write_f64_vec(w, &doubles)
                }
                SingleValue::String(_) => {
                    if !write_basic(w, &MULTI_TYPE_STRING_VEC) {
                        return false;
                    }
                    let strings: Vec<String> = values
                        .iter()
                        .filter_map(|v| {
                            if let SingleValue::String(s) = v {
                                Some(s.clone())
                            } else {
                                None
                            }
                        })
                        .collect();
                    write_string_vec(w, &strings)
                }
                _ => write_basic(w, &MULTI_TYPE_NULL),
            }
        }
    }
}

fn read_multi_value<R: IoRead>(
    r: &mut R,
) -> Option<Vec<crate::predicates::operations::SingleValue>> {
    use crate::predicates::operations::SingleValue;
    let type_id: u8 = read_basic(r)?;
    // Apply offset to get actual type (C++ adds TYPE_NULL offset)
    let adjusted = type_id;
    match adjusted {
        MULTI_TYPE_NULL => Some(Vec::new()),
        MULTI_TYPE_INT_VEC => {
            let ints = read_i32_vec(r)?;
            Some(ints.into_iter().map(SingleValue::Int).collect())
        }
        MULTI_TYPE_LONG_VEC => {
            let longs = read_i64_vec(r)?;
            Some(longs.into_iter().map(SingleValue::Long).collect())
        }
        MULTI_TYPE_DOUBLE_VEC => {
            let doubles = read_f64_vec(r)?;
            Some(doubles.into_iter().map(SingleValue::Double).collect())
        }
        MULTI_TYPE_STRING_VEC => {
            let strings = read_string_vec(r)?;
            Some(strings.into_iter().map(SingleValue::String).collect())
        }
        _ => None,
    }
}

fn write_multi_value_vec<W: IoWrite>(
    w: &mut W,
    values: &[Vec<crate::predicates::operations::SingleValue>],
) -> bool {
    let len = values.len();
    if !write_basic(w, &len) {
        return false;
    }
    for v in values {
        if !write_multi_value(w, &Some(v.clone())) {
            return false;
        }
    }
    true
}

fn read_multi_value_vec<R: IoRead>(
    r: &mut R,
) -> Option<Vec<Vec<crate::predicates::operations::SingleValue>>> {
    let len: usize = read_basic(r)?;
    let mut result = Vec::with_capacity(len.min(1024));
    for _ in 0..len {
        result.push(read_multi_value(r)?);
    }
    Some(result)
}

// --- OperationItem serialization ---

fn write_operation_item<W: IoWrite>(w: &mut W, item: &OperationItem) -> bool {
    let op: i32 = item.operation as i32;
    if !write_basic(w, &op) {
        return false;
    }
    // singleParams: field name (if present) followed by value (if present) and extra params
    let mut single_params: Vec<crate::predicates::operations::SingleValue> = Vec::new();
    if !item.field.is_empty() {
        single_params.push(SingleValue::String(item.field.clone()));
    }
    if let Some(v) = &item.value {
        single_params.push(v.clone());
    }
    for v in &item.extra_single_params {
        single_params.push(v.clone());
    }
    if !write_single_value_vec(w, &single_params) {
        return false;
    }
    // multiParams
    let multi_params: Vec<Vec<crate::predicates::operations::SingleValue>> =
        match &item.multi_values {
            Some(v) => vec![v.clone()],
            None => Vec::new(),
        };
    write_multi_value_vec(w, &multi_params)
}

fn read_operation_item<R: IoRead>(r: &mut R) -> Option<OperationItem> {
    let op: i32 = read_basic(r)?;
    let operation = OperationType::from_i32(op).unwrap_or(OperationType::EqualTo);
    let single_params = read_single_value_vec(r)?;
    let multi_params = read_multi_value_vec(r)?;

    let mut sp_iter = single_params.into_iter();
    let first = sp_iter.next();
    let (field, value, extra_single_params) = match first {
        None => (String::new(), None, Vec::new()),
        Some(first_val) => {
            if matches!(operation, OperationType::Limit | OperationType::Offset) {
                (String::new(), Some(first_val), sp_iter.collect())
            } else if let SingleValue::String(s) = first_val {
                let second = sp_iter.next();
                (s, second, sp_iter.collect())
            } else {
                (String::new(), Some(first_val), sp_iter.collect())
            }
        }
    };

    let multi_values = if multi_params.is_empty() {
        None
    } else {
        multi_params.into_iter().next()
    };

    Some(OperationItem {
        operation,
        field,
        value,
        multi_values,
        extra_single_params,
    })
}

fn write_operation_item_vec<W: IoWrite>(w: &mut W, items: &[OperationItem]) -> bool {
    let len = items.len();
    if !write_basic(w, &len) {
        return false;
    }
    for item in items {
        if !write_operation_item(w, item) {
            return false;
        }
    }
    true
}

fn read_operation_item_vec<R: IoRead>(r: &mut R) -> Option<Vec<OperationItem>> {
    let len: usize = read_basic(r)?;
    let mut result = Vec::with_capacity(len.min(1024));
    for _ in 0..len {
        result.push(read_operation_item(r)?);
    }
    Some(result)
}

// --- DataShareValue serialization ---

/// C++ DataShareValueObjectType enum values
const VALUE_TYPE_NULL: u8 = 0;
const VALUE_TYPE_INT: u8 = 1;
const VALUE_TYPE_DOUBLE: u8 = 2;
const VALUE_TYPE_STRING: u8 = 3;
const VALUE_TYPE_BOOL: u8 = 4;
const VALUE_TYPE_BLOB: u8 = 5;

fn write_datashare_value<W: IoWrite>(w: &mut W, key: &str, value: &DataShareValue) -> bool {
    if !write_string(w, key) {
        return false;
    }
    match value {
        DataShareValue::Null => write_basic(w, &VALUE_TYPE_NULL),
        DataShareValue::Int(v) => write_basic(w, &VALUE_TYPE_INT) && write_basic(w, v),
        DataShareValue::Double(v) => write_basic(w, &VALUE_TYPE_DOUBLE) && write_basic(w, v),
        DataShareValue::String(v) => write_basic(w, &VALUE_TYPE_STRING) && write_string(w, v),
        DataShareValue::Bool(v) => write_basic(w, &VALUE_TYPE_BOOL) && write_basic(w, v),
        DataShareValue::Blob(v) => write_basic(w, &VALUE_TYPE_BLOB) && write_u8_vec(w, v),
    }
}

fn read_datashare_value<R: IoRead>(r: &mut R) -> Option<(String, DataShareValue)> {
    let key = read_string(r)?;
    let type_id: u8 = read_basic(r)?;
    let value = match type_id {
        VALUE_TYPE_NULL => DataShareValue::Null,
        VALUE_TYPE_INT => {
            let v: i64 = read_basic(r)?;
            DataShareValue::Int(v)
        }
        VALUE_TYPE_DOUBLE => {
            let v: f64 = read_basic(r)?;
            DataShareValue::Double(v)
        }
        VALUE_TYPE_STRING => {
            let v = read_string(r)?;
            DataShareValue::String(v)
        }
        VALUE_TYPE_BOOL => {
            let v: bool = read_basic(r)?;
            DataShareValue::Bool(v)
        }
        VALUE_TYPE_BLOB => {
            let v = read_u8_vec(r)?;
            DataShareValue::Blob(v)
        }
        _ => return None,
    };
    Some((key, value))
}

// --- Public API: Binary buffer predicates ---

/// Serialize DataSharePredicates to binary buffer.
/// Equivalent to C++ `MarshalPredicatesToBuffer`.
pub fn marshal_predicates_to_buffer(predicates: &DataSharePredicates) -> Option<Vec<u8>> {
    let mut buf = Vec::new();
    let operations = predicates.get_operation_list();
    if !write_operation_item_vec(&mut buf, operations) {
        return None;
    }
    if !write_string(&mut buf, predicates.get_where_clause()) {
        return None;
    }
    if !write_string_vec(&mut buf, predicates.get_where_args()) {
        return None;
    }
    if !write_string(&mut buf, predicates.get_order()) {
        return None;
    }
    let mode = predicates.get_setting_mode();
    if !write_basic(&mut buf, &mode) {
        return None;
    }
    Some(buf)
}

/// Deserialize DataSharePredicates from binary buffer.
/// Equivalent to C++ `UnmarshalPredicatesToBuffer`.
pub fn unmarshal_predicates_from_buffer(data: &[u8]) -> Option<DataSharePredicates> {
    let mut cursor = Cursor::new(data);
    let operations = read_operation_item_vec(&mut cursor)?;
    let where_clause = read_string(&mut cursor)?;
    let where_args = read_string_vec(&mut cursor)?;
    let order = read_string(&mut cursor)?;
    let mode: i16 = read_basic(&mut cursor)?;

    let mut predicates = DataSharePredicates::from_operations(operations);
    predicates.set_where_clause(&where_clause);
    predicates.set_where_args(where_args);
    predicates.set_order(&order);
    predicates.set_setting_mode(mode);
    Some(predicates)
}

/// Serialize a Vec<DataShareValuesBucket> to binary buffer.
/// Equivalent to C++ `MarshalValuesBucketVecToBuffer`.
pub fn marshal_values_bucket_vec_to_buffer(values: &[DataShareValuesBucket]) -> Option<Vec<u8>> {
    let mut buf = Vec::new();
    let size = values.len();
    if !write_basic(&mut buf, &size) {
        return None;
    }
    for bucket in values {
        let map = bucket.get_all();
        let map_size = map.len();
        if !write_basic(&mut buf, &map_size) {
            return None;
        }
        for (key, value) in map {
            if !write_datashare_value(&mut buf, key, value) {
                return None;
            }
        }
    }
    Some(buf)
}

/// Deserialize a Vec<DataShareValuesBucket> from binary buffer.
/// Equivalent to C++ `UnmarshalValuesBucketVecToBuffer`.
pub fn unmarshal_values_bucket_vec_from_buffer(data: &[u8]) -> Option<Vec<DataShareValuesBucket>> {
    let mut cursor = Cursor::new(data);
    let size: usize = read_basic(&mut cursor)?;
    let mut result = Vec::with_capacity(size.min(1024));
    for _ in 0..size {
        let map_size: usize = read_basic(&mut cursor)?;
        let mut bucket = DataShareValuesBucket::new();
        for _ in 0..map_size {
            let (key, value) = read_datashare_value(&mut cursor)?;
            bucket.put(&key, value);
        }
        result.push(bucket);
    }
    Some(result)
}

/// Serialize predicates to a raw data buffer with size prefix.
/// Equivalent to C++ `MarshalPredicates` (writes size + raw data to parcel).
/// Returns (size: i32, data: Vec<u8>) suitable for WriteRawData.
pub fn marshal_predicates_raw(predicates: &DataSharePredicates) -> Option<(i32, Vec<u8>)> {
    let buf = marshal_predicates_to_buffer(predicates)?;
    let size = buf.len();
    if size > MAX_IPC_SIZE {
        return None;
    }
    Some((size as i32, buf))
}

/// Deserialize predicates from a raw data buffer with known size.
/// Equivalent to C++ `UnmarshalPredicates` (reads from ReadRawData result).
pub fn unmarshal_predicates_raw(data: &[u8]) -> Option<DataSharePredicates> {
    if data.is_empty() || data.len() > MAX_IPC_SIZE {
        return None;
    }
    unmarshal_predicates_from_buffer(data)
}

/// Serialize ValuesBucketVec to a raw data buffer with size prefix.
/// Equivalent to C++ `MarshalValuesBucketVec`.
pub fn marshal_values_bucket_vec_raw(values: &[DataShareValuesBucket]) -> Option<(i32, Vec<u8>)> {
    let buf = marshal_values_bucket_vec_to_buffer(values)?;
    let size = buf.len();
    if size > MAX_IPC_SIZE {
        return None;
    }
    Some((size as i32, buf))
}

/// Deserialize ValuesBucketVec from a raw data buffer.
/// Equivalent to C++ `UnmarshalValuesBucketVec`.
pub fn unmarshal_values_bucket_vec_raw(data: &[u8]) -> Option<Vec<DataShareValuesBucket>> {
    if data.is_empty() || data.len() > MAX_IPC_SIZE {
        return None;
    }
    unmarshal_values_bucket_vec_from_buffer(data)
}

// --- BackReference binary buffer serialization ---

fn write_back_reference<W: IoWrite>(w: &mut W, br: &BackReference) -> bool {
    write_string(w, &br.column) && write_basic(w, &br.from_index)
}

fn read_back_reference<R: IoRead>(r: &mut R) -> Option<BackReference> {
    let column = read_string(r)?;
    let from_index: i32 = read_basic(r)?;
    Some(BackReference { column, from_index })
}

// --- ValuesBucket binary buffer serialization (single) ---

fn write_values_bucket_to_buffer<W: IoWrite>(w: &mut W, bucket: &DataShareValuesBucket) -> bool {
    let map = bucket.get_all();
    let map_size = map.len();
    if !write_basic(w, &map_size) {
        return false;
    }
    for (key, value) in map {
        if !write_datashare_value(w, key, value) {
            return false;
        }
    }
    true
}

fn read_values_bucket_from_buffer<R: IoRead>(r: &mut R) -> Option<DataShareValuesBucket> {
    let map_size: usize = read_basic(r)?;
    let mut bucket = DataShareValuesBucket::new();
    for _ in 0..map_size {
        let (key, value) = read_datashare_value(r)?;
        bucket.put(&key, value);
    }
    Some(bucket)
}

// --- OperationStatement binary blob (MarshalOperationStatementVec) ---

fn marshal_operation_statement_vec_to_buffer(stmts: &[OperationStatement]) -> Option<Vec<u8>> {
    let mut buf = Vec::new();
    let size = stmts.len();
    if !write_basic(&mut buf, &size) {
        return None;
    }
    for stmt in stmts {
        let op = stmt.operation_type as i32;
        if !write_basic(&mut buf, &op) {
            return None;
        }
        if !write_string(&mut buf, &stmt.uri) {
            return None;
        }
        let preds_buf = marshal_predicates_to_buffer(&stmt.predicates)?;
        buf.extend_from_slice(&preds_buf);
        if !write_values_bucket_to_buffer(&mut buf, &stmt.values_bucket) {
            return None;
        }
        if !write_back_reference(&mut buf, &stmt.back_reference) {
            return None;
        }
    }
    Some(buf)
}

fn unmarshal_operation_statement_vec_from_buffer(data: &[u8]) -> Option<Vec<OperationStatement>> {
    let mut cursor = Cursor::new(data);
    let size: usize = read_basic(&mut cursor)?;
    let mut stmts = Vec::with_capacity(size.min(1024));
    for _ in 0..size {
        let op: i32 = read_basic(&mut cursor)?;
        let operation_type = crate::operation::Operation::from_i32(op)
            .unwrap_or(crate::operation::Operation::Insert);
        let uri = read_string(&mut cursor)?;
        let predicates = {
            let operations = read_operation_item_vec(&mut cursor)?;
            let where_clause = read_string(&mut cursor)?;
            let where_args = read_string_vec(&mut cursor)?;
            let order = read_string(&mut cursor)?;
            let mode: i16 = read_basic(&mut cursor)?;
            let mut p = DataSharePredicates::from_operations(operations);
            p.set_where_clause(&where_clause);
            p.set_where_args(where_args);
            p.set_order(&order);
            p.set_setting_mode(mode);
            p
        };
        let values_bucket = read_values_bucket_from_buffer(&mut cursor)?;
        let back_reference = read_back_reference(&mut cursor)?;
        stmts.push(OperationStatement {
            operation_type,
            uri,
            predicates,
            values_bucket,
            back_reference,
        });
    }
    Some(stmts)
}

/// Serialize OperationStatementVec to binary blob for IPC WriteRawData.
/// Equivalent to C++ `MarshalOperationStatementVecToBuffer`.
pub fn marshal_operation_statement_vec(stmts: &[OperationStatement]) -> Option<Vec<u8>> {
    let buf = marshal_operation_statement_vec_to_buffer(stmts)?;
    if buf.len() > MAX_IPC_SIZE {
        return None;
    }
    Some(buf)
}

/// Deserialize OperationStatementVec from binary blob.
/// Equivalent to C++ `UnmarshalOperationStatementVecToBuffer`.
pub fn unmarshal_operation_statement_vec(data: &[u8]) -> Option<Vec<OperationStatement>> {
    if data.is_empty() || data.len() > MAX_IPC_SIZE {
        return None;
    }
    unmarshal_operation_statement_vec_from_buffer(data)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::predicates::operations::SingleValue;

    #[test]
    fn test_write_read_string() {
        let mut buf = Vec::new();
        assert!(write_string(&mut buf, "hello"));
        let mut cursor = Cursor::new(&buf[..]);
        assert_eq!(read_string(&mut cursor), Some("hello".to_string()));
    }

    #[test]
    fn test_write_read_empty_string() {
        let mut buf = Vec::new();
        assert!(write_string(&mut buf, ""));
        let mut cursor = Cursor::new(&buf[..]);
        assert_eq!(read_string(&mut cursor), Some(String::new()));
    }

    #[test]
    fn test_write_read_string_vec() {
        let values = vec!["a".to_string(), "bb".to_string(), "ccc".to_string()];
        let mut buf = Vec::new();
        assert!(write_string_vec(&mut buf, &values));
        let mut cursor = Cursor::new(&buf[..]);
        assert_eq!(read_string_vec(&mut cursor), Some(values));
    }

    #[test]
    fn test_write_read_i32() {
        let mut buf = Vec::new();
        let val: i32 = -42;
        assert!(write_basic(&mut buf, &val));
        let mut cursor = Cursor::new(&buf[..]);
        assert_eq!(read_basic::<_, i32>(&mut cursor), Some(-42));
    }

    #[test]
    fn test_write_read_i64() {
        let mut buf = Vec::new();
        let val: i64 = i64::MAX;
        assert!(write_basic(&mut buf, &val));
        let mut cursor = Cursor::new(&buf[..]);
        assert_eq!(read_basic::<_, i64>(&mut cursor), Some(i64::MAX));
    }

    #[test]
    fn test_write_read_f64() {
        let mut buf = Vec::new();
        let val: f64 = 3.14159;
        assert!(write_basic(&mut buf, &val));
        let mut cursor = Cursor::new(&buf[..]);
        let result: f64 = read_basic(&mut cursor).unwrap();
        assert!((result - 3.14159).abs() < 1e-10);
    }

    #[test]
    fn test_write_read_i32_vec() {
        let values = vec![1, -2, 3, 0, i32::MAX];
        let mut buf = Vec::new();
        assert!(write_i32_vec(&mut buf, &values));
        let mut cursor = Cursor::new(&buf[..]);
        assert_eq!(read_i32_vec(&mut cursor), Some(values));
    }

    #[test]
    fn test_write_read_u8_vec() {
        let values = vec![0u8, 1, 255, 128];
        let mut buf = Vec::new();
        assert!(write_u8_vec(&mut buf, &values));
        let mut cursor = Cursor::new(&buf[..]);
        assert_eq!(read_u8_vec(&mut cursor), Some(values));
    }

    #[test]
    fn test_single_value_roundtrip() {
        let values = vec![
            SingleValue::Null,
            SingleValue::Int(42),
            SingleValue::Double(3.14),
            SingleValue::String("test".to_string()),
            SingleValue::Bool(true),
            SingleValue::Long(i64::MAX),
        ];
        let mut buf = Vec::new();
        assert!(write_single_value_vec(&mut buf, &values));
        let mut cursor = Cursor::new(&buf[..]);
        let result = read_single_value_vec(&mut cursor).unwrap();
        assert_eq!(result.len(), values.len());
        assert_eq!(result[1], SingleValue::Int(42));
        assert_eq!(result[3], SingleValue::String("test".to_string()));
        assert_eq!(result[5], SingleValue::Long(i64::MAX));
    }

    #[test]
    fn test_datashare_value_roundtrip() {
        let test_cases = vec![
            ("null_key", DataShareValue::Null),
            ("int_key", DataShareValue::Int(12345)),
            ("double_key", DataShareValue::Double(2.718)),
            (
                "string_key",
                DataShareValue::String("hello world".to_string()),
            ),
            ("bool_key", DataShareValue::Bool(false)),
            ("blob_key", DataShareValue::Blob(vec![1, 2, 3, 4, 5])),
        ];
        for (key, value) in &test_cases {
            let mut buf = Vec::new();
            assert!(write_datashare_value(&mut buf, key, value));
            let mut cursor = Cursor::new(&buf[..]);
            let (read_key, read_value) = read_datashare_value(&mut cursor).unwrap();
            assert_eq!(&read_key, key);
            assert_eq!(&read_value, value);
        }
    }

    #[test]
    fn test_values_bucket_vec_roundtrip() {
        let mut bucket1 = DataShareValuesBucket::new();
        bucket1.put("name", DataShareValue::String("Alice".to_string()));
        bucket1.put("age", DataShareValue::Int(30));

        let mut bucket2 = DataShareValuesBucket::new();
        bucket2.put("name", DataShareValue::String("Bob".to_string()));
        bucket2.put("score", DataShareValue::Double(95.5));

        let buckets = vec![bucket1, bucket2];

        let buf = marshal_values_bucket_vec_to_buffer(&buckets).unwrap();
        let result = unmarshal_values_bucket_vec_from_buffer(&buf).unwrap();

        assert_eq!(result.len(), 2);
        assert_eq!(
            result[0].get("name"),
            Some(&DataShareValue::String("Alice".to_string()))
        );
        assert_eq!(result[0].get("age"), Some(&DataShareValue::Int(30)));
        assert_eq!(
            result[1].get("name"),
            Some(&DataShareValue::String("Bob".to_string()))
        );
        assert_eq!(result[1].get("score"), Some(&DataShareValue::Double(95.5)));
    }

    #[test]
    fn test_predicates_roundtrip() {
        let mut predicates = DataSharePredicates::new();
        predicates.set_where_clause("age > ?");
        predicates.set_where_args(vec!["18".to_string()]);
        predicates.set_order("name ASC");
        predicates.set_setting_mode(1);

        let buf = marshal_predicates_to_buffer(&predicates).unwrap();
        let result = unmarshal_predicates_from_buffer(&buf).unwrap();

        assert_eq!(result.get_where_clause(), "age > ?");
        assert_eq!(result.get_where_args(), &["18".to_string()]);
        assert_eq!(result.get_order(), "name ASC");
        assert_eq!(result.get_setting_mode(), 1);
    }

    #[test]
    fn test_marshal_predicates_raw() {
        let predicates = DataSharePredicates::new();
        let (size, data) = marshal_predicates_raw(&predicates).unwrap();
        assert!(size > 0);
        assert_eq!(data.len(), size as usize);

        let result = unmarshal_predicates_raw(&data).unwrap();
        assert_eq!(result.get_where_clause(), predicates.get_where_clause());
    }

    #[test]
    fn test_marshal_values_bucket_vec_raw() {
        let mut bucket = DataShareValuesBucket::new();
        bucket.put("key", DataShareValue::Int(100));
        let buckets = vec![bucket];

        let (size, data) = marshal_values_bucket_vec_raw(&buckets).unwrap();
        assert!(size > 0);
        assert_eq!(data.len(), size as usize);

        let result = unmarshal_values_bucket_vec_raw(&data).unwrap();
        assert_eq!(result.len(), 1);
        assert_eq!(result[0].get("key"), Some(&DataShareValue::Int(100)));
    }

    #[test]
    fn test_empty_values_bucket_vec() {
        let buckets: Vec<DataShareValuesBucket> = Vec::new();
        let buf = marshal_values_bucket_vec_to_buffer(&buckets).unwrap();
        let result = unmarshal_values_bucket_vec_from_buffer(&buf).unwrap();
        assert!(result.is_empty());
    }

    #[test]
    fn test_empty_predicates() {
        let predicates = DataSharePredicates::new();
        let buf = marshal_predicates_to_buffer(&predicates).unwrap();
        let result = unmarshal_predicates_from_buffer(&buf).unwrap();
        assert!(result.get_operation_list().is_empty());
        assert!(result.get_where_clause().is_empty());
        assert!(result.get_where_args().is_empty());
    }
}
