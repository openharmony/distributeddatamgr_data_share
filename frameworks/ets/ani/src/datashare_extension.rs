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

use ani_rs::{objects::AniObject, AniEnv};

use crate::{
    wrapper::{ValuesBucketArrayWrap, ValuesBucketHashWrap},
    DATA_SHARE_EXTENSION, DATA_SHARE_PREDICATES,
};

pub fn call_arkts_insert(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    value_bucket: &ValuesBucketHashWrap,
) {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"insert\0") };
    let class = env.find_class(DATA_SHARE_EXTENSION).unwrap();
    let method = env.find_method(&class, method_name).unwrap();

    let arg1 = env.serialize(&uri).unwrap();
    let arg2 = env.serialize(value_bucket.as_ref()).unwrap();
    env.call_method(&extension_ability_obj, &method, (arg1, arg2))
        .unwrap();
}

pub fn call_arkts_batch_insert(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    value_buckets: &ValuesBucketArrayWrap,
) {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"batchInsert\0") };
    let class = env.find_class(DATA_SHARE_EXTENSION).unwrap();
    let method = env.find_method(&class, method_name).unwrap();

    let arg1 = env.serialize(&uri).unwrap();
    let arg2 = env.serialize(value_buckets.as_ref()).unwrap();

    env.call_method(&extension_ability_obj, &method, (arg1, arg2))
        .unwrap();
}

pub fn call_arkts_update(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    predicates_ptr: i64,
    value_bucket: &ValuesBucketHashWrap,
) {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"update\0") };
    let class = env.find_class(DATA_SHARE_EXTENSION).unwrap();
    let method = env.find_method(&class, method_name).unwrap();

    let ctor_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"J:V\0") };
    let predicates_class = env.find_class(DATA_SHARE_PREDICATES).unwrap();
    let arg1 = env.serialize(&uri).unwrap();
    let arg2 = env
        .new_object_with_signature(&predicates_class, ctor_signature, (predicates_ptr,))
        .unwrap();
    let arg3 = env.serialize(value_bucket.as_ref()).unwrap();
    env.call_method(&extension_ability_obj, &method, (arg1, arg2, arg3))
        .unwrap();
}

pub fn call_arkts_delete(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    predicates_ptr: i64,
) {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"delete\0") };
    let extension_class = env.find_class(DATA_SHARE_EXTENSION).unwrap();
    let method = env.find_method(&extension_class, method_name).unwrap();

    let arg1 = env.serialize(&uri).unwrap();
    let ctor_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"J:V\0") };
    let predicates_class = env.find_class(DATA_SHARE_PREDICATES).unwrap();
    let arg2 = env
        .new_object_with_signature(&predicates_class, ctor_signature, (predicates_ptr,))
        .unwrap();

    env.call_method(&extension_ability_obj, &method, (arg1, arg2))
        .unwrap();
}

pub fn call_arkts_query(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    predicates_ptr: i64,
    columns: Vec<String>,
) {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"query\0") };
    let class = env.find_class(DATA_SHARE_EXTENSION).unwrap();
    let method = env.find_method(&class, method_name).unwrap();

    let arg1 = env.serialize(&uri).unwrap();
    let ctor_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"J:V\0") };
    let predicates_class = env.find_class(DATA_SHARE_PREDICATES).unwrap();
    let arg2 = env
        .new_object_with_signature(&predicates_class, ctor_signature, (predicates_ptr,))
        .unwrap();
    let arg3 = env.serialize(&columns).unwrap();
    env.call_method(&extension_ability_obj, &method, (arg1, arg2, arg3))
        .unwrap();
}

pub fn call_arkts_on_create(extension_ability_ptr: i64, env_ptr: i64, ani_want: i64) {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"onCreate\0") };
    let class = env.find_class(DATA_SHARE_EXTENSION).unwrap();
    let method = env.find_method(&class, method_name).unwrap();

    let want_object = AniObject::from_raw(ani_want as _);

    env.call_method(&extension_ability_obj, &method, (want_object,))
        .unwrap();
}
