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
    AniEnv, typed_array::Uint8Array,
    business_error::BusinessError,
};

use crate::{get_native_ptr, wrapper, datashare::DataShareResultSet, datashare_error};

#[ani_rs::ani(path = "L@ohos/data/DataShareResultSet/DataType")]
#[derive(Debug)]
pub enum AniDataType {
    TypeNull = 0,
    TypeLong = 1,
    TypeDouble = 2,
    TypeString = 3,
    TypeBlob = 4,
}

impl AniDataType {
    pub fn from_i32(index: i32) -> Self {
        match index {
            0 => Self::TypeNull,
            1 => Self::TypeLong,
            2 => Self::TypeDouble,
            3 => Self::TypeString,
            4 => Self::TypeBlob,
            _ => Self::TypeNull,
        }
    }
}

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
pub fn go_to_previous_row<'local>(this: DataShareResultSet) -> Result<bool, BusinessError> {
    let result_set_ptr = this.native_ptr;
    let res = wrapper::ffi::GoToPreviousRow(result_set_ptr);
    Ok(res)
}

#[ani_rs::native]
pub fn go_to<'local>(
    this: DataShareResultSet,
    offset: i32
) -> Result<bool, BusinessError> {
    let result_set_ptr = this.native_ptr;
    let res = wrapper::ffi::GoTo(result_set_ptr, offset);
    Ok(res)
}

#[ani_rs::native]
pub fn go_to_row<'local>(
    this: DataShareResultSet,
    position: i32
) -> Result<bool, BusinessError> {
    let result_set_ptr = this.native_ptr;
    let res = wrapper::ffi::GoToRow(result_set_ptr, position);
    Ok(res)
}

#[ani_rs::native]
pub fn get_blob<'local>(
    this: DataShareResultSet,
    column_index: i32,
) -> Result<Uint8Array, BusinessError> {
    let result_set_ptr = this.native_ptr;
    let mut vec = Vec::new();
    if (result_set_ptr == 0) {
        datashare_error!("GetInnerResultSet failed!");
    } else {
        vec = wrapper::ffi::GetBlob(result_set_ptr, column_index);
    }
    Ok(Uint8Array::new_with_vec(vec))
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
pub fn get_double<'local>(
    this: DataShareResultSet,
    column_index: i32
) -> Result<f64, BusinessError> {
    let result_set_ptr = this.native_ptr;
    let res = wrapper::ffi::GetDouble(result_set_ptr, column_index);
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

#[ani_rs::native]
pub fn get_column_name<'local>(
    this: DataShareResultSet,
    column_index: i32,
) -> Result<String, BusinessError> {
    let result_set_ptr = this.native_ptr;
    let res = wrapper::ffi::GetColumnName(result_set_ptr, column_index);
    Ok(res)
}

#[ani_rs::native]
pub fn get_data_type<'local>(
    this: DataShareResultSet,
    column_index: i32,
) -> Result<AniDataType, BusinessError> {
    let result_set_ptr = this.native_ptr;
    if (result_set_ptr == 0) {
        datashare_error!("Inner ResultSet is nullptr!");
        return Ok(AniDataType::TypeNull);
    }
    let dt = wrapper::ffi::GetDataType(result_set_ptr, column_index);
    let res = AniDataType::from_i32(dt);
    Ok(res)
}
