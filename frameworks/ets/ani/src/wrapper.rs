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

use crate::datashare::{
    ChangeInfo, PublishedDataChangeNode, PublishedItem, RdbDataChangeNode, Template, TemplateId,
    AniDataProxyConfig, AniProxyData, AniDataProxyType, data_share_data_proxy_config_get_type,
    ani_proxy_data_get_uri, ani_proxy_data_get_enum_type, ani_proxy_data_get_value_string,
    ani_proxy_data_get_value_i64, ani_proxy_data_get_value_f64, ani_proxy_data_get_value_boolean,
    ani_proxy_data_get_data,
};
use crate::datashare_extension::*;
use crate::predicates::ValueType;
use ani_rs::{
    objects::GlobalRefCallback,
    business_error::BusinessError,
};
use std::sync::Arc;

mod change_info;
pub use change_info::*;
mod rdb_data_change_node;
pub use rdb_data_change_node::*;
mod published_data_change_node;
pub use published_data_change_node::*;
mod values_bucket;
pub use values_bucket::*;
mod published_item;
pub use published_item::*;
mod template;
pub use template::*;
mod value_type;
pub use value_type::*;
mod batch_update;
pub use batch_update::*;

pub const fn convert_to_business_error(code: i32) -> BusinessError {
    match code {
        202 => BusinessError::new_static(code, "Not system app."),
        401 => BusinessError::new_static(code, "Parameter error.Possible causes: 1. Mandatory parameters are left unspecified; 2. Incorrect parameters types."),
        15700000 => BusinessError::new_static(code, "Inner error. Possible causes: 1.The internal status is abnormal; 2.The interface is incorrectly used; 3.Permission configuration error; 4.A system error."),
        15700010 => BusinessError::new_static(code, "DataShareHelper fails to be initialized."),
        15700011 => BusinessError::new_static(code, "The URI does not exist."),
        15700012 => BusinessError::new_static(code, "The data area does not exist."),
        15700013 => BusinessError::new_static(code, "The DataShareHelper instance is already closed."),
        _ => BusinessError::new_static(code, "Unknown error"),
    }
}

#[cxx::bridge(namespace = "OHOS::DataShareAni")]
pub mod ffi {
    enum EnumType {
        StringType = 0,
        F64Type = 1,
        BooleanType = 2,
        Uint8ArrayType = 3,
        NullType = 4,
        ArrayBufferType = 5,
        I64Type = 6,
    }

    struct PtrWrap {
        dataShareHelperPtr: i64,
        callback: Box<DataShareCallback>,
    }

    struct VersionWrap {
        version_is_undefined: bool,
        version: i32,
    }

    struct I32ResultWrap {
        result: i32,
        err_code: i32,
    }

    struct StringResultWrap { 
        result: String,
        errCode: i32,
    }

    struct I64ResultWrap {
        result: i64,
        err_code: i32,
    }

    extern "Rust" {
        type ValuesBucketKvItem;
        fn value_bucket_get_key(kv: &ValuesBucketKvItem) -> String;
        fn value_bucket_get_vtype(kv: &ValuesBucketKvItem) -> EnumType;
        fn value_bucket_get_string(kv: &ValuesBucketKvItem) -> String;
        fn value_bucket_get_f64(kv: &ValuesBucketKvItem) -> f64;
        fn value_bucket_get_bool(kv: &ValuesBucketKvItem) -> bool;
        fn value_bucket_get_i64(kv: &ValuesBucketKvItem) -> i64;
        fn value_bucket_get_uint8array(kv: &ValuesBucketKvItem) -> Vec<u8>;

        type ValueType;
        fn value_type_get_type(v: &ValueType) -> EnumType;
        fn value_type_get_string(v: &ValueType) -> String;
        fn value_type_get_f64(v: &ValueType) -> f64;
        fn value_type_get_bool(v: &ValueType) -> bool;
        fn value_type_get_i64(v: &ValueType) -> i64;

        type PublishedItem;
        type PublishSretParam;
        fn publish_sret_push(sret: &mut PublishSretParam, key: String, result: i32);
        fn published_item_get_key(item: &PublishedItem) -> String;
        fn published_item_get_subscriber_id(item: &PublishedItem) -> String;
        fn published_item_get_data_type(item: &PublishedItem) -> EnumType;
        fn published_item_get_data_string(item: &PublishedItem) -> String;
        unsafe fn published_item_get_data_arraybuffer(item: &PublishedItem) -> Vec<u8>;

        type GetPublishedDataSretParam;
        fn published_data_sret_push_str(
            sret: &mut GetPublishedDataSretParam,
            key: String,
            subscriber_id: String,
            data_str: String,
        );

        fn published_data_sret_push_array(
            sret: &mut GetPublishedDataSretParam,
            key: String,
            subscriber_id: String,
            data_buffer: Vec<u8>,
        );

        type Template;
        type TemplatePredicatesKvItem;
        fn template_get_scheduler(temp: &Template) -> String;
        fn template_get_update(temp: &Template) -> String;
        fn template_get_predicates(temp: &Template) -> Vec<TemplatePredicatesKvItem>;
        fn template_predicates_get_key(kv: &TemplatePredicatesKvItem) -> &String;
        fn template_predicates_get_value(kv: &TemplatePredicatesKvItem) -> &String;

        type ValuesBucketWrap;
        unsafe fn values_bucket_wrap_inner(
            kv: &ValuesBucketWrap,
        ) -> &Vec<ValuesBucketKvItem>;

        type ChangeInfo;
        fn rust_create_change_info(change_index: i32, uri: String) -> Box<ChangeInfo>;
        fn change_info_push_kv_str(
            change_info: &mut ChangeInfo,
            key: String,
            value: String,
            new_hashmap: bool,
        );
        fn change_info_push_kv_f64(
            change_info: &mut ChangeInfo,
            key: String,
            value: f64,
            new_hashmap: bool,
        );
        fn change_info_push_kv_boolean(
            change_info: &mut ChangeInfo,
            key: String,
            value: bool,
            new_hashmap: bool,
        );
        unsafe fn change_info_push_kv_uint8array(
            change_info: &mut ChangeInfo,
            key: String,
            value: Vec<u8>,
            new_hashmap: bool,
        );
        fn change_info_push_kv_null(change_info: &mut ChangeInfo, key: String, new_hashmap: bool);

        type TemplateId;
        type RdbDataChangeNode;
        fn template_id_get_subscriber_id(template_id: &TemplateId) -> &String;
        fn template_id_get_bundle_name_of_owner(template_id: &TemplateId) -> &String;
        fn rust_create_rdb_data_change_node(
            uri: String,
            subscriber_id: String,
            bundle_name_of_owner: String,
        ) -> Box<RdbDataChangeNode>;
        fn rdb_data_change_node_push_data(node: &mut RdbDataChangeNode, data_item: String);

        type PublishedDataChangeNode;
        fn rust_create_published_data_change_node(
            bundle_name: String,
        ) -> Box<PublishedDataChangeNode>;
        fn published_data_change_node_push_item_str(
            node: &mut PublishedDataChangeNode,
            key: String,
            data: String,
            subscriber_id: String,
        );
        unsafe fn published_data_change_node_push_item_arraybuffer(
            node: &mut PublishedDataChangeNode,
            key: String,
            data: Vec<u8>,
            subscriber_id: String,
        );

        type DataShareCallback;
        fn execute_callback(self: &DataShareCallback);
        fn execute_callback_changeinfo(
            self: &DataShareCallback,
            change_info: &ChangeInfo,
        );
        fn execute_callback_rdb_data_change(
            self: &DataShareCallback,
            node: &RdbDataChangeNode,
        );
        fn execute_callback_published_data_change(
            self: &DataShareCallback,
            node: &PublishedDataChangeNode,
        );
        fn callback_is_equal(
            org_callback: &DataShareCallback,
            new_callback: &DataShareCallback,
        ) -> bool;


        type ValuesBucketHashWrap;
        fn call_arkts_insert(
            extension_ability_ptr: i64,
            env_ptr: i64,
            uri: String,
            value_bucket: &ValuesBucketHashWrap,
            native_ptr: i64,
        );
        fn rust_create_values_bucket() -> Box<ValuesBucketHashWrap>;
        fn value_bucket_push_kv_str(
            value_bucket: &mut ValuesBucketHashWrap,
            key: String,
            value: String,
        );
        fn value_bucket_push_kv_f64(
            value_bucket: &mut ValuesBucketHashWrap,
            key: String,
            value: f64,
        );
        fn value_bucket_push_kv_i64(
            value_bucket: &mut ValuesBucketHashWrap,
            key: String,
            value: i64,
        );
        fn value_bucket_push_kv_boolean(
            value_bucket: &mut ValuesBucketHashWrap,
            key: String,
            value: bool,
        );
        unsafe fn value_bucket_push_kv_uint8array(
            value_bucket: &mut ValuesBucketHashWrap,
            key: String,
            value: Vec<u8>,
        );
        fn value_bucket_push_kv_null(value_bucket: &mut ValuesBucketHashWrap, key: String);

        type ValuesBucketArrayWrap;
        fn call_arkts_batch_insert(
            extension_ability_ptr: i64,
            env_ptr: i64,
            uri: String,
            value_buckets: &ValuesBucketArrayWrap,
            native_ptr: i64,
        );
        fn rust_create_values_bucket_array() -> Box<ValuesBucketArrayWrap>;
        fn values_bucket_array_push_kv_str(
            value_buckets: &mut ValuesBucketArrayWrap,
            key: String,
            value: String,
            new_hashmap: bool,
        );
        fn values_bucket_array_push_kv_f64(
            value_buckets: &mut ValuesBucketArrayWrap,
            key: String,
            value: f64,
            new_hashmap: bool,
        );
        fn values_bucket_array_push_kv_i64(
            value_buckets: &mut ValuesBucketArrayWrap,
            key: String,
            value: i64,
            new_hashmap: bool,
        );
        fn values_bucket_array_push_kv_boolean(
            value_buckets: &mut ValuesBucketArrayWrap,
            key: String,
            value: bool,
            new_hashmap: bool,
        );
        unsafe fn values_bucket_array_push_kv_uint8array(
            value_buckets: &mut ValuesBucketArrayWrap,
            key: String,
            value: Vec<u8>,
            new_hashmap: bool,
        );
        fn values_bucket_array_push_kv_null(
            value_buckets: &mut ValuesBucketArrayWrap,
            key: String,
            new_hashmap: bool,
        );
        fn call_arkts_update(
            extension_ability_ptr: i64,
            env_ptr: i64,
            uri: String,
            predicates_ptr: i64,
            value_bucket: &ValuesBucketHashWrap,
            native_ptr: i64,
        );
        fn call_arkts_delete(
            extension_ability_ptr: i64,
            env_ptr: i64,
            uri: String,
            predicates_ptr: i64,
            native_ptr: i64,
        );
        fn call_arkts_query(
            extension_ability_ptr: i64,
            env_ptr: i64,
            uri: String,
            predicates_ptr: i64,
            columns: Vec<String>,
            native_ptr: i64,
        );
        pub fn call_arkts_on_create(
            extension_ability_ptr: i64,
            env_ptr: i64,
            ani_want: i64,
            native_ptr: i64,
        );
        fn call_arkts_normalize_uri(
            extension_ability_ptr: i64,
            env_ptr: i64,
            uri: String,
            native_ptr: i64,
        );
        fn call_arkts_denormalize_uri(
            extension_ability_ptr: i64,
            env_ptr: i64,
            uri: String,
            native_ptr: i64,
        );
        type DataShareBatchUpdateParamIn;
        type DataShareBatchUpdateParamOut;
        type ExtensionBatchUpdateParamIn;
        type ExtensionBatchUpdateParamOut;
        fn data_share_batch_update_param_out_push(
            param_out: &mut DataShareBatchUpdateParamOut,
            key: String,
            result: Vec<i32>,
        );
        pub fn data_share_batch_update_param_out_error_code(
            param_out: &mut DataShareBatchUpdateParamOut,
            error_code: i32,
        );
        pub fn data_share_batch_update_param_in_get_value(
            param_in: &DataShareBatchUpdateParamIn,
            vec_key: &mut Vec<String>,
            vec_predicates: &mut Vec<i64>,
            vec_bucket: &mut Vec<ValuesBucketWrap>,
            vec_step: &mut Vec<i64>,
        );
        pub fn extension_batch_update_param_in_get_value(
            param_in: &ExtensionBatchUpdateParamIn,
            vec_key: &mut Vec<String>,
            vec_value: &mut Vec<i32>,
            vec_steps: &mut Vec<i32>,
        );
        pub fn rust_create_extension_batch_update_param_out() -> Box<ExtensionBatchUpdateParamOut>;
        pub fn extension_batch_update_param_out_set_bucket(
            param_out: &mut ExtensionBatchUpdateParamOut,
            bucket: &ValuesBucketHashWrap,
        );
        pub fn extension_batch_update_param_out_set_value(
            param_out: &mut ExtensionBatchUpdateParamOut,
            env_ptr: i64,
            vec_key: Vec<String>,
            vec_predicates: Vec<i64>,
            vec_steps: Vec<i64>,
        );
        pub fn call_arkts_batch_update(
            extension_ability_ptr: i64,
            env_ptr: i64,
            param_in: &ExtensionBatchUpdateParamOut,
            native_ptr: i64,
        );

        type AniProxyData;
        type AniDataProxyConfig;
        type AniDataProxyResultSretParam;
        type AniDataProxyGetResultSretParam;
        pub fn data_proxy_result_sret_push(sret: &mut AniDataProxyResultSretParam, uri: String, result: i32);
        pub fn data_share_data_proxy_config_get_type(config: &AniDataProxyConfig) -> i32;
        pub fn ani_proxy_data_get_uri(data: &AniProxyData) -> String;
        pub fn ani_proxy_data_get_enum_type(data: &AniProxyData) -> EnumType;
        pub fn ani_proxy_data_get_value_string(data: &AniProxyData) -> String;
        pub fn ani_proxy_data_get_value_i64(data: &AniProxyData) -> i64;
        pub fn ani_proxy_data_get_value_f64(data: &AniProxyData) -> f64;
        pub fn ani_proxy_data_get_value_boolean(data: &AniProxyData) -> bool;
        pub fn ani_proxy_data_get_data(data: &AniProxyData, vec: &mut Vec<String>);
        pub fn data_proxy_get_result_sret_push_i64(
            sret: &mut AniDataProxyGetResultSretParam,
            uri: String,
            result: i32,
            value: i64,
            allowList: Vec<String>,
        );
        pub fn data_proxy_get_result_sret_push_f64(
            sret: &mut AniDataProxyGetResultSretParam,
            uri: String,
            result: i32,
            value: f64,
            allowList: Vec<String>,
        );
        pub fn data_proxy_get_result_sret_push_bool(
            sret: &mut AniDataProxyGetResultSretParam,
            uri: String,
            result: i32,
            value: bool,
            allowList: Vec<String>,
        );
        pub fn data_proxy_get_result_sret_push_string(
            sret: &mut AniDataProxyGetResultSretParam,
            uri: String,
            result: i32,
            value: String,
            allowList: Vec<String>,
        );
    }

    unsafe extern "C++" {
        include!("datashare_ani.h");

        fn GetColumnNames(resultSetPtr: i64) -> Vec<String>;
        fn GetColumnCount(resultSetPtr: i64) -> i32;
        fn GetRowCount(resultSetPtr: i64) -> i32;
        fn GetIsClosed(resultSetPtr: i64) -> bool;
        fn GoToFirstRow(resultSetPtr: i64) -> bool;
        fn GoToLastRow(resultSetPtr: i64) -> bool;
        fn GoToNextRow(resultSetPtr: i64) -> bool;
        fn GoToPreviousRow(resultSetPtr: i64) -> bool;
        fn GoTo(resultSetPtr: i64, offset: i32) -> bool;
        fn GoToRow(resultSetPtr: i64, position: i32) -> bool;
        fn GetBlob(resultSetPtr: i64, columnIndex: i32) -> Vec<u8>;
        fn GetString(resultSetPtr: i64, columnIndex: i32) -> String;
        fn GetLong(resultSetPtr: i64, columnIndex: i32) -> i64;
        fn GetDouble(resultSetPtr: i64, columnIndex: i32) -> f64;
        fn Close(resultSetPtr: i64);
        fn GetColumnIndex(resultSetPtr: i64, s: String) -> i32;
        fn GetColumnName(resultSetPtr: i64, columnIndex: i32) -> String;
        fn GetDataType(resultSetPtr: i64, columnIndex: i32) -> i32;

        fn DataSharePredicatesNew() -> i64;
        fn DataSharePredicatesClean(predicatesPtr: i64);
        fn DataSharePredicatesEqualTo(predicatesPtr: i64, field: String, value: &ValueType);
        fn DataSharePredicatesNotEqualTo(predicatesPtr: i64, field: String, value: &ValueType);
        fn DataSharePredicatesBeginWrap(predicatesPtr: i64);
        fn DataSharePredicatesEndWrap(predicatesPtr: i64);
        fn DataSharePredicatesOr(predicatesPtr: i64);
        fn DataSharePredicatesAnd(predicatesPtr: i64);
        fn DataSharePredicatesContains(predicatesPtr: i64, field: String, value: String);
        fn DataSharePredicatesBeginsWith(predicatesPtr: i64, field: String, value: String);
        fn DataSharePredicatesEndsWith(predicatesPtr: i64, field: String, value: String);
        fn DataSharePredicatesIsNull(predicatesPtr: i64, field: String);
        fn DataSharePredicatesIsNotNull(predicatesPtr: i64, field: String);
        fn DataSharePredicatesLike(predicatesPtr: i64, field: String, value: String);
        fn DataSharePredicatesUnlike(predicatesPtr: i64, field: String, value: String);
        fn DataSharePredicatesGlob(predicatesPtr: i64, field: String, value: String);
        fn DataSharePredicatesBetween(
            predicatesPtr: i64,
            field: String,
            low: &ValueType,
            high: &ValueType,
        );
        fn DataSharePredicatesNotBetween(
            predicatesPtr: i64,
            field: String,
            low: &ValueType,
            high: &ValueType,
        );
        fn DataSharePredicatesGreaterThan(predicatesPtr: i64, field: String, value: &ValueType);
        fn DataSharePredicatesGreaterThanOrEqualTo(
            predicatesPtr: i64,
            field: String,
            value: &ValueType,
        );
        fn DataSharePredicatesLessThanOrEqualTo(
            predicatesPtr: i64,
            field: String,
            value: &ValueType,
        );
        fn DataSharePredicatesLessThan(predicatesPtr: i64, field: String, value: &ValueType);
        fn DataSharePredicatesOrderByAsc(predicatesPtr: i64, field: String);
        fn DataSharePredicatesOrderByDesc(predicatesPtr: i64, field: String);
        fn DataSharePredicatesDistinct(predicatesPtr: i64);
        fn DataSharePredicatesLimit(predicatesPtr: i64, total: i32, offset: i32);
        fn DataSharePredicatesGroupBy(predicatesPtr: i64, field: Vec<String>);
        fn DataSharePredicatesIndexedBy(predicatesPtr: i64, field: String);
        fn DataSharePredicatesIn(predicatesPtr: i64, field: String, value: Vec<ValueType>);
        fn DataSharePredicatesNotIn(predicatesPtr: i64, field: String, value: Vec<ValueType>);
        fn DataSharePredicatesPrefixKey(predicatesPtr: i64, prefix: String);
        fn DataSharePredicatesInKeys(predicatesPtr: i64, keys: Vec<String>);

        fn DataShareNativeCreate(
            context: i64,
            strUri: String,
            optionIsUndefined: bool,
            isProxy: bool,
            waitTime: i32,
        ) -> I64ResultWrap;

        fn DataShareNativeClean(dataShareHelperPtr: i64);

        fn DataShareNativeEnableSilentProxy(
            context: i64,
            strUri: String,
        ) -> i32;

        fn DataShareNativeDisableSilentProxy(
            context: i64,
            strUri: String,
        ) -> i32;

        fn DataShareNativeQuery(
            dataShareHelperPtr: i64,
            strUri: String,
            dataSharePredicatesPtr: i64,
            columns: Vec<String>,
        ) -> I64ResultWrap;

        fn DataShareNativeUpdate(
            dataShareHelperPtr: i64,
            strUri: String,
            dataSharePredicatesPtr: i64,
            bucket: Vec<ValuesBucketKvItem>,
        ) -> I32ResultWrap;

        fn DataShareNativePublish(
            dataShareHelperPtr: i64,
            data: Vec<PublishedItem>,
            bundle_name: String,
            version: VersionWrap,
            sret: &mut PublishSretParam,
        ) -> i32;
        fn DataShareNativeGetPublishedData(
            dataShareHelperPtr: i64,
            bundle_name: String,
            sret: &mut GetPublishedDataSretParam,
        ) -> i32;

        fn DataShareNativeAddTemplate(
            dataShareHelperPtr: i64,
            uri: String,
            subscriberId: String,
            temp: &Template,
        ) -> i32;

        fn DataShareNativeDelTemplate(dataShareHelperPtr: i64, uri: String, subscriberId: String) -> i32;

        fn DataShareNativeInsert(
            dataShareHelperPtr: i64,
            strUri: String,
            bucket: Vec<ValuesBucketKvItem>,
        ) -> I32ResultWrap;

        fn DataShareNativeBatchInsert(
            dataShareHelperPtr: i64,
            strUri: String,
            buckets: Vec<ValuesBucketWrap>,
        ) -> I32ResultWrap;

        fn DataShareNativeBatchUpdate(
            dataShareHelperPtr: i64,
            param_in: &DataShareBatchUpdateParamIn,
            param_out: &mut DataShareBatchUpdateParamOut,
        );

        fn DataShareNativeNormalizeUri(
            dataShareHelperPtr: i64,
            strUri: String,
        ) -> StringResultWrap;

        fn DataShareNativeDeNormalizeUri(
            dataShareHelperPtr: i64,
            strUri: String,
        ) -> StringResultWrap;

        fn DataShareNativeNotifyChange(
            dataShareHelperPtr: i64,
            strUri: String,
        ) -> i32;

        fn DataShareNativeNotifyChangeInfo(
            dataShareHelperPtr: i64,
            changeType: i32,
            strUri: String,
            buckets: Vec<ValuesBucketWrap>,
        ) -> i32;

        fn DataShareNativeDelete(
            dataShareHelperPtr: i64,
            strUri: String,
            dataSharePredicatesPtr: i64,
        ) -> I32ResultWrap;

        fn DataShareNativeClose(dataShareHelperPtr: i64) -> i32;

        fn DataShareNativeOn(ptrWrap: PtrWrap, strUri: String) -> i32;

        fn DataShareNativeOnChangeinfo(
            ptrWrap: PtrWrap,
            arktype: i32,
            strUri: String,
        ) -> i32;

        fn DataShareNativeOnRdbDataChange(
            ptrWrap: PtrWrap,
            uris: Vec<String>,
            templateId: &TemplateId,
            sret: &mut PublishSretParam,
        ) -> i32;

        fn DataShareNativeOnPublishedDataChange(
            ptrWrap: PtrWrap,
            uris: Vec<String>,
            subscriberId: String,
            sret: &mut PublishSretParam,
        ) -> i32;

        fn DataShareNativeOff(ptrWrap: PtrWrap, strUri: String) -> i32;

        fn DataShareNativeOffNone(dataShareHelperPtr: i64, strUri: String) -> i32;

        fn DataShareNativeOffChangeinfo(
            ptrWrap: PtrWrap,
            arktype: i32,
            strUri: String,
        ) -> i32;

        fn DataShareNativeOffChangeinfoNone(
            dataShareHelperPtr: i64,
            arktype: i32,
            strUri: String,
        ) -> i32;

        fn DataShareNativeOffRdbDataChange(
            ptrWrap: PtrWrap,
            uris: Vec<String>,
            templateId: &TemplateId,
            sret: &mut PublishSretParam,
        ) -> i32;

        fn DataShareNativeOffRdbDataChangeNone(
            dataShareHelperPtr: i64,
            uris: Vec<String>,
            templateId: &TemplateId,
            sret: &mut PublishSretParam,
        ) -> i32;

        fn DataShareNativeOffPublishedDataChange(
            ptrWrap: PtrWrap,
            uris: Vec<String>,
            subscriberId: String,
            sret: &mut PublishSretParam,
        ) -> i32;

        fn DataShareNativeOffPublishedDataChangeNone(
            dataShareHelperPtr: i64,
            uris: Vec<String>,
            subscriberId: String,
            sret: &mut PublishSretParam,
        ) -> i32;

        fn DataShareNativeExtensionCallbackInt(error_code: f64, error_msg: String, data: i32, native_ptr: i64);

        fn DataShareNativeExtensionCallbackObject(
            error_code: f64,
            error_msg: String,
            ptr: i64,
            native_ptr: i64,
        );

        fn DataShareNativeExtensionCallbackVoid(error_code: f64, error_msg: String, native_ptr: i64);

        fn DataShareNativeExtensionCallbackString(error_code: f64, error_msg: String, data: String, native_ptr: i64);

        fn DataShareNativeExtensionCallbackBatchUpdate(
            error_code: f64,
            error_msg: String,
            param_in: &ExtensionBatchUpdateParamIn,
            native_ptr: i64,
        );
    }
}

#[derive(PartialEq, Eq)]
pub enum CallbackFlavor {
    DataChange(GlobalRefCallback<()>),
    DataChangeInfo(GlobalRefCallback<(ChangeInfo,)>),
    RdbDataChange(GlobalRefCallback<(RdbDataChangeNode,)>),
    PublishedDataChange(GlobalRefCallback<(PublishedDataChangeNode,)>),
}

#[derive(PartialEq, Eq)]
pub struct DataShareCallback {
    inner: CallbackFlavor,
}

pub fn callback_is_equal(org_callback: &DataShareCallback, new_callback: &DataShareCallback) -> bool {
    return org_callback == new_callback;
}

impl DataShareCallback {
    pub fn new(flavor: CallbackFlavor) -> Self {
        Self { inner: flavor }
    }

    pub fn execute_callback(&self) {
        if let CallbackFlavor::DataChange(callback) = &self.inner {
            callback.execute(());
        }
    }

    pub fn execute_callback_changeinfo(&self, change_info: &ChangeInfo) {
        if let CallbackFlavor::DataChangeInfo(callback) = &self.inner {
            callback.execute((change_info.clone(),));
        }
    }
    
    pub fn execute_callback_rdb_data_change(&self, node: &RdbDataChangeNode) {
        if let CallbackFlavor::RdbDataChange(callback) = &self.inner {
            callback.execute((node.clone(),));
        }
    }

    pub fn execute_callback_published_data_change(&self, node: &PublishedDataChangeNode) {
        if let CallbackFlavor::PublishedDataChange(callback) = &self.inner {
            callback.execute((node.clone(),));
        }
    }
}

impl ffi::PtrWrap {
    pub fn new(datashare_helper_ptr: i64, datashare_callback: Box<DataShareCallback>) -> Self {
        Self {
            dataShareHelperPtr: datashare_helper_ptr,
            callback: datashare_callback,
        }
    }
}

impl ffi::VersionWrap {
    pub fn new(version_is_undefined: bool, version: i32) -> Self {
        Self {
            version_is_undefined: version_is_undefined,
            version,
        }
    }
}
