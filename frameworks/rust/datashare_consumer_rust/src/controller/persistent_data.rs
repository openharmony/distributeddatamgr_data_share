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

//! PersistentDataController — RDB template subscription controller.
//!
//! Corresponds to C++ `PersistentDataController` in
//! `frameworks/native/consumer/controller/service/src/persistent_data_controller.cpp` (92 lines).
//!
//! Data flow: App → DataShareHelper → PersistentDataController
//!     → DataShareServiceProxy → IPC → DataShareService
//!     → RdbSubscriberManager for subscription operations
//!
//! Features:
//! - Add/delete query templates for RDB data subscriptions
//! - Subscribe/unsubscribe to RDB data changes
//! - Enable/disable RDB data subscriptions

use std::sync::Arc;

use datashare_common::template::{Template, TemplateId};
use datashare_common::types::{OperationResult, RdbChangeNode};

use crate::connection::DataShareManagerImpl;
use crate::subscriber::rdb_subscriber_manager::RdbSubscriberManager;

const INVALID_VALUE: i32 = -1;

/// PersistentDataController — controller for RDB template subscriptions.
///
/// Corresponds to C++ class `PersistentDataController`.
///
/// Uses `DataShareManagerImpl::GetServiceProxy()` for template management
/// and `RdbSubscriberManager::GetInstance()` for observer management.
pub struct PersistentDataController;

impl PersistentDataController {
    /// Create a new persistent data controller.
    pub fn new() -> Self {
        Self
    }

    /// Add a query template for RDB data subscription.
    ///
    /// Corresponds to C++ `AddQueryTemplate(const std::string& uri, int64_t subscriberId, Template& tpl)`.
    pub fn add_query_template(&self, uri: &str, subscriber_id: i64, tpl: &Template) -> i32 {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return INVALID_VALUE,
        };
        proxy.add_query_template(uri, subscriber_id, tpl)
    }

    /// Delete a query template for RDB data subscription.
    ///
    /// Corresponds to C++ `DelQueryTemplate(const std::string& uri, int64_t subscriberId)`.
    pub fn del_query_template(&self, uri: &str, subscriber_id: i64) -> i32 {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return INVALID_VALUE,
        };
        proxy.del_query_template(uri, subscriber_id)
    }

    /// Subscribe to RDB data changes.
    ///
    /// Corresponds to C++ `SubscribeRdbData(...)`.
    ///
    /// Uses RdbSubscriberManager::AddObservers to register the observer with the service.
    pub fn subscribe_rdb_data(
        &self,
        subscriber: u64,
        uris: &[String],
        template_id: &TemplateId,
        callback: Box<dyn Fn(&RdbChangeNode) + Send + Sync>,
    ) -> Vec<OperationResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        RdbSubscriberManager::get_instance().add_observers(
            subscriber,
            &proxy,
            uris,
            template_id,
            Arc::from(callback),
        )
    }

    /// Unsubscribe from RDB data changes.
    ///
    /// Corresponds to C++ `UnSubscribeRdbData(...)`.
    ///
    /// If uris is empty, removes all observers for the subscriber.
    pub fn unsubscribe_rdb_data(
        &self,
        subscriber: u64,
        uris: &[String],
        template_id: &TemplateId,
    ) -> Vec<OperationResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        if uris.is_empty() {
            return RdbSubscriberManager::get_instance().del_all_observers(subscriber, &proxy);
        }
        RdbSubscriberManager::get_instance().del_observers(subscriber, &proxy, uris, template_id)
    }

    /// Enable RDB data subscription.
    ///
    /// Corresponds to C++ `EnableSubscribeRdbData(...)`.
    pub fn enable_subscribe_rdb_data(
        &self,
        subscriber: u64,
        uris: &[String],
        template_id: &TemplateId,
    ) -> Vec<OperationResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        RdbSubscriberManager::get_instance().enable_observers(subscriber, &proxy, uris, template_id)
    }

    /// Disable RDB data subscription.
    ///
    /// Corresponds to C++ `DisableSubscribeRdbData(...)`.
    pub fn disable_subscribe_rdb_data(
        &self,
        subscriber: u64,
        uris: &[String],
        template_id: &TemplateId,
    ) -> Vec<OperationResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        RdbSubscriberManager::get_instance().disable_observers(
            subscriber,
            &proxy,
            uris,
            template_id,
        )
    }
}

impl Default for PersistentDataController {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_persistent_data_controller_construction() {
        let _controller = PersistentDataController::new();
    }

    #[test]
    fn test_add_query_template_placeholder() {
        let controller = PersistentDataController::new();
        let tpl = Template::default();
        let result = controller.add_query_template("datashare:///test", 1, &tpl);
        assert_eq!(result, INVALID_VALUE);
    }

    #[test]
    fn test_del_query_template_placeholder() {
        let controller = PersistentDataController::new();
        let result = controller.del_query_template("datashare:///test", 1);
        assert_eq!(result, INVALID_VALUE);
    }

    #[test]
    fn test_subscribe_rdb_data_placeholder() {
        let controller = PersistentDataController::new();
        let template_id = TemplateId::new(0, String::new());
        let result = controller.subscribe_rdb_data(
            1,
            &["datashare:///test".to_string()],
            &template_id,
            Box::new(|_| {}),
        );
        assert!(result.is_empty());
    }

    #[test]
    fn test_unsubscribe_rdb_data_placeholder() {
        let controller = PersistentDataController::new();
        let template_id = TemplateId::new(0, String::new());
        let result = controller.unsubscribe_rdb_data(1, &[], &template_id);
        assert!(result.is_empty());
    }
}
