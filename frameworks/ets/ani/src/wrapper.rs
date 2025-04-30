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

use crate::datashare::{
    ChangeInfo, PublishedDataChangeNode, PublishedItem, RdbDataChangeNode, Template, TemplateId,
};
use crate::predicates::ValueType;

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

#[cxx::bridge(namespace = "OHOS::DataShareAni")]
pub mod ffi {
    enum EnumType {
        StringType = 0,
        F64Type = 1,
        BooleanType = 2,
        Uint8ArrayType = 3,
        NullType = 4,
        ArrayBufferType = 5,
    }

    struct EnvPtrWrap {
        dataShareHelperPtr: i64,
        callbackPtr: i64,
        envPtr: i64,
    }

    struct VersionWrap {
        versionIsUndefined: bool,
        version: i32,
    }

    extern "Rust" {
        type ValuesBucketKvItem<'a>;
        fn value_bucket_get_key(kv: &ValuesBucketKvItem) -> String;
        fn value_bucket_get_vtype(kv: &ValuesBucketKvItem) -> EnumType;
        fn value_bucket_get_string(kv: &ValuesBucketKvItem) -> String;
        fn value_bucket_get_f64(kv: &ValuesBucketKvItem) -> f64;
        fn value_bucket_get_bool(kv: &ValuesBucketKvItem) -> bool;
        fn value_bucket_get_uint8array(kv: &ValuesBucketKvItem) -> Vec<u8>;

        type ValueType;
        fn value_type_get_type(v: &ValueType) -> EnumType;
        fn value_type_get_string(v: &ValueType) -> String;
        fn value_type_get_f64(v: &ValueType) -> f64;
        fn value_type_get_bool(v: &ValueType) -> bool;

        type PublishedItem<'a>;
        type PublishSretParam;
        fn publish_sret_push(sret: &mut PublishSretParam, key: String, result: i32);
        fn published_item_get_key(item: &PublishedItem<'_>) -> String;
        fn published_item_get_subscriber_id(item: &PublishedItem<'_>) -> String;
        fn published_item_get_data_type(item: &PublishedItem<'_>) -> EnumType;
        fn published_item_get_data_string(item: &PublishedItem<'_>) -> String;
        unsafe fn published_item_get_data_arraybuffer<'a>(item: &'a PublishedItem<'_>) -> &'a [u8];

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

        type ValuesBucketWrap<'a>;
        unsafe fn values_bucket_wrap_inner<'a>(
            kv: &'a ValuesBucketWrap,
        ) -> &'a Vec<ValuesBucketKvItem<'a>>;
        fn execute_callback(callback_ptr: i64, env_ptr: i64);

        type ChangeInfo<'a>;
        fn rust_create_change_info(change_index: i32, uri: String) -> Box<ChangeInfo<'static>>;
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
        unsafe fn change_info_push_kv_uint8array<'a>(
            change_info: &mut ChangeInfo<'a>,
            key: String,
            value: &'a [u8],
            new_hashmap: bool,
        );
        fn change_info_push_kv_null(change_info: &mut ChangeInfo, key: String, new_hashmap: bool);
        fn execute_callback_changeinfo(callback_ptr: i64, env_ptr: i64, change_info: &ChangeInfo);

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
        fn execute_callback_rdb_data_change(
            callback_ptr: i64,
            env_ptr: i64,
            node: &RdbDataChangeNode,
        );

        type PublishedDataChangeNode<'a>;
        fn rust_create_published_data_change_node(
            bundle_name: String,
        ) -> Box<PublishedDataChangeNode<'static>>;
        fn published_data_change_node_push_item_str(
            node: &mut PublishedDataChangeNode,
            key: String,
            data: String,
            subscriber_id: String,
        );
        unsafe fn published_data_change_node_push_item_arraybuffer<'a>(
            node: &mut PublishedDataChangeNode<'a>,
            key: String,
            data: &'a [u8],
            subscriber_id: String,
        );
        fn execute_callback_published_data_change(
            callback_ptr: i64,
            env_ptr: i64,
            node: &PublishedDataChangeNode,
        );
    }

    unsafe extern "C++" {
        include!("datashare_ani.h");

        fn GoToFirstRow(result_set_ptr: i64) -> bool;
        fn GoToLastRow(result_set_ptr: i64) -> bool;
        fn GoToNextRow(result_set_ptr: i64) -> bool;
        fn GetString(result_set_ptr: i64, column_index: i32) -> String;
        fn GetLong(result_set_ptr: i64, column_index: i32) -> i64;
        fn Close(result_set_ptr: i64);
        fn GetColumnIndex(result_set_ptr: i64, s: String) -> i32;

        fn DataSharePredicatesNew() -> i64;
        fn DataSharePredicatesEqualTo(predicates_ptr: i64, field: String, value: &ValueType);
        fn DataSharePredicatesNotEqualTo(predicates_ptr: i64, field: String, value: &ValueType);
        fn DataSharePredicatesBeginWrap(predicates_ptr: i64);
        fn DataSharePredicatesEndWrap(predicates_ptr: i64);
        fn DataSharePredicatesOr(predicates_ptr: i64);
        fn DataSharePredicatesAnd(predicates_ptr: i64);
        fn DataSharePredicatesContains(predicates_ptr: i64, field: String, value: String);
        fn DataSharePredicatesIsNull(predicates_ptr: i64, field: String);
        fn DataSharePredicatesIsNotNull(predicates_ptr: i64, field: String);
        fn DataSharePredicatesLike(predicates_ptr: i64, field: String, value: String);
        fn DataSharePredicatesBetween(
            predicates_ptr: i64,
            field: String,
            low: &ValueType,
            high: &ValueType,
        );
        fn DataSharePredicatesGreaterThan(predicates_ptr: i64, field: String, value: &ValueType);
        fn DataSharePredicatesGreaterThanOrEqualTo(
            predicates_ptr: i64,
            field: String,
            value: &ValueType,
        );
        fn DataSharePredicatesLessThanOrEqualTo(
            predicates_ptr: i64,
            field: String,
            value: &ValueType,
        );
        fn DataSharePredicatesLessThan(predicates_ptr: i64, field: String, value: &ValueType);
        fn DataSharePredicatesOrderByAsc(predicates_ptr: i64, field: String);
        fn DataSharePredicatesOrderByDesc(predicates_ptr: i64, field: String);
        fn DataSharePredicatesLimit(predicates_ptr: i64, total: f64, offset: f64);
        fn DataSharePredicatesGroupBy(predicates_ptr: i64, field: Vec<String>);
        fn DataSharePredicatesIn(predicates_ptr: i64, field: String, value: Vec<ValueType>);
        fn DataSharePredicatesNotIn(predicates_ptr: i64, field: String, value: Vec<ValueType>);

        fn DataShareNativeCreate(
            context: i64,
            str_uri: String,
            option_is_undefined: bool,
            is_proxy: bool,
        ) -> i64;
        fn DataShareNativeQuery(
            dataShareHelperPtr: i64,
            strUri: String,
            dataSharePredicatesPtr: i64,
            columns: Vec<String>,
        ) -> i64;

        fn DataShareNativeUpdate(
            dataShareHelperPtr: i64,
            strUri: String,
            dataSharePredicatesPtr: i64,
            bucket: Vec<ValuesBucketKvItem>,
        ) -> i32;

        fn DataShareNativePublish(
            dataShareHelperPtr: i64,
            data: Vec<PublishedItem>,
            bundle_name: String,
            version: VersionWrap,
            sret: &mut PublishSretParam,
        );
        fn DataShareNativeGetPublishedData(
            dataShareHelperPtr: i64,
            bundle_name: String,
            sret: &mut GetPublishedDataSretParam,
        );

        fn DataShareNativeAddTemplate(
            dataShareHelperPtr: i64,
            uri: String,
            subscriberId: String,
            temp: &Template,
        );

        fn DataShareNativeDelTemplate(dataShareHelperPtr: i64, uri: String, subscriberId: String);

        fn DataShareNativeInsert(
            dataShareHelperPtr: i64,
            strUri: String,
            bucket: Vec<ValuesBucketKvItem>,
        ) -> i32;

        fn DataShareNativeBatchInsert(
            dataShareHelperPtr: i64,
            strUri: String,
            buckets: Vec<ValuesBucketWrap<'_>>,
        ) -> i32;

        fn DataShareNativeDelete(
            dataShareHelperPtr: i64,
            strUri: String,
            dataSharePredicatesPtr: i64,
        ) -> i32;

        fn DataShareNativeClose(dataShareHelperPtr: i64);

        fn DataShareNativeOn(
            ptrWrap: EnvPtrWrap,
            strType: String,
            strUri: String,
        );

        fn DataShareNativeOnChangeinfo(
            ptrWrap: EnvPtrWrap,
            event: String,
            arktype: i32,
            strUri: String,
        );

        fn DataShareNativeOnRdbDataChange(
            ptrWrap: EnvPtrWrap,
            arktype: String,
            uris: Vec<String>,
            templateId: &TemplateId,
            sret: &mut PublishSretParam,
        );

        fn DataShareNativeOnPublishedDataChange(
            ptrWrap: EnvPtrWrap,
            arktype: String,
            uris: Vec<String>,
            subscriberId: String,
            sret: &mut PublishSretParam,
        );

        fn DataShareNativeOff(
            ptrWrap: EnvPtrWrap,
            strType: String,
            strUri: String,
        );

        fn DataShareNativeOffChangeinfo(
            ptrWrap: EnvPtrWrap,
            event: String,
            arktype: i32,
            strUri: String,
        );

        fn DataShareNativeOffRdbDataChange(
            ptrWrap: EnvPtrWrap,
            arktype: String,
            uris: Vec<String>,
            templateId: &TemplateId,
            sret: &mut PublishSretParam,
        );

        fn DataShareNativeOffPublishedDataChange(
            ptrWrap: EnvPtrWrap,
            arktype: String,
            uris: Vec<String>,
            subscriberId: String,
            sret: &mut PublishSretParam,
        );
    }
}

impl ffi::EnvPtrWrap {
    pub fn new(datashare_helper_ptr: i64, callback_ptr: i64, env_ptr: i64) -> Self {
        Self {
            dataShareHelperPtr: datashare_helper_ptr,
            callbackPtr: callback_ptr,
            envPtr: env_ptr,
        }
    }
}

impl ffi::VersionWrap {
    pub fn new(version_is_undefined: bool, version: i32,) -> Self {
        Self {
            versionIsUndefined: version_is_undefined,
            version,
        }
    }
}
