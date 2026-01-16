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

use std::ffi::CStr;

use ani_rs::{ani_constructor, objects::AniObject, AniEnv};
mod datashare;
mod predicates;
mod result_set;
mod wrapper;
mod datashare_extension;
mod log;

const DATA_SHARE_PREDICATES: &CStr = unsafe {
    CStr::from_bytes_with_nul_unchecked(
        b"@ohos.data.dataSharePredicates.dataSharePredicates.DataSharePredicates\0",
    )
};

const DATA_SHARE: &CStr = unsafe {
    CStr::from_bytes_with_nul_unchecked(b"@ohos.data.dataShare.dataShare.DataShareHelperInner\0")
};

const DATA_SHARE_DATA_PROXY_HANDLE: &CStr = unsafe {
    CStr::from_bytes_with_nul_unchecked(b"@ohos.data.dataShare.dataShare.DataProxyHandleInner\0")
};

const DATA_SHARE_EXTENSION_HELPER: &CStr = unsafe {
    CStr::from_bytes_with_nul_unchecked(b"@ohos.application.DataShareExtensionAbility.dataShareExtensionAbilityHelper.CallbackWrap\0")
};

pub fn get_native_ptr<'local>(env: &AniEnv<'local>, obj: &AniObject) -> i64 {
    let native_str = unsafe { CStr::from_bytes_with_nul_unchecked(b"nativePtr\0") };
    env.get_field::<i64>(obj, native_str).unwrap_or(0)
}

ani_constructor!(
    class "@ohos.data.DataShareResultSet.DataShareResultSetInner"
    [
        "getColumnNames" : result_set::get_column_names,
        "getColumnCount" : result_set::get_column_count,
        "getRowCount" : result_set::get_row_count,
        "getIsClosed" : result_set::get_is_closed,
        "goToFirstRow" : result_set::go_to_first_row,
        "goToLastRow" : result_set::go_to_last_row,
        "goToNextRow" : result_set::go_to_next_row,
        "goToPreviousRow" : result_set::go_to_previous_row,
        "goTo" : result_set::go_to,
        "goToRow" : result_set::go_to_row,
        "getBlob" : result_set::get_blob,
        "getString" : result_set::get_string,
        "getLong" : result_set::get_long,
        "getDouble" : result_set::get_double,
        "getColumnIndex" : result_set::get_column_index,
        "close" : result_set::close,
        "getColumnName" : result_set::get_column_name,
        "getDataType" : result_set::get_data_type,
    ]
    class "@ohos.data.dataSharePredicates.dataSharePredicates.DataSharePredicates"
    [
        "equalTo" : predicates::native_equal_to,
        "notEqualTo" : predicates::native_not_equal_to,
        "beginWrap" : predicates::native_begin_wrap,
        "endWrap" : predicates::native_end_wrap,
        "or" : predicates::native_or,
        "and" : predicates::native_and,
        "contains" : predicates::native_contains,
        "beginsWith" : predicates::native_begins_with,
        "endsWith" : predicates::native_ends_with,
        "isNull" : predicates::native_is_null,
        "isNotNull" : predicates::native_is_not_null,
        "like" : predicates::native_like,
        "unlike" : predicates::native_unlike,
        "glob" : predicates::native_glob,
        "between" : predicates::native_between,
        "notBetween" : predicates::native_not_between,
        "greaterThan" : predicates::native_greater_than,
        "greaterThanOrEqualTo" : predicates::native_greater_than_or_equal_to,
        "lessThanOrEqualTo" : predicates::native_less_than_or_equal_to,
        "lessThan" : predicates::native_less_than,
        "orderByAsc" : predicates::native_order_by_asc,
        "orderByDesc" : predicates::native_order_by_desc,
        "distinct" : predicates::native_distinct,
        "limit" : predicates::native_limit,
        "groupBy" : predicates::native_group_by,
        "indexedBy" : predicates::native_indexed_by,
        "inValues" : predicates::native_in,
        "notInValues" : predicates::native_not_in,
        "prefixKey" : predicates::native_prefix_key,
        "inKeys" : predicates::native_in_keys,
    ]
    namespace "@ohos.data.dataSharePredicates.dataSharePredicates"
    [
        "native_create" : predicates::create,
    ]
    namespace "@ohos.data.dataShare.dataShare"
    [
        "native_create": datashare::native_create,
        "native_enableSilentProxy": datashare::native_enable_silent_proxy,
        "native_disableSilentProxy": datashare::native_disable_silent_proxy,
        "native_query": datashare::native_query,
        "native_update": datashare::native_update,
        "native_publish": datashare::native_publish,
        "native_get_published_data": datashare::native_get_published_data,
        "native_insert": datashare::native_insert,
        "native_batch_insert": datashare::native_batch_insert,
        "native_batch_update": datashare::native_batch_update,
        "native_normalize_uri": datashare::native_normalize_uri,
        "native_denormalize_uri": datashare::native_denormalize_uri,
        "native_notify_change": datashare::native_notify_change,
        "native_notify_change_info": datashare::native_notify_change_info,
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
        "check_uris": datashare::check_uris,
        "publish_check_uris": datashare::publish_check_uris,
        "native_create_data_proxy_handle": datashare::native_create_data_proxy_handle,
        "native_on_data_proxy_handle_data_change": datashare::native_on_data_proxy_handle_data_change,
        "native_off_data_proxy_handle_data_change": datashare::native_off_data_proxy_handle_data_change,
        "native_data_proxy_handle_publish": datashare::native_data_proxy_handle_publish,
        "native_data_proxy_handle_delete": datashare::native_data_proxy_handle_delete,
        "native_data_proxy_handle_get": datashare::native_data_proxy_handle_get,
    ]
    class "@ohos.data.dataShare.dataShare.DataShareHelperInner"
    [
        "addTemplate" : datashare::native_add_template,
        "delTemplate" : datashare::native_del_template,
    ]
    class "@ohos.data.dataSharePredicates.dataSharePredicates.Cleaner"
    [
        "native_clean" : predicates::native_clean,
    ]
    class "@ohos.data.dataShare.dataShare.Cleaner"
    [
        "native_clean" : datashare::native_clean,
    ]
    class "@ohos.data.dataShare.dataShare.DataProxyHandleCleaner"
    [
        "native_proxy_handle_clean" : datashare::native_proxy_handle_clean,
    ]
    namespace "@ohos.application.DataShareExtensionAbility.dataShareExtensionAbilityHelper"
    [
        "nativeExtensionCallbackInt": datashare_extension::native_extension_callback_int,
        "nativeExtensionCallbackObject": datashare_extension::native_extension_callback_object,
        "nativeExtensionCallbackVoid": datashare_extension::native_extension_callback_void,
        "nativeExtensionCallbackString": datashare_extension::native_extension_callback_string,
        "nativeExtensionCallbackBatchUpdate": datashare_extension::native_extension_callback_batch_update
    ]
);

const LOG_LABEL: hilog_rust::HiLogLabel = hilog_rust::HiLogLabel {
    log_type: hilog_rust::LogType::LogCore,
    domain: 0xD001651,
    tag: "DataShare",
};
 
#[used]
#[link_section = ".init_array"]
static G_DATASHARE_PANIC_HOOK: extern "C" fn() = {
    #[link_section = ".text.startup"]
    extern "C" fn init() {
        std::panic::set_hook(Box::new(|info| {
            datashare_error!("Panic occurred: {:?}", info);
        }));
    }
    init
};
