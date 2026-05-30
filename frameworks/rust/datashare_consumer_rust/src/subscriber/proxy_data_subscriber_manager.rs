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

//! ProxyDataSubscriberManager — manages proxy data change subscriptions.
//!
//! Corresponds to C++ `ProxyDataSubscriberManager` in
//! `frameworks/native/proxy/src/proxy_data_subscriber_manager.cpp` (185 lines).
//!
//! This manager handles subscriptions for DataProxy data changes,
//! which is a different mechanism from RDB and published data.

use std::collections::BTreeMap;
use std::sync::{Arc, Mutex, OnceLock};

use datashare_common::types::{DataProxyChangeInfo, DataProxyResult};

use ipc::remote::RemoteObj;

use crate::proxy::DataShareServiceProxy;
use crate::subscriber::observer_stub::ProxyDataObserverStub;

/// Key for the proxy data observer map.
///
/// Corresponds to C++ `ProxyDataObserverMapKey`.
/// Uses URI as the identity key.
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub struct ProxyDataObserverMapKey {
    /// URI string
    pub uri: String,
}

impl ProxyDataObserverMapKey {
    pub fn new(uri: &str) -> Self {
        Self {
            uri: uri.to_string(),
        }
    }
}

/// Proxy data observer wrapper.
struct ProxyDataObserver {
    callback: Arc<dyn Fn(&[DataProxyChangeInfo]) + Send + Sync>,
}

impl ProxyDataObserver {
    fn new(callback: Arc<dyn Fn(&[DataProxyChangeInfo]) + Send + Sync>) -> Self {
        Self { callback }
    }

    fn on_change(&self, change_info: &[DataProxyChangeInfo]) {
        (self.callback)(change_info);
    }
}

/// Subscriber entry.
struct SubscriberEntry {
    subscriber: u64,
    observer: ProxyDataObserver,
}

/// ProxyDataSubscriberManager — manages proxy data subscriptions.
///
/// Corresponds to C++ `ProxyDataSubscriberManager : public CallbacksManager<ProxyDataObserverMapKey, ProxyDataObserver>`.
pub struct ProxyDataSubscriberManager {
    /// Map of key → list of subscriber entries
    callbacks: Mutex<BTreeMap<ProxyDataObserverMapKey, Vec<SubscriberEntry>>>,

    /// Lazily-created IPC callback stub for receiving service notifications
    service_callback: OnceLock<RemoteObj>,
}

/// Singleton instance.
static INSTANCE: OnceLock<Arc<ProxyDataSubscriberManager>> = OnceLock::new();

impl ProxyDataSubscriberManager {
    /// Get the singleton instance.
    pub fn get_instance() -> Arc<ProxyDataSubscriberManager> {
        INSTANCE
            .get_or_init(|| {
                Arc::new(ProxyDataSubscriberManager {
                    callbacks: Mutex::new(BTreeMap::new()),
                    service_callback: OnceLock::new(),
                })
            })
            .clone()
    }

    /// Get or create the IPC callback stub for receiving service notifications.
    fn get_or_create_service_callback(&self) -> &RemoteObj {
        self.service_callback.get_or_init(|| {
            let stub = ProxyDataObserverStub::new(Box::new(|change_info| {
                ProxyDataSubscriberManager::get_instance().emit(change_info);
            }));
            stub.into_remote()
                .expect("Failed to create ProxyDataObserverStub remote object")
        })
    }

    /// Add observers for the given URIs.
    ///
    /// Corresponds to C++ `AddObservers()`.
    pub fn add_observers(
        &self,
        subscriber: u64,
        proxy: &Arc<DataShareServiceProxy>,
        uris: &[String],
        callback: Arc<dyn Fn(&[DataProxyChangeInfo]) + Send + Sync>,
    ) -> Vec<DataProxyResult> {
        let mut callbacks = self.callbacks.lock().unwrap();

        for uri in uris {
            let key = ProxyDataObserverMapKey::new(uri);
            let entry = SubscriberEntry {
                subscriber,
                observer: ProxyDataObserver::new(callback.clone()),
            };
            callbacks.entry(key).or_insert_with(Vec::new).push(entry);
        }
        drop(callbacks);

        let cb = self.get_or_create_service_callback();
        proxy.subscribe_proxy_data(uris, cb)
    }

    /// Remove observers for specific URIs.
    ///
    /// Corresponds to C++ `DelObservers(subscriber, proxy, uris)`.
    pub fn del_observers(
        &self,
        subscriber: u64,
        _proxy: &Arc<DataShareServiceProxy>,
        uris: &[String],
    ) -> Vec<DataProxyResult> {
        let mut callbacks = self.callbacks.lock().unwrap();

        for uri in uris {
            let key = ProxyDataObserverMapKey::new(uri);
            if let Some(entries) = callbacks.get_mut(&key) {
                entries.retain(|e| e.subscriber != subscriber);
                if entries.is_empty() {
                    callbacks.remove(&key);
                }
            }
        }

        Vec::new()
    }

    /// Remove all observers for a subscriber.
    ///
    /// Corresponds to C++ `DelObservers(subscriber, proxy)`.
    pub fn del_all_observers(
        &self,
        subscriber: u64,
        _proxy: &Arc<DataShareServiceProxy>,
    ) -> Vec<DataProxyResult> {
        let mut callbacks = self.callbacks.lock().unwrap();

        callbacks.retain(|_key, entries| {
            entries.retain(|e| e.subscriber != subscriber);
            !entries.is_empty()
        });

        Vec::new()
    }

    /// Recover all observers after service restart.
    pub fn recover_observers(&self, proxy: &Arc<DataShareServiceProxy>) {
        let all_uris: Vec<String> = {
            let callbacks = self.callbacks.lock().unwrap();
            callbacks.keys().map(|k| k.uri.clone()).collect()
        };
        if !all_uris.is_empty() {
            let cb = self.get_or_create_service_callback();
            proxy.subscribe_proxy_data(&all_uris, cb);
        }
    }

    /// Emit change notifications to matching observers.
    pub fn emit(&self, change_info: &[DataProxyChangeInfo]) {
        let callbacks = self.callbacks.lock().unwrap();

        for (_key, entries) in callbacks.iter() {
            for entry in entries {
                entry.observer.on_change(change_info);
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_proxy_data_observer_map_key() {
        let key = ProxyDataObserverMapKey::new("datashare:///test");
        assert_eq!(key.uri, "datashare:///test");
    }

    #[test]
    fn test_proxy_data_subscriber_manager_singleton() {
        let instance1 = ProxyDataSubscriberManager::get_instance();
        let instance2 = ProxyDataSubscriberManager::get_instance();
        assert!(Arc::ptr_eq(&instance1, &instance2));
    }
}
