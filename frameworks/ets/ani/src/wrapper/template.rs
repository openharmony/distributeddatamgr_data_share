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

use crate::datashare::Template;

pub fn template_get_scheduler(temp: &Template) -> String {
    temp.scheduler.clone()
}

pub fn template_get_update(temp: &Template) -> String {
    if let Some(s) = &temp.update {
        s.clone()
    } else {
        "".to_string()
    }
}

pub struct TemplatePredicatesKvItem {
    key: String,
    value: String,
}

pub fn template_get_predicates(temp: &Template) -> Vec<TemplatePredicatesKvItem> {
    let vec_predicates: Vec<TemplatePredicatesKvItem> = temp
        .predicates
        .iter()
        .map(|(k, v)| TemplatePredicatesKvItem {
            key: k.clone(),
            value: v.clone(),
        })
        .collect();
    vec_predicates
}

pub fn template_predicates_get_key(kv: &TemplatePredicatesKvItem) -> &String {
    &kv.key
}

pub fn template_predicates_get_value(kv: &TemplatePredicatesKvItem) -> &String {
    &kv.value
}
