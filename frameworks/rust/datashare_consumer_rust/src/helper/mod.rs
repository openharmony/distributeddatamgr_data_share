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

//! Helper module for DataShare consumer.
//!
//! Contains:
//! - `DataShareHelper` trait — abstract factory/interface for DataShare operations
//! - `DataShareHelperImpl` — concrete implementation routing to controllers
//! - `AbilityMgrProxy` — ability manager service proxy
//! - `DataProxyHandle` — proxy data operations handle

pub mod ability_mgr_proxy;
pub mod dataproxy_handle;
pub mod datashare_helper;
pub mod datashare_helper_impl;

pub use ability_mgr_proxy::AbilityMgrProxy;
pub use dataproxy_handle::DataProxyHandle;
pub use datashare_helper::DataShareHelper;
pub use datashare_helper_impl::DataShareHelperImpl;
