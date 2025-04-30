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

use std::{collections::HashMap, ffi::CStr};

use ani_rs::{
    objects::{AniFnObject, AniObject, AniRef},
    typed_array::Uint8Array,
    AniEnv,
};

use crate::{
    get_native_ptr,
    wrapper::{
        self, ffi::{EnvPtrWrap, VersionWrap}, GetPublishedDataSretParam, PublishSretParam, ValuesBucketKvItem, ValuesBucketWrap
    },
};

pub const NATIVE_CREATE: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"native_create\0") };

pub const NATIVE_QUERY: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"native_query\0") };

pub const NATIVE_UPDATE: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"native_update\0") };

pub const NATIVE_INSERT: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"native_insert\0") };

pub const NATIVE_BATCH_INSERT: &CStr =
    unsafe { CStr::from_bytes_with_nul_unchecked(b"native_batch_insert\0") };

pub const NATIVE_DELETE: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"native_delete\0") };

pub const NATIVE_CLOSE: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"native_close\0") };

pub const NATIVE_PUBLISH: &CStr =
    unsafe { CStr::from_bytes_with_nul_unchecked(b"native_publish\0") };

pub const NATIVE_GET_PUBLISHED_DATA: &CStr =
    unsafe { CStr::from_bytes_with_nul_unchecked(b"native_get_published_data\0") };

pub const NATIVE_ADD_TEMPLATE: &CStr =
    unsafe { CStr::from_bytes_with_nul_unchecked(b"addTemplate\0") };

pub const NATIVE_DEL_TEMPLATE: &CStr =
    unsafe { CStr::from_bytes_with_nul_unchecked(b"delTemplate\0") };

pub const NATIVE_ON: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"native_on\0") };

pub const NATIVE_ON_CHANGEINFO: &CStr =
    unsafe { CStr::from_bytes_with_nul_unchecked(b"native_on_changeinfo\0") };

pub const NATIVE_ON_RGB_DATA_CHANGE: &CStr =
    unsafe { CStr::from_bytes_with_nul_unchecked(b"native_on_rdb_data_change\0") };

pub const NATIVE_ON_PUBLISHED_DATA_CHANGE: &CStr =
    unsafe { CStr::from_bytes_with_nul_unchecked(b"native_on_published_data_change\0") };

pub const NATIVE_OFF: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"native_off\0") };

pub const NATIVE_OFF_CHANGEINFO: &CStr =
    unsafe { CStr::from_bytes_with_nul_unchecked(b"native_off_changeinfo\0") };

pub const NATIVE_OFF_RGB_DATA_CHANGE: &CStr =
    unsafe { CStr::from_bytes_with_nul_unchecked(b"native_off_rdb_data_change\0") };

pub const NATIVE_OFF_PUBLISHED_DATA_CHANGE: &CStr =
    unsafe { CStr::from_bytes_with_nul_unchecked(b"native_off_published_data_change\0") };

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/DataShareHelperInner;")]
#[derive(Debug)]
struct DataShareHelperInner {
    native_ptr: i64,
}

impl DataShareHelperInner {
    pub fn new(native_ptr: i64) -> Self {
        Self { native_ptr }
    }
}

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/DataShareHelperOptionsInner;")]
#[derive(Debug)]
struct DataShareHelperOptions {
    is_proxy: Option<bool>,
}

#[ani_rs::ani(path = "L@ohos/data/DataShareResultSet/DataShareResultSetInner;")]
#[derive(Debug)]
struct DataShareResultSet {
    row_count: i32,
    native_ptr: i64,
}

impl DataShareResultSet {
    fn new(native_ptr: i64) -> Self {
        Self {
            row_count: 0,
            native_ptr,
        }
    }
}

#[derive(serde::Serialize, serde::Deserialize)]
pub enum BucketValue<'a> {
    S(String),
    F64(f64),
    Boolean(bool),
    #[serde(borrow)]
    Uint8Array(Uint8Array<'a>),
    Null(()),
}

#[derive(serde::Serialize, serde::Deserialize, Debug)]
pub enum PublishedItemData<'a> {
    S(String),
    ArrayBuffer(&'a [u8]),
}

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/PublishedItemInner;")]
#[derive(Debug)]
pub struct PublishedItem<'a> {
    pub key: String,
    #[serde(borrow)]
    pub data: PublishedItemData<'a>,
    pub subscriber_id: String,
}

impl<'a> PublishedItem<'a> {
    pub fn new(key: String, data: PublishedItemData<'a>, subscriber_id: String) -> Self {
        Self {
            key,
            data,
            subscriber_id,
        }
    }
}

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/OperationResultInner;")]
#[derive(Debug)]
pub struct OperationResult {
    key: String,
    result: i32,
}

impl OperationResult {
    pub fn new(key: String, result: i32) -> Self {
        Self { key, result }
    }
}

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/TemplateInner;")]
#[derive(Debug)]
pub struct Template {
    pub predicates: HashMap<String, String>,
    pub scheduler: String,
    pub update: Option<String>,
}

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/ChangeType;")]
#[derive(Debug)]
pub enum ChangeType {
    Insert = 0,
    Delete = 1,
    Update = 2,
}

impl ChangeType {
    pub fn from_i32(index: i32) -> Self {
        match index {
            0 => Self::Insert,
            1 => Self::Delete,
            2 => Self::Update,
            _ => Self::Insert,
        }
    }
}

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/SubscriptionType;")]
#[derive(Debug)]
pub enum SubscriptionType {
    SubscriptionTypeExactUri = 0,
}

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/ChangeInfoInner;")]
pub struct ChangeInfo<'a> {
    pub change_type: ChangeType,
    pub uri: String,
    #[serde(borrow)]
    pub values: Vec<HashMap<String, BucketValue<'a>>>,
}

impl<'a> ChangeInfo<'a> {
    pub fn new(change_type: ChangeType, uri: String) -> Self {
        Self {
            change_type,
            uri,
            values: Vec::new(),
        }
    }
}

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/TemplateIdInner;")]
#[derive(Debug)]
pub struct TemplateId {
    pub subscriber_id: String,
    pub bundle_name_of_owner: String,
}

impl TemplateId {
    pub fn new(subscriber_id: String, bundle_name_of_owner: String) -> Self {
        Self {
            subscriber_id,
            bundle_name_of_owner,
        }
    }
}

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/RdbDataChangeNodeInner;")]
#[derive(Debug)]
pub struct RdbDataChangeNode {
    uri: String,
    template_id: TemplateId,
    data: Vec<String>,
}

impl RdbDataChangeNode {
    pub fn new(uri: String, template_id: TemplateId) -> Self {
        Self {
            uri,
            template_id,
            data: Vec::new(),
        }
    }

    pub fn push_data(&mut self, data_item: String) {
        self.data.push(data_item);
    }
}

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/PublishedDataChangeNodeInner;")]
#[derive(Debug)]
pub struct PublishedDataChangeNode<'a> {
    bundle_name: String,
    #[serde(borrow)]
    data: Vec<PublishedItem<'a>>,
}

impl<'a> PublishedDataChangeNode<'a> {
    pub fn new(bundle_name: String) -> Self {
        Self {
            bundle_name,
            data: Vec::new(),
        }
    }

    pub fn push_data(&mut self, data_item: PublishedItem<'a>) {
        self.data.push(data_item);
    }
}

fn get_stage_mode_context<'local>(env: &AniEnv<'local>, context: AniObject<'local>) -> i64 {
    let native_context_str = unsafe { CStr::from_bytes_with_nul_unchecked(b"nativeContext\0") };
    env.get_field::<i64>(&context, native_context_str)
        .unwrap_or(0)
}

pub fn native_create<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    context: AniObject<'local>,
    uri: AniObject<'local>,
    options: AniObject<'local>,
) -> AniRef<'local> {
    let str_url: String = env.deserialize(uri).unwrap();
    let opt: Option<DataShareHelperOptions> = env.deserialize(options).unwrap();
    let native_context = get_stage_mode_context(&env, context);

    let datashare_helper_ptr;
    if let Some(opt_inner) = opt {
        let is_proxy = opt_inner.is_proxy.unwrap_or(false);
        datashare_helper_ptr =
            wrapper::ffi::DataShareNativeCreate(native_context, str_url, false, is_proxy);
    } else {
        datashare_helper_ptr =
            wrapper::ffi::DataShareNativeCreate(native_context, str_url, true, false);
    }

    let datashare_obj = DataShareHelperInner::new(datashare_helper_ptr);
    env.serialize(&datashare_obj).unwrap()
}

pub fn native_query<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    uri: AniObject<'local>,
    predicates: AniObject<'local>,
    columns: AniObject<'local>,
) -> AniRef<'local> {
    let str_uri: String = env.deserialize(uri).unwrap();
    let columns_rust: Vec<String> = env.deserialize(columns).unwrap();
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let predicates_ptr = get_native_ptr(&env, &predicates);

    let result_set_ptr = wrapper::ffi::DataShareNativeQuery(
        datashare_helper_ptr,
        str_uri,
        predicates_ptr,
        columns_rust,
    );
    let result_set_obj = DataShareResultSet::new(result_set_ptr);
    env.serialize(&result_set_obj).unwrap()
}

pub fn native_update<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    uri: AniObject<'local>,
    predicates: AniObject<'local>,
    value: AniObject<'local>,
) -> i32 {
    let str_uri: String = env.deserialize(uri).unwrap();
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let predicates_ptr = get_native_ptr(&env, &predicates);
    let map_bucket: HashMap<String, BucketValue> = env.deserialize(value).unwrap();

    let vec_bucket: Vec<ValuesBucketKvItem> = map_bucket
        .into_iter()
        .map(|(k, v)| ValuesBucketKvItem::new(k, v))
        .collect();

    wrapper::ffi::DataShareNativeUpdate(datashare_helper_ptr, str_uri, predicates_ptr, vec_bucket)
}

pub fn native_publish<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    data: AniObject<'local>,
    bundle_name: AniObject<'local>,
    version: AniObject<'local>,
) -> AniRef<'local> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let data_inner: Vec<PublishedItem> = env.deserialize(data).unwrap();
    let bundle_name_inner: String = env.deserialize(bundle_name).unwrap();
    let version: Option<i32> = env.deserialize(version).unwrap();

    let mut sret_param = PublishSretParam::new();
    let version = match version {
        Some(v) => {
            VersionWrap::new(false, v)
        }
        None => {
            VersionWrap::new(true, 0)
        }
    };
    wrapper::ffi::DataShareNativePublish(
        datashare_helper_ptr,
        data_inner,
        bundle_name_inner,
        version,
        &mut sret_param,
    );

    let ret = sret_param.into_inner();
    env.serialize(&ret).unwrap()
}

pub fn native_get_published_data<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    bundle_name: AniObject<'local>,
) -> AniRef<'local> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let bundle_name_inner: String = env.deserialize(bundle_name).unwrap();
    let mut sret = GetPublishedDataSretParam::new();

    wrapper::ffi::DataShareNativeGetPublishedData(
        datashare_helper_ptr,
        bundle_name_inner,
        &mut sret,
    );
    let item_array = sret.transform_to_item();
    env.serialize(&item_array).unwrap()
}

pub fn native_add_template<'local>(
    env: AniEnv<'local>,
    object: AniObject<'local>,
    uri: AniObject<'local>,
    subscriber_id: AniObject<'local>,
    template: AniObject<'local>,
) {
    let datashare_helper_ptr = get_native_ptr(&env, &object);
    let uri_inner: String = env.deserialize(uri).unwrap();
    let subscriber_id_inner: String = env.deserialize(subscriber_id).unwrap();
    let template_inner: Template = env.deserialize(template).unwrap();

    wrapper::ffi::DataShareNativeAddTemplate(
        datashare_helper_ptr,
        uri_inner,
        subscriber_id_inner,
        &template_inner,
    );
}

pub fn native_del_template<'local>(
    env: AniEnv<'local>,
    object: AniObject<'local>,
    uri: AniObject<'local>,
    subscriber_id: AniObject<'local>,
) {
    let datashare_helper_ptr = get_native_ptr(&env, &object);
    let uri_inner: String = env.deserialize(uri).unwrap();
    let subscriber_id_inner: String = env.deserialize(subscriber_id).unwrap();

    wrapper::ffi::DataShareNativeDelTemplate(datashare_helper_ptr, uri_inner, subscriber_id_inner);
}

pub fn native_insert<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    uri: AniObject<'local>,
    value: AniObject<'local>,
) -> i32 {
    let str_uri: String = env.deserialize(uri).unwrap();
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let map_bucket: HashMap<String, BucketValue> = env.deserialize(value).unwrap();

    let vec_bucket: Vec<ValuesBucketKvItem> = map_bucket
        .into_iter()
        .map(|(k, v)| ValuesBucketKvItem::new(k, v))
        .collect();

    wrapper::ffi::DataShareNativeInsert(datashare_helper_ptr, str_uri, vec_bucket)
}

pub fn native_batch_insert<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    uri: AniObject<'local>,
    values: AniObject<'local>,
) -> i32 {
    let str_uri: String = env.deserialize(uri).unwrap();
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let map_bucket: Vec<HashMap<String, BucketValue>> = env.deserialize(values).unwrap();

    let vec_bucket: Vec<ValuesBucketWrap> = map_bucket
        .into_iter()
        .map(|hm| {
            let res = hm
                .into_iter()
                .map(|(k, v)| ValuesBucketKvItem::new(k, v))
                .collect::<Vec<ValuesBucketKvItem>>();
            ValuesBucketWrap(res)
        })
        .collect();

    wrapper::ffi::DataShareNativeBatchInsert(datashare_helper_ptr, str_uri, vec_bucket)
}

pub fn native_delete<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    uri: AniObject<'local>,
    predicates: AniObject<'local>,
) -> i32 {
    let str_uri: String = env.deserialize(uri).unwrap();
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let predicates_ptr = get_native_ptr(&env, &predicates);

    wrapper::ffi::DataShareNativeDelete(datashare_helper_ptr, str_uri, predicates_ptr)
}

pub fn native_close<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
) {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);

    wrapper::ffi::DataShareNativeClose(datashare_helper_ptr);
}

pub fn native_on<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    arktype: AniObject<'local>,
    uri: AniObject<'local>,
    callback: AniFnObject<'local>,
) {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let str_type: String = env.deserialize(arktype).unwrap();
    let str_uri: String = env.deserialize(uri).unwrap();
    let callback_ptr = callback.as_raw() as i64;
    let env_ptr = env.inner as i64;
    let ptr_wrap = EnvPtrWrap::new(datashare_helper_ptr, callback_ptr, env_ptr);
    wrapper::ffi::DataShareNativeOn(
        ptr_wrap,
        str_type,
        str_uri,
    );
}

pub fn native_on_changeinfo<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    event: AniObject<'local>,
    arktype: AniObject<'local>,
    uri: AniObject<'local>,
    callback: AniFnObject<'local>,
) {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let str_event: String = env.deserialize(event).unwrap();
    let str_uri: String = env.deserialize(uri).unwrap();
    let callback_ptr = callback.as_raw() as i64;
    let env_ptr = env.inner as i64;
    let type_inner: SubscriptionType = env.deserialize(arktype).unwrap();
    let ptr_wrap = EnvPtrWrap::new(datashare_helper_ptr, callback_ptr, env_ptr);

    wrapper::ffi::DataShareNativeOnChangeinfo(
        ptr_wrap,
        str_event,
        type_inner as i32,
        str_uri,
    );
}

pub fn native_on_rdb_data_change<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    arktype: AniObject<'local>,
    uris: AniObject<'local>,
    template_id: AniObject<'local>,
    callback: AniFnObject<'local>,
) -> AniRef<'local> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let type_inner: String = env.deserialize(arktype).unwrap();
    let uris_inner: Vec<String> = env.deserialize(uris).unwrap();
    let template_id_inner: TemplateId = env.deserialize(template_id).unwrap();
    let callback_ptr = callback.as_raw() as i64;
    let env_ptr = env.inner as i64;
    let ptr_wrap = EnvPtrWrap::new(datashare_helper_ptr, callback_ptr, env_ptr);

    let mut sret_param = PublishSretParam::new();
    wrapper::ffi::DataShareNativeOnRdbDataChange(
        ptr_wrap,
        type_inner,
        uris_inner,
        &template_id_inner,
        &mut sret_param,
    );

    let ret = sret_param.into_inner();
    env.serialize(&ret).unwrap()
}

pub fn native_on_published_data_change<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    arktype: AniObject<'local>,
    uris: AniObject<'local>,
    subscriber_id: AniObject<'local>,
    callback: AniFnObject<'local>,
) -> AniRef<'local> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let type_inner: String = env.deserialize(arktype).unwrap();
    let uris_inner: Vec<String> = env.deserialize(uris).unwrap();
    let subscriber_id_inner: String = env.deserialize(subscriber_id).unwrap();
    let callback_ptr = callback.as_raw() as i64;
    let env_ptr = env.inner as i64;
    let ptr_wrap = EnvPtrWrap::new(datashare_helper_ptr, callback_ptr, env_ptr);

    let mut sret_param = PublishSretParam::new();
    wrapper::ffi::DataShareNativeOnPublishedDataChange(
        ptr_wrap,
        type_inner,
        uris_inner,
        subscriber_id_inner,
        &mut sret_param,
    );

    let ret = sret_param.into_inner();
    env.serialize(&ret).unwrap()
}

pub fn native_off<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    arktype: AniObject<'local>,
    uri: AniObject<'local>,
    callback: AniFnObject<'local>,
) {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let str_type: String = env.deserialize(arktype).unwrap();
    let str_uri: String = env.deserialize(uri).unwrap();

    let callback_ptr = if env.is_undefined(&callback).unwrap() {
        0
    } else {
        callback.as_raw() as i64
    };

    let env_ptr = env.inner as i64;
    let ptr_wrap = EnvPtrWrap::new(datashare_helper_ptr, callback_ptr, env_ptr);
    wrapper::ffi::DataShareNativeOff(
        ptr_wrap,
        str_type,
        str_uri,
    );
}

pub fn native_off_changeinfo<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    event: AniObject<'local>,
    arktype: AniObject<'local>,
    uri: AniObject<'local>,
    callback: AniFnObject<'local>,
) {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let str_event: String = env.deserialize(event).unwrap();
    let str_uri: String = env.deserialize(uri).unwrap();
    let callback_ptr = if env.is_undefined(&callback).unwrap() {
        0
    } else {
        callback.as_raw() as i64
    };
    let env_ptr = env.inner as i64;
    let type_inner: SubscriptionType = env.deserialize(arktype).unwrap();
    let ptr_wrap = EnvPtrWrap::new(datashare_helper_ptr, callback_ptr, env_ptr);

    wrapper::ffi::DataShareNativeOffChangeinfo(
        ptr_wrap,
        str_event,
        type_inner as i32,
        str_uri,
    );
}

pub fn native_off_rdb_data_change<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    arktype: AniObject<'local>,
    uris: AniObject<'local>,
    template_id: AniObject<'local>,
    callback: AniFnObject<'local>,
) -> AniRef<'local> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let type_inner: String = env.deserialize(arktype).unwrap();
    let uris_inner: Vec<String> = env.deserialize(uris).unwrap();
    let template_id_inner: TemplateId = env.deserialize(template_id).unwrap();
    let callback_ptr = if env.is_undefined(&callback).unwrap() {
        0
    } else {
        callback.as_raw() as i64
    };
    let env_ptr = env.inner as i64;
    let ptr_wrap = EnvPtrWrap::new(datashare_helper_ptr, callback_ptr, env_ptr);

    let mut sret_param = PublishSretParam::new();
    wrapper::ffi::DataShareNativeOffRdbDataChange(
        ptr_wrap,
        type_inner,
        uris_inner,
        &template_id_inner,
        &mut sret_param,
    );

    let ret = sret_param.into_inner();
    env.serialize(&ret).unwrap()
}

pub fn native_off_published_data_change<'local>(
    env: AniEnv<'local>,
    _object: AniObject<'local>,
    datashare_helper: AniObject<'local>,
    arktype: AniObject<'local>,
    uris: AniObject<'local>,
    subscriber_id: AniObject<'local>,
    callback: AniFnObject<'local>,
) -> AniRef<'local> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let type_inner: String = env.deserialize(arktype).unwrap();
    let uris_inner: Vec<String> = env.deserialize(uris).unwrap();
    let subscriber_id_inner: String = env.deserialize(subscriber_id).unwrap();
    let callback_ptr = if env.is_undefined(&callback).unwrap() {
        0
    } else {
        callback.as_raw() as i64
    };
    let env_ptr = env.inner as i64;
    let ptr_wrap = EnvPtrWrap::new(datashare_helper_ptr, callback_ptr, env_ptr);

    let mut sret_param = PublishSretParam::new();
    wrapper::ffi::DataShareNativeOffPublishedDataChange(
        ptr_wrap,
        type_inner,
        uris_inner,
        subscriber_id_inner,
        &mut sret_param,
    );

    let ret = sret_param.into_inner();
    env.serialize(&ret).unwrap()
}
