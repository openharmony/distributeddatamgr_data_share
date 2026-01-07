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

use std::{collections::HashMap, ffi::CStr};

use ani_rs::{
    business_error::BusinessError,
    objects::{AniFnObject, AniAsyncCallback, AniObject, AniRef, GlobalRefCallback},
    typed_array::{Uint8Array, ArrayBuffer},
    AniEnv,
};

use crate::{
    get_native_ptr,
    predicates::ValueType,
    wrapper::{
        self,
        ffi::{PtrWrap, VersionWrap, I32ResultWrap, StringResultWrap, I64ResultWrap},
        GetPublishedDataSretParam, PublishSretParam, ValuesBucketKvItem, ValuesBucketWrap, CallbackFlavor,
        DataShareCallback,AniDataProxyResultSetParam, AniDataProxyGetResultSetParam,
        DataShareBatchUpdateParamIn, DataShareBatchUpdateParamOut,
        convert_to_business_error,
    },
    DATA_SHARE, DATA_SHARE_DATA_PROXY_HANDLE,
    datashare_info,
};

const DEFAULT_WAITTIME: i32 = 2;

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.DataShareHelperOptionsInner")]
struct DataShareHelperOptions {
    is_proxy: Option<bool>,
    wait_time: Option<i32>,
}

#[ani_rs::ani(path = "@ohos.data.DataShareResultSet.DataShareResultSetInner")]
pub struct DataShareResultSet {
    pub native_ptr: i64,
}

impl DataShareResultSet {
    fn new(native_ptr: i64) -> Self {
        Self {
            native_ptr,
        }
    }
}

#[ani_rs::ani(path = "@ohos.data.dataSharePredicates.DataSharePredicates")]
#[derive(Clone)]
pub struct DataSharePredicates {
    pub native_ptr: i64,
}

#[derive(serde::Serialize, serde::Deserialize, Clone, Debug)]
pub enum BucketValue {
    S(String),
    F64(f64),
    Boolean(bool),
    I64(i64),
    Uint8Array(Uint8Array),
    Null(()),
}

#[derive(serde::Serialize, serde::Deserialize, Clone)]
pub enum PublishedItemData {
    S(String),
    ArrayBuffer(ArrayBuffer),
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.PublishedItemInner")]
#[derive(Clone)]
pub struct PublishedItem {
    pub key: String,
    pub data: PublishedItemData,
    pub subscriber_id: String,
}

impl PublishedItem {
    pub fn new(key: String, data: PublishedItemData, subscriber_id: String) -> Self {
        Self {
            key,
            data,
            subscriber_id,
        }
    }
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.OperationResultInner")]
pub struct OperationResult {
    key: String,
    result: i32,
}

impl OperationResult {
    pub fn new(key: String, result: i32) -> Self {
        Self { key, result }
    }
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.UpdateOperationInner")]
#[derive(Clone)]
pub struct UpdateOperation {
    pub values: HashMap<String, BucketValue>,
    pub predicates: DataSharePredicates,
}

impl UpdateOperation {
    pub fn new() -> Self {
        Self {
            values: HashMap::new(),
            predicates: DataSharePredicates { native_ptr: 0 },
        }
    }
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.UpdateOperationInner")]
#[derive(Clone)]
pub struct UpdateOperationPredicates<'local> {
    pub values: HashMap<String, BucketValue>,
    pub predicates: AniObject<'local>,
}

impl<'local> UpdateOperationPredicates<'local> {
    pub fn new(iter: AniObject<'local>) -> Self {
        Self {
            values: HashMap::new(),
            predicates: iter,
        }
    }
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.TemplateInner")]
pub struct Template {
    pub predicates: HashMap<String, String>,
    pub scheduler: String,
    pub update: Option<String>,
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.ChangeType")]
#[derive(Clone)]
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

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.SubscriptionType")]
pub enum SubscriptionType {
    SubscriptionTypeExactUri = 0,
}

#[derive(serde::Serialize, serde::Deserialize, Clone)]
#[serde(rename = "@ohos.data.dataShare.dataShare.ChangeInfoInner\0")]
pub struct ChangeInfo {
    #[serde(rename = "type\0")]
    pub change_type: ChangeType,
    #[serde(rename = "uri\0")]
    pub uri: String,
    #[serde(rename = "values\0")]
    pub values: Vec<HashMap<String, BucketValue>>,
}

impl ChangeInfo {
    pub fn new(change_type: ChangeType, uri: String) -> Self {
        Self {
            change_type,
            uri,
            values: Vec::new(),
        }
    }
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.TemplateIdInner")]
#[derive(Clone)]
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

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.RdbDataChangeNodeInner")]
#[derive(Clone)]
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

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.PublishedDataChangeNodeInner")]
#[derive(Clone)]
pub struct PublishedDataChangeNode {
    bundle_name: String,
    data: Vec<PublishedItem>,
}

impl PublishedDataChangeNode {
    pub fn new(bundle_name: String) -> Self {
        Self {
            bundle_name,
            data: Vec::new(),
        }
    }

    pub fn push_data(&mut self, data_item: PublishedItem) {
        self.data.push(data_item);
    }
}

fn get_stage_mode_context<'local>(env: &AniEnv<'local>, context: AniObject<'local>) -> i64 {
    let native_context_str = unsafe { CStr::from_bytes_with_nul_unchecked(b"nativeContext\0") };
    env.get_field::<i64>(&context, native_context_str)
        .unwrap_or(0)
}

#[ani_rs::native]
pub fn native_create<'local>( 
    env: &AniEnv<'local>,
    context: AniObject<'local>,
    uri: String,
    options: Option<DataShareHelperOptions>,
) -> Result<AniRef<'local>, BusinessError> {
    let native_context = get_stage_mode_context(&env, context);

    let result_wrap;
    if let Some(opt_inner) = options {
        let is_proxy = opt_inner.is_proxy.unwrap_or(false);
        let wait_time = opt_inner.wait_time.unwrap_or(DEFAULT_WAITTIME);
        result_wrap =
            wrapper::ffi::DataShareNativeCreate(native_context, uri, false, is_proxy, wait_time);
    } else {
        result_wrap =
            wrapper::ffi::DataShareNativeCreate(native_context, uri, true, false, DEFAULT_WAITTIME);
    }

    if result_wrap.err_code != 0 {
        return Err(convert_to_business_error(result_wrap.err_code));
    }

    let datashare_helper_ptr = result_wrap.result;
    let ctor_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"l:\0") };
    let datashare_class = env.find_class(DATA_SHARE)?;
    let datashare_obj = env
        .new_object_with_signature(&datashare_class, ctor_signature, (datashare_helper_ptr,))?;
    return Ok(datashare_obj.into());
}

#[ani_rs::native]
pub fn native_enable_silent_proxy<'local>(
    env: &AniEnv<'local>,
    context: AniObject<'local>,
    uri: Option<String>,
) -> Result<i32, BusinessError>{
    let native_context = get_stage_mode_context(&env, context);
    let mut err_code = 0;
    if let Some(uri) = uri {
        err_code = wrapper::ffi::DataShareNativeEnableSilentProxy(native_context, uri);
    } else {
        err_code = wrapper::ffi::DataShareNativeEnableSilentProxy(native_context, "".to_string());
    }
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    return Ok(err_code);
}

#[ani_rs::native]
pub fn native_disable_silent_proxy<'local>(
    env: &AniEnv<'local>,
    context: AniObject<'local>,
    uri: Option<String>,
) -> Result<i32, BusinessError> {
    let native_context = get_stage_mode_context(&env, context);
    let mut err_code = 0;
    if let Some(uri) = uri {
        err_code = wrapper::ffi::DataShareNativeDisableSilentProxy(native_context, uri);
    } else {
        err_code = wrapper::ffi::DataShareNativeDisableSilentProxy(native_context, "".to_string());
    }
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    return Ok(err_code);
}

pub fn native_clean<'local>(env: AniEnv<'local>, ani_this: AniObject<'local>) {
    let datashare_helper_ptr = get_native_ptr(&env, &ani_this.into());
    wrapper::ffi::DataShareNativeClean(datashare_helper_ptr);
}

#[ani_rs::native]
pub fn native_query<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uri: String,
    predicates: AniObject<'local>,
    columns: Vec<String>,
) -> Result<AniRef<'local>, BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let predicates_ptr = get_native_ptr(&env, &predicates);

    let result_wrap = wrapper::ffi::DataShareNativeQuery(
        datashare_helper_ptr,
        uri,
        predicates_ptr,
        columns,
    );

    if result_wrap.err_code != 0 {
        return Err(convert_to_business_error(result_wrap.err_code));
    }

    let result_set_obj = DataShareResultSet::new(result_wrap.result);
    let result_ref = env.serialize(&result_set_obj)?;
    return Ok(result_ref);
}

#[ani_rs::native]
pub fn native_update<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uri: String,
    predicates: AniObject<'local>,
    value: HashMap<String, BucketValue>,
) -> Result<i32, BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let predicates_ptr = get_native_ptr(&env, &predicates);

    let vec_bucket: Vec<ValuesBucketKvItem> = value
        .into_iter()
        .map(|(k, v)| ValuesBucketKvItem::new(k, v))
        .collect();

    let result_wrap = wrapper::ffi::DataShareNativeUpdate(datashare_helper_ptr, uri, predicates_ptr, vec_bucket);
    if result_wrap.err_code != 0 {
        return Err(convert_to_business_error(result_wrap.err_code));
    }
    return Ok(result_wrap.result);
}

#[ani_rs::native]
pub fn native_publish<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    data: Vec<PublishedItem>,
    bundle_name: String,
    version: Option<i32>,
) -> Result<AniRef<'local>, BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);

    let mut sret_param = PublishSretParam::new();
    let version = match version {
        Some(v) => VersionWrap::new(false, v),
        None => VersionWrap::new(true, 0),
    };
    let err_code = wrapper::ffi::DataShareNativePublish(
        datashare_helper_ptr,
        data,
        bundle_name,
        version,
        &mut sret_param,
    );
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    let ret = sret_param.into_inner();
    let ret_ref = env.serialize(&ret)?;
    return Ok(ret_ref);
}

#[ani_rs::native]
pub fn native_get_published_data<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    bundle_name: String,
) -> Result<AniRef<'local>, BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let mut sret = GetPublishedDataSretParam::new();

    let err_code = wrapper::ffi::DataShareNativeGetPublishedData(
        datashare_helper_ptr,
        bundle_name,
        &mut sret,
    );
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    let item_array = sret.transform_to_item();
    let item_array_ref = env.serialize(&item_array)?;
    return Ok(item_array_ref);
}

#[ani_rs::native]
pub fn native_add_template<'local>(
    env: &AniEnv<'local>,
    object: AniObject<'local>,
    uri: String,
    subscriber_id: String,
    template: Template,
) -> Result<(), BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &object);

    let err_code = wrapper::ffi::DataShareNativeAddTemplate(
        datashare_helper_ptr,
        uri,
        subscriber_id,
        &template,
    );
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    return Ok(());
}

#[ani_rs::native]
pub fn native_del_template<'local>(
    env: &AniEnv<'local>,
    object: AniObject<'local>,
    uri: String,
    subscriber_id: String,
) -> Result<(), BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &object);

    let err_code = wrapper::ffi::DataShareNativeDelTemplate(datashare_helper_ptr, uri, subscriber_id);
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    return Ok(());
}

#[ani_rs::native]
pub fn native_insert<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uri: String,
    value: HashMap<String, BucketValue>,
) -> Result<i32, BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);

    let vec_bucket: Vec<ValuesBucketKvItem> = value
        .into_iter()
        .map(|(k, v)| ValuesBucketKvItem::new(k, v))
        .collect();

    let result_wrap = wrapper::ffi::DataShareNativeInsert(datashare_helper_ptr, uri, vec_bucket);
    if result_wrap.err_code != 0 {
        return Err(convert_to_business_error(result_wrap.err_code));
    }
    return Ok(result_wrap.result);
}

#[ani_rs::native]
pub fn native_batch_insert<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uri: String,
    values: Vec<HashMap<String, BucketValue>>,
) -> Result<i32, BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);

    let vec_bucket: Vec<ValuesBucketWrap> = values
        .into_iter()
        .map(|hm| {
            let res = hm
                .into_iter()
                .map(|(k, v)| ValuesBucketKvItem::new(k, v))
                .collect::<Vec<ValuesBucketKvItem>>();
            ValuesBucketWrap(res)
        })
        .collect();

    let result_wrap = wrapper::ffi::DataShareNativeBatchInsert(datashare_helper_ptr, uri, vec_bucket);
    if result_wrap.err_code != 0 {
        return Err(convert_to_business_error(result_wrap.err_code));
    }
    return Ok(result_wrap.result);
}

#[ani_rs::native]
pub fn native_batch_update<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    operations: AniObject<'local>,
) -> Result<HashMap<String, Vec<i32>>, BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let mut param_in: DataShareBatchUpdateParamIn = DataShareBatchUpdateParamIn::new();
    param_in.operations = env.deserialize(operations).unwrap();
    let mut param_out = DataShareBatchUpdateParamOut::new();
    wrapper::ffi::DataShareNativeBatchUpdate(
        datashare_helper_ptr,
        &param_in,
        &mut param_out,
    );
    if (param_out.error_code != 0) {
        return Err(convert_to_business_error(param_out.error_code));
    }
    return Ok(param_out.results);
}

#[ani_rs::native]
pub fn native_normalize_uri<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uri: AniObject<'local>,
) -> Result<String, BusinessError> {
    let str_uri: String = env.deserialize(uri)?;
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let result_wrap = wrapper::ffi::DataShareNativeNormalizeUri(datashare_helper_ptr, str_uri);
    if result_wrap.errCode != 0 {
        return Err(convert_to_business_error(result_wrap.errCode));
    }
    return Ok(result_wrap.result);
}

#[ani_rs::native]
pub fn native_denormalize_uri<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uri: String,
) -> Result<String, BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let result_wrap = wrapper::ffi::DataShareNativeDeNormalizeUri(datashare_helper_ptr, uri);
    if result_wrap.errCode != 0 {
        return Err(convert_to_business_error(result_wrap.errCode));
    }
    return Ok(result_wrap.result);
}

#[ani_rs::native]
pub fn native_notify_change<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uri: String,
) -> Result<(), BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let err_code = wrapper::ffi::DataShareNativeNotifyChange(datashare_helper_ptr, uri);
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    return Ok(());
}

#[ani_rs::native]
pub fn native_notify_change_info<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    data: ChangeInfo,
) -> Result<(), BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let vec_bucket: Vec<ValuesBucketWrap> = data.values
        .into_iter()
        .map(|hm| {
            let res = hm
                .into_iter()
                .map(|(k, v)| ValuesBucketKvItem::new(k, v))
                .collect::<Vec<ValuesBucketKvItem>>();
            ValuesBucketWrap(res)
        })
        .collect();
    let err_code = wrapper::ffi::DataShareNativeNotifyChangeInfo(
        datashare_helper_ptr,
        data.change_type as i32,
        data.uri,
        vec_bucket);
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    return Ok(());
}

#[ani_rs::native]
pub fn native_delete<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uri: String,
    predicates: AniObject<'local>,
) -> Result<i32, BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let predicates_ptr = get_native_ptr(&env, &predicates);

    let result_wrap = wrapper::ffi::DataShareNativeDelete(datashare_helper_ptr, uri, predicates_ptr);
    if result_wrap.err_code != 0 {
        return Err(convert_to_business_error(result_wrap.err_code));
    }
    return Ok(result_wrap.result);
}

#[ani_rs::native]
pub fn native_close<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
) -> Result<(), BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);

    let err_code = wrapper::ffi::DataShareNativeClose(datashare_helper_ptr);
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    return Ok(());
}

#[ani_rs::native]
pub fn native_on<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uri: String,
    callback: AniFnObject<'local>,
) -> Result<(), BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let callback_global = callback.into_global_callback(&env)?;
    let datashare_callback = DataShareCallback::new(CallbackFlavor::DataChange(callback_global));
    let ptr_wrap = PtrWrap::new(datashare_helper_ptr, Box::new(datashare_callback));

    let err_code = wrapper::ffi::DataShareNativeOn(ptr_wrap, uri);
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    return Ok(());
}

#[ani_rs::native]
pub fn native_on_changeinfo<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    arktype: SubscriptionType,
    uri: String,
    callback: AniFnObject<'local>,
) -> Result<(), BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let callback_global = callback.into_global_callback(&env)?;
    let datashare_callback = DataShareCallback::new(CallbackFlavor::DataChangeInfo(callback_global));
    let ptr_wrap = PtrWrap::new(datashare_helper_ptr, Box::new(datashare_callback));

    let err_code = wrapper::ffi::DataShareNativeOnChangeinfo(
        ptr_wrap,
        arktype as i32,
        uri,
    );
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    return Ok(());
}

#[ani_rs::native]
pub fn native_on_rdb_data_change<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uris: Vec<String>,
    template_id: TemplateId,
    callback: AniFnObject<'local>,
) -> Result<AniRef<'local>, BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let callback_global = callback.into_global_callback(&env)?;
    let datashare_callback = DataShareCallback::new(CallbackFlavor::RdbDataChange(callback_global));
    let ptr_wrap = PtrWrap::new(datashare_helper_ptr, Box::new(datashare_callback));

    let mut sret_param = PublishSretParam::new();
    let err_code = wrapper::ffi::DataShareNativeOnRdbDataChange(
        ptr_wrap,
        uris,
        &template_id,
        &mut sret_param,
    );

    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    let ret = sret_param.into_inner();
    let ret_ref = env.serialize(&ret)?;
    return Ok(ret_ref);
}

#[ani_rs::native]
pub fn native_on_published_data_change<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uris: Vec<String>,
    subscriber_id: String,
    callback: AniFnObject<'local>,
) -> Result<AniRef<'local>, BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let callback_global = callback.into_global_callback(&env)?;
    let datashare_callback = DataShareCallback::new(CallbackFlavor::PublishedDataChange(callback_global));
    let ptr_wrap = PtrWrap::new(datashare_helper_ptr, Box::new(datashare_callback));

    let mut sret_param = PublishSretParam::new();
    let err_code = wrapper::ffi::DataShareNativeOnPublishedDataChange(
        ptr_wrap,
        uris,
        subscriber_id,
        &mut sret_param,
    );

    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    let ret = sret_param.into_inner();
    let ret_ref = env.serialize(&ret)?;
    return Ok(ret_ref);
}

#[ani_rs::native]
pub fn native_off<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uri: String,
    callback: AniFnObject<'local>,
) -> Result<(), BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let err_code;
    if env.is_undefined(&callback)? { // if语句用?
        err_code = wrapper::ffi::DataShareNativeOffNone(datashare_helper_ptr, uri);
    } else {
        let callback_global = callback.into_global_callback(&env)?;
        let datashare_callback = DataShareCallback::new(CallbackFlavor::DataChange(callback_global));
        let ptr_wrap = PtrWrap::new(datashare_helper_ptr, Box::new(datashare_callback));
        err_code = wrapper::ffi::DataShareNativeOff(ptr_wrap, uri);
    };
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    return Ok(());
}

#[ani_rs::native]
pub fn native_off_changeinfo<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    arktype: SubscriptionType,
    uri: String,
    callback: AniFnObject<'local>,
) -> Result<(), BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let err_code;
    if env.is_undefined(&callback)? {
        err_code = wrapper::ffi::DataShareNativeOffChangeinfoNone(datashare_helper_ptr, arktype as i32, uri);
    } else {
        let callback_global = callback.into_global_callback(&env)?;
        let datashare_callback = DataShareCallback::new(CallbackFlavor::DataChangeInfo(callback_global));
        let ptr_wrap = PtrWrap::new(datashare_helper_ptr, Box::new(datashare_callback));
        err_code = wrapper::ffi::DataShareNativeOffChangeinfo(ptr_wrap, arktype as i32, uri);
    };
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    return Ok(());
}

#[ani_rs::native]
pub fn native_off_rdb_data_change<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uris: Vec<String>,
    template_id: TemplateId,
    callback: AniFnObject<'local>,
) -> Result<AniRef<'local>, BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let mut sret_param = PublishSretParam::new();
    let err_code;
    if env.is_undefined(&callback)? {
        err_code = wrapper::ffi::DataShareNativeOffRdbDataChangeNone(
            datashare_helper_ptr,
            uris,
            &template_id,
            &mut sret_param,
        );
    } else {
        let callback_global = callback.into_global_callback(&env)?;
        let datashare_callback = DataShareCallback::new(CallbackFlavor::RdbDataChange(callback_global));
        let ptr_wrap = PtrWrap::new(datashare_helper_ptr, Box::new(datashare_callback));
        err_code = wrapper::ffi::DataShareNativeOffRdbDataChange(
            ptr_wrap,
            uris,
            &template_id,
            &mut sret_param,
        );
    };
    
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    let ret = sret_param.into_inner();
    let ret_ref = env.serialize(&ret)?;
    return Ok(ret_ref);
}

#[ani_rs::native]
pub fn native_off_published_data_change<'local>(
    env: &AniEnv<'local>,
    datashare_helper: AniObject<'local>,
    uris: Vec<String>,
    subscriber_id: String,
    callback: AniFnObject<'local>,
) -> Result<AniRef<'local>, BusinessError> {
    let datashare_helper_ptr = get_native_ptr(&env, &datashare_helper);
    let mut sret_param = PublishSretParam::new();
    let err_code;
    if env.is_undefined(&callback)? {
        err_code = wrapper::ffi::DataShareNativeOffPublishedDataChangeNone(
            datashare_helper_ptr,
            uris,
            subscriber_id,
            &mut sret_param,
        );
    } else {
        let callback_global = callback.into_global_callback(&env)?;
        let datashare_callback = DataShareCallback::new(CallbackFlavor::PublishedDataChange(callback_global));
        let ptr_wrap = PtrWrap::new(datashare_helper_ptr, Box::new(datashare_callback));
        err_code = wrapper::ffi::DataShareNativeOffPublishedDataChange(
            ptr_wrap,
            uris,
            subscriber_id,
            &mut sret_param,
        );
    };

    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    let ret = sret_param.into_inner();
    let ret_ref = env.serialize(&ret)?;
    return Ok(ret_ref);
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.ProxyDataInner")]
#[derive(Clone)]
pub struct AniProxyData {
    uri: String,
    value: Option<ValueType>,
    allowList: Option<Vec<String>>,
}

impl AniProxyData {
    pub fn new() -> Self {
        Self {
            uri: "".to_string(),
            value: None,
            allowList: None,
        }
    }
}

pub fn ani_proxy_data_get_uri(data: &AniProxyData) -> String {
    data.uri.clone()
}

pub fn ani_proxy_data_get_enum_type(data: &AniProxyData) -> wrapper::ffi::EnumType {
    if let Some(v) = &data.value {
        match v {
            ValueType::S(_) => wrapper::ffi::EnumType::StringType,
            ValueType::F64(_) => wrapper::ffi::EnumType::F64Type,
            ValueType::Boolean(_) => wrapper::ffi::EnumType::BooleanType,
            ValueType::I64(_) => wrapper::ffi::EnumType::I64Type,
        }
    } else {
        wrapper::ffi::EnumType::NullType
    }
}

pub fn ani_proxy_data_get_value_string(data: &AniProxyData) -> String {
    if let Some(v) = &data.value {
        match v {
            ValueType::S(s) => s.clone(),
            _ => "".to_string(),
        }
    } else {
        "".to_string()
    }
}

pub fn ani_proxy_data_get_value_f64(data: &AniProxyData) -> f64 {
    if let Some(v) = &data.value {
        match v {
            ValueType::F64(f) => *f,
            _ => 0.0,
        }
    } else {
        0.0
    }
}

pub fn ani_proxy_data_get_value_boolean(data: &AniProxyData) -> bool {
    if let Some(v) = &data.value {
        match v {
            ValueType::Boolean(b) => *b,
            _ => false,
        }
    } else {
        false
    }
}

pub fn ani_proxy_data_get_value_i64(data: &AniProxyData) -> i64 {
    if let Some(v) = &data.value {
        match v {
            ValueType::I64(i) => *i,
            _ => 0,
        }
    } else {
        0
    }
}

pub fn ani_proxy_data_get_data(data: &AniProxyData, vec: &mut Vec<String>) -> bool {
    if let Some(v) = &data.allowList {
        vec.clear();
        vec.extend(v.iter().cloned());
        false
    } else {
        vec.clear();
        true
    }
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.DataProxyChangeInfoInner")]
#[derive(Clone)]
pub struct DataProxyChangeInfo {
    type_: ChangeType,
    uri: String,
    value: ValueType,
}

impl DataProxyChangeInfo {
    pub fn new(type_: ChangeType, uri: String, value: ValueType) -> Self {
        Self { type_, uri, value }
    }
}

pub fn rust_create_proxy_data_change_info() -> Vec<DataProxyChangeInfo> {
    Vec::new()
}

pub fn data_proxy_change_info_push_i64(
    node: &mut Vec<DataProxyChangeInfo>, change_type: i32, change_info_uri: String, change_info_value: i64) {

    let adpgr = DataProxyChangeInfo {
        type_: ChangeType::from_i32(change_type),
        uri: change_info_uri,
        value: ValueType::I64(change_info_value),
    };
    node.push(adpgr);
}

pub fn data_proxy_change_info_push_f64(
    node: &mut Vec<DataProxyChangeInfo>, change_type: i32, change_info_uri: String, change_info_value: f64) {

    let adpgr = DataProxyChangeInfo {
        type_: ChangeType::from_i32(change_type),
        uri: change_info_uri,
        value: ValueType::F64(change_info_value),
    };
    node.push(adpgr);
}

pub fn data_proxy_change_info_push_bool(
    node: &mut Vec<DataProxyChangeInfo>, change_type: i32, change_info_uri: String, change_info_value: bool) {

    let adpgr = DataProxyChangeInfo {
        type_: ChangeType::from_i32(change_type),
        uri: change_info_uri,
        value: ValueType::Boolean(change_info_value),
    };
    node.push(adpgr);
}

pub fn data_proxy_change_info_push_string(
    node: &mut Vec<DataProxyChangeInfo>, change_type: i32, change_info_uri: String, change_info_value: String) {

    let adpgr = DataProxyChangeInfo {
        type_: ChangeType::from_i32(change_type),
        uri: change_info_uri,
        value: ValueType::S(change_info_value),
    };
    node.push(adpgr);
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.DataProxyErrorCode")]
#[derive(Debug)]
pub enum DataProxyErrorCode {
    Success = 0,
    UriNotExist = 1,
    NoPermission = 2,
    OverLimit = 3,
}

impl DataProxyErrorCode {
    pub fn from_i32(index: i32) -> Self {
        match index {
            0 => Self::Success,
            1 => Self::UriNotExist,
            2 => Self::NoPermission,
            3 => Self::OverLimit,
            _ => Self::Success,
        }
    }
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.DataProxyResultInner")]
#[derive(Debug)]
pub struct AniDataProxyResult {
    pub uri: String,
    pub result: DataProxyErrorCode,
}

impl AniDataProxyResult {
    pub fn new() -> Self {
        Self { uri: "".to_string(), result: DataProxyErrorCode::Success }
    }
}

pub fn data_share_data_proxy_result_set_info(adpr: &mut AniDataProxyResult, uri: String, result: i32) {
    adpr.uri = uri;
    adpr.result = DataProxyErrorCode::from_i32(result);
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.DataProxyGetResultInner")]
#[derive(Debug)]
pub struct AniDataProxyGetResult {
    pub uri: String,
    pub result: DataProxyErrorCode,
    pub value: Option<ValueType>,
    pub allowList: Option<Vec<String>>,
}

impl AniDataProxyGetResult {
    pub fn new() -> Self {
        Self {
            uri: "".to_string(),
            result: DataProxyErrorCode::Success,
            value: None,
            allowList: None,
        }
    }
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.DataProxyType")]
#[derive(Debug)]
pub enum AniDataProxyType {
    SharedConfig = 0,
}

impl AniDataProxyType {
    pub fn from_i32(index: i32) -> Self {
        match index {
            0 => Self::SharedConfig,
            _ => Self::SharedConfig,
        }
    }
    pub fn as_i32(&self) -> i32 {
        match self {
            Self::SharedConfig => 0,
        }
    }
}

#[ani_rs::ani(path = "@ohos.data.dataShare.dataShare.DataProxyConfigInner")]
#[derive(Debug)]
pub struct AniDataProxyConfig {
    type_: AniDataProxyType,
}

impl AniDataProxyConfig {
    pub fn new(type_: AniDataProxyType) -> Self {
        Self { type_ }
    }
}

pub fn data_share_data_proxy_config_get_type(config: &AniDataProxyConfig) -> i32 {
    config.type_.as_i32()
}

#[ani_rs::native]
pub fn native_create_data_proxy_handle<'local>(
    env: &AniEnv<'local>
) -> Result<AniRef<'local>, BusinessError> {
    let result_wrap = wrapper::ffi::DataProxyHandleNativeCreate();

    if result_wrap.err_code != 0 {
        return Err(convert_to_business_error(result_wrap.err_code));
    }

    let dataproxy_handle_ptr = result_wrap.result;
    let ctor_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"l:\0") };
    let dataproxy_handle_class = env.find_class(DATA_SHARE_DATA_PROXY_HANDLE)?;
    let dataproxy_handle_obj = env
        .new_object_with_signature(&dataproxy_handle_class, ctor_signature, (dataproxy_handle_ptr,))?;
    return Ok(dataproxy_handle_obj.into());
}

pub fn native_proxy_handle_clean<'local>(env: AniEnv<'local>, ani_this: AniObject<'local>) {
    let dataproxy_handle_ptr = get_native_ptr(&env, &ani_this.into());
    wrapper::ffi::CleanupDataProxyHandle(dataproxy_handle_ptr);
}

#[ani_rs::native]
pub fn native_on_data_proxy_handle_data_change<'local>(
    env: &AniEnv<'local>,
    datashare_proxy: AniObject<'local>,
    uris: Vec<String>,
    config: AniDataProxyConfig,
    callback: AniFnObject<'local>,
) -> Result<AniRef<'local>, BusinessError> {
    let datashare_data_proxy_handle = get_native_ptr(&env, &datashare_proxy);
    let callback_global = callback.into_global_callback(&env)?;
    let datashare_callback = DataShareCallback::new(CallbackFlavor::ProxyDataChange(callback_global));
    let ptr_wrap = PtrWrap::new(datashare_data_proxy_handle, Box::new(datashare_callback));
    let mut set_param = AniDataProxyResultSetParam::new();
    let err_code = wrapper::ffi::DataShareNativeDataProxyHandleOnDataProxy(
        ptr_wrap,
        uris,
        &mut set_param,
    );
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    let ret = set_param.into_inner();
    Ok(env.serialize(&ret)?)
}

#[ani_rs::native]
pub fn native_off_data_proxy_handle_data_change<'local>(
    env: &AniEnv<'local>,
    datashare_proxy: AniObject<'local>,
    uris: Vec<String>,
    config: AniObject<'local>,
    callback: AniFnObject<'local>,
) -> Result<AniRef<'local>, BusinessError> {
    let datashare_data_proxy_handle = get_native_ptr(&env, &datashare_proxy);
    let config_inner: AniDataProxyConfig = env.deserialize(config)?;
    let mut set_param = AniDataProxyResultSetParam::new();
    let err_code;
    if env.is_undefined(&callback)? {
        err_code = wrapper::ffi::DataShareNativeDataProxyHandleOffDataProxyNone(
            datashare_data_proxy_handle,
            uris,
            &mut set_param,
        );
    } else {
        let callback_global = callback.into_global_callback(&env)?;
        let datashare_callback = DataShareCallback::new(CallbackFlavor::ProxyDataChange(callback_global));
        let ptr_wrap = PtrWrap::new(datashare_data_proxy_handle, Box::new(datashare_callback));
        err_code = wrapper::ffi::DataShareNativeDataProxyHandleOffDataProxy(
            ptr_wrap,
            uris,
            &mut set_param,
        );
    };
    
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    let ret = set_param.into_inner();
    Ok(env.serialize(&ret)?)
}

#[ani_rs::native]
pub fn check_uris<'local>(
    env: &AniEnv<'local>,
    uris: Vec<String>,
) -> Result<i32, BusinessError> {
    let err_code = wrapper::ffi::ValidateUrisForDataProxy(uris);
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    Ok(err_code)
}

#[ani_rs::native]
pub fn publish_check_uris<'local>(
    env: &AniEnv<'local>,
    proxydata: AniObject<'local>,
) -> Result<i32, BusinessError> {
    let vec_proxydata: Vec<AniProxyData> = env.deserialize(proxydata)?;

    let err_code = wrapper::ffi::ValidateDataShareNativePublishParameters(vec_proxydata);
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    Ok(err_code)
}

#[ani_rs::native]
pub fn native_data_proxy_handle_publish<'local>(
    env: &AniEnv<'local>,
    datashare_proxy: AniObject<'local>,
    proxydata: AniObject<'local>,
    config: AniObject<'local>,
) -> Result<AniRef<'local>, BusinessError> {
    let datashare_data_proxy_handle = get_native_ptr(&env, &datashare_proxy);
    let vec_proxydata: Vec<AniProxyData> = env.deserialize(proxydata)?;
    let config_inner: AniDataProxyConfig = env.deserialize(config)?;
    let mut set_param = AniDataProxyResultSetParam::new();
    let err_code = wrapper::ffi::DataShareNativeDataProxyHandlePublish(
        datashare_data_proxy_handle,
        vec_proxydata,
        &config_inner,
        &mut set_param,
    );
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    let ret = set_param.into_inner();
    Ok(env.serialize(&ret)?)
}

#[ani_rs::native]
pub fn native_data_proxy_handle_delete<'local>(
    env: &AniEnv<'local>,
    datashare_proxy: AniObject<'local>,
    uris: Vec<String>,
    config: AniObject<'local>,
) -> Result<AniRef<'local>, BusinessError> {
    let datashare_data_proxy_handle = get_native_ptr(&env, &datashare_proxy);
    let config_inner: AniDataProxyConfig = env.deserialize(config)?;
    let mut set_param = AniDataProxyResultSetParam::new();
    let err_code = wrapper::ffi::DataShareNativeDataProxyHandleDelete(
        datashare_data_proxy_handle,
        uris,
        &config_inner,
        &mut set_param,
    );
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    let ret = set_param.into_inner();
    Ok(env.serialize(&ret)?)
}

#[ani_rs::native]
pub fn native_data_proxy_handle_get<'local>(
    env: &AniEnv<'local>,
    datashare_proxy: AniObject<'local>,
    uris: Vec<String>,
    config: AniObject<'local>,
) -> Result<AniRef<'local>, BusinessError> {
    let datashare_data_proxy_handle = get_native_ptr(&env, &datashare_proxy);
    let config_inner: AniDataProxyConfig = env.deserialize(config)?;
    let mut set_param = AniDataProxyGetResultSetParam::new();
    let err_code = wrapper::ffi::DataShareNativeDataProxyHandleGet(
        datashare_data_proxy_handle,
        uris,
        &config_inner,
        &mut set_param,
    );
    if err_code != 0 {
        return Err(convert_to_business_error(err_code));
    }
    let ret = set_param.into_inner();
    Ok(env.serialize(&ret)?)
}