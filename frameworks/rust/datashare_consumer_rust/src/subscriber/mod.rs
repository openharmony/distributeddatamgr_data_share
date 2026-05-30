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

//! Subscriber management module for DataShare consumer.
//!
//! Contains:
//! - Observer stubs (RDB, Published, Proxy) for IPC callbacks
//! - Subscription managers for RDB, Published, and Proxy data

pub mod observer_stub;
pub mod proxy_data_subscriber_manager;
pub mod published_data_subscriber_manager;
pub mod rdb_subscriber_manager;

pub use observer_stub::{ProxyDataObserverStub, PublishedDataObserverStub, RdbObserverStub};
pub use proxy_data_subscriber_manager::ProxyDataSubscriberManager;
pub use published_data_subscriber_manager::PublishedDataSubscriberManager;
pub use rdb_subscriber_manager::RdbSubscriberManager;
