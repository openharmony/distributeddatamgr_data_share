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

use ani_rs::typed_array::Uint8Array;

use crate::datashare::BucketValue;

use super::ffi;

pub struct ValuesBucketKvItem<'a> {
    key: String,
    value: BucketValue<'a>,
}

impl<'a> ValuesBucketKvItem<'a> {
    pub fn new(key: String, value: BucketValue<'a>) -> Self {
        Self { key, value }
    }
}

// called by c++, get ValuesBucketKvItem key
pub fn value_bucket_get_key(kv: &ValuesBucketKvItem) -> String {
    kv.key.clone()
}

// called by c++, get BucketValue type
pub fn value_bucket_get_vtype(kv: &ValuesBucketKvItem) -> ffi::EnumType {
    match &kv.value {
        BucketValue::S(_) => ffi::EnumType::StringType,
        BucketValue::F64(_) => ffi::EnumType::F64Type,
        BucketValue::Boolean(_) => ffi::EnumType::BooleanType,
        BucketValue::Uint8Array(_) => ffi::EnumType::Uint8ArrayType,
        BucketValue::Null(_) => ffi::EnumType::NullType,
    }
}

// called by c++, if BucketValue is String, get String.
pub fn value_bucket_get_string(kv: &ValuesBucketKvItem) -> String {
    if let BucketValue::S(s) = &kv.value {
        return s.clone();
    }

    panic!("Not String Type!!!");
}

// called by c++, if BucketValue is f64, get 64.
pub fn value_bucket_get_f64(kv: &ValuesBucketKvItem) -> f64 {
    if let BucketValue::F64(f) = &kv.value {
        return *f;
    }

    panic!("Not F64 Type!!!");
}

// called by c++, if BucketValue is bool, get bool.
pub fn value_bucket_get_bool(kv: &ValuesBucketKvItem) -> bool {
    if let BucketValue::Boolean(b) = &kv.value {
        return *b;
    }

    panic!("Not Boolean Type!!!");
}

// called by c++, if BucketValue is uint8array, get uint8array.
pub fn value_bucket_get_uint8array(kv: &ValuesBucketKvItem) -> Vec<u8> {
    if let BucketValue::Uint8Array(a) = &kv.value {
        return a.as_slice().to_vec();
    }

    panic!("Not Array Type!!!");
}

pub struct ValuesBucketWrap<'a>(pub Vec<ValuesBucketKvItem<'a>>);

pub fn values_bucket_wrap_inner<'a>(kv: &'a ValuesBucketWrap) -> &'a Vec<ValuesBucketKvItem<'a>> {
    &kv.0
}

pub struct ValuesBucketHashWrap<'a> {
    value_bucket: HashMap<String, BucketValue<'a>>,
}

impl<'a> ValuesBucketHashWrap<'a> {
    pub fn new() -> Self {
        Self {
            value_bucket: HashMap::new()
        }
    }

    pub fn as_ref(&self) -> &HashMap<String, BucketValue> {
        &self.value_bucket
    }
}

pub fn rust_create_values_bucket() -> Box<ValuesBucketHashWrap<'static>> {
    let value_bucket = ValuesBucketHashWrap::new();
    Box::new(value_bucket)
}

fn values_bucket_push_kv<'a>(
    value_bucket: &mut ValuesBucketHashWrap<'a>,
    key: String,
    value: BucketValue<'a>,
) {
    value_bucket.value_bucket.insert(key, value);
}

pub fn value_bucket_push_kv_str(
    value_bucket: &mut ValuesBucketHashWrap,
    key: String,
    value: String,
) {
    let value = BucketValue::S(value);
    values_bucket_push_kv(value_bucket, key, value);
}

pub fn value_bucket_push_kv_f64(
    value_bucket: &mut ValuesBucketHashWrap,
    key: String,
    value: f64,
) {
    let value = BucketValue::F64(value);
    values_bucket_push_kv(value_bucket, key, value);
}

pub fn value_bucket_push_kv_boolean(
    value_bucket: &mut ValuesBucketHashWrap,
    key: String,
    value: bool,
) {
    let value = BucketValue::Boolean(value);
    values_bucket_push_kv(value_bucket, key, value);
}

pub fn value_bucket_push_kv_uint8array<'a>(
    value_bucket: &mut ValuesBucketHashWrap<'a>,
    key: String,
    value: &'a [u8],
) {
    let arr = Uint8Array::new(value);
    let value = BucketValue::Uint8Array(arr);
    values_bucket_push_kv(value_bucket, key, value);
}

pub fn value_bucket_push_kv_null(value_bucket: &mut ValuesBucketHashWrap, key: String) {
    let value = BucketValue::Null(());
    values_bucket_push_kv(value_bucket, key, value);
}

pub struct ValuesBucketArrayWrap<'a> {
    value_buckets: Vec<HashMap<String, BucketValue<'a>>>,
}

impl<'a> ValuesBucketArrayWrap<'a> {
    pub fn new() -> Self {
        Self {
            value_buckets: Vec::new()
        }
    }

    pub fn as_ref(&self) -> &Vec<HashMap<String, BucketValue<'a>>> {
        &self.value_buckets
    }
}

pub fn rust_create_values_bucket_array() -> Box<ValuesBucketArrayWrap<'static>> {
    let value_buckets = ValuesBucketArrayWrap::new();
    Box::new(value_buckets)
}

fn values_bucket_array_push_kv<'a>(
    value_buckets: &mut ValuesBucketArrayWrap<'a>,
    key: String,
    value: BucketValue<'a>,
    new_hashmap: bool,
) {
    if new_hashmap || value_buckets.value_buckets.is_empty() {
        let mut hm = HashMap::new();
        hm.insert(key, value);
        value_buckets.value_buckets.push(hm);
    } else {
        let hm = value_buckets.value_buckets.last_mut().unwrap();
        hm.insert(key, value);
    }
}

pub fn values_bucket_array_push_kv_str(
    value_buckets: &mut ValuesBucketArrayWrap,
    key: String,
    value: String,
    new_hashmap: bool,
) {
    let value = BucketValue::S(value);
    values_bucket_array_push_kv(value_buckets, key, value, new_hashmap);
}

pub fn values_bucket_array_push_kv_f64(
    value_buckets: &mut ValuesBucketArrayWrap,
    key: String,
    value: f64,
    new_hashmap: bool,
) {
    let value = BucketValue::F64(value);
    values_bucket_array_push_kv(value_buckets, key, value, new_hashmap);
}

pub fn values_bucket_array_push_kv_boolean(
    value_buckets: &mut ValuesBucketArrayWrap,
    key: String,
    value: bool,
    new_hashmap: bool,
) {
    let value = BucketValue::Boolean(value);
    values_bucket_array_push_kv(value_buckets, key, value, new_hashmap);
}

pub fn values_bucket_array_push_kv_uint8array<'a>(
    value_buckets: &mut ValuesBucketArrayWrap<'a>,
    key: String,
    value: &'a [u8],
    new_hashmap: bool,
) {
    let arr = Uint8Array::new(value);
    let value = BucketValue::Uint8Array(arr);
    values_bucket_array_push_kv(value_buckets, key, value, new_hashmap);
}

pub fn values_bucket_array_push_kv_null(value_buckets: &mut ValuesBucketArrayWrap, key: String, new_hashmap: bool) {
    let value = BucketValue::Null(());
    values_bucket_array_push_kv(value_buckets, key, value, new_hashmap);
}