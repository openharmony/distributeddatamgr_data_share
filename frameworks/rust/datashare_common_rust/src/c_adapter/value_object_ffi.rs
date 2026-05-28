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

use crate::value_object::{DataShareValue, DataShareValueType};
use std::ffi::CStr;

type OpaqueDataShareValue = DataShareValue;

/// Create a null DataShare value
#[no_mangle]
pub extern "C" fn DataShareValueCreateNull() -> *mut OpaqueDataShareValue {
    Box::into_raw(Box::new(DataShareValue::Null))
}

/// Create an integer DataShare value
#[no_mangle]
pub extern "C" fn DataShareValueCreateInt(value: i64) -> *mut OpaqueDataShareValue {
    Box::into_raw(Box::new(DataShareValue::Int(value)))
}

/// Create a double DataShare value
#[no_mangle]
pub extern "C" fn DataShareValueCreateDouble(value: f64) -> *mut OpaqueDataShareValue {
    Box::into_raw(Box::new(DataShareValue::Double(value)))
}

/// Create a string DataShare value
#[no_mangle]
pub extern "C" fn DataShareValueCreateString(
    value: *const u8,
    len: u32,
) -> *mut OpaqueDataShareValue {
    if value.is_null() {
        return Box::into_raw(Box::new(DataShareValue::Null));
    }
    unsafe {
        let s = std::str::from_utf8_unchecked(std::slice::from_raw_parts(value, len as usize));
        Box::into_raw(Box::new(DataShareValue::String(s.to_string())))
    }
}

/// Create a boolean DataShare value
#[no_mangle]
pub extern "C" fn DataShareValueCreateBool(value: bool) -> *mut OpaqueDataShareValue {
    Box::into_raw(Box::new(DataShareValue::Bool(value)))
}

/// Create a blob DataShare value
#[no_mangle]
pub extern "C" fn DataShareValueCreateBlob(
    data: *const u8,
    size: u32,
) -> *mut OpaqueDataShareValue {
    if data.is_null() {
        return Box::into_raw(Box::new(DataShareValue::Null));
    }
    unsafe {
        let blob = std::slice::from_raw_parts(data, size as usize).to_vec();
        Box::into_raw(Box::new(DataShareValue::Blob(blob)))
    }
}

/// Get the type of a DataShare value
#[no_mangle]
pub extern "C" fn DataShareValueGetType(value: *const OpaqueDataShareValue) -> i32 {
    if value.is_null() {
        return -1;
    }
    unsafe { (*value).get_type() as i32 }
}

/// Get the integer value
#[no_mangle]
pub extern "C" fn DataShareValueGetInt(value: *const OpaqueDataShareValue) -> i64 {
    if value.is_null() {
        return 0;
    }
    unsafe { (*value).as_i64() }
}

/// Get the double value
#[no_mangle]
pub extern "C" fn DataShareValueGetDouble(value: *const OpaqueDataShareValue) -> f64 {
    if value.is_null() {
        return 0.0;
    }
    unsafe { (*value).as_f64() }
}

/// Get the boolean value
#[no_mangle]
pub extern "C" fn DataShareValueGetBool(value: *const OpaqueDataShareValue) -> bool {
    if value.is_null() {
        return false;
    }
    unsafe { (*value).as_bool() }
}

/// Free a DataShare value
#[no_mangle]
pub extern "C" fn DataShareValueFree(value: *mut OpaqueDataShareValue) {
    if !value.is_null() {
        unsafe {
            let _ = Box::from_raw(value);
        }
    }
}
