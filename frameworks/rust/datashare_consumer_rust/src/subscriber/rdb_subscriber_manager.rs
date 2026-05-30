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

//! RdbSubscriberManager — manages RDB data change subscriptions.
//!
//! Corresponds to C++ `RdbSubscriberManager` in
//! `frameworks/native/proxy/src/rdb_subscriber_manager.cpp` (300 lines).
//!
//! This manager:
//! - Maintains a map of (uri, templateId) → observer callbacks
//! - Communicates with DataShareServiceProxy to subscribe/unsubscribe
//! - Dispatches change notifications to registered observers
//! - Supports enable/disable of subscriptions
//! - Handles service restart recovery via RecoverObservers

use std::collections::BTreeMap;
use std::sync::{Arc, Mutex, OnceLock};

use datashare_common::template::TemplateId;
use datashare_common::types::{OperationResult, RdbChangeNode};

use ipc::remote::RemoteObj;

use crate::proxy::DataShareServiceProxy;
use crate::subscriber::observer_stub::RdbObserverStub;

/// Key for the RDB observer map.
///
/// Corresponds to C++ `RdbObserverMapKey`.
/// Uses (clear_uri, templateId) as the identity for deduplication.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct RdbObserverMapKey {
    /// Original URI
    pub uri: String,
    /// URI without query parameters
    pub clear_uri: String,
    /// Template identifier
    pub template_id: TemplateId,
}

impl RdbObserverMapKey {
    /// Create a new key, stripping query parameters from the URI.
    pub fn new(uri: &str, template_id: TemplateId) -> Self {
        let clear_uri = uri.split('?').next().unwrap_or(uri).to_string();
        Self {
            uri: uri.to_string(),
            clear_uri,
            template_id,
        }
    }
}

impl PartialOrd for RdbObserverMapKey {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

impl Ord for RdbObserverMapKey {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        match self.clear_uri.cmp(&other.clear_uri) {
            std::cmp::Ordering::Equal => {
                match self
                    .template_id
                    .subscriber_id
                    .cmp(&other.template_id.subscriber_id)
                {
                    std::cmp::Ordering::Equal => self
                        .template_id
                        .bundle_name
                        .cmp(&other.template_id.bundle_name),
                    other => other,
                }
            }
            other => other,
        }
    }
}

/// RDB observer wrapper.
///
/// Corresponds to C++ `RdbObserver`.
struct RdbObserver {
    callback: Arc<dyn Fn(&RdbChangeNode) + Send + Sync>,
}

impl RdbObserver {
    fn new(callback: Arc<dyn Fn(&RdbChangeNode) + Send + Sync>) -> Self {
        Self { callback }
    }

    fn on_change(&self, change_node: &RdbChangeNode) {
        (self.callback)(change_node);
    }
}

/// Subscriber entry: subscriber handle → observer with enable/disable state.
///
/// Mirrors C++ `ObserverNode` in `CallbacksManager`:
/// - `enabled`: whether this observer is active
/// - `is_notify_on_enabled`: set when Emit fires while disabled; cleared on Enable (triggers replay)
struct SubscriberEntry {
    subscriber: u64,
    observer: RdbObserver,
    enabled: bool,
    is_notify_on_enabled: bool,
}

/// RdbSubscriberManager — manages RDB data change subscriptions.
///
/// Corresponds to C++ `RdbSubscriberManager : public CallbacksManager<RdbObserverMapKey, RdbObserver>`.
///
/// Singleton that manages observer registrations and dispatching.
pub struct RdbSubscriberManager {
    /// Map of key → list of subscriber entries
    callbacks: Mutex<BTreeMap<RdbObserverMapKey, Vec<SubscriberEntry>>>,

    /// Last change node per key (for emitting on enable)
    last_change_node_map: Mutex<BTreeMap<RdbObserverMapKey, Arc<RdbChangeNode>>>,

    /// Lazily-created IPC callback stub for receiving service notifications
    service_callback: OnceLock<RemoteObj>,
}

/// Singleton instance.
static INSTANCE: OnceLock<Arc<RdbSubscriberManager>> = OnceLock::new();

impl RdbSubscriberManager {
    /// Get the singleton instance.
    ///
    /// Corresponds to C++ `RdbSubscriberManager::GetInstance()`.
    pub fn get_instance() -> Arc<RdbSubscriberManager> {
        INSTANCE
            .get_or_init(|| {
                Arc::new(RdbSubscriberManager {
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
            let stub = RdbObserverStub::new(Box::new(|node| {
                RdbSubscriberManager::get_instance().emit(node);
            }));
            stub.into_remote()
                .expect("Failed to create RdbObserverStub remote object")
        })
    }

    /// Add observers for the given URIs.
    ///
    /// Corresponds to C++ `AddObservers()`.
    /// Registers callbacks locally and subscribes via the service proxy.
    pub fn add_observers(
        &self,
        subscriber: u64,
        proxy: &Arc<DataShareServiceProxy>,
        uris: &[String],
        template_id: &TemplateId,
        callback: Arc<dyn Fn(&RdbChangeNode) + Send + Sync>,
    ) -> Vec<OperationResult> {
        let mut results = Vec::new();
        let mut first_add_uris = Vec::new();
        let mut local_register_keys = Vec::new();
        {
            let mut callbacks = self.callbacks.lock().unwrap();
            for uri in uris {
                let key = RdbObserverMapKey::new(uri, template_id.clone());
                let has_enabled = callbacks
                    .get(&key)
                    .map(|entries| entries.iter().any(|e| e.enabled))
                    .unwrap_or(false);
                let entry = SubscriberEntry {
                    subscriber,
                    observer: RdbObserver::new(callback.clone()),
                    enabled: true,
                    is_notify_on_enabled: false,
                };
                if has_enabled {
                    local_register_keys.push(key.clone());
                    results.push(OperationResult::new(uri.clone(), 0));
                } else {
                    first_add_uris.push(uri.clone());
                }
                callbacks.entry(key).or_insert_with(Vec::new).push(entry);
            }
        }

        if !local_register_keys.is_empty() {
            let last_map = self.last_change_node_map.lock().unwrap();
            for key in &local_register_keys {
                if let Some(node) = last_map.get(key) {
                    callback(node);
                }
            }
        }

        if !first_add_uris.is_empty() {
            let cb = self.get_or_create_service_callback();
            let svc_results = proxy.subscribe_rdb_data(
                &first_add_uris,
                template_id.subscriber_id,
                &template_id.bundle_name,
                cb,
            );
            results.extend(svc_results);
        }
        results
    }

    /// Remove observers for specific URIs.
    ///
    /// Corresponds to C++ `DelObservers(subscriber, proxy, uris, templateId)`.
    pub fn del_observers(
        &self,
        subscriber: u64,
        proxy: &Arc<DataShareServiceProxy>,
        uris: &[String],
        template_id: &TemplateId,
    ) -> Vec<OperationResult> {
        let mut callbacks = self.callbacks.lock().unwrap();

        let mut uris_to_unsub = Vec::new();
        let mut keys_to_erase = Vec::new();
        for uri in uris {
            let key = RdbObserverMapKey::new(uri, template_id.clone());
            if let Some(entries) = callbacks.get_mut(&key) {
                entries.retain(|e| e.subscriber != subscriber);
                if entries.is_empty() {
                    callbacks.remove(&key);
                    uris_to_unsub.push(uri.clone());
                    keys_to_erase.push(key);
                }
            }
        }
        drop(callbacks);

        if !keys_to_erase.is_empty() {
            let mut last_map = self.last_change_node_map.lock().unwrap();
            for key in &keys_to_erase {
                last_map.remove(key);
            }
        }

        if !uris_to_unsub.is_empty() {
            return proxy.unsubscribe_rdb_data(
                &uris_to_unsub,
                template_id.subscriber_id,
                &template_id.bundle_name,
            );
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
    ) -> Vec<OperationResult> {
        let mut callbacks = self.callbacks.lock().unwrap();

        callbacks.retain(|_key, entries| {
            entries.retain(|e| e.subscriber != subscriber);
            !entries.is_empty()
        });

        Vec::new()
    }

    /// Enable observers for specific URIs.
    ///
    /// Corresponds to C++ `EnableObservers()`.
    /// Sends EnableSubscribeRdbData only when transitioning from all-disabled to any-enabled.
    /// Replays cached change nodes to observers that were notified while disabled (EmitOnEnable).
    pub fn enable_observers(
        &self,
        subscriber: u64,
        proxy: &Arc<DataShareServiceProxy>,
        uris: &[String],
        template_id: &TemplateId,
    ) -> Vec<OperationResult> {
        let mut results = Vec::new();
        let mut send_service_uris = Vec::new();
        let mut replay_list: Vec<(
            Arc<dyn Fn(&RdbChangeNode) + Send + Sync>,
            Arc<RdbChangeNode>,
        )> = Vec::new();
        {
            let mut callbacks = self.callbacks.lock().unwrap();
            let last_map = self.last_change_node_map.lock().unwrap();
            for uri in uris {
                let key = RdbObserverMapKey::new(uri, template_id.clone());
                let entries = match callbacks.get_mut(&key) {
                    Some(e) => e,
                    None => {
                        results.push(OperationResult::new(uri.clone(), 1047));
                        continue;
                    }
                };
                let has_subscriber = entries.iter().any(|e| e.subscriber == subscriber);
                if !has_subscriber {
                    results.push(OperationResult::new(uri.clone(), 1047));
                    continue;
                }
                let already_enabled = entries
                    .iter()
                    .any(|e| e.subscriber == subscriber && e.enabled);
                if already_enabled {
                    results.push(OperationResult::new(uri.clone(), 0));
                    continue;
                }
                let any_enabled_before = entries.iter().any(|e| e.enabled);
                for entry in entries.iter_mut() {
                    if entry.subscriber == subscriber && !entry.enabled {
                        entry.enabled = true;
                        if entry.is_notify_on_enabled {
                            entry.is_notify_on_enabled = false;
                            if let Some(node) = last_map.get(&key) {
                                replay_list.push((entry.observer.callback.clone(), node.clone()));
                            }
                        }
                    }
                }
                if !any_enabled_before {
                    send_service_uris.push(uri.clone());
                }
            }
        }

        if !send_service_uris.is_empty() {
            let svc_results = proxy.enable_subscribe_rdb_data(
                &send_service_uris,
                template_id.subscriber_id,
                &template_id.bundle_name,
            );
            results.extend(svc_results);
        }
        for (callback, node) in &replay_list {
            callback(node);
        }
        results
    }

    /// Disable observers for specific URIs.
    ///
    /// Corresponds to C++ `DisableObservers()`.
    /// Only sends DisableSubscribeRdbData when the last enabled observer for a key is disabled.
    pub fn disable_observers(
        &self,
        subscriber: u64,
        proxy: &Arc<DataShareServiceProxy>,
        uris: &[String],
        template_id: &TemplateId,
    ) -> Vec<OperationResult> {
        let mut results = Vec::new();
        let mut last_disabled_uris = Vec::new();
        {
            let mut callbacks = self.callbacks.lock().unwrap();
            for uri in uris {
                let key = RdbObserverMapKey::new(uri, template_id.clone());
                let entries = match callbacks.get_mut(&key) {
                    Some(e) => e,
                    None => {
                        results.push(OperationResult::new(uri.clone(), 1047));
                        continue;
                    }
                };
                let any_enabled_before = entries.iter().any(|e| e.enabled);
                if !any_enabled_before {
                    results.push(OperationResult::new(uri.clone(), 1047));
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
                    results.push(OperationResult::new(uri.clone(), 1047));
                    continue;
                }
                let any_enabled = entries.iter().any(|e| e.enabled);
                if any_enabled {
                    results.push(OperationResult::new(uri.clone(), 0));
                    continue;
                }
                last_disabled_uris.push(uri.clone());
            }
        }
        if !last_disabled_uris.is_empty() {
            let svc_results = proxy.disable_subscribe_rdb_data(
                &last_disabled_uris,
                template_id.subscriber_id,
                &template_id.bundle_name,
            );
            results.extend(svc_results);
        }
        results
    }

    /// Recover all observers after service restart.
    ///
    /// Corresponds to C++ `RecoverObservers()`.
    pub fn recover_observers(&self, proxy: &Arc<DataShareServiceProxy>) {
        let uris: Vec<(String, i64, String)> = {
            let callbacks = self.callbacks.lock().unwrap();
            callbacks
                .iter()
                .map(|(key, _)| {
                    (
                        key.uri.clone(),
                        key.template_id.subscriber_id,
                        key.template_id.bundle_name.clone(),
                    )
                })
                .collect()
        };
        let cb = self.get_or_create_service_callback();
        for (uri, subscriber_id, bundle_name) in &uris {
            proxy.subscribe_rdb_data(&[uri.clone()], *subscriber_id, bundle_name, cb);
        }
    }

    /// Emit a change notification to matching observers.
    ///
    /// Corresponds to C++ `Emit(const RdbChangeNode&)`.
    /// Only fires enabled observers. Disabled observers get `is_notify_on_enabled` set.
    /// Stores the change node in `last_change_node_map` for EmitOnEnable replay.
    pub fn emit(&self, change_node: RdbChangeNode) {
        let change_uri_owned = change_node
            .uri
            .split('?')
            .next()
            .unwrap_or(&change_node.uri)
            .to_string();
        let arc_node = Arc::new(change_node);

        let mut callbacks = self.callbacks.lock().unwrap();

        let mut matching_keys = Vec::new();
        let mut to_fire: Vec<Arc<dyn Fn(&RdbChangeNode) + Send + Sync>> = Vec::new();
        for (key, entries) in callbacks.iter_mut() {
            if key.clear_uri == change_uri_owned {
                matching_keys.push(key.clone());
                for entry in entries.iter_mut() {
                    if entry.enabled {
                        to_fire.push(entry.observer.callback.clone());
                        entry.is_notify_on_enabled = false;
                    } else {
                        entry.is_notify_on_enabled = true;
                    }
                }
            }
        }
        drop(callbacks);

        {
            let mut last_map = self.last_change_node_map.lock().unwrap();
            for key in matching_keys {
                last_map.insert(key, arc_node.clone());
            }
        }

        for callback in &to_fire {
            callback(&arc_node);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_rdb_observer_map_key() {
        let key = RdbObserverMapKey::new(
            "datashare:///test?query=1",
            TemplateId::new(1, "bundle".to_string()),
        );
        assert_eq!(key.uri, "datashare:///test?query=1");
        assert_eq!(key.clear_uri, "datashare:///test");
    }

    #[test]
    fn test_rdb_observer_map_key_no_query() {
        let key = RdbObserverMapKey::new(
            "datashare:///test",
            TemplateId::new(1, "bundle".to_string()),
        );
        assert_eq!(key.clear_uri, "datashare:///test");
    }

    #[test]
    fn test_rdb_subscriber_manager_singleton() {
        let instance1 = RdbSubscriberManager::get_instance();
        let instance2 = RdbSubscriberManager::get_instance();
        assert!(Arc::ptr_eq(&instance1, &instance2));
    }
}
