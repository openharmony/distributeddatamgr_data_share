// Copyright (c) 2023 Huawei Device Co., Ltd.
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

use std::{ffi::CStr, os::raw::c_uint};

use ani_rs::{objects::AniObject, AniEnv, AniVm};
mod datashare;
mod predicates;
mod result_set;
mod wrapper;

const DATA_SHARE_RESULT_SET: &CStr = unsafe {
    CStr::from_bytes_with_nul_unchecked(
        b"L@ohos/data/DataShareResultSet/DataShareResultSetInner;\0",
    )
};

const DATA_SHARE_PREDICATES: &CStr = unsafe {
    CStr::from_bytes_with_nul_unchecked(
        b"L@ohos/data/dataSharePredicates/dataSharePredicates/DataSharePredicates;\0",
    )
};

const DATA_SHARE_NAMESPACE: &CStr =
    unsafe { CStr::from_bytes_with_nul_unchecked(b"L@ohos/data/dataShare/dataShare;\0") };

const DATA_SHARE: &CStr = unsafe {
    CStr::from_bytes_with_nul_unchecked(b"L@ohos/data/dataShare/dataShare/DataShareHelperInner;\0")
};

pub fn get_native_ptr<'local>(env: &AniEnv<'local>, obj: &AniObject) -> i64 {
    let native_str = unsafe { CStr::from_bytes_with_nul_unchecked(b"nativePtr\0") };
    env.get_field::<i64>(obj, native_str).unwrap_or(0)
}

fn bind_result_set<'local>(env: &AniEnv<'local>) {
    let class = env.find_class(DATA_SHARE_RESULT_SET).unwrap();
    let methods = [
        (
            result_set::GO_TO_FIRST_ROW,
            result_set::go_to_first_row as _,
        ),
        (result_set::GO_TO_LAST_ROW, result_set::go_to_last_row as _),
        (result_set::GO_TO_NEXT_ROW, result_set::go_to_next_row as _),
        (result_set::GET_STRING, result_set::get_string as _),
        (result_set::GET_LONG, result_set::get_long as _),
        (
            result_set::GET_COLUMN_INDEX,
            result_set::get_column_index as _,
        ),
        (result_set::CLOSE, result_set::close as _),
    ];
    env.bind_class_methods(class, &methods).unwrap();
}

fn bind_predicates<'local>(env: &AniEnv<'local>) {
    let class = env.find_class(DATA_SHARE_PREDICATES).unwrap();
    let methods = [
        (predicates::CREATE, predicates::create as _),
        (predicates::EQUAL_TO, predicates::native_equal_to as _),
        (
            predicates::NOT_EQUAL_TO,
            predicates::native_not_equal_to as _,
        ),
        (predicates::BEGIN_WRAP, predicates::native_begin_wrap as _),
        (predicates::END_WRAP, predicates::native_end_wrap as _),
        (predicates::OR, predicates::native_or as _),
        (predicates::AND, predicates::native_and as _),
        (predicates::CONTAINS, predicates::native_contains as _),
        (predicates::IS_NULL, predicates::native_is_null as _),
        (predicates::IS_NOT_NULL, predicates::native_is_not_null as _),
        (predicates::LIKE, predicates::native_like as _),
        (predicates::BETWEEN, predicates::native_between as _),
        (
            predicates::GREATER_THAN,
            predicates::native_greater_than as _,
        ),
        (
            predicates::GREATER_THAN_OR_EQUAL_TO,
            predicates::native_greater_than_or_equal_to as _,
        ),
        (
            predicates::LESS_THAN_OR_EQUAL_TO,
            predicates::native_less_than_or_equal_to as _,
        ),
        (predicates::LESS_THAN, predicates::native_less_than as _),
        (
            predicates::ORDER_BY_ASC,
            predicates::native_order_by_asc as _,
        ),
        (
            predicates::ORDER_BY_DESC,
            predicates::native_order_by_desc as _,
        ),
        (predicates::LIMIT, predicates::native_limit as _),
        (predicates::GROUP_BY, predicates::native_group_by as _),
        (predicates::IN, predicates::native_in as _),
        (predicates::NOT_IN, predicates::native_not_in as _),
    ];

    env.bind_class_methods(class, &methods).unwrap();
}

fn bind_datashare_namespace<'local>(env: &AniEnv<'local>) {
    let namespace = env.find_namespace(DATA_SHARE_NAMESPACE).unwrap();

    let methods = [
        (datashare::NATIVE_CREATE, datashare::native_create as _),
        (datashare::NATIVE_QUERY, datashare::native_query as _),
        (datashare::NATIVE_UPDATE, datashare::native_update as _),
        (datashare::NATIVE_PUBLISH, datashare::native_publish as _),
        (
            datashare::NATIVE_GET_PUBLISHED_DATA,
            datashare::native_get_published_data as _,
        ),
        (datashare::NATIVE_INSERT, datashare::native_insert as _),
        (
            datashare::NATIVE_BATCH_INSERT,
            datashare::native_batch_insert as _,
        ),
        (datashare::NATIVE_DELETE, datashare::native_delete as _),
        (datashare::NATIVE_CLOSE, datashare::native_close as _),
        (datashare::NATIVE_ON, datashare::native_on as _),
        (
            datashare::NATIVE_ON_CHANGEINFO,
            datashare::native_on_changeinfo as _,
        ),
        (
            datashare::NATIVE_ON_RGB_DATA_CHANGE,
            datashare::native_on_rdb_data_change as _,
        ),
        (
            datashare::NATIVE_ON_PUBLISHED_DATA_CHANGE,
            datashare::native_on_published_data_change as _,
        ),
        (datashare::NATIVE_OFF, datashare::native_off as _),
        (
            datashare::NATIVE_OFF_CHANGEINFO,
            datashare::native_off_changeinfo as _,
        ),
        (
            datashare::NATIVE_OFF_RGB_DATA_CHANGE,
            datashare::native_off_rdb_data_change as _,
        ),
        (
            datashare::NATIVE_OFF_PUBLISHED_DATA_CHANGE,
            datashare::native_off_published_data_change as _,
        ),
    ];

    env.bind_namespace_functions(namespace, &methods).unwrap();
}

fn bind_datashare_class<'local>(env: &AniEnv<'local>) {
    let class = env.find_class(DATA_SHARE).unwrap();
    let methods = [
        (
            datashare::NATIVE_ADD_TEMPLATE,
            datashare::native_add_template as _,
        ),
        (
            datashare::NATIVE_DEL_TEMPLATE,
            datashare::native_del_template as _,
        ),
    ];

    env.bind_class_methods(class, &methods).unwrap();
}

#[no_mangle]
pub extern "C" fn ANI_Constructor(vm: AniVm, result: *mut u32) -> c_uint {
    unsafe {
        let env = vm.get_env().unwrap();
        AniVm::init(vm);
        bind_result_set(&env);
        bind_predicates(&env);
        bind_datashare_namespace(&env);
        bind_datashare_class(&env);
        *result = 1;
    };
    0
}
