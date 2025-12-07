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

use std::collections::HashMap;
use std::ffi::CStr;

use ani_rs::{objects::AniObject, AniEnv};

use crate::{
    wrapper::{ValuesBucketHashWrap, ValuesBucketKvItem, ValuesBucketWrap},
    datashare::UpdateOperation, datashare::UpdateOperationPredicates,
    DATA_SHARE_PREDICATES,
};

use super::ffi;

pub struct DataShareBatchUpdateParamIn {
    pub operations: HashMap<String, Vec<UpdateOperation>>,
}

impl DataShareBatchUpdateParamIn {
    pub fn new() -> Self {
        Self {
            operations: HashMap::new(),
        }
    }

    pub fn as_ref(&self) -> &HashMap<String, Vec<UpdateOperation>> {
        &self.operations
    }
}

pub fn data_share_batch_update_param_in_get_value(
    param_in: &DataShareBatchUpdateParamIn,
    vec_key: &mut Vec<String>,
    vec_predicates: &mut Vec<i64>,
    vec_bucket: &mut Vec<ValuesBucketWrap>,
    vec_step: &mut Vec<i64>,
) {
    for (key, ops) in &param_in.operations {
        vec_key.push(key.clone());
        vec_step.push(ops.len() as i64);
        for op in ops {
            vec_predicates.push(op.predicates.native_ptr);
            let res = op.values
                .iter()
                .map(|(k, v)| ValuesBucketKvItem::new(k.clone(), v.clone()))
                .collect();
            vec_bucket.push(ValuesBucketWrap(res));
        }
    }
}

pub struct DataShareBatchUpdateParamOut {
    pub results: HashMap<String, Vec<i32>>,
    pub error_code: i32,
}

impl DataShareBatchUpdateParamOut {
    pub fn new() -> Self {
        Self {
            error_code: 0,
            results: HashMap::new(),
        }
    }

    pub fn as_ref(&self) -> &HashMap<String, Vec<i32>> {
        &self.results
    }
}

pub fn data_share_batch_update_param_out_push(
    param_out: &mut DataShareBatchUpdateParamOut,
    key: String,
    result: Vec<i32>,
) {
    param_out.results.insert(key, result);
}

pub fn data_share_batch_update_param_out_error_code(
    param_out: &mut DataShareBatchUpdateParamOut,
    error_code: i32,
) {
    param_out.error_code = error_code;
}

pub struct ExtensionBatchUpdateParamIn {
    pub data: HashMap<String, Vec<i32>>,
}

impl ExtensionBatchUpdateParamIn {
    pub fn new() -> Self {
        Self {
            data: HashMap::new(),
        }
    }

    pub fn as_ref(&self) -> &HashMap<String, Vec<i32>> {
        &self.data
    }
}

pub fn extension_batch_update_param_in_get_value(
    param_in: &ExtensionBatchUpdateParamIn,
    vec_key: &mut Vec<String>,
    vec_value: &mut Vec<i32>,
    vec_steps: &mut Vec<i32>,
) {
    for (key, value) in &param_in.data {
        vec_key.push(key.clone());
        vec_steps.push(value.len() as i32);
        for v in value {
            vec_value.push(v.clone());
        }
    }
}

pub struct ExtensionBatchUpdateParamOut {
    pub operations: HashMap<String, Vec<UpdateOperationPredicates<'static>>>,
    pub vec_bucket: Vec<ValuesBucketHashWrap>,
}

impl ExtensionBatchUpdateParamOut {
    pub fn new() -> Self {
        Self {
            operations: HashMap::new(),
            vec_bucket: Vec::new(),
        }
    }

    pub fn as_ref(&self) -> &HashMap<String, Vec<UpdateOperationPredicates>> {
        &self.operations
    }
}

pub fn rust_create_extension_batch_update_param_out() -> Box<ExtensionBatchUpdateParamOut> {
    Box::new(ExtensionBatchUpdateParamOut::new())
}

pub fn extension_batch_update_param_out_set_bucket(
    param_out: &mut ExtensionBatchUpdateParamOut,
    bucket: &ValuesBucketHashWrap,
) {
    param_out.vec_bucket.push(bucket.clone());
}

pub fn extension_batch_update_param_out_set_value(
    param_out: &mut ExtensionBatchUpdateParamOut,
    env_ptr: i64,
    vec_key: Vec<String>,
    vec_predicates: Vec<i64>,
    vec_steps: Vec<i64>,
) {
    let env = AniEnv::from_raw(env_ptr as _);
    let ctor_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"l:\0") };
    let predicates_class = env.find_class(DATA_SHARE_PREDICATES).unwrap();
    let mut sum: usize = 0;
    for i in 0..vec_key.len() {
        let mut ops: Vec<UpdateOperationPredicates> = Vec::new();
        for j in 0..(vec_steps[i] as usize) {
            let predicates = env
                .new_object_with_signature(&predicates_class, ctor_signature, (vec_predicates[sum + j],))
                .unwrap();
            let mut op = UpdateOperationPredicates::new(predicates);
            op.values = param_out.vec_bucket[sum + j].value_bucket.clone();
            ops.push(op);
        }
        sum += vec_steps[i] as usize;
        param_out.operations.insert(vec_key[i].clone(), ops);
    }
}