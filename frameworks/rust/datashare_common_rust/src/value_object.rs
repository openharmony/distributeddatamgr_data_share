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

use crate::error::DataShareError;

/// DataShare value types, corresponding to C++ DataShareValueObjectType
#[repr(i32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum DataShareValueType {
    /// Null type
    Null = 0,
    /// Int (i64) type
    Int = 1,
    /// Double (f64) type
    Double = 2,
    /// String type
    String = 3,
    /// Bool type
    Bool = 4,
    /// Blob (Vec<u8>) type
    Blob = 5,
}

impl DataShareValueType {
    pub fn from_i32(val: i32) -> Result<Self, DataShareError> {
        match val {
            0 => Ok(DataShareValueType::Null),
            1 => Ok(DataShareValueType::Int),
            2 => Ok(DataShareValueType::Double),
            3 => Ok(DataShareValueType::String),
            4 => Ok(DataShareValueType::Bool),
            5 => Ok(DataShareValueType::Blob),
            _ => Err(DataShareError::InvalidObjectType),
        }
    }
}

/// DataShare value object, can hold different data types
#[derive(Debug, Clone, PartialEq)]
pub enum DataShareValue {
    /// Null value
    Null,
    /// Integer value (i64)
    Int(i64),
    /// Double value (f64)
    Double(f64),
    /// String value
    String(String),
    /// Boolean value
    Bool(bool),
    /// Binary blob (Vec<u8>)
    Blob(Vec<u8>),
}

impl DataShareValue {
    /// Get the type of this value
    pub fn get_type(&self) -> DataShareValueType {
        match self {
            DataShareValue::Null => DataShareValueType::Null,
            DataShareValue::Int(_) => DataShareValueType::Int,
            DataShareValue::Double(_) => DataShareValueType::Double,
            DataShareValue::String(_) => DataShareValueType::String,
            DataShareValue::Bool(_) => DataShareValueType::Bool,
            DataShareValue::Blob(_) => DataShareValueType::Blob,
        }
    }

    /// Get as i64, returns 0 if not an int
    pub fn as_i64(&self) -> i64 {
        match self {
            DataShareValue::Int(v) => *v,
            _ => 0,
        }
    }

    /// Get as f64, returns 0.0 if not a double
    pub fn as_f64(&self) -> f64 {
        match self {
            DataShareValue::Double(v) => *v,
            _ => 0.0,
        }
    }

    /// Get as string, returns empty string if not a string
    pub fn as_str(&self) -> &str {
        match self {
            DataShareValue::String(v) => v.as_str(),
            _ => "",
        }
    }

    /// Get as bool, returns false if not a bool
    pub fn as_bool(&self) -> bool {
        match self {
            DataShareValue::Bool(v) => *v,
            _ => false,
        }
    }

    /// Get as bytes, returns empty slice if not a blob
    pub fn as_bytes(&self) -> &[u8] {
        match self {
            DataShareValue::Blob(v) => v.as_slice(),
            _ => &[],
        }
    }
}

impl From<i64> for DataShareValue {
    fn from(val: i64) -> Self {
        DataShareValue::Int(val)
    }
}

impl From<f64> for DataShareValue {
    fn from(val: f64) -> Self {
        DataShareValue::Double(val)
    }
}

impl From<String> for DataShareValue {
    fn from(val: String) -> Self {
        DataShareValue::String(val)
    }
}

impl From<&str> for DataShareValue {
    fn from(val: &str) -> Self {
        DataShareValue::String(val.to_string())
    }
}

impl From<bool> for DataShareValue {
    fn from(val: bool) -> Self {
        DataShareValue::Bool(val)
    }
}

impl From<Vec<u8>> for DataShareValue {
    fn from(val: Vec<u8>) -> Self {
        DataShareValue::Blob(val)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_value_type() {
        assert_eq!(DataShareValue::Null.get_type(), DataShareValueType::Null);
        assert_eq!(DataShareValue::Int(42).get_type(), DataShareValueType::Int);
        assert_eq!(
            DataShareValue::Double(3.14).get_type(),
            DataShareValueType::Double
        );
        assert_eq!(
            DataShareValue::String("test".to_string()).get_type(),
            DataShareValueType::String
        );
        assert_eq!(
            DataShareValue::Bool(true).get_type(),
            DataShareValueType::Bool
        );
        assert_eq!(
            DataShareValue::Blob(vec![1, 2, 3]).get_type(),
            DataShareValueType::Blob
        );
    }

    #[test]
    fn test_value_accessors() {
        assert_eq!(DataShareValue::Int(42).as_i64(), 42);
        assert_eq!(DataShareValue::Double(3.14).as_f64(), 3.14);
        assert_eq!(
            DataShareValue::String("hello".to_string()).as_str(),
            "hello"
        );
        assert_eq!(DataShareValue::Bool(true).as_bool(), true);
        assert_eq!(DataShareValue::Blob(vec![1, 2, 3]).as_bytes(), &[1, 2, 3]);
    }

    #[test]
    fn test_value_conversions() {
        assert_eq!(DataShareValue::from(42i64), DataShareValue::Int(42));
        assert_eq!(DataShareValue::from(3.14f64), DataShareValue::Double(3.14));
        assert_eq!(
            DataShareValue::from("test"),
            DataShareValue::String("test".to_string())
        );
        assert_eq!(DataShareValue::from(true), DataShareValue::Bool(true));
    }
}
