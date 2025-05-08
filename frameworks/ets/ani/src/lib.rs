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

use std::ffi::CStr;

use ani_rs::{ani_constructor, objects::AniObject, AniEnv, AniVm};
mod datashare;
mod predicates;
mod result_set;
mod wrapper;
mod datashare_extension;

const DATA_SHARE_PREDICATES: &CStr = unsafe {
    CStr::from_bytes_with_nul_unchecked(
        b"L@ohos/data/dataSharePredicates/dataSharePredicates/DataSharePredicates;\0",
    )
};

const DATA_SHARE: &CStr = unsafe {
    CStr::from_bytes_with_nul_unchecked(b"L@ohos/data/dataShare/dataShare/DataShareHelperInner;\0")
};

const DATA_SHARE_EXTENSION: &CStr = unsafe {
    CStr::from_bytes_with_nul_unchecked(b"L@ohos/application/DataShareExtensionAbility/DataShareExtensionAbility;\0")
};

pub fn get_native_ptr<'local>(env: &AniEnv<'local>, obj: &AniObject) -> i64 {
    let native_str = unsafe { CStr::from_bytes_with_nul_unchecked(b"nativePtr\0") };
    env.get_field::<i64>(obj, native_str).unwrap_or(0)
}

ani_constructor!(
    class "L@ohos/data/DataShareResultSet/DataShareResultSetInner"
    [
        "goToFirstRow" : result_set::go_to_first_row,
        "goToLastRow" : result_set::go_to_last_row,
        "goToNextRow" : result_set::go_to_next_row,
        "getString" : result_set::get_string,
        "getLong" : result_set::get_long,
        "getColumnIndex" : result_set::get_column_index,
        "close" : result_set::close,
    ]

    class "L@ohos/data/dataSharePredicates/dataSharePredicates/DataSharePredicates"
    [
        "create" : predicates::create,
        "equalTo" : predicates::native_equal_to,
        "notEqualTo" : predicates::native_not_equal_to,
        "beginWrap" : predicates::native_begin_wrap,
        "endWrap" : predicates::native_end_wrap,
        "or" : predicates::native_or,
        "and" : predicates::native_and,
        "contains" : predicates::native_contains,
        "isNull" : predicates::native_is_null,
        "isNotNull" : predicates::native_is_not_null,
        "like" : predicates::native_like,
        "between" : predicates::native_between,
        "greaterThan" : predicates::native_greater_than,
        "greaterThanOrEqualTo" : predicates::native_greater_than_or_equal_to,
        "lessThanOrEqualTo" : predicates::native_less_than_or_equal_to,
        "lessThan" : predicates::native_less_than,
        "orderByAsc" : predicates::native_order_by_asc,
        "orderByDesc" : predicates::native_order_by_desc,
        "limit" : predicates::native_limit,
        "groupBy" : predicates::native_group_by,
        "in" : predicates::native_in,
        "notIn" : predicates::native_not_in,
    ]
    namespace "L@ohos/data/dataShare/dataShare"
    [
        "native_create": datashare::native_create,
        "native_query": datashare::native_query,
        "native_update": datashare::native_update,
        "native_publish": datashare::native_publish,
        "native_get_published_data": datashare::native_get_published_data,
        "native_insert": datashare::native_insert,
        "native_batch_insert": datashare::native_batch_insert,
        "native_delete": datashare::native_delete,
        "native_close": datashare::native_close,
        "native_on": datashare::native_on,
        "native_on_changeinfo": datashare::native_on_changeinfo,
        "native_on_rdb_data_change": datashare::native_on_rdb_data_change,
        "native_on_published_data_change": datashare::native_on_published_data_change,
        "native_off": datashare::native_off,
        "native_off_changeinfo": datashare::native_off_changeinfo,
        "native_off_rdb_data_change": datashare::native_off_rdb_data_change,
        "native_off_published_data_change": datashare::native_off_published_data_change,

    ]
    class "L@ohos/data/dataShare/dataShare/DataShareHelperInner"
    [
        "addTemplate" : datashare::native_add_template,
        "delTemplate" : datashare::native_del_template,
    ]
    class "L@ohos/data/dataSharePredicates/dataSharePredicates/Cleaner"
    [
        "native_clean" : predicates::native_clean,
    ]
    class "L@ohos/data/dataShare/dataShare/Cleaner"
    [
        "native_clean" : datashare::native_clean,
    ]
);
