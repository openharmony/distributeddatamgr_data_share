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

//! C FFI adapter for DataSharePredicates.
//!
//! Provides C-compatible functions for creating, manipulating, and destroying
//! DataSharePredicates objects. Thin C++ wrappers can call these functions
//! to delegate to the Rust implementation.

#![allow(unused_imports)]

use crate::predicates::operations::{OperationItem, OperationType, SingleValue};
use crate::predicates::predicates::DataSharePredicates;

pub(crate) type OpaquePredicates = DataSharePredicates;

// ============================================================
// The following FFI functions are commented out because they are
// not declared in any C++ header and have no known callers.
// Kept for potential future use. Remove block comment to re-enable.
// ============================================================
/*
// --- Construction / Destruction ---

/// Create a new empty DataSharePredicates.
#[no_mangle]
pub extern "C" fn DataSharePredicatesNew() -> *mut OpaquePredicates {
    Box::into_raw(Box::new(DataSharePredicates::new()))
}

/// Free a DataSharePredicates.
#[no_mangle]
pub extern "C" fn DataSharePredicatesFree(pred: *mut OpaquePredicates) {
    if !pred.is_null() {
        unsafe {
            let _ = Box::from_raw(pred);
        }
    }
}

// --- Predicate operations with SingleValue ---

/// Add an EqualTo operation.
#[no_mangle]
pub extern "C" fn DataSharePredicatesEqualToInt(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: i64,
) -> i32 {
    add_single_value_op(pred, field, field_len, OperationType::EqualTo, SingleValue::Long(value))
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesEqualToDouble(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: f64,
) -> i32 {
    add_single_value_op(pred, field, field_len, OperationType::EqualTo, SingleValue::Double(value))
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesEqualToString(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: *const u8,
    value_len: u32,
) -> i32 {
    let val = match read_string(value, value_len) {
        Some(s) => s,
        None => return -1,
    };
    add_single_value_op(pred, field, field_len, OperationType::EqualTo, SingleValue::String(val))
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesEqualToBool(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: bool,
) -> i32 {
    add_single_value_op(pred, field, field_len, OperationType::EqualTo, SingleValue::Bool(value))
}

/// Add a NotEqualTo operation.
#[no_mangle]
pub extern "C" fn DataSharePredicatesNotEqualToInt(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: i64,
) -> i32 {
    add_single_value_op(pred, field, field_len, OperationType::NotEqualTo, SingleValue::Long(value))
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesNotEqualToString(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: *const u8,
    value_len: u32,
) -> i32 {
    let val = match read_string(value, value_len) {
        Some(s) => s,
        None => return -1,
    };
    add_single_value_op(pred, field, field_len, OperationType::NotEqualTo, SingleValue::String(val))
}

/// Add a GreaterThan operation.
#[no_mangle]
pub extern "C" fn DataSharePredicatesGreaterThanInt(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: i64,
) -> i32 {
    add_single_value_op(pred, field, field_len, OperationType::GreaterThan, SingleValue::Long(value))
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesGreaterThanDouble(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: f64,
) -> i32 {
    add_single_value_op(
        pred,
        field,
        field_len,
        OperationType::GreaterThan,
        SingleValue::Double(value),
    )
}

/// Add a LessThan operation.
#[no_mangle]
pub extern "C" fn DataSharePredicatesLessThanInt(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: i64,
) -> i32 {
    add_single_value_op(pred, field, field_len, OperationType::LessThan, SingleValue::Long(value))
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesLessThanDouble(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: f64,
) -> i32 {
    add_single_value_op(
        pred,
        field,
        field_len,
        OperationType::LessThan,
        SingleValue::Double(value),
    )
}

/// Add a GreaterThanOrEqualTo operation.
#[no_mangle]
pub extern "C" fn DataSharePredicatesGreaterThanOrEqualToInt(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: i64,
) -> i32 {
    add_single_value_op(
        pred,
        field,
        field_len,
        OperationType::GreaterThanOrEqualTo,
        SingleValue::Long(value),
    )
}

/// Add a LessThanOrEqualTo operation.
#[no_mangle]
pub extern "C" fn DataSharePredicatesLessThanOrEqualToInt(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: i64,
) -> i32 {
    add_single_value_op(
        pred,
        field,
        field_len,
        OperationType::LessThanOrEqualTo,
        SingleValue::Long(value),
    )
}

// --- String-field operations (Like, Unlike, Contains, BeginWith, EndWith, Glob) ---

#[no_mangle]
pub extern "C" fn DataSharePredicatesLike(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: *const u8,
    value_len: u32,
) -> i32 {
    let val = match read_string(value, value_len) {
        Some(s) => s,
        None => return -1,
    };
    add_single_value_op(pred, field, field_len, OperationType::Like, SingleValue::String(val))
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesUnlike(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: *const u8,
    value_len: u32,
) -> i32 {
    let val = match read_string(value, value_len) {
        Some(s) => s,
        None => return -1,
    };
    add_single_value_op(pred, field, field_len, OperationType::Unlike, SingleValue::String(val))
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesContains(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: *const u8,
    value_len: u32,
) -> i32 {
    let val = match read_string(value, value_len) {
        Some(s) => s,
        None => return -1,
    };
    add_single_value_op(pred, field, field_len, OperationType::Contains, SingleValue::String(val))
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesBeginsWith(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: *const u8,
    value_len: u32,
) -> i32 {
    let val = match read_string(value, value_len) {
        Some(s) => s,
        None => return -1,
    };
    add_single_value_op(pred, field, field_len, OperationType::BeginWith, SingleValue::String(val))
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesEndsWith(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    value: *const u8,
    value_len: u32,
) -> i32 {
    let val = match read_string(value, value_len) {
        Some(s) => s,
        None => return -1,
    };
    add_single_value_op(pred, field, field_len, OperationType::EndWith, SingleValue::String(val))
}

// --- Field-only operations (IsNull, IsNotNull, OrderByAsc, OrderByDesc) ---

#[no_mangle]
pub extern "C" fn DataSharePredicatesIsNull(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
) -> i32 {
    add_field_only_op(pred, field, field_len, OperationType::IsNull)
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesIsNotNull(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
) -> i32 {
    add_field_only_op(pred, field, field_len, OperationType::IsNotNull)
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesOrderByAsc(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
) -> i32 {
    if pred.is_null() || field.is_null() {
        return -1;
    }
    unsafe {
        let f = std::str::from_utf8_unchecked(std::slice::from_raw_parts(field, field_len as usize));
        (*pred).get_operations_mut().push(OperationItem {
            operation: OperationType::OrderByAsc,
            field: f.to_string(),
            value: None,
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        0
    }
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesOrderByDesc(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
) -> i32 {
    if pred.is_null() || field.is_null() {
        return -1;
    }
    unsafe {
        let f = std::str::from_utf8_unchecked(std::slice::from_raw_parts(field, field_len as usize));
        (*pred).get_operations_mut().push(OperationItem {
            operation: OperationType::OrderByDesc,
            field: f.to_string(),
            value: None,
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        0
    }
}

// --- No-argument operations (And, Or) ---

#[no_mangle]
pub extern "C" fn DataSharePredicatesAnd(pred: *mut OpaquePredicates) -> i32 {
    add_no_arg_op(pred, OperationType::And)
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesOr(pred: *mut OpaquePredicates) -> i32 {
    add_no_arg_op(pred, OperationType::Or)
}

// --- Limit / Offset ---

#[no_mangle]
pub extern "C" fn DataSharePredicatesLimit(
    pred: *mut OpaquePredicates,
    number: i32,
    offset: i32,
) -> i32 {
    if pred.is_null() {
        return -1;
    }
    unsafe {
        let p = &mut *pred;
        p.get_operations_mut().push(OperationItem {
            operation: OperationType::Limit,
            field: String::new(),
            value: Some(SingleValue::Int(number)),
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        p.get_operations_mut().push(OperationItem {
            operation: OperationType::Offset,
            field: String::new(),
            value: Some(SingleValue::Int(offset)),
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        0
    }
}

// --- In / NotIn ---

#[no_mangle]
pub extern "C" fn DataSharePredicatesInInt(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    values: *const i64,
    values_count: u32,
) -> i32 {
    if pred.is_null() || field.is_null() || values.is_null() {
        return -1;
    }
    unsafe {
        let f = std::str::from_utf8_unchecked(std::slice::from_raw_parts(field, field_len as usize));
        let vals = std::slice::from_raw_parts(values, values_count as usize);
        let multi: Vec<SingleValue> = vals.iter().map(|&v| SingleValue::Long(v)).collect();
        (*pred).get_operations_mut().push(OperationItem {
            operation: OperationType::In,
            field: f.to_string(),
            value: None,
            multi_values: Some(multi),
            extra_single_params: Vec::new(),
        });
        0
    }
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesNotInInt(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    values: *const i64,
    values_count: u32,
) -> i32 {
    if pred.is_null() || field.is_null() || values.is_null() {
        return -1;
    }
    unsafe {
        let f = std::str::from_utf8_unchecked(std::slice::from_raw_parts(field, field_len as usize));
        let vals = std::slice::from_raw_parts(values, values_count as usize);
        let multi: Vec<SingleValue> = vals.iter().map(|&v| SingleValue::Long(v)).collect();
        (*pred).get_operations_mut().push(OperationItem {
            operation: OperationType::NotIn,
            field: f.to_string(),
            value: None,
            multi_values: Some(multi),
            extra_single_params: Vec::new(),
        });
        0
    }
}

// --- Accessors for WhereClause / WhereArgs / Order / SettingMode ---

#[no_mangle]
pub extern "C" fn DataSharePredicatesGetWhereClause(
    pred: *const OpaquePredicates,
    out_buf: *mut u8,
    buf_len: u32,
    out_len: *mut u32,
) -> i32 {
    if pred.is_null() || out_len.is_null() {
        return -1;
    }
    unsafe {
        let clause = (*pred).get_where_clause();
        let bytes = clause.as_bytes();
        *out_len = bytes.len() as u32;
        if !out_buf.is_null() && buf_len >= bytes.len() as u32 {
            std::ptr::copy_nonoverlapping(bytes.as_ptr(), out_buf, bytes.len());
        }
        0
    }
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesSetWhereClause(
    pred: *mut OpaquePredicates,
    clause: *const u8,
    clause_len: u32,
) -> i32 {
    if pred.is_null() || clause.is_null() {
        return -1;
    }
    unsafe {
        let s = std::str::from_utf8_unchecked(std::slice::from_raw_parts(clause, clause_len as usize));
        (*pred).set_where_clause(s);
        0
    }
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesGetOrder(
    pred: *const OpaquePredicates,
    out_buf: *mut u8,
    buf_len: u32,
    out_len: *mut u32,
) -> i32 {
    if pred.is_null() || out_len.is_null() {
        return -1;
    }
    unsafe {
        let order = (*pred).get_order();
        let bytes = order.as_bytes();
        *out_len = bytes.len() as u32;
        if !out_buf.is_null() && buf_len >= bytes.len() as u32 {
            std::ptr::copy_nonoverlapping(bytes.as_ptr(), out_buf, bytes.len());
        }
        0
    }
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesSetOrder(
    pred: *mut OpaquePredicates,
    order: *const u8,
    order_len: u32,
) -> i32 {
    if pred.is_null() || order.is_null() {
        return -1;
    }
    unsafe {
        let s = std::str::from_utf8_unchecked(std::slice::from_raw_parts(order, order_len as usize));
        (*pred).set_order(s);
        0
    }
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesGetSettingMode(pred: *const OpaquePredicates) -> i16 {
    if pred.is_null() {
        return -1;
    }
    unsafe { (*pred).get_setting_mode() }
}

#[no_mangle]
pub extern "C" fn DataSharePredicatesSetSettingMode(pred: *mut OpaquePredicates, mode: i16) {
    if !pred.is_null() {
        unsafe {
            (*pred).set_setting_mode(mode);
        }
    }
}

/// Get number of operations.
#[no_mangle]
pub extern "C" fn DataSharePredicatesGetOperationCount(pred: *const OpaquePredicates) -> u32 {
    if pred.is_null() {
        return 0;
    }
    unsafe { (*pred).count() as u32 }
}

/// Check if predicates is empty.
#[no_mangle]
pub extern "C" fn DataSharePredicatesIsEmpty(pred: *const OpaquePredicates) -> bool {
    if pred.is_null() {
        return true;
    }
    unsafe { (*pred).is_empty() }
}

/// Clear all operations.
#[no_mangle]
pub extern "C" fn DataSharePredicatesClear(pred: *mut OpaquePredicates) -> i32 {
    if pred.is_null() {
        return -1;
    }
    unsafe {
        (*pred).clear();
        0
    }
}

// --- Internal helpers ---

fn read_string(ptr: *const u8, len: u32) -> Option<String> {
    if ptr.is_null() {
        return None;
    }
    unsafe {
        let s = std::str::from_utf8_unchecked(std::slice::from_raw_parts(ptr, len as usize));
        Some(s.to_string())
    }
}

fn add_single_value_op(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    op: OperationType,
    value: SingleValue,
) -> i32 {
    if pred.is_null() || field.is_null() {
        return -1;
    }
    unsafe {
        let f = std::str::from_utf8_unchecked(std::slice::from_raw_parts(field, field_len as usize));
        (*pred).get_operations_mut().push(OperationItem {
            operation: op,
            field: f.to_string(),
            value: Some(value),
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        0
    }
}

fn add_field_only_op(
    pred: *mut OpaquePredicates,
    field: *const u8,
    field_len: u32,
    op: OperationType,
) -> i32 {
    if pred.is_null() || field.is_null() {
        return -1;
    }
    unsafe {
        let f = std::str::from_utf8_unchecked(std::slice::from_raw_parts(field, field_len as usize));
        (*pred).get_operations_mut().push(OperationItem {
            operation: op,
            field: f.to_string(),
            value: None,
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        0
    }
}

fn add_no_arg_op(pred: *mut OpaquePredicates, op: OperationType) -> i32 {
    if pred.is_null() {
        return -1;
    }
    unsafe {
        (*pred).get_operations_mut().push(OperationItem {
            operation: op,
            field: String::new(),
            value: None,
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        0
    }
}
*/ // end of commented-out predicates FFI functions
