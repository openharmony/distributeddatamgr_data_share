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

//! PublishedDataController — publish/subscribe data controller.
//!
//! Corresponds to C++ `PublishedDataController` in
//! `frameworks/native/consumer/controller/service/src/published_data_controller.cpp` (90 lines).
//!
//! Data flow: App → DataShareHelper → PublishedDataController
//!     → DataShareServiceProxy → IPC → DataShareService
//!     → PublishedDataSubscriberManager for subscription operations
//!
//! Features:
//! - Publish data to DataShare service
//! - Get published data by bundle name
//! - Subscribe/unsubscribe to published data changes
//! - Enable/disable published data subscriptions

use std::sync::Arc;

use datashare_common::types::{Data, OperationResult, PublishedDataChangeNode};

use crate::connection::DataShareManagerImpl;
use crate::subscriber::published_data_subscriber_manager::PublishedDataSubscriberManager;

/// PublishedDataController — controller for publish/subscribe data operations.
///
/// Corresponds to C++ class `PublishedDataController`.
///
/// Uses `DataShareManagerImpl::GetServiceProxy()` for publish/get operations
/// and `PublishedDataSubscriberManager::GetInstance()` for observer management.
pub struct PublishedDataController;

impl PublishedDataController {
    /// Create a new published data controller.
    pub fn new() -> Self {
        Self
    }

    /// Publish data to DataShare service.
    ///
    /// Corresponds to C++ `Publish(const Data& data, const std::string& bundleName)`.
    pub fn publish(&self, data: &Data, bundle_name: &str) -> Vec<OperationResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        proxy.publish(data, bundle_name)
    }

    /// Get published data by bundle name.
    ///
    /// Corresponds to C++ `GetPublishedData(const std::string& bundleName, int& resultCode)`.
    pub fn get_published_data(&self, bundle_name: &str) -> (Data, i32) {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return (Data::default(), -1),
        };
        proxy.get_published_data(bundle_name)
    }

    /// Subscribe to published data changes.
    pub fn subscribe_published_data(
        &self,
        subscriber: u64,
        uris: &[String],
        subscriber_id: i64,
        callback: Box<dyn Fn(&PublishedDataChangeNode) + Send + Sync>,
    ) -> Vec<OperationResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        PublishedDataSubscriberManager::get_instance().add_observers(
            subscriber,
            &proxy,
            uris,
            subscriber_id,
            Arc::from(callback),
        )
    }

    /// Unsubscribe from published data changes.
    ///
    /// If uris is empty, removes all observers for the subscriber.
    pub fn unsubscribe_published_data(
        &self,
        subscriber: u64,
        uris: &[String],
        subscriber_id: i64,
    ) -> Vec<OperationResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        if uris.is_empty() {
            return PublishedDataSubscriberManager::get_instance()
                .del_all_observers(subscriber, &proxy);
        }
        PublishedDataSubscriberManager::get_instance().del_observers(
            subscriber,
            &proxy,
            uris,
            subscriber_id,
        )
    }

    /// Enable published data subscription.
    pub fn enable_subscribe_published_data(
        &self,
        subscriber: u64,
        uris: &[String],
        subscriber_id: i64,
    ) -> Vec<OperationResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        PublishedDataSubscriberManager::get_instance().enable_observers(
            subscriber,
            &proxy,
            uris,
            subscriber_id,
        )
    }

    /// Disable published data subscription.
    pub fn disable_subscribe_published_data(
        &self,
        subscriber: u64,
        uris: &[String],
        subscriber_id: i64,
    ) -> Vec<OperationResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        PublishedDataSubscriberManager::get_instance().disable_observers(
            subscriber,
            &proxy,
            uris,
            subscriber_id,
        )
    }
}

impl Default for PublishedDataController {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_published_data_controller_construction() {
        let _controller = PublishedDataController::new();
    }

    #[test]
    fn test_publish_placeholder() {
        let controller = PublishedDataController::new();
        let data = Data::default();
        let result = controller.publish(&data, "com.test.bundle");
        assert!(result.is_empty());
    }

    #[test]
    fn test_get_published_data_placeholder() {
        let controller = PublishedDataController::new();
        let (_data, code) = controller.get_published_data("com.test.bundle");
        assert_eq!(code, -1);
    }

    #[test]
    fn test_subscribe_published_data_placeholder() {
        let controller = PublishedDataController::new();
        let result = controller.subscribe_published_data(
            1,
            &["datashare:///test".to_string()],
            100,
            Box::new(|_| {}),
        );
        assert!(result.is_empty());
    }

    #[test]
    fn test_unsubscribe_published_data_placeholder() {
        let controller = PublishedDataController::new();
        let result = controller.unsubscribe_published_data(1, &[], 100);
        assert!(result.is_empty());
    }
}
