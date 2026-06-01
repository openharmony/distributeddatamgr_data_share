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
use crate::values_bucket::DataShareValuesBucket;

type OpaqueValuesBucket = DataShareValuesBucket;

/// Create a new DataShare values bucket
#[no_mangle]
pub extern "C" fn DataShareValuesBucketNew() -> *mut OpaqueValuesBucket {
    Box::into_raw(Box::new(DataShareValuesBucket::new()))
}

/// Put an integer value into the bucket
#[no_mangle]
pub extern "C" fn DataShareValuesBucketPutInt(
    bucket: *mut OpaqueValuesBucket,
    column: *const u8,
    column_len: u32,
    value: i64,
) -> i32 {
    if bucket.is_null() || column.is_null() {
        return -1;
    }
    unsafe {
        let col =
            std::str::from_utf8_unchecked(std::slice::from_raw_parts(column, column_len as usize));
        (*bucket).put_int(col.to_string(), value);
        0
    }
}

/// Put a double value into the bucket
#[no_mangle]
pub extern "C" fn DataShareValuesBucketPutDouble(
    bucket: *mut OpaqueValuesBucket,
    column: *const u8,
    column_len: u32,
    value: f64,
) -> i32 {
    if bucket.is_null() || column.is_null() {
        return -1;
    }
    unsafe {
        let col =
            std::str::from_utf8_unchecked(std::slice::from_raw_parts(column, column_len as usize));
        (*bucket).put_double(col.to_string(), value);
        0
    }
}

/// Put a string value into the bucket
#[no_mangle]
pub extern "C" fn DataShareValuesBucketPutString(
    bucket: *mut OpaqueValuesBucket,
    column: *const u8,
    column_len: u32,
    value: *const u8,
    value_len: u32,
) -> i32 {
    if bucket.is_null() || column.is_null() || value.is_null() {
        return -1;
    }
    unsafe {
        let col =
            std::str::from_utf8_unchecked(std::slice::from_raw_parts(column, column_len as usize));
        let val =
            std::str::from_utf8_unchecked(std::slice::from_raw_parts(value, value_len as usize));
        (*bucket).put_string(col.to_string(), val.to_string());
        0
    }
}

/// Put a boolean value into the bucket
#[no_mangle]
pub extern "C" fn DataShareValuesBucketPutBool(
    bucket: *mut OpaqueValuesBucket,
    column: *const u8,
    column_len: u32,
    value: bool,
) -> i32 {
    if bucket.is_null() || column.is_null() {
        return -1;
    }
    unsafe {
        let col =
            std::str::from_utf8_unchecked(std::slice::from_raw_parts(column, column_len as usize));
        (*bucket).put_bool(col.to_string(), value);
        0
    }
}

/// Put a blob value into the bucket
#[no_mangle]
pub extern "C" fn DataShareValuesBucketPutBlob(
    bucket: *mut OpaqueValuesBucket,
    column: *const u8,
    column_len: u32,
    data: *const u8,
    data_len: u32,
) -> i32 {
    if bucket.is_null() || column.is_null() || data.is_null() {
        return -1;
    }
    unsafe {
        let col =
            std::str::from_utf8_unchecked(std::slice::from_raw_parts(column, column_len as usize));
        let blob = std::slice::from_raw_parts(data, data_len as usize).to_vec();
        (*bucket).put_blob(col.to_string(), blob);
        0
    }
}

/// Clear all values from the bucket
#[no_mangle]
pub extern "C" fn DataShareValuesBucketClear(bucket: *mut OpaqueValuesBucket) -> i32 {
    if bucket.is_null() {
        return -1;
    }
    unsafe {
        (*bucket).clear();
        0
    }
}

/// Check if the bucket is empty
#[no_mangle]
pub extern "C" fn DataShareValuesBucketIsEmpty(bucket: *const OpaqueValuesBucket) -> bool {
    if bucket.is_null() {
        return true;
    }
    unsafe { (*bucket).is_empty() }
}

/// Get the number of entries in the bucket
#[no_mangle]
pub extern "C" fn DataShareValuesBucketSize(bucket: *const OpaqueValuesBucket) -> u32 {
    if bucket.is_null() {
        return 0;
    }
    unsafe { (*bucket).size() as u32 }
}

/// Free a DataShare values bucket
#[no_mangle]
pub extern "C" fn DataShareValuesBucketFree(bucket: *mut OpaqueValuesBucket) {
    if !bucket.is_null() {
        unsafe {
            let _ = Box::from_raw(bucket);
        }
    }
}
