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

//! DataProxyHandle — proxy data operations handle.
//!
//! Corresponds to C++ `DataProxyHandle` in
//! `frameworks/native/consumer/src/dataproxy_handle.cpp` (102 lines).
//!
//! Provides a simplified interface for proxy data operations
//! (publish, delete, get, subscribe) using DataShareManagerImpl.

use std::sync::Arc;

use datashare_common::types::{
    DataProxyConfig, DataProxyGetResult, DataProxyResult, DataShareProxyData,
};

use crate::connection::DataShareManagerImpl;
use crate::subscriber::proxy_data_subscriber_manager::ProxyDataSubscriberManager;

/// DataProxyHandle — simplified interface for proxy data operations.
///
/// Corresponds to C++ `DataProxyHandle`.
///
/// Uses `DataShareManagerImpl::GetServiceProxy()` to obtain the service proxy
/// and delegates all proxy data operations to it.
pub struct DataProxyHandle;

impl DataProxyHandle {
    /// Create a new DataProxyHandle.
    pub fn new() -> Self {
        Self
    }

    /// Publish proxy data.
    ///
    /// Corresponds to C++ `PublishProxyData()`.
    pub fn publish_proxy_data(
        &self,
        proxy_data: &[DataShareProxyData],
        proxy_config: &DataProxyConfig,
    ) -> Vec<DataProxyResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        proxy.publish_proxy_data(proxy_data, proxy_config)
    }

    /// Delete proxy data.
    ///
    /// Corresponds to C++ `DeleteProxyData()`.
    pub fn delete_proxy_data(
        &self,
        uris: &[String],
        proxy_config: &DataProxyConfig,
    ) -> Vec<DataProxyResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        proxy.delete_proxy_data(uris, proxy_config)
    }

    /// Get proxy data.
    ///
    /// Corresponds to C++ `GetProxyData()`.
    pub fn get_proxy_data(
        &self,
        uris: &[String],
        proxy_config: &DataProxyConfig,
    ) -> Vec<DataProxyGetResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        proxy.get_proxy_data(uris, proxy_config)
    }

    /// Subscribe to proxy data changes.
    ///
    /// Corresponds to C++ `SubscribeProxyData()`.
    pub fn subscribe_proxy_data(
        &self,
        uris: &[String],
        callback: Arc<dyn Fn(&[datashare_common::types::DataProxyChangeInfo]) + Send + Sync>,
    ) -> Vec<DataProxyResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        ProxyDataSubscriberManager::get_instance().add_observers(0, &proxy, uris, callback)
    }

    /// Unsubscribe from proxy data changes.
    ///
    /// Corresponds to C++ `UnsubscribeProxyData()`.
    pub fn unsubscribe_proxy_data(&self, uris: &[String]) -> Vec<DataProxyResult> {
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        ProxyDataSubscriberManager::get_instance().del_observers(0, &proxy, uris)
    }
}

impl Default for DataProxyHandle {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_dataproxy_handle_construction() {
        let _handle = DataProxyHandle::new();
    }

    #[test]
    fn test_publish_proxy_data_placeholder() {
        let handle = DataProxyHandle::new();
        let config = DataProxyConfig::default();
        let result = handle.publish_proxy_data(&[], &config);
        assert!(result.is_empty());
    }

    #[test]
    fn test_get_proxy_data_placeholder() {
        let handle = DataProxyHandle::new();
        let config = DataProxyConfig::default();
        let result = handle.get_proxy_data(&[], &config);
        assert!(result.is_empty());
    }
}
