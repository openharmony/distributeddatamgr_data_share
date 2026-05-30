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

/// Change type for observer notifications
#[repr(u32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum ChangeType {
    /// Insert operation
    Insert = 0,
    /// Delete operation
    Delete = 1,
    /// Update operation
    Update = 2,
    /// Other operation
    Other = 3,
    /// Invalid change type
    Invalid = 4,
}

impl ChangeType {
    pub fn from_u32(val: u32) -> Self {
        match val {
            0 => ChangeType::Insert,
            1 => ChangeType::Delete,
            2 => ChangeType::Update,
            3 => ChangeType::Other,
            _ => ChangeType::Invalid,
        }
    }
}

/// Change information for observer callbacks
#[derive(Debug, Clone, PartialEq)]
pub struct ChangeInfo {
    /// Type of change (insert/delete/update/other)
    pub change_type: ChangeType,
    /// List of affected URIs
    pub uris: Vec<String>,
    /// Raw data payload (optional)
    pub data: Vec<u8>,
    /// Value buckets for changed data
    pub value_buckets: Vec<BTreeMap<String, DataShareValue>>,
}

impl ChangeInfo {
    /// Create a new ChangeInfo
    pub fn new(change_type: ChangeType) -> Self {
        Self {
            change_type,
            uris: Vec::new(),
            data: Vec::new(),
            value_buckets: Vec::new(),
        }
    }

    /// Add a URI to the change info
    pub fn add_uri(&mut self, uri: String) {
        self.uris.push(uri);
    }

    /// Add multiple URIs
    pub fn add_uris(&mut self, uris: Vec<String>) {
        self.uris.extend(uris);
    }

    /// Set the raw data payload
    pub fn set_data(&mut self, data: Vec<u8>) {
        self.data = data;
    }

    /// Add a value bucket
    pub fn add_value_bucket(&mut self, bucket: BTreeMap<String, DataShareValue>) {
        self.value_buckets.push(bucket);
    }

    /// Add multiple value buckets
    pub fn add_value_buckets(&mut self, buckets: Vec<BTreeMap<String, DataShareValue>>) {
        self.value_buckets.extend(buckets);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_change_type() {
        assert_eq!(ChangeType::from_u32(0), ChangeType::Insert);
        assert_eq!(ChangeType::from_u32(1), ChangeType::Delete);
        assert_eq!(ChangeType::from_u32(2), ChangeType::Update);
        assert_eq!(ChangeType::from_u32(999), ChangeType::Invalid);
    }

    #[test]
    fn test_change_info_basic() {
        let mut info = ChangeInfo::new(ChangeType::Insert);
        assert_eq!(info.change_type, ChangeType::Insert);
        assert!(info.uris.is_empty());

        info.add_uri("content://authority/table/1".to_string());
        assert_eq!(info.uris.len(), 1);
    }

    #[test]
    fn test_change_info_multiple_uris() {
        let mut info = ChangeInfo::new(ChangeType::Update);
        info.add_uris(vec![
            "content://auth/table/1".to_string(),
            "content://auth/table/2".to_string(),
        ]);
        assert_eq!(info.uris.len(), 2);
    }
}
