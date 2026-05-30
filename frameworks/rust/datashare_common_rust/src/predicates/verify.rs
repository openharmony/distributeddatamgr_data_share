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

//! Predicate field name verification.
//!
//! Validates that field names in predicate operations conform to allowed patterns,
//! preventing SQL injection and invalid column references.
//! Equivalent to C++ `datashare_predicates_verify.cpp`.

use crate::error::DataShareError;
use crate::predicates::operations::{OperationItem, OperationType, SingleValue};
use crate::predicates::DataSharePredicates;

/// Classification of predicate operations for verification purposes.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum PredicatesVerifyType {
    /// Default: no verification needed
    Default,
    /// Public API, single field, 2 params (e.g., ORDER_BY_ASC/DESC)
    Single2ParamsPublic,
    /// Public API, single field, 3 params (e.g., EQUAL_TO)
    Single3ParamsPublic,
    /// System API, single field, 2 params (e.g., IS_NULL, INDEXED_BY)
    Single2ParamsSys,
    /// System API, single field, 3 params (e.g., NOT_EQUAL_TO, LIKE, BETWEEN)
    Single3ParamsSys,
    /// System API, multi field, 2 params (e.g., IN_KEY, GROUP_BY)
    Multi2ParamsSys,
}

// PLACEHOLDER_FOR_APPEND

/// Verify field name against allowed patterns.
///
/// Allowed formats (with optional surrounding brackets/quotes and whitespace):
/// - `colName` — bare column name
/// - `tableName.colName` — table-qualified
/// - `$.colName` — JSON path style
/// - `store.table.colName` — fully qualified
///
/// Returns `true` if the field is valid or empty.
pub fn verify_field(field: &str) -> bool {
    if field.is_empty() {
        return true;
    }
    let field = field.trim();
    // Try to extract the inner name from optional brackets/quotes
    let inner = unwrap_brackets(field);
    // Validate the inner identifier pattern
    is_valid_identifier(inner)
}

/// Strip optional surrounding brackets: `(name)`, `[name]`, `"name"`.
fn unwrap_brackets(s: &str) -> &str {
    let bytes = s.as_bytes();
    if bytes.len() >= 2 {
        let (first, last) = (bytes[0], bytes[bytes.len() - 1]);
        if (first == b'(' && last == b')')
            || (first == b'[' && last == b']')
            || (first == b'"' && last == b'"')
        {
            return &s[1..s.len() - 1];
        }
    }
    s
}

/// Check if the identifier matches one of the allowed patterns:
/// - `word` (colName)
/// - `word.word` (table.col)
/// - `$.word` (JSON path)
/// - `word.word.word` (store.table.col)
fn is_valid_identifier(s: &str) -> bool {
    if s.is_empty() {
        return false;
    }
    // $.colName pattern
    if let Some(rest) = s.strip_prefix("$.") {
        return is_word(rest);
    }
    let parts: Vec<&str> = s.split('.').collect();
    match parts.len() {
        1 => is_word(parts[0]),
        2 => is_word(parts[0]) && is_word(parts[1]),
        3 => is_word(parts[0]) && is_word(parts[1]) && is_word(parts[2]),
        _ => false,
    }
}

/// Check if a string is a valid word: `[a-zA-Z0-9_]+`
fn is_word(s: &str) -> bool {
    !s.is_empty() && s.bytes().all(|b| b.is_ascii_alphanumeric() || b == b'_')
}

/// Verify all fields in a multi-value list.
fn verify_fields(values: &[SingleValue]) -> bool {
    for val in values {
        if let SingleValue::String(s) = val {
            if !verify_field(s) {
                return false;
            }
        }
    }
    true
}

/// Classify an operation type for verification.
fn get_verify_type(op: OperationType) -> PredicatesVerifyType {
    match op {
        OperationType::OrderByAsc | OperationType::OrderByDesc => {
            PredicatesVerifyType::Single2ParamsPublic
        }
        OperationType::EqualTo => PredicatesVerifyType::Single3ParamsPublic,
        OperationType::IsNull
        | OperationType::IsNotNull
        | OperationType::IndexedBy
        | OperationType::KeyPrefix => PredicatesVerifyType::Single2ParamsSys,
        OperationType::NotEqualTo
        | OperationType::GreaterThan
        | OperationType::LessThan
        | OperationType::GreaterThanOrEqualTo
        | OperationType::LessThanOrEqualTo
        | OperationType::NotIn
        | OperationType::Like
        | OperationType::Unlike
        | OperationType::BeginWith
        | OperationType::EndWith
        | OperationType::Contains
        | OperationType::Glob
        | OperationType::Between
        | OperationType::NotBetween => PredicatesVerifyType::Single3ParamsSys,
        OperationType::InKey | OperationType::GroupBy => PredicatesVerifyType::Multi2ParamsSys,
        _ => PredicatesVerifyType::Default,
    }
}

/// Check that the operation item has the required parameters for its type.
fn check_param_num(verify_type: PredicatesVerifyType, item: &OperationItem) -> bool {
    match verify_type {
        PredicatesVerifyType::Default => true,
        PredicatesVerifyType::Multi2ParamsSys => item.multi_values.is_some(),
        _ => !item.field.is_empty() || item.value.is_some(),
    }
}

/// Verify a single operation item based on its type classification.
///
/// Returns `E_OK` (0) on success, `E_FIELD_INVALID` for public API violations,
/// `E_FIELD_ILLEGAL` for system API violations.
fn verify_by_type(verify_type: PredicatesVerifyType, item: &OperationItem) -> i32 {
    if verify_type == PredicatesVerifyType::Default || !check_param_num(verify_type, item) {
        return i32::from(DataShareError::Ok);
    }

    match verify_type {
        PredicatesVerifyType::Single2ParamsPublic | PredicatesVerifyType::Single3ParamsPublic => {
            if !verify_field(&item.field) {
                return i32::from(DataShareError::FieldInvalid);
            }
        }
        PredicatesVerifyType::Single2ParamsSys | PredicatesVerifyType::Single3ParamsSys => {
            if !verify_field(&item.field) {
                return i32::from(DataShareError::FieldIllegal);
            }
        }
        PredicatesVerifyType::Multi2ParamsSys => {
            if let Some(ref vals) = item.multi_values {
                if !verify_fields(vals) {
                    return i32::from(DataShareError::FieldIllegal);
                }
            }
        }
        PredicatesVerifyType::Default => {}
    }
    i32::from(DataShareError::Ok)
}

/// Verify all predicates in a `DataSharePredicates` instance.
///
/// Returns `(0, 0)` if all operations pass verification.
/// On failure, returns `(operation_type, error_code)`.
///
/// Equivalent to C++ `DataSharePredicatesVerify::VerifyPredicates`.
pub fn verify_predicates(predicates: &DataSharePredicates) -> (i32, i32) {
    for item in predicates.get_operations() {
        let verify_type = get_verify_type(item.operation);
        let err_code = verify_by_type(verify_type, item);
        if err_code != i32::from(DataShareError::Ok) {
            return (item.operation as i32, err_code);
        }
    }
    (0, 0)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_verify_field_valid() {
        assert!(verify_field("colName"));
        assert!(verify_field("table_1.col_2"));
        assert!(verify_field("$.jsonCol"));
        assert!(verify_field("store.table.col"));
        assert!(verify_field("  colName  "));
        assert!(verify_field("(colName)"));
        assert!(verify_field("[table.col]"));
        assert!(verify_field("\"store.table.col\""));
        assert!(verify_field("")); // empty is valid
    }

    #[test]
    fn test_verify_field_invalid() {
        assert!(!verify_field("col name")); // space in name
        assert!(!verify_field("col;DROP TABLE")); // SQL injection
        assert!(!verify_field("a.b.c.d")); // too many segments
        assert!(!verify_field("col-name")); // hyphen not allowed
    }

    #[test]
    fn test_verify_predicates_ok() {
        let pred = DataSharePredicates::new().equal_to("name", SingleValue::String("test".into()));
        assert_eq!(verify_predicates(&pred), (0, 0));
    }

    #[test]
    fn test_verify_predicates_invalid_field() {
        let pred = DataSharePredicates::new()
            .equal_to("invalid;field", SingleValue::String("test".into()));
        let (op, err) = verify_predicates(&pred);
        assert_eq!(op, OperationType::EqualTo as i32);
        assert_eq!(err, i32::from(DataShareError::FieldInvalid));
    }
}
