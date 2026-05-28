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

/// Predicate operation types.
/// Values match C++ `OperationType` in `datashare_predicates_def.h`.
#[repr(i32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum OperationType {
    /// Invalid operation
    InvalidOperation = 0,
    /// Equal to
    EqualTo = 1,
    /// Not equal to
    NotEqualTo = 2,
    /// Greater than
    GreaterThan = 3,
    /// Less than
    LessThan = 4,
    /// Greater than or equal to
    GreaterThanOrEqualTo = 5,
    /// Less than or equal to
    LessThanOrEqualTo = 6,
    /// And
    And = 7,
    /// Or
    Or = 8,
    /// Is null
    IsNull = 9,
    /// Is not null
    IsNotNull = 10,
    /// SQL In
    In = 11,
    /// Not in
    NotIn = 12,
    /// Like
    Like = 13,
    /// Unlike
    Unlike = 14,
    /// Order by ascending
    OrderByAsc = 15,
    /// Order by descending
    OrderByDesc = 16,
    /// Limit
    Limit = 17,
    /// Offset
    Offset = 18,
    /// Begin warp (parenthesis open)
    BeginWarp = 19,
    /// End warp (parenthesis close)
    EndWarp = 20,
    /// Begin with
    BeginWith = 21,
    /// End with
    EndWith = 22,
    /// In key
    InKey = 23,
    /// Distinct
    Distinct = 24,
    /// Group by
    GroupBy = 25,
    /// Indexed by
    IndexedBy = 26,
    /// Contains
    Contains = 27,
    /// Glob
    Glob = 28,
    /// Between
    Between = 29,
    /// Not between
    NotBetween = 30,
    /// Key prefix
    KeyPrefix = 31,
    /// Cross join
    CrossJoin = 32,
    /// Inner join
    InnerJoin = 33,
    /// Left outer join
    LeftOuterJoin = 34,
    /// Using
    Using = 35,
    /// On
    On = 36,
}

impl OperationType {
    pub fn from_i32(val: i32) -> Option<Self> {
        match val {
            0 => Some(OperationType::InvalidOperation),
            1 => Some(OperationType::EqualTo),
            2 => Some(OperationType::NotEqualTo),
            3 => Some(OperationType::GreaterThan),
            4 => Some(OperationType::LessThan),
            5 => Some(OperationType::GreaterThanOrEqualTo),
            6 => Some(OperationType::LessThanOrEqualTo),
            7 => Some(OperationType::And),
            8 => Some(OperationType::Or),
            9 => Some(OperationType::IsNull),
            10 => Some(OperationType::IsNotNull),
            11 => Some(OperationType::In),
            12 => Some(OperationType::NotIn),
            13 => Some(OperationType::Like),
            14 => Some(OperationType::Unlike),
            15 => Some(OperationType::OrderByAsc),
            16 => Some(OperationType::OrderByDesc),
            17 => Some(OperationType::Limit),
            18 => Some(OperationType::Offset),
            19 => Some(OperationType::BeginWarp),
            20 => Some(OperationType::EndWarp),
            21 => Some(OperationType::BeginWith),
            22 => Some(OperationType::EndWith),
            23 => Some(OperationType::InKey),
            24 => Some(OperationType::Distinct),
            25 => Some(OperationType::GroupBy),
            26 => Some(OperationType::IndexedBy),
            27 => Some(OperationType::Contains),
            28 => Some(OperationType::Glob),
            29 => Some(OperationType::Between),
            30 => Some(OperationType::NotBetween),
            31 => Some(OperationType::KeyPrefix),
            32 => Some(OperationType::CrossJoin),
            33 => Some(OperationType::InnerJoin),
            34 => Some(OperationType::LeftOuterJoin),
            35 => Some(OperationType::Using),
            36 => Some(OperationType::On),
            _ => None,
        }
    }
}

/// Single value for predicate operations
#[derive(Debug, Clone, PartialEq)]
pub enum SingleValue {
    /// Null value
    Null,
    /// Integer value (C++ int, 4 bytes)
    Int(i32),
    /// Double value
    Double(f64),
    /// String value
    String(String),
    /// Boolean value
    Bool(bool),
    /// Long integer value (C++ int64_t, 8 bytes)
    Long(i64),
    /// Blob value
    Blob(Vec<u8>),
}

impl From<i32> for SingleValue {
    fn from(val: i32) -> Self {
        SingleValue::Int(val)
    }
}

impl From<i64> for SingleValue {
    fn from(val: i64) -> Self {
        SingleValue::Long(val)
    }
}

impl From<f64> for SingleValue {
    fn from(val: f64) -> Self {
        SingleValue::Double(val)
    }
}

impl From<String> for SingleValue {
    fn from(val: String) -> Self {
        SingleValue::String(val)
    }
}

impl From<&str> for SingleValue {
    fn from(val: &str) -> Self {
        SingleValue::String(val.to_string())
    }
}

impl From<bool> for SingleValue {
    fn from(val: bool) -> Self {
        SingleValue::Bool(val)
    }
}

impl From<Vec<u8>> for SingleValue {
    fn from(val: Vec<u8>) -> Self {
        SingleValue::Blob(val)
    }
}

/// Multiple values for predicate operations
#[derive(Debug, Clone, PartialEq)]
pub struct MultiValue {
    pub values: Vec<SingleValue>,
}

impl MultiValue {
    pub fn new(values: Vec<SingleValue>) -> Self {
        Self { values }
    }
}

/// Single predicate operation item
#[derive(Debug, Clone, PartialEq)]
pub struct OperationItem {
    pub operation: OperationType,
    pub field: String,
    pub value: Option<SingleValue>,
    pub multi_values: Option<Vec<SingleValue>>,
    /// Extra single params beyond field+value (e.g. Between/NotBetween high bound).
    pub extra_single_params: Vec<SingleValue>,
}

impl OperationItem {
    pub fn new(operation: OperationType, field: String) -> Self {
        Self {
            operation,
            field,
            value: None,
            multi_values: None,
            extra_single_params: Vec::new(),
        }
    }

    pub fn with_value(mut self, value: SingleValue) -> Self {
        self.value = Some(value);
        self
    }

    pub fn with_multi_values(mut self, values: Vec<SingleValue>) -> Self {
        self.multi_values = Some(values);
        self
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_operation_type_from_i32() {
        assert_eq!(
            OperationType::from_i32(0),
            Some(OperationType::InvalidOperation)
        );
        assert_eq!(OperationType::from_i32(1), Some(OperationType::EqualTo));
        assert_eq!(OperationType::from_i32(2), Some(OperationType::NotEqualTo));
        assert_eq!(OperationType::from_i32(15), Some(OperationType::OrderByAsc));
        assert_eq!(
            OperationType::from_i32(16),
            Some(OperationType::OrderByDesc)
        );
        assert_eq!(OperationType::from_i32(999), None);
    }

    #[test]
    fn test_single_value_conversions() {
        let int_val = SingleValue::from(42i32);
        assert_eq!(int_val, SingleValue::Int(42));

        let long_val = SingleValue::from(42i64);
        assert_eq!(long_val, SingleValue::Long(42));

        let str_val = SingleValue::from("test");
        assert_eq!(str_val, SingleValue::String("test".to_string()));

        let bool_val = SingleValue::from(true);
        assert_eq!(bool_val, SingleValue::Bool(true));
    }

    #[test]
    fn test_multi_value() {
        let vals = vec![
            SingleValue::Int(1),
            SingleValue::Int(2),
            SingleValue::Int(3),
        ];
        let multi = MultiValue::new(vals.clone());
        assert_eq!(multi.values.len(), 3);
    }
}
