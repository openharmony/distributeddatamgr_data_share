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

use crate::value_object::DataShareValue;
use std::collections::BTreeMap;

/// DataShare values bucket - container for key-value pairs
#[derive(Debug, Clone, PartialEq)]
pub struct DataShareValuesBucket {
    values: BTreeMap<String, DataShareValue>,
}

impl DataShareValuesBucket {
    /// Create a new empty values bucket
    pub fn new() -> Self {
        Self {
            values: BTreeMap::new(),
        }
    }

    /// Put a value into the bucket
    pub fn put<K: Into<String>>(&mut self, column: K, value: DataShareValue) {
        self.values.insert(column.into(), value);
    }

    /// Put an i64 value
    pub fn put_int<K: Into<String>>(&mut self, column: K, value: i64) {
        self.put(column, DataShareValue::Int(value));
    }

    /// Put a f64 value
    pub fn put_double<K: Into<String>>(&mut self, column: K, value: f64) {
        self.put(column, DataShareValue::Double(value));
    }

    /// Put a string value
    pub fn put_string<K: Into<String>, V: Into<String>>(&mut self, column: K, value: V) {
        self.put(column, DataShareValue::String(value.into()));
    }

    /// Put a bool value
    pub fn put_bool<K: Into<String>>(&mut self, column: K, value: bool) {
        self.put(column, DataShareValue::Bool(value));
    }

    /// Put a blob value
    pub fn put_blob<K: Into<String>>(&mut self, column: K, value: Vec<u8>) {
        self.put(column, DataShareValue::Blob(value));
    }

    /// Get a value from the bucket
    pub fn get(&self, column: &str) -> Option<&DataShareValue> {
        self.values.get(column)
    }

    /// Clear all values from the bucket
    pub fn clear(&mut self) {
        self.values.clear();
    }

    /// Check if the bucket is empty
    pub fn is_empty(&self) -> bool {
        self.values.is_empty()
    }

    /// Get the number of entries
    pub fn size(&self) -> usize {
        self.values.len()
    }

    /// Get all key-value pairs as a reference to the underlying map
    pub fn get_all(&self) -> &BTreeMap<String, DataShareValue> {
        &self.values
    }

    /// Get all keys
    pub fn keys(&self) -> Vec<String> {
        self.values.keys().cloned().collect()
    }

    /// Get all key-value pairs
    pub fn entries(&self) -> Vec<(String, DataShareValue)> {
        self.values
            .iter()
            .map(|(k, v)| (k.clone(), v.clone()))
            .collect()
    }
}

impl Default for DataShareValuesBucket {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_values_bucket_basic() {
        let mut bucket = DataShareValuesBucket::new();
        assert!(bucket.is_empty());
        assert_eq!(bucket.size(), 0);

        bucket.put_int("age", 25);
        assert!(!bucket.is_empty());
        assert_eq!(bucket.size(), 1);

        assert_eq!(bucket.get("age"), Some(&DataShareValue::Int(25)));
    }

    #[test]
    fn test_values_bucket_multiple_types() {
        let mut bucket = DataShareValuesBucket::new();
        bucket.put_int("age", 25);
        bucket.put_double("salary", 3000.50);
        bucket.put_string("name", "Alice");
        bucket.put_bool("active", true);
        bucket.put_blob("data", vec![1, 2, 3]);

        assert_eq!(bucket.size(), 5);
        assert_eq!(bucket.get("age"), Some(&DataShareValue::Int(25)));
        assert_eq!(
            bucket.get("name"),
            Some(&DataShareValue::String("Alice".to_string()))
        );
    }

    #[test]
    fn test_values_bucket_clear() {
        let mut bucket = DataShareValuesBucket::new();
        bucket.put_int("age", 25);
        bucket.put_string("name", "Bob");
        assert_eq!(bucket.size(), 2);

        bucket.clear();
        assert!(bucket.is_empty());
        assert_eq!(bucket.size(), 0);
    }

    #[test]
    fn test_values_bucket_keys() {
        let mut bucket = DataShareValuesBucket::new();
        bucket.put_int("age", 25);
        bucket.put_string("name", "Charlie");

        let keys = bucket.keys();
        assert_eq!(keys.len(), 2);
        assert!(keys.contains(&"age".to_string()));
        assert!(keys.contains(&"name".to_string()));
    }
}
