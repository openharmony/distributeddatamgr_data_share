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

use ani_rs::{business_error::BusinessError, objects::AniObject, AniEnv};

use crate::{
    get_native_ptr, wrapper::{self, ValuesBucketArrayWrap, ValuesBucketHashWrap}, DATA_SHARE_EXTENSION_HELPER, DATA_SHARE_PREDICATES
};

pub fn call_arkts_insert(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    value_bucket: &ValuesBucketHashWrap,
    native_ptr: i64,
) {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER).unwrap();
    let instance = env
        .new_object(&helper_class, (extension_ability_obj, native_ptr))
        .unwrap();
    let do_insert_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"doInsert\0") };
    let do_insert_method = env
        .find_method(&helper_class, do_insert_method_name)
        .unwrap();

    let arg1 = env.serialize(&uri).unwrap();
    let arg2 = env.serialize(value_bucket.as_ref()).unwrap();
    env.call_method(&instance, &do_insert_method, (arg1, arg2))
        .unwrap();
}

pub fn call_arkts_batch_insert(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    value_buckets: &ValuesBucketArrayWrap,
    native_ptr: i64,
) {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER).unwrap();
    let instance = env
        .new_object(&helper_class, (extension_ability_obj, native_ptr))
        .unwrap();
    let do_batch_insert_method_name =
        unsafe { CStr::from_bytes_with_nul_unchecked(b"doBatchInsert\0") };
    let do_batch_insert_method = env
        .find_method(&helper_class, do_batch_insert_method_name)
        .unwrap();

    let arg1 = env.serialize(&uri).unwrap();
    let arg2 = env.serialize(value_buckets.as_ref()).unwrap();

    env.call_method(&instance, &do_batch_insert_method, (arg1, arg2))
        .unwrap();
}

pub fn call_arkts_update(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    predicates_ptr: i64,
    value_bucket: &ValuesBucketHashWrap,
    native_ptr: i64,
) {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER).unwrap();
    let instance = env
        .new_object(&helper_class, (extension_ability_obj, native_ptr))
        .unwrap();
    let do_update_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"doUpdate\0") };
    let do_update_method = env
        .find_method(&helper_class, do_update_method_name)
        .unwrap();

    let ctor_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"l:\0") };
    let predicates_class = env.find_class(DATA_SHARE_PREDICATES).unwrap();
    let arg1 = env.serialize(&uri).unwrap();
    let arg2 = env
        .new_object_with_signature(&predicates_class, ctor_signature, (predicates_ptr,))
        .unwrap();
    let arg3 = env.serialize(value_bucket.as_ref()).unwrap();
    env.call_method(&instance, &do_update_method, (arg1, arg2, arg3))
        .unwrap();
}

pub fn call_arkts_delete(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    predicates_ptr: i64,
    native_ptr: i64,
) {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER).unwrap();
    let instance = env
        .new_object(&helper_class, (extension_ability_obj, native_ptr))
        .unwrap();
    let do_delete_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"doDelete\0") };
    let do_delete_method = env
        .find_method(&helper_class, do_delete_method_name)
        .unwrap();

    let arg1 = env.serialize(&uri).unwrap();
    let ctor_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"l:\0") };
    let predicates_class = env.find_class(DATA_SHARE_PREDICATES).unwrap();
    let arg2 = env
        .new_object_with_signature(&predicates_class, ctor_signature, (predicates_ptr,))
        .unwrap();

    env.call_method(&instance, &do_delete_method, (arg1, arg2))
        .unwrap();
}

pub fn call_arkts_query(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    predicates_ptr: i64,
    columns: Vec<String>,
    native_ptr: i64,
) {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER).unwrap();
    let instance = env
        .new_object(&helper_class, (extension_ability_obj, native_ptr))
        .unwrap();
    let do_query_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"doQuery\0") };
    let do_query_method = env
        .find_method(&helper_class, do_query_method_name)
        .unwrap();

    let arg1 = env.serialize(&uri).unwrap();
    let ctor_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"l:\0") };
    let predicates_class = env.find_class(DATA_SHARE_PREDICATES).unwrap();
    let arg2 = env
        .new_object_with_signature(&predicates_class, ctor_signature, (predicates_ptr,))
        .unwrap();
    let arg3 = env.serialize(&columns).unwrap();
    env.call_method(&instance, &do_query_method, (arg1, arg2, arg3))
        .unwrap();
}

pub fn call_arkts_on_create(extension_ability_ptr: i64, env_ptr: i64, ani_want: i64, native_ptr: i64) {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER).unwrap();
    let instance = env
        .new_object(&helper_class, (extension_ability_obj, native_ptr))
        .unwrap();
    let do_on_create_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"doOnCreate\0") };
    let do_on_create_method = env
        .find_method(&helper_class, do_on_create_method_name)
        .unwrap();

    let want_object = AniObject::from_raw(ani_want as _);

    env.call_method(&instance, &do_on_create_method, (want_object,))
        .unwrap();
}

#[ani_rs::native]
pub fn native_extension_callback_int(error_code: f64, error_msg: String, data: i32, native_ptr: i64) -> Result<(), BusinessError> {
    wrapper::ffi::DataShareNativeExtensionCallbackInt(error_code, error_msg, data, native_ptr);
    Ok(())
}

pub fn native_extension_callback_object<'local>(
    env: AniEnv<'local>,
    error_code: f64,
    error_msg: String,
    data: AniObject,
    native_ptr: i64,
) {
    let object_ptr = get_native_ptr(&env, &data);
    wrapper::ffi::DataShareNativeExtensionCallbackObject(error_code, error_msg, object_ptr, native_ptr);
}

#[ani_rs::native]
pub fn native_extension_callback_void(error_code: f64, error_msg: String, native_ptr: i64) -> Result<(), BusinessError> {
    wrapper::ffi::DataShareNativeExtensionCallbackVoid(error_code, error_msg, native_ptr);
    Ok(())
}
