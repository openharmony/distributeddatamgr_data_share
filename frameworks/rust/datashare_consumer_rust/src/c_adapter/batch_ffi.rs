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

//! FFI functions for DataShareHelper batch operations.

use std::slice;

use super::types_ffi::{c_str_to_rust, DataShareHelperHandle};
use crate::helper::datashare_helper::DataShareHelper;
use datashare_common::types::UpdateOperations;
use datashare_common::values_bucket::DataShareValuesBucket;

const DATA_SHARE_ERROR: i32 = -1;

/// Batch insert rows.
///
/// # Safety
/// `values` must point to an array of `count` DataShareValuesBucket instances.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperBatchInsert(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    values: *const DataShareValuesBucket,
    count: u32,
) -> i32 {
    if handle.is_null() || values.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let values_slice = slice::from_raw_parts(values, count as usize);
    (*handle).batch_insert(uri, values_slice)
}

/// Batch update rows.
///
/// # Safety
/// `operations` must point to a valid UpdateOperations instance.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperBatchUpdate(
    handle: DataShareHelperHandle,
    operations: *const UpdateOperations,
) -> i32 {
    if handle.is_null() || operations.is_null() {
        return DATA_SHARE_ERROR;
    }
    (*handle).batch_update(&*operations)
}
