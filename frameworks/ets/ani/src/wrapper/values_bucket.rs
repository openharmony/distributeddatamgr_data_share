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
