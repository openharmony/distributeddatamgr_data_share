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

//! DataSharePredicates IPC marshalling.
//!
//! Equivalent to C++ `datashare_predicates.cpp` (32 lines).
//! This module is a thin delegate to the binary buffer serialization
//! in `itypes_utils`, matching the C++ pattern where
//! `DataSharePredicates::Marshalling` delegates to
//! `ITypesUtil::MarshalPredicates`.

use crate::ipc::itypes_utils;
use crate::predicates::DataSharePredicates;

/// Serialize DataSharePredicates to a binary buffer.
///
/// Equivalent to C++ `DataSharePredicates::Marshalling(MessageParcel &parcel)`.
/// In the C++ implementation, this calls `ITypesUtil::MarshalPredicates(...)`,
/// which writes predicates as raw binary data into the parcel.
///
/// Returns `Some((size, data))` where `size` is the buffer length and
/// `data` is the serialized bytes, suitable for writing to a MessageParcel
/// via `WriteInt32(size)` + `WriteRawData(data, size)`.
///
/// Returns `None` if serialization fails or the result exceeds MAX_IPC_SIZE.
pub fn marshalling(predicates: &DataSharePredicates) -> Option<(i32, Vec<u8>)> {
    itypes_utils::marshal_predicates_raw(predicates)
}

/// Deserialize DataSharePredicates from a binary buffer.
///
/// Equivalent to C++ `DataSharePredicates::Unmarshalling(MessageParcel &parcel)`.
/// In the C++ implementation, this calls `ITypesUtil::UnmarshalPredicates(...)`,
/// which reads the raw binary data from the parcel.
///
/// `data` should be the raw bytes obtained from `ReadRawData(size)`.
///
/// Returns `None` if deserialization fails or the buffer exceeds MAX_IPC_SIZE.
pub fn unmarshalling(data: &[u8]) -> Option<DataSharePredicates> {
    itypes_utils::unmarshal_predicates_raw(data)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::predicates::operations::SingleValue;

    #[test]
    fn test_marshalling_unmarshalling_empty() {
        let predicates = DataSharePredicates::new();
        let (size, data) = marshalling(&predicates).unwrap();
        assert!(size > 0);

        let result = unmarshalling(&data).unwrap();
        assert!(result.get_operation_list().is_empty());
        assert!(result.get_where_clause().is_empty());
    }

    #[test]
    fn test_marshalling_unmarshalling_with_operations() {
        let predicates = DataSharePredicates::new()
            .equal_to("name", SingleValue::String("Alice".to_string()))
            .greater_than("age", SingleValue::Int(18))
            .limit(10);

        let (size, data) = marshalling(&predicates).unwrap();
        assert!(size > 0);

        let result = unmarshalling(&data).unwrap();
        assert_eq!(result.get_operation_list().len(), 3);
    }

    #[test]
    fn test_marshalling_unmarshalling_with_where() {
        let mut predicates = DataSharePredicates::new();
        predicates.set_where_clause("status = ? AND age > ?");
        predicates.set_where_args(vec!["active".to_string(), "18".to_string()]);
        predicates.set_order("name ASC");
        predicates.set_setting_mode(2);

        let (_, data) = marshalling(&predicates).unwrap();
        let result = unmarshalling(&data).unwrap();

        assert_eq!(result.get_where_clause(), "status = ? AND age > ?");
        assert_eq!(result.get_where_args(), &["active", "18"]);
        assert_eq!(result.get_order(), "name ASC");
        assert_eq!(result.get_setting_mode(), 2);
    }

    #[test]
    fn test_unmarshalling_empty_data() {
        let result = unmarshalling(&[]);
        assert!(result.is_none());
    }
}
