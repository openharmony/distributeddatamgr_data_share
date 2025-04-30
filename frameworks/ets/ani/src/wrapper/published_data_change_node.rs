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

use ani_rs::{callback::Callback, objects::AniFnObject, AniEnv};

use crate::datashare::{PublishedDataChangeNode, PublishedItem, PublishedItemData};

pub fn rust_create_published_data_change_node(
    bundle_name: String,
) -> Box<PublishedDataChangeNode<'static>> {
    let node = PublishedDataChangeNode::new(bundle_name);
    Box::new(node)
}

pub fn published_data_change_node_push_item_str(
    node: &mut PublishedDataChangeNode,
    key: String,
    data: String,
    subscriber_id: String,
) {
    let item = PublishedItem::new(key, PublishedItemData::S(data), subscriber_id);
    node.push_data(item);
}

pub fn published_data_change_node_push_item_arraybuffer<'a>(
    node: &mut PublishedDataChangeNode<'a>,
    key: String,
    data: &'a [u8],
    subscriber_id: String,
) {
    let item = PublishedItem::new(key, PublishedItemData::ArrayBuffer(data), subscriber_id);
    node.push_data(item);
}

pub fn execute_callback_published_data_change(
    callback_ptr: i64,
    env_ptr: i64,
    node: &PublishedDataChangeNode,
) {
    let env = AniEnv::from_raw(env_ptr as _);

    let callback = AniFnObject::from_raw(callback_ptr as _);
    let callback = Callback::new(callback);
    callback.execute_local(env, (node,)).unwrap();
}
