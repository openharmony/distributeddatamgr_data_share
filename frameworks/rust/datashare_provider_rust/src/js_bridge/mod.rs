/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//! JS bridge module — NAPI bindings for DataShare provider.

pub mod js_ext_ability;
pub mod js_ext_ability_context;
pub mod uv_queue;

pub use js_ext_ability::JsDataShareExtAbility;
pub use js_ext_ability_context::JsDataShareExtAbilityContext;
pub use uv_queue::DataShareUvQueue;
