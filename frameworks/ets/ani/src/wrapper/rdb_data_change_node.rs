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

use ani_rs::{objects::AniFnObject, AniEnv};

use crate::datashare::{RdbDataChangeNode, TemplateId};

pub fn template_id_get_subscriber_id(template_id: &TemplateId) -> &String {
    &template_id.subscriber_id
}

pub fn template_id_get_bundle_name_of_owner(template_id: &TemplateId) -> &String {
    &template_id.bundle_name_of_owner
}

pub fn rust_create_rdb_data_change_node(
    uri: String,
    subscriber_id: String,
    bundle_name_of_owner: String,
) -> Box<RdbDataChangeNode> {
    let template_id = TemplateId::new(subscriber_id, bundle_name_of_owner);
    let node = RdbDataChangeNode::new(uri, template_id);
    Box::new(node)
}

pub fn rdb_data_change_node_push_data(node: &mut RdbDataChangeNode, data_item: String) {
    node.push_data(data_item);
}
