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

use super::operations::{OperationItem, OperationType, SingleValue};

/// Invalid setting mode constant
pub const INVALID_MODE: i16 = -1;

/// DataShare Predicates for query filtering
#[derive(Debug, Clone, PartialEq)]
pub struct DataSharePredicates {
    operations: Vec<OperationItem>,
    where_clause: String,
    where_args: Vec<String>,
    order: String,
    setting_mode: i16,
}

impl DataSharePredicates {
    /// Create a new empty predicates object
    pub fn new() -> Self {
        Self {
            operations: Vec::new(),
            where_clause: String::new(),
            where_args: Vec::new(),
            order: String::new(),
            setting_mode: INVALID_MODE,
        }
    }

    /// Create from an existing operations list
    pub fn from_operations(operations: Vec<OperationItem>) -> Self {
        Self {
            operations,
            where_clause: String::new(),
            where_args: Vec::new(),
            order: String::new(),
            setting_mode: INVALID_MODE,
        }
    }

    /// Equal to operation
    pub fn equal_to<S: Into<String>>(mut self, field: S, value: SingleValue) -> Self {
        self.operations.push(OperationItem {
            operation: OperationType::EqualTo,
            field: field.into(),
            value: Some(value),
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        self
    }

    /// Not equal to operation
    pub fn not_equal_to<S: Into<String>>(mut self, field: S, value: SingleValue) -> Self {
        self.operations.push(OperationItem {
            operation: OperationType::NotEqualTo,
            field: field.into(),
            value: Some(value),
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        self
    }

    /// Greater than operation
    pub fn greater_than<S: Into<String>>(mut self, field: S, value: SingleValue) -> Self {
        self.operations.push(OperationItem {
            operation: OperationType::GreaterThan,
            field: field.into(),
            value: Some(value),
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        self
    }

    /// Less than operation
    pub fn less_than<S: Into<String>>(mut self, field: S, value: SingleValue) -> Self {
        self.operations.push(OperationItem {
            operation: OperationType::LessThan,
            field: field.into(),
            value: Some(value),
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        self
    }

    /// In operation
    pub fn in_set<S: Into<String>>(mut self, field: S, values: Vec<SingleValue>) -> Self {
        self.operations.push(OperationItem {
            operation: OperationType::In,
            field: field.into(),
            value: None,
            multi_values: Some(values),
            extra_single_params: Vec::new(),
        });
        self
    }

    /// Order by operation (ascending)
    pub fn order_by_asc<S: Into<String>>(mut self, field: S) -> Self {
        self.operations.push(OperationItem {
            operation: OperationType::OrderByAsc,
            field: field.into(),
            value: None,
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        self
    }

    /// Order by operation (descending)
    pub fn order_by_desc<S: Into<String>>(mut self, field: S) -> Self {
        self.operations.push(OperationItem {
            operation: OperationType::OrderByDesc,
            field: field.into(),
            value: None,
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        self
    }

    /// Limit operation
    pub fn limit(mut self, count: i64) -> Self {
        self.operations.push(OperationItem {
            operation: OperationType::Limit,
            field: String::new(),
            value: Some(SingleValue::Long(count)),
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        self
    }

    /// Offset operation
    pub fn offset(mut self, offset: i64) -> Self {
        self.operations.push(OperationItem {
            operation: OperationType::Offset,
            field: String::new(),
            value: Some(SingleValue::Long(offset)),
            multi_values: None,
            extra_single_params: Vec::new(),
        });
        self
    }

    // --- IPC-related accessors ---

    pub fn get_operation_list(&self) -> &[OperationItem] {
        &self.operations
    }

    pub fn set_operation_list(&mut self, operations: Vec<OperationItem>) {
        self.operations = operations;
    }

    pub fn get_operations(&self) -> &[OperationItem] {
        &self.operations
    }

    pub fn get_operations_mut(&mut self) -> &mut Vec<OperationItem> {
        &mut self.operations
    }

    pub fn get_where_clause(&self) -> &str {
        &self.where_clause
    }

    pub fn set_where_clause(&mut self, clause: &str) {
        self.where_clause = clause.to_string();
    }

    pub fn get_where_args(&self) -> &[String] {
        &self.where_args
    }

    pub fn set_where_args(&mut self, args: Vec<String>) {
        self.where_args = args;
    }

    pub fn get_order(&self) -> &str {
        &self.order
    }

    pub fn set_order(&mut self, order: &str) {
        self.order = order.to_string();
    }

    pub fn get_setting_mode(&self) -> i16 {
        self.setting_mode
    }

    pub fn set_setting_mode(&mut self, mode: i16) {
        self.setting_mode = mode;
    }

    /// Clear all operations
    pub fn clear(&mut self) {
        self.operations.clear();
    }

    /// Check if predicates is empty
    pub fn is_empty(&self) -> bool {
        self.operations.is_empty()
    }

    /// Get number of operations
    pub fn count(&self) -> usize {
        self.operations.len()
    }

    /// Get limit value from operations (for IPC serialization)
    pub fn get_limit(&self) -> i64 {
        for op in &self.operations {
            if op.operation == OperationType::Limit {
                if let Some(SingleValue::Long(v)) = op.value {
                    return v;
                }
            }
        }
        0
    }

    /// Get offset value from operations (for IPC serialization)
    pub fn get_offset(&self) -> i64 {
        for op in &self.operations {
            if op.operation == OperationType::Offset {
                if let Some(SingleValue::Long(v)) = op.value {
                    return v;
                }
            }
        }
        0
    }

    /// Get distinct flag (for IPC serialization)
    pub fn is_distinct(&self) -> bool {
        for op in &self.operations {
            if op.operation == OperationType::Distinct {
                return true;
            }
        }
        false
    }

    /// Get order by string (for IPC serialization)
    pub fn get_order_by(&self) -> &str {
        &self.order
    }
}

impl Default for DataSharePredicates {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_predicates_chain() {
        let pred = DataSharePredicates::new()
            .equal_to("name", SingleValue::String("Alice".to_string()))
            .greater_than("age", SingleValue::Int(18))
            .order_by_asc("id");

        assert_eq!(pred.count(), 3);
    }

    #[test]
    fn test_predicates_limit_offset() {
        let pred = DataSharePredicates::new().limit(10).offset(5);

        assert_eq!(pred.count(), 2);
    }

    #[test]
    fn test_predicates_in() {
        let pred = DataSharePredicates::new().in_set(
            "status",
            vec![
                SingleValue::Int(1),
                SingleValue::Int(2),
                SingleValue::Int(3),
            ],
        );

        assert_eq!(pred.count(), 1);
    }
}
