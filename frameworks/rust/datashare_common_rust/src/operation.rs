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

use crate::predicates::DataSharePredicates;
use crate::value_object::DataShareValue;
use crate::values_bucket::DataShareValuesBucket;

/// Database operation type
#[repr(i32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Operation {
    /// Insert operation
    Insert = 0,
    /// Update operation
    Update = 1,
    /// Delete operation
    Delete = 2,
}

impl Operation {
    pub fn from_i32(val: i32) -> Option<Self> {
        match val {
            0 => Some(Operation::Insert),
            1 => Some(Operation::Update),
            2 => Some(Operation::Delete),
            _ => None,
        }
    }
}

/// Back-reference for batch operations (maps to C++ BackReference).
#[derive(Debug, Clone, PartialEq)]
pub struct BackReference {
    pub column: String,
    pub from_index: i32,
}

impl Default for BackReference {
    fn default() -> Self {
        Self {
            column: String::new(),
            from_index: -1,
        }
    }
}

/// A single operation statement in a batch operation
#[derive(Debug, Clone, PartialEq)]
pub struct OperationStatement {
    /// Type of operation (insert/update/delete)
    pub operation_type: Operation,
    /// Target URI
    pub uri: String,
    /// Predicates for filtering (used in update/delete)
    pub predicates: DataSharePredicates,
    /// Values to insert or update
    pub values_bucket: DataShareValuesBucket,
    /// Back-reference to a previous operation's result
    pub back_reference: BackReference,
}

impl OperationStatement {
    /// Create a new operation statement
    pub fn new(operation_type: Operation, uri: String) -> Self {
        Self {
            operation_type,
            uri,
            predicates: DataSharePredicates::new(),
            values_bucket: DataShareValuesBucket::new(),
            back_reference: BackReference::default(),
        }
    }

    /// Create an insert operation statement
    pub fn insert<S: Into<String>>(uri: S) -> Self {
        Self::new(Operation::Insert, uri.into())
    }

    /// Create an update operation statement
    pub fn update<S: Into<String>>(uri: S) -> Self {
        Self::new(Operation::Update, uri.into())
    }

    /// Create a delete operation statement
    pub fn delete<S: Into<String>>(uri: S) -> Self {
        Self::new(Operation::Delete, uri.into())
    }

    /// Set predicates for the operation
    pub fn set_predicates(&mut self, predicates: DataSharePredicates) {
        self.predicates = predicates;
    }

    /// Set values for the operation
    pub fn set_values_bucket(&mut self, values_bucket: DataShareValuesBucket) {
        self.values_bucket = values_bucket;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_operation_from_i32() {
        assert_eq!(Operation::from_i32(0), Some(Operation::Insert));
        assert_eq!(Operation::from_i32(1), Some(Operation::Update));
        assert_eq!(Operation::from_i32(2), Some(Operation::Delete));
        assert_eq!(Operation::from_i32(999), None);
    }

    #[test]
    fn test_operation_statement() {
        let stmt = OperationStatement::insert("content://authority/table");
        assert_eq!(stmt.operation_type, Operation::Insert);
        assert_eq!(stmt.uri, "content://authority/table");
        assert!(stmt.values_bucket.is_empty());
    }
}
