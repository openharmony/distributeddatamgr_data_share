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

use ani_rs::typed_array::ArrayBuffer;

use crate::datashare::{OperationResult, PublishedItem, PublishedItemData, AniDataProxyResult,
    AniDataProxyGetResult, DataProxyErrorCode};
use crate::predicates::ValueType;

use super::ffi;

// called by c++, used to accept c++ return value
pub struct PublishSretParam(Vec<OperationResult>);
impl PublishSretParam {
    pub fn new() -> Self {
        Self(Vec::new())
    }

    pub fn into_inner(self) -> Vec<OperationResult> {
        self.0
    }
}

// called by c++, Call this method on the C++ side to fill data into the Rust structure
pub fn publish_sret_push(sret: &mut PublishSretParam, key: String, result: i32) {
    sret.0.push(OperationResult::new(key, result));
}

// called by c++, get PublishedItem key.
pub fn published_item_get_key(item: &PublishedItem) -> String {
    item.key.clone()
}

// called by c++, get PublishedItem subscriber_id.
pub fn published_item_get_subscriber_id(item: &PublishedItem) -> String {
    item.subscriber_id.clone()
}

// called by c++, get PublishedItem data's type.
pub fn published_item_get_data_type(item: &PublishedItem) -> ffi::EnumType {
    match &item.data {
        PublishedItemData::S(_) => ffi::EnumType::StringType,
        PublishedItemData::ArrayBuffer(_) => ffi::EnumType::ArrayBufferType,
    }
}

// called by c++, if PublishedItem data's type is String, get String.
pub fn published_item_get_data_string(item: &PublishedItem) -> String {
    if let PublishedItemData::S(s) = &item.data {
        return s.clone();
    }

    panic!("Not String Type!!!");
}

// called by c++, if PublishedItem data's type is ArrayBuffer, get ArrayBuffer.
pub fn published_item_get_data_arraybuffer(item: &PublishedItem) -> Vec<u8> {
    if let PublishedItemData::ArrayBuffer(arr) = &item.data {
        return arr.to_vec();
    }

    panic!("Not arraybuffer Type!!!");
}

pub struct GetPublishedDataSretParamHelper {
    pub key: String,
    pub subscriber_id: String,
    pub is_data_str: bool,
    pub data_str: Option<String>,
    pub data_arr: Option<Vec<u8>>,
}

// called by c++, used to accept c++ return value
pub struct GetPublishedDataSretParam(Vec<GetPublishedDataSretParamHelper>);

impl GetPublishedDataSretParam {
    pub fn new() -> Self {
        Self(Vec::new())
    }

    pub fn transform_to_item(&self) -> Vec<PublishedItem> {
        let res: Vec<PublishedItem> = self
            .0
            .iter()
            .map(|help| {
                let v = if help.is_data_str {
                    PublishedItem {
                        key: help.key.clone(),
                        data: PublishedItemData::S(help.data_str.clone().unwrap()),
                        subscriber_id: help.subscriber_id.clone(),
                    }
                } else {
                    PublishedItem {
                        key: help.key.clone(),
                        data: PublishedItemData::ArrayBuffer(
                            ArrayBuffer::new_with_vec(help.data_arr.as_ref().unwrap().clone()),
                        ),
                        subscriber_id: help.subscriber_id.clone(),
                    }
                };

                v
            })
            .collect();
        res
    }
}

// called by c++, Call this method on the C++ side to fill data into the Rust structure
pub fn published_data_sret_push_str(
    sret: &mut GetPublishedDataSretParam,
    key: String,
    subscriber_id: String,
    data_str: String,
) {
    let item = GetPublishedDataSretParamHelper {
        key,
        subscriber_id,
        is_data_str: true,
        data_str: Some(data_str),
        data_arr: None,
    };
    sret.0.push(item);
}

// called by c++, Call this method on the C++ side to fill data into the Rust structure
pub fn published_data_sret_push_array(
    sret: &mut GetPublishedDataSretParam,
    key: String,
    subscriber_id: String,
    data_buffer: Vec<u8>,
) {
    let item = GetPublishedDataSretParamHelper {
        key,
        subscriber_id,
        is_data_str: false,
        data_str: None,
        data_arr: Some(data_buffer),
    };
    sret.0.push(item);
}

// called by c++, used to accept c++ return value
pub struct AniDataProxyResultSetParam(Vec<AniDataProxyResult>);
impl AniDataProxyResultSetParam {
    pub fn new() -> Self {
        Self(Vec::new())
    }

    pub fn into_inner(self) -> Vec<AniDataProxyResult> {
        self.0
    }
}
// called by c++, Call this method on the C++ side to fill data into the Rust structure
pub fn data_proxy_result_set_push(set: &mut AniDataProxyResultSetParam, uri: String, result: i32) {
    let mut adpgr = AniDataProxyResult::new();
    adpgr.uri = uri;
    adpgr.result = DataProxyErrorCode::from_i32(result);
    set.0.push(adpgr);
}

pub struct AniDataProxyGetResultSetParam(Vec<AniDataProxyGetResult>);
impl AniDataProxyGetResultSetParam {
    pub fn new() -> Self {
        Self(Vec::new())
    }

    pub fn into_inner(self) -> Vec<AniDataProxyGetResult> {
        self.0
    }
}

// called by c++, Call this method on the C++ side to fill data into the Rust structure
pub fn data_proxy_get_result_set_push_i64(set: &mut AniDataProxyGetResultSetParam, uri: String, result: i32,
        value: i64, allowList: Vec<String>) {
    let mut adpgr = AniDataProxyGetResult::new();
    adpgr.uri = uri;
    adpgr.result = DataProxyErrorCode::from_i32(result);
    adpgr.value = Some(ValueType::I64(value));
    if result == 0 {
        adpgr.allowList = Some(allowList);
    } else {
        adpgr.allowList = None;
    }
    set.0.push(adpgr);
}

pub fn data_proxy_get_result_set_push_f64(set: &mut AniDataProxyGetResultSetParam, uri: String, result: i32,
        value: f64, allowList: Vec<String>) {
    let mut adpgr = AniDataProxyGetResult::new();
    adpgr.uri = uri;
    adpgr.result = DataProxyErrorCode::from_i32(result);
    adpgr.value = Some(ValueType::F64(value));
    if result == 0 {
        adpgr.allowList = Some(allowList);
    } else {
        adpgr.allowList = None;
    }
    set.0.push(adpgr);
}

pub fn data_proxy_get_result_set_push_bool(set: &mut AniDataProxyGetResultSetParam, uri: String, result: i32,
        value: bool, allowList: Vec<String>) {
    let mut adpgr = AniDataProxyGetResult::new();
    adpgr.uri = uri;
    adpgr.result = DataProxyErrorCode::from_i32(result);
    adpgr.value = Some(ValueType::Boolean(value));
    if result == 0 {
        adpgr.allowList = Some(allowList);
    } else {
        adpgr.allowList = None;
    }
    set.0.push(adpgr);
}

pub fn data_proxy_get_result_set_push_string(set: &mut AniDataProxyGetResultSetParam, uri: String, result: i32,
        value: String, allowList: Vec<String>) {
    let mut adpgr = AniDataProxyGetResult::new();
    adpgr.uri = uri;
    adpgr.result = DataProxyErrorCode::from_i32(result);
    if result == 0 {
        adpgr.value = Some(ValueType::S(value));
        adpgr.allowList = Some(allowList);
    } else {
        adpgr.value = None;
        adpgr.allowList = None;
    }
    set.0.push(adpgr);
}
