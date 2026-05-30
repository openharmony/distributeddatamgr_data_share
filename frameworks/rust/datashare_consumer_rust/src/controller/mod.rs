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

//! Controller module for DataShare consumer
//!
//! Implements the controller pattern from C++:
//! - `GeneralController` trait (abstract base)
//! - `GeneralControllerServiceImpl` (silent/service path)
//! - `GeneralControllerProviderImpl` (non-silent/provider path)
//! - `ExtSpecialController` (extension special operations)
//! - `PersistentDataController` (RDB template subscription)
//! - `PublishedDataController` (publish/subscribe data)

pub mod ext_special;
pub mod general_controller;
pub mod persistent_data;
pub mod provider_impl;
pub mod published_data;
pub mod service_impl;

pub use ext_special::ExtSpecialController;
pub use general_controller::{
    DatashareBusinessError, GeneralController, DATA_SHARE_ERROR, DB_NOT_EXIST_ERR,
};
pub use persistent_data::PersistentDataController;
pub use provider_impl::GeneralControllerProviderImpl;
pub use published_data::PublishedDataController;
pub use service_impl::{GeneralControllerServiceImpl, TimedQueryResult};
