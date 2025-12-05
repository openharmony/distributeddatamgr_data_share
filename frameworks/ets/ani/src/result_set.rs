// Copyright (c) 2025 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

use ani_rs::{
    objects::{AniObject, AniRef},
    AniEnv,
    business_error::BusinessError,
};

use crate::{get_native_ptr, wrapper, datashare::DataShareResultSet};

#[ani_rs::native]
pub fn get_row_count(this: DataShareResultSet) -> Result<i32, BusinessError> {
    let result_set_ptr = this.native_ptr;
    let res = wrapper::ffi::GetRowCount(result_set_ptr);
    Ok(res)
}

#[ani_rs::native]
pub fn go_to_first_row(this: DataShareResultSet) -> Result<bool, BusinessError> {
    let result_set_ptr = this.native_ptr;
    let res = wrapper::ffi::GoToFirstRow(result_set_ptr);
    Ok(res)
}

#[ani_rs::native]
pub fn go_to_last_row(this: DataShareResultSet) -> Result<bool, BusinessError> {
    let result_set_ptr = this.native_ptr;
    let res = wrapper::ffi::GoToLastRow(result_set_ptr);
    Ok(res)
}

#[ani_rs::native]
pub fn go_to_next_row(this: DataShareResultSet) -> Result<bool, BusinessError> {
    let result_set_ptr = this.native_ptr;
    let res = wrapper::ffi::GoToNextRow(result_set_ptr);
    Ok(res)
}

#[ani_rs::native]
pub fn get_string(
    this: DataShareResultSet,
    column_index: i32,
) -> Result<String, BusinessError> {
    let result_set_ptr = this.native_ptr;
    let res = wrapper::ffi::GetString(result_set_ptr, column_index);
    Ok(res)
}

#[ani_rs::native]
pub fn get_long(this: DataShareResultSet, column_index: i32) -> Result<i64, BusinessError> {
    let result_set_ptr = this.native_ptr;
    let res = wrapper::ffi::GetLong(result_set_ptr, column_index);
    Ok(res)
}

#[ani_rs::native]
pub fn get_column_index(this: DataShareResultSet, column_name: String) -> Result<i32, BusinessError> {
    let result_set_ptr = this.native_ptr;
    let res = wrapper::ffi::GetColumnIndex(result_set_ptr, column_name);
    Ok(res)
}

#[ani_rs::native]
pub fn close(this: DataShareResultSet) -> Result<(), BusinessError>{
    let result_set_ptr = this.native_ptr;
    wrapper::ffi::Close(result_set_ptr);
    Ok(())
}
