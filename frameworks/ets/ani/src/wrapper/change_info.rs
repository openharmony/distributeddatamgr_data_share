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

use std::collections::HashMap;

use ani_rs::{objects::AniFnObject, typed_array::Uint8Array, AniEnv};

use crate::datashare::{BucketValue, ChangeInfo, ChangeType};

pub fn rust_create_change_info(change_index: i32, uri: String) -> Box<ChangeInfo<'static>> {
    let change_info = ChangeInfo::new(ChangeType::from_i32(change_index), uri);
    Box::new(change_info)
}

fn change_info_push_kv<'a>(
    change_info: &mut ChangeInfo<'a>,
    key: String,
    value: BucketValue<'a>,
    new_hashmap: bool,
) {
    if new_hashmap || change_info.values.is_empty() {
        let mut hm = HashMap::new();
        hm.insert(key, value);
        change_info.values.push(hm);
    } else {
        let hm = change_info.values.last_mut().unwrap();
        hm.insert(key, value);
    }
}

pub fn change_info_push_kv_str(
    change_info: &mut ChangeInfo,
    key: String,
    value: String,
    new_hashmap: bool,
) {
    let value = BucketValue::S(value);
    change_info_push_kv(change_info, key, value, new_hashmap);
}

pub fn change_info_push_kv_f64(
    change_info: &mut ChangeInfo,
    key: String,
    value: f64,
    new_hashmap: bool,
) {
    let value = BucketValue::F64(value);
    change_info_push_kv(change_info, key, value, new_hashmap);
}

pub fn change_info_push_kv_boolean(
    change_info: &mut ChangeInfo,
    key: String,
    value: bool,
    new_hashmap: bool,
) {
    let value = BucketValue::Boolean(value);
    change_info_push_kv(change_info, key, value, new_hashmap);
}

pub fn change_info_push_kv_uint8array<'a>(
    change_info: &mut ChangeInfo<'a>,
    key: String,
    value: &'a [u8],
    new_hashmap: bool,
) {
    let arr = Uint8Array::new(value);
    let value = BucketValue::Uint8Array(arr);
    change_info_push_kv(change_info, key, value, new_hashmap);
}

pub fn change_info_push_kv_null(change_info: &mut ChangeInfo, key: String, new_hashmap: bool) {
    let value = BucketValue::Null(());
    change_info_push_kv(change_info, key, value, new_hashmap);
}

pub fn execute_callback_changeinfo(callback_ptr: i64, env_ptr: i64, change_info: &ChangeInfo) {
    let env = AniEnv::from_raw(env_ptr as _);
    let callback = AniFnObject::from_raw(callback_ptr as _);
    callback.execute_local(&env, (change_info,)).unwrap();
}

pub fn execute_callback(callback_ptr: i64, env_ptr: i64) {
    let env = AniEnv::from_raw(env_ptr as _);

    let callback = AniFnObject::from_raw(callback_ptr as _);

    callback.execute_local(&env, ((),)).unwrap();
}
