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

//! C FFI for predicate verification functions.

use crate::c_adapter::predicates_ffi::OpaquePredicates;
use crate::predicates::operations::OperationType;
use crate::predicates::verify::{verify_field, verify_predicates};

/// Verify all predicates in a DataSharePredicates instance.
///
/// On success, writes (0, 0) to `out_op_type` and `out_err_code`.
/// On failure, writes the failing operation type and error code.
/// Returns 0 on success, -1 if pointers are null.
///
/// # Safety
/// - `pred` must be a valid pointer to an `OpaquePredicates` created by
///   `DataSharePredicatesNew`.
/// - `out_op_type` and `out_err_code` must be valid, non-null pointers.
#[no_mangle]
pub unsafe extern "C" fn DataSharePredicatesVerify(
    pred: *const OpaquePredicates,
    out_op_type: *mut i32,
    out_err_code: *mut i32,
) -> i32 {
    if pred.is_null() || out_op_type.is_null() || out_err_code.is_null() {
        return -1;
    }
    let (op_type, err_code) = verify_predicates(&*pred);
    *out_op_type = op_type;
    *out_err_code = err_code;
    0
}

/// Verify a single field name against allowed patterns.
///
/// Returns true if the field is valid (or empty), false if invalid.
/// Matches C++ `DataSharePredicatesVerify::VerifyField` behavior.
///
/// # Safety
/// - `field` must be a valid pointer to `field_len` bytes, or null.
#[no_mangle]
pub unsafe extern "C" fn DataShareVerifyField(field: *const u8, field_len: u32) -> bool {
    if field.is_null() || field_len == 0 {
        return true; // empty field is valid (matches C++ behavior)
    }
    let s = std::str::from_utf8_unchecked(std::slice::from_raw_parts(field, field_len as usize));
    verify_field(s)
}

// =====================================================================
// PredicatesVerifyType constants (match C++ enum values)
// =====================================================================
const VERIFY_DEFAULT: i32 = 0;
const SINGLE_2_PARAMS_PUBLIC: i32 = 1;
const SINGLE_3_PARAMS_PUBLIC: i32 = 2;
const SINGLE_2_PARAMS_SYS: i32 = 3;
const SINGLE_3_PARAMS_SYS: i32 = 4;
const MULTI_2_PARAMS_SYS: i32 = 5;

// Error codes (match C++ datashare_errno.h)
const E_OK: i32 = 0;
const E_FIELD_ILLEGAL: i32 = 1087;
const E_FIELD_INVALID: i32 = 1088;

/// Get the verification type for an operation type code.
///
/// Returns PredicatesVerifyType as i32 (0-5).
/// Matches C++ `DataSharePredicatesVerify::GetPredicatesVerifyType`.
#[no_mangle]
pub extern "C" fn DataShareGetPredicatesVerifyType(op_type: i32) -> i32 {
    match OperationType::from_i32(op_type) {
        Some(OperationType::OrderByAsc) | Some(OperationType::OrderByDesc) => {
            SINGLE_2_PARAMS_PUBLIC
        }
        Some(OperationType::EqualTo) => SINGLE_3_PARAMS_PUBLIC,
        Some(OperationType::IsNull)
        | Some(OperationType::IsNotNull)
        | Some(OperationType::IndexedBy)
        | Some(OperationType::KeyPrefix) => SINGLE_2_PARAMS_SYS,
        Some(OperationType::NotEqualTo)
        | Some(OperationType::GreaterThan)
        | Some(OperationType::LessThan)
        | Some(OperationType::GreaterThanOrEqualTo)
        | Some(OperationType::LessThanOrEqualTo)
        | Some(OperationType::NotIn)
        | Some(OperationType::Like)
        | Some(OperationType::Unlike)
        | Some(OperationType::BeginWith)
        | Some(OperationType::EndWith)
        | Some(OperationType::Contains)
        | Some(OperationType::Glob)
        | Some(OperationType::Between)
        | Some(OperationType::NotBetween) => SINGLE_3_PARAMS_SYS,
        Some(OperationType::InKey) | Some(OperationType::GroupBy) => MULTI_2_PARAMS_SYS,
        _ => VERIFY_DEFAULT,
    }
}

/// Check if the operation item has sufficient parameters for its verify type.
///
/// Matches C++ `DataSharePredicatesVerify::CheckParamNum`.
#[no_mangle]
pub extern "C" fn DataShareCheckParamNum(
    verify_type: i32,
    single_count: u32,
    multi_count: u32,
) -> bool {
    match verify_type {
        VERIFY_DEFAULT => true,
        MULTI_2_PARAMS_SYS => multi_count > 0,
        _ => single_count > 0,
    }
}

/// Verify a vector of field names.
///
/// Each field is a (ptr, len) pair. Returns true if all fields are valid.
/// Matches C++ `DataSharePredicatesVerify::VerifyFields`.
///
/// # Safety
/// - `fields` must point to `count` valid pointers.
/// - `field_lens` must point to `count` u32 values.
#[no_mangle]
pub unsafe extern "C" fn DataShareVerifyFields(
    fields: *const *const u8,
    field_lens: *const u32,
    count: u32,
) -> bool {
    if count == 0 {
        return true;
    }
    if fields.is_null() || field_lens.is_null() {
        return true;
    }
    let ptrs = std::slice::from_raw_parts(fields, count as usize);
    let lens = std::slice::from_raw_parts(field_lens, count as usize);
    for i in 0..count as usize {
        if ptrs[i].is_null() || lens[i] == 0 {
            continue; // empty field is valid
        }
        let s =
            std::str::from_utf8_unchecked(std::slice::from_raw_parts(ptrs[i], lens[i] as usize));
        if !verify_field(s) {
            return false;
        }
    }
    true
}

/// Verify a single operation by its classification type.
///
/// Parameters:
/// - `verify_type`: from DataShareGetPredicatesVerifyType
/// - `field/field_len`: the primary field name (first single param)
/// - `single_count`: number of single params (for CheckParamNum)
/// - `multi_count`: number of multi params (for CheckParamNum)
/// - `multi_fields/multi_field_lens/multi_field_count`: multi-value fields to verify
///
/// Returns E_OK (0), E_FIELD_INVALID (1088), or E_FIELD_ILLEGAL (1087).
/// Matches C++ `DataSharePredicatesVerify::VerifyPredicatesByType`.
///
/// # Safety
/// - All pointer parameters must be valid when non-null.
#[no_mangle]
pub unsafe extern "C" fn DataShareVerifyPredicatesByType(
    verify_type: i32,
    field: *const u8,
    field_len: u32,
    single_count: u32,
    multi_count: u32,
    multi_fields: *const *const u8,
    multi_field_lens: *const u32,
    multi_field_count: u32,
) -> i32 {
    // VERIFY_DEFAULT or insufficient params → E_OK (matches C++ behavior)
    if verify_type == VERIFY_DEFAULT
        || !DataShareCheckParamNum(verify_type, single_count, multi_count)
    {
        return E_OK;
    }

    match verify_type {
        SINGLE_2_PARAMS_PUBLIC | SINGLE_3_PARAMS_PUBLIC => {
            if !DataShareVerifyField(field, field_len) {
                return E_FIELD_INVALID;
            }
        }
        SINGLE_2_PARAMS_SYS | SINGLE_3_PARAMS_SYS => {
            if !DataShareVerifyField(field, field_len) {
                return E_FIELD_ILLEGAL;
            }
        }
        MULTI_2_PARAMS_SYS => {
            if !DataShareVerifyFields(multi_fields, multi_field_lens, multi_field_count) {
                return E_FIELD_ILLEGAL;
            }
        }
        _ => {}
    }
    E_OK
}
