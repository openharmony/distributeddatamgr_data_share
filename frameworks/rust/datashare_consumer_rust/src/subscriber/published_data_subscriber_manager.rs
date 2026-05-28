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

//! PublishedDataSubscriberManager — manages published data change subscriptions.
//!
//! Corresponds to C++ `PublishedDataSubscriberManager` in
//! `frameworks/native/proxy/src/published_data_subscriber_manager.cpp` (326 lines).
//!
//! This manager:
//! - Maintains a map of (uri, subscriberId) → observer callbacks
//! - Communicates with DataShareServiceProxy to subscribe/unsubscribe
//! - Dispatches change notifications to registered observers
//! - Supports enable/disable of subscriptions
//! - Handles service restart recovery via RecoverObservers

use std::collections::BTreeMap;
use std::sync::{Arc, Mutex, OnceLock};

use datashare_common::types::{OperationResult, PublishedDataChangeNode, PublishedDataItem};

use ipc::remote::RemoteObj;

use crate::proxy::DataShareServiceProxy;
use crate::subscriber::observer_stub::PublishedDataObserverStub;

/// Key for the published data observer map.
///
/// Corresponds to C++ `PublishedObserverMapKey`.
/// Uses (clear_uri, subscriberId) as the identity.
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub struct PublishedObserverMapKey {
    /// Original URI
    pub uri: String,
    /// URI without query parameters
    pub clear_uri: String,
    /// Subscriber identifier
    pub subscriber_id: i64,
}

impl PublishedObserverMapKey {
    /// Create a new key, stripping query parameters from the URI.
    pub fn new(uri: &str, subscriber_id: i64) -> Self {
        let clear_uri = uri.split('?').next().unwrap_or(uri).to_string();
        Self {
            uri: uri.to_string(),
            clear_uri,
            subscriber_id,
        }
    }
}

/// Published data observer wrapper.
struct PublishedDataObserver {
    callback: Arc<dyn Fn(&PublishedDataChangeNode) + Send + Sync>,
}

impl PublishedDataObserver {
    fn new(callback: Arc<dyn Fn(&PublishedDataChangeNode) + Send + Sync>) -> Self {
        Self { callback }
    }

    fn on_change(&self, change_node: &PublishedDataChangeNode) {
        (self.callback)(change_node);
    }
}

/// Subscriber entry — mirrors C++ `ObserverNode` in `CallbacksManager`.
struct SubscriberEntry {
    subscriber: u64,
    observer: PublishedDataObserver,
    enabled: bool,
    is_notify_on_enabled: bool,
}

/// PublishedDataSubscriberManager — manages published data subscriptions.
///
/// Corresponds to C++ `PublishedDataSubscriberManager : public CallbacksManager<PublishedObserverMapKey, PublishedDataObserver>`.
pub struct PublishedDataSubscriberManager {
    /// Map of key → list of subscriber entries
    callbacks: Mutex<BTreeMap<PublishedObserverMapKey, Vec<SubscriberEntry>>>,

    /// Last change node per key
    last_change_node_map: Mutex<BTreeMap<PublishedObserverMapKey, PublishedDataChangeNode>>,

    /// Lazily-created IPC callback stub for receiving service notifications
    service_callback: OnceLock<RemoteObj>,
}

/// Singleton instance.
static INSTANCE: OnceLock<Arc<PublishedDataSubscriberManager>> = OnceLock::new();

impl PublishedDataSubscriberManager {
    /// Get the singleton instance.
    pub fn get_instance() -> Arc<PublishedDataSubscriberManager> {
        INSTANCE
            .get_or_init(|| {
                Arc::new(PublishedDataSubscriberManager {
                    callbacks: Mutex::new(BTreeMap::new()),
                    last_change_node_map: Mutex::new(BTreeMap::new()),
                    service_callback: OnceLock::new(),
                })
            })
            .clone()
    }

    /// Get or create the IPC callback stub for receiving service notifications.
    fn get_or_create_service_callback(&self) -> &RemoteObj {
        self.service_callback.get_or_init(|| {
            let stub = PublishedDataObserverStub::new(Box::new(|node| {
                PublishedDataSubscriberManager::get_instance().emit(node);
            }));
            stub.into_remote()
                .expect("Failed to create PublishedDataObserverStub remote object")
        })
    }

    /// Add observers for the given URIs.
    ///
    /// Corresponds to C++ `AddObservers()`.
    /// Only calls proxy for keys that have no existing local observers (firstAddKeys).
    /// For keys that already have local observers, returns success without re-subscribing.
    pub fn add_observers(
        &self,
        subscriber: u64,
        proxy: &Arc<DataShareServiceProxy>,
        uris: &[String],
        subscriber_id: i64,
        callback: Arc<dyn Fn(&PublishedDataChangeNode) + Send + Sync>,
    ) -> Vec<OperationResult> {
        let mut callbacks = self.callbacks.lock().unwrap();

        let mut first_add_uris = Vec::new();
        let mut existing_uris = Vec::new();

        for uri in uris {
            let key = PublishedObserverMapKey::new(uri, subscriber_id);
            let entry = SubscriberEntry {
                subscriber,
                observer: PublishedDataObserver::new(callback.clone()),
                enabled: true,
                is_notify_on_enabled: false,
            };
            let entries = callbacks.entry(key).or_insert_with(Vec::new);
            if entries.is_empty() {
                first_add_uris.push(uri.clone());
            } else {
                existing_uris.push(uri.clone());
            }
            entries.push(entry);
        }
        drop(callbacks);

        let mut results: Vec<OperationResult> = existing_uris
            .iter()
            .map(|uri| OperationResult::new(uri.clone(), 0))
            .collect();

        if !first_add_uris.is_empty() {
            let cb = self.get_or_create_service_callback();
            let sub_results = proxy.subscribe_published_data(&first_add_uris, subscriber_id, cb);
            let mut failed_keys = Vec::new();
            for result in &sub_results {
                if result.err_code != 0 {
                    failed_keys.push(PublishedObserverMapKey::new(&result.key, subscriber_id));
                }
            }
            if !failed_keys.is_empty() {
                let mut cbs = self.callbacks.lock().unwrap();
                for key in &failed_keys {
                    if let Some(entries) = cbs.get_mut(key) {
                        entries.retain(|e| e.subscriber != subscriber);
                        if entries.is_empty() {
                            cbs.remove(key);
                        }
                    }
                }
            }
            results.extend(sub_results);
        }

        results
    }

    /// Remove observers for specific URIs.
    ///
    /// Corresponds to C++ `DelObservers(subscriber, proxy, uris, subscriberId)`.
    /// Calls proxy to unsubscribe only for keys where the last observer is removed.
    pub fn del_observers(
        &self,
        subscriber: u64,
        proxy: &Arc<DataShareServiceProxy>,
        uris: &[String],
        subscriber_id: i64,
    ) -> Vec<OperationResult> {
        let mut callbacks = self.callbacks.lock().unwrap();
        let mut last_del_uris = Vec::new();

        for uri in uris {
            let key = PublishedObserverMapKey::new(uri, subscriber_id);
            if let Some(entries) = callbacks.get_mut(&key) {
                entries.retain(|e| e.subscriber != subscriber);
                if entries.is_empty() {
                    callbacks.remove(&key);
                    last_del_uris.push(uri.clone());
                }
            }
        }
        drop(callbacks);

        if last_del_uris.is_empty() {
            return Vec::new();
        }

        proxy.unsubscribe_published_data(&last_del_uris, subscriber_id)
    }

    /// Remove all observers for a subscriber.
    ///
    /// Corresponds to C++ `DelObservers(subscriber, proxy)`.
    pub fn del_all_observers(
        &self,
        subscriber: u64,
        proxy: &Arc<DataShareServiceProxy>,
    ) -> Vec<OperationResult> {
        let mut callbacks = self.callbacks.lock().unwrap();
        let mut last_del_keys: Vec<PublishedObserverMapKey> = Vec::new();

        callbacks.retain(|key, entries| {
            entries.retain(|e| e.subscriber != subscriber);
            if entries.is_empty() {
                last_del_keys.push(key.clone());
                return false;
            }
            true
        });
        drop(callbacks);

        let mut results = Vec::new();
        let mut keys_map: BTreeMap<i64, Vec<String>> = BTreeMap::new();
        for key in &last_del_keys {
            keys_map
                .entry(key.subscriber_id)
                .or_default()
                .push(key.uri.clone());
        }
        for (subscriber_id, uris) in &keys_map {
            let unsub_results = proxy.unsubscribe_published_data(uris, *subscriber_id);
            results.extend(unsub_results);
        }
        results
    }

    /// Enable observers.
    ///
    /// Corresponds to C++ `EnableObservers` in `CallbacksManager` + `PublishedDataSubscriberManager`.
    /// Re-enables locally, calls proxy if needed, and replays stored data for observers
    /// that were marked is_notify_on_enabled during Emit while disabled.
    pub fn enable_observers(
        &self,
        subscriber: u64,
        proxy: &Arc<DataShareServiceProxy>,
        uris: &[String],
        subscriber_id: i64,
    ) -> Vec<OperationResult> {
        let mut callbacks = self.callbacks.lock().unwrap();
        let mut results = Vec::new();
        let mut send_service_keys = Vec::new();

        let mut refresh_observers: BTreeMap<
            PublishedObserverMapKey,
            Vec<(Arc<dyn Fn(&PublishedDataChangeNode) + Send + Sync>, bool)>,
        > = BTreeMap::new();

        for uri in uris {
            let key = PublishedObserverMapKey::new(uri, subscriber_id);
            let entries = match callbacks.get_mut(&key) {
                Some(e) => e,
                None => {
                    results.push(OperationResult::new(uri.clone(), -1));
                    continue;
                }
            };
            let target_idx = match entries.iter().position(|e| e.subscriber == subscriber) {
                Some(idx) => idx,
                None => {
                    results.push(OperationResult::new(uri.clone(), -1));
                    continue;
                }
            };
            if entries[target_idx].enabled {
                results.push(OperationResult::new(uri.clone(), 0));
                continue;
            }
            let notify_flag = entries[target_idx].is_notify_on_enabled;
            let cb = entries[target_idx].observer.callback.clone();
            if !entries.iter().any(|e| e.enabled) {
                send_service_keys.push(uri.clone());
            }
            entries[target_idx].enabled = true;
            refresh_observers
                .entry(key)
                .or_default()
                .push((cb, notify_flag));
        }
        drop(callbacks);

        if !send_service_keys.is_empty() {
            let proxy_results =
                proxy.enable_subscribe_published_data(&send_service_keys, subscriber_id);
            let mut failed_keys = Vec::new();
            for result in &proxy_results {
                if result.err_code != 0 {
                    failed_keys.push(PublishedObserverMapKey::new(&result.key, subscriber_id));
                }
            }
            if !failed_keys.is_empty() {
                let mut cbs = self.callbacks.lock().unwrap();
                for key in &failed_keys {
                    if let Some(entries) = cbs.get_mut(key) {
                        for entry in entries.iter_mut() {
                            if entry.subscriber == subscriber {
                                entry.enabled = false;
                            }
                        }
                    }
                }
            }
            results.extend(proxy_results);
        }

        self.emit_on_enable(&refresh_observers);
        results
    }

    /// Replay stored change data to observers that were marked is_notify_on_enabled.
    ///
    /// Corresponds to C++ `EmitOnEnable`.
    fn emit_on_enable(
        &self,
        refresh_observers: &BTreeMap<
            PublishedObserverMapKey,
            Vec<(Arc<dyn Fn(&PublishedDataChangeNode) + Send + Sync>, bool)>,
        >,
    ) {
        let last_map = self.last_change_node_map.lock().unwrap();
        for (key, observers) in refresh_observers {
            if let Some(stored_node) = last_map.get(key) {
                for (callback, is_notify) in observers {
                    if *is_notify {
                        callback(stored_node);
                    }
                }
            }
        }
    }

    /// Disable observers.
    ///
    /// Corresponds to C++ `DisableObservers` in `CallbacksManager` + `PublishedDataSubscriberManager`.
    /// Sets enabled=false locally; only calls proxy when the last enabled observer is disabled.
    pub fn disable_observers(
        &self,
        subscriber: u64,
        proxy: &Arc<DataShareServiceProxy>,
        uris: &[String],
        subscriber_id: i64,
    ) -> Vec<OperationResult> {
        let mut callbacks = self.callbacks.lock().unwrap();
        let mut results = Vec::new();
        let mut last_disabled_keys = Vec::new();

        for uri in uris {
            let key = PublishedObserverMapKey::new(uri, subscriber_id);
            let entries = match callbacks.get_mut(&key) {
                Some(e) => e,
                None => {
                    results.push(OperationResult::new(uri.clone(), -1));
                    continue;
                }
            };
            if !entries.iter().any(|e| e.enabled) {
                results.push(OperationResult::new(uri.clone(), -1));
                continue;
            }
            let mut has_disabled = false;
            for entry in entries.iter_mut() {
                if entry.subscriber == subscriber {
                    if entry.enabled {
                        entry.enabled = false;
                        entry.is_notify_on_enabled = false;
                    }
                    has_disabled = true;
                }
            }
            if !has_disabled {
                results.push(OperationResult::new(uri.clone(), -1));
                continue;
            }
            if entries.iter().any(|e| e.enabled) {
                results.push(OperationResult::new(uri.clone(), 0));
            } else {
                last_disabled_keys.push(uri.clone());
            }
        }
        drop(callbacks);

        if !last_disabled_keys.is_empty() {
            let proxy_results =
                proxy.disable_subscribe_published_data(&last_disabled_keys, subscriber_id);
            results.extend(proxy_results);
        }
        results
    }

    /// Recover all observers after service restart.
    pub fn recover_observers(&self, proxy: &Arc<DataShareServiceProxy>) {
        let uris: Vec<(String, i64)> = {
            let callbacks = self.callbacks.lock().unwrap();
            callbacks
                .iter()
                .map(|(key, _)| (key.uri.clone(), key.subscriber_id))
                .collect()
        };
        let cb = self.get_or_create_service_callback();
        for (uri, subscriber_id) in &uris {
            proxy.subscribe_published_data(&[uri.clone()], *subscriber_id, cb);
        }
    }

    /// Emit a change notification to matching observers.
    ///
    /// Corresponds to C++ `Emit(PublishedDataChangeNode &changeNode)`.
    /// Stores data in last_change_node_map, marks disabled observers for notify-on-enable,
    /// and only notifies enabled observers.
    pub fn emit(&self, change_node: &PublishedDataChangeNode) {
        let mut callbacks = self.callbacks.lock().unwrap();
        let mut last_map = self.last_change_node_map.lock().unwrap();

        for data in &change_node.datas {
            let key = PublishedObserverMapKey::new(&data.key, data.subscriber_id);

            if let Some(entries) = callbacks.get_mut(&key) {
                for entry in entries.iter_mut() {
                    if !entry.enabled {
                        entry.is_notify_on_enabled = true;
                    }
                }
            }

            let stored = last_map
                .entry(key)
                .or_insert_with(PublishedDataChangeNode::default);
            stored.datas.clear();
            stored.datas.push(PublishedDataItem::new(
                data.key.clone(),
                data.subscriber_id,
                data.get_data(),
            ));
            stored
                .owner_bundle_name
                .clone_from(&change_node.owner_bundle_name);
        }

        for data in &change_node.datas {
            let key = PublishedObserverMapKey::new(&data.key, data.subscriber_id);
            if let Some(entries) = callbacks.get(&key) {
                for entry in entries {
                    if entry.enabled {
                        entry.observer.on_change(change_node);
                    }
                }
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_published_observer_map_key() {
        let key = PublishedObserverMapKey::new("datashare:///test?query=1", 100);
        assert_eq!(key.clear_uri, "datashare:///test");
        assert_eq!(key.subscriber_id, 100);
    }

    #[test]
    fn test_published_data_subscriber_manager_singleton() {
        let instance1 = PublishedDataSubscriberManager::get_instance();
        let instance2 = PublishedDataSubscriberManager::get_instance();
        assert!(Arc::ptr_eq(&instance1, &instance2));
    }
}
