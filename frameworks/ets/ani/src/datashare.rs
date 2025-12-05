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
    wrapper::{
        self,
        ffi::{PtrWrap, VersionWrap, I32ResultWrap, I64ResultWrap},
        GetPublishedDataSretParam, PublishSretParam, ValuesBucketKvItem, ValuesBucketWrap, CallbackFlavor,
        DataShareCallback,
        convert_to_business_error,
    },
    DATA_SHARE,
    datashare_info,
};

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/DataShareHelperOptionsInner")]
struct DataShareHelperOptions {
    is_proxy: Option<bool>,
}

#[ani_rs::ani(path = "L@ohos/data/DataShareResultSet/DataShareResultSetInner")]
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

#[derive(serde::Serialize, serde::Deserialize, Clone, Debug)]
pub enum BucketValue {
    S(String),
    F64(f64),
    Boolean(bool),
    Uint8Array(Uint8Array),
    Null(()),
}

#[derive(serde::Serialize, serde::Deserialize, Clone)]
pub enum PublishedItemData {
    S(String),
    ArrayBuffer(ArrayBuffer),
}

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/PublishedItemInner")]
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

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/OperationResultInner")]
pub struct OperationResult {
    key: String,
    result: i32,
}

impl OperationResult {
    pub fn new(key: String, result: i32) -> Self {
        Self { key, result }
    }
}

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/TemplateInner")]
pub struct Template {
    pub predicates: HashMap<String, String>,
    pub scheduler: String,
    pub update: Option<String>,
}

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/ChangeType")]
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

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/SubscriptionType")]
pub enum SubscriptionType {
    SubscriptionTypeExactUri = 0,
}

#[derive(serde::Serialize, serde::Deserialize, Clone)]
#[serde(rename = "L@ohos/data/dataShare/dataShare/ChangeInfoInner;\0")]
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

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/TemplateIdInner")]
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

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/RdbDataChangeNodeInner")]
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

#[ani_rs::ani(path = "L@ohos/data/dataShare/dataShare/PublishedDataChangeNodeInner")]
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
        result_wrap =
            wrapper::ffi::DataShareNativeCreate(native_context, uri, false, is_proxy);
    } else {
        result_wrap =
            wrapper::ffi::DataShareNativeCreate(native_context, uri, true, false);
    }

    if result_wrap.err_code != 0 {
        return Err(convert_to_business_error(result_wrap.err_code));
    }

    let datashare_helper_ptr = result_wrap.result;
    let ctor_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"J:V\0") };
    let datashare_class = env.find_class(DATA_SHARE)?;
    let datashare_obj = env
        .new_object_with_signature(&datashare_class, ctor_signature, (datashare_helper_ptr,))?;
    return Ok(datashare_obj.into());
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
