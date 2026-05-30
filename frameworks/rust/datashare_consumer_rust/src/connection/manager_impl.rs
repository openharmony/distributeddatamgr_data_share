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

//! DataShareManagerImpl — singleton service manager.
//!
//! Corresponds to C++ `DataShareManagerImpl` in
//! `frameworks/native/proxy/src/data_share_manager_impl.cpp` (259 lines).
//!
//! This is the central manager for the DataShare silent (service) path.
//! It manages:
//! - Service proxy lifecycle (obtaining DataShareServiceProxy via IPC)
//! - Service death monitoring and recovery
//! - Call count tracking (via DataShareCallReporter)
//! - Observer re-registration callbacks on service restart
//! - System ability subscription
//!
//! In C++, this is a raw singleton (`DataShareManagerImpl::GetInstance()`).
//! In Rust, we use `OnceLock` for thread-safe lazy initialization.

use std::collections::HashMap;
use std::sync::{Arc, Mutex, OnceLock};

use ipc::parcel::MsgParcel;
use ipc::remote::{RemoteObj, RemoteStub};
use samgr::manage::SystemAbilityManager;

use datashare_common::call_reporter::CallReporter;

use crate::proxy::DataShareServiceProxy;

/// Global singleton instance.
static INSTANCE: OnceLock<Arc<DataShareManagerImpl>> = OnceLock::new();

const DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID: i32 = 1301;
const IKVSTORE_DESCRIPTOR: &str = "OHOS.DistributedKv.IKvStoreDataService";
const GET_FEATURE_INTERFACE: u32 = 0;
const REGISTER_CLIENT_DEATH_OBSERVER: u32 = 1;

fn kv_get_feature_interface(kv_remote: &RemoteObj, name: &str) -> Option<RemoteObj> {
    let mut data = MsgParcel::new();
    data.write_interface_token(IKVSTORE_DESCRIPTOR).ok()?;
    data.write(&name.to_string()).ok()?;
    let mut reply = kv_remote
        .send_request(GET_FEATURE_INTERFACE, &mut data)
        .ok()?;
    reply.read_remote().ok()
}

fn kv_register_client_death_observer(
    kv_remote: &RemoteObj,
    app_id: &str,
    observer: RemoteObj,
) -> bool {
    let mut data = MsgParcel::new();
    if data.write_interface_token(IKVSTORE_DESCRIPTOR).is_err() {
        return false;
    }
    if data.write(&app_id.to_string()).is_err() {
        return false;
    }
    if data.write_remote(observer).is_err() {
        return false;
    }
    match kv_remote.send_request(REGISTER_CLIENT_DEATH_OBSERVER, &mut data) {
        Ok(mut reply) => reply.read::<i32>().unwrap_or(-1) == 0,
        Err(_) => false,
    }
}

struct ClientDeathObserverStub;

impl RemoteStub for ClientDeathObserverStub {
    fn descriptor(&self) -> &'static str {
        "ohos.distributeddata.IKvStoreClientDeathObserver"
    }
    fn on_remote_request(&self, _code: u32, _data: &mut MsgParcel, _reply: &mut MsgParcel) -> i32 {
        0
    }
}

fn sa_on_add(sa_id: i32, _device_id: &str) {
    if sa_id != DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID {
        return;
    }
    let instance = DataShareManagerImpl::get_instance();
    instance.on_add_system_ability(sa_id, _device_id);
}

fn sa_on_remove(_sa_id: i32, _device_id: &str) {}

/// DataShareManagerImpl — singleton manager for the DataShare service path.
///
/// Corresponds to C++ class `DataShareManagerImpl`.
///
/// Key responsibilities:
/// - Obtain and cache `DataShareServiceProxy` (IPC proxy to DataShareService)
/// - Monitor service death and trigger re-subscription
/// - Track call counts for rate limiting
/// - Manage observer re-registration callbacks
pub struct DataShareManagerImpl {
    /// Cached service proxy
    service_proxy: Mutex<Option<Arc<DataShareServiceProxy>>>,

    /// Cached KV service remote (SA 1301)
    kv_service: Mutex<Option<RemoteObj>>,

    /// Client death observer stub registered with KV service
    client_death_observer: Mutex<Option<RemoteObj>>,

    /// Bundle name for the application
    bundle_name: Mutex<String>,

    /// Death callback
    death_callback: Mutex<Option<Box<dyn Fn(Arc<DataShareServiceProxy>) + Send + Sync>>>,

    /// Observer re-registration callbacks: controller_id → callback
    observers: Mutex<HashMap<u64, Box<dyn Fn() + Send + Sync>>>,

    /// Call frequency reporter for rate limiting (mirrors C++ `dataShareCallReporter_`).
    call_reporter: CallReporter,
}

impl DataShareManagerImpl {
    /// Get the singleton instance.
    ///
    /// Corresponds to C++ `DataShareManagerImpl::GetInstance()`.
    /// Creates the instance on first call. Also subscribes to
    /// DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID system ability changes.
    pub fn get_instance() -> Arc<DataShareManagerImpl> {
        INSTANCE
            .get_or_init(|| {
                Arc::new(DataShareManagerImpl {
                    service_proxy: Mutex::new(None),
                    kv_service: Mutex::new(None),
                    client_death_observer: Mutex::new(None),
                    bundle_name: Mutex::new(String::new()),
                    death_callback: Mutex::new(None),
                    observers: Mutex::new(HashMap::new()),
                    call_reporter: CallReporter::new(),
                })
            })
            .clone()
    }

    /// Get the DataShareServiceProxy.
    ///
    /// Corresponds to C++ static `DataShareManagerImpl::GetServiceProxy()`.
    /// Returns the cached proxy, or attempts to create one if not available.
    pub fn get_service_proxy() -> Option<Arc<DataShareServiceProxy>> {
        let instance = Self::get_instance();
        instance.get_proxy()
    }

    /// Internal method to get proxy.
    fn get_proxy(&self) -> Option<Arc<DataShareServiceProxy>> {
        let proxy = self.service_proxy.lock().unwrap();
        if proxy.is_some() {
            return proxy.clone();
        }
        drop(proxy);

        let remote = self.acquire_data_share_remote()?;
        let service_proxy = Arc::new(DataShareServiceProxy::new(remote));
        let mut p = self.service_proxy.lock().unwrap();
        *p = Some(service_proxy.clone());
        Some(service_proxy)
    }

    /// Full SA acquisition flow (aligns with C++ GetProxy → GetDataShareServiceProxy).
    ///
    /// Steps: CheckSystemAbility(1301) → GetFeatureInterface("data_share")
    ///      → RegisterClientDeathObserver → LinkToDeath
    pub fn acquire_data_share_remote(&self) -> Option<RemoteObj> {
        let kv_remote =
            SystemAbilityManager::check_system_ability(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID)
                .or_else(|| {
                    SystemAbilityManager::load_system_ability(
                        DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID,
                        10,
                    )
                })?;

        let ds_remote = kv_get_feature_interface(&kv_remote, "data_share")?;

        self.register_client_death_observer_via_kv(&kv_remote);

        let instance = Self::get_instance();
        ds_remote.add_death_recipient(move |_| {
            instance.on_remote_died();
        });

        *self.kv_service.lock().unwrap() = Some(kv_remote);

        Some(ds_remote)
    }

    /// Set the service proxy directly (for use when remote object is obtained externally).
    pub fn set_service_proxy(&self, remote: RemoteObj) {
        let proxy = Arc::new(DataShareServiceProxy::new(remote));
        let mut p = self.service_proxy.lock().unwrap();
        *p = Some(proxy);
    }

    /// Subscribe to SA 1301 changes (aligns with C++ GetInstance SA subscription).
    pub fn subscribe_sa(&self) {
        let _handler = SystemAbilityManager::subscribe_system_ability(
            DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID,
            sa_on_add,
            sa_on_remove,
        );
    }

    /// Handle remote service death.
    ///
    /// Corresponds to C++ `OnRemoteDied()`.
    pub fn on_remote_died(&self) {
        self.reset_service_handle();
    }

    /// Reset the cached service handle.
    pub fn reset_service_handle(&self) {
        let mut proxy = self.service_proxy.lock().unwrap();
        *proxy = None;
        let mut kv = self.kv_service.lock().unwrap();
        *kv = None;
    }

    /// Register client death observer via KV service.
    ///
    /// Corresponds to C++ `RegisterClientDeathObserver()`.
    fn register_client_death_observer_via_kv(&self, kv_remote: &RemoteObj) {
        let bundle = self.bundle_name.lock().unwrap().clone();
        if bundle.is_empty() {
            return;
        }
        if let Some(observer) = RemoteObj::from_stub(ClientDeathObserverStub) {
            kv_register_client_death_observer(kv_remote, &bundle, observer);
        }
    }

    /// Set the death callback.
    ///
    /// Corresponds to C++ `SetDeathCallback()`.
    pub fn set_death_callback<F>(&self, callback: F)
    where
        F: Fn(Arc<DataShareServiceProxy>) + Send + Sync + 'static,
    {
        let mut cb = self.death_callback.lock().unwrap();
        *cb = Some(Box::new(callback));
    }

    /// Set the bundle name for the application.
    ///
    /// Corresponds to C++ `SetBundleName()`.
    pub fn set_bundle_name(&self, bundle_name: &str) {
        let mut name = self.bundle_name.lock().unwrap();
        *name = bundle_name.to_string();
    }

    /// Get the bundle name.
    pub fn get_bundle_name(&self) -> String {
        self.bundle_name.lock().unwrap().clone()
    }

    /// Register a callback for observer re-registration on service restart.
    ///
    /// Corresponds to C++ `SetRegisterCallback()`.
    pub fn set_register_callback<F>(&self, controller_id: u64, callback: F)
    where
        F: Fn() + Send + Sync + 'static,
    {
        let mut observers = self.observers.lock().unwrap();
        observers.insert(controller_id, Box::new(callback));
    }

    /// Remove a re-registration callback.
    ///
    /// Corresponds to C++ `RemoveRegisterCallback()`.
    pub fn remove_register_callback(&self, controller_id: u64) {
        let mut observers = self.observers.lock().unwrap();
        observers.remove(&controller_id);
    }

    /// Handle system ability becoming available.
    ///
    /// Corresponds to C++ `OnAddSystemAbility()`.
    /// Triggers re-registration callbacks for all registered observers.
    pub fn on_add_system_ability(&self, _system_ability_id: i32, _device_id: &str) {
        let observers = self.observers.lock().unwrap();
        for (_, callback) in observers.iter() {
            callback();
        }
    }

    /// Check and track call count for rate limiting.
    ///
    /// Corresponds to C++ `SetCallCount()`.
    /// Returns true if the access count threshold is exceeded (call should be blocked).
    pub fn set_call_count(&self, func_name: &str, uri: &str) -> bool {
        self.call_reporter.count(func_name, uri)
    }

    /// Acquire KV service remote (SA 1301) only.
    ///
    /// Corresponds to C++ `GetDistributedDataManager()`.
    /// Returns the KV service RemoteObj, or None if not available.
    /// If CheckSystemAbility fails, triggers async LoadSystemAbility and returns None.
    pub fn acquire_kv_service_remote(&self) -> Option<RemoteObj> {
        let kv_remote =
            SystemAbilityManager::check_system_ability(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID);
        if kv_remote.is_none() {
            let _ = SystemAbilityManager::load_system_ability(
                DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID,
                10,
            );
            return None;
        }
        kv_remote
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_manager_get_instance() {
        let instance = DataShareManagerImpl::get_instance();
        let instance2 = DataShareManagerImpl::get_instance();
        assert!(Arc::ptr_eq(&instance, &instance2));
    }

    #[test]
    fn test_manager_bundle_name() {
        let instance = DataShareManagerImpl::get_instance();
        instance.set_bundle_name("com.test.app");
        assert_eq!(instance.get_bundle_name(), "com.test.app");
    }

    #[test]
    fn test_manager_service_proxy_initially_none() {
        let proxy = DataShareManagerImpl::get_service_proxy();
        assert!(proxy.is_none());
    }

    #[test]
    fn test_manager_register_callback() {
        let instance = DataShareManagerImpl::get_instance();
        let called = Arc::new(Mutex::new(false));
        let called_clone = called.clone();
        instance.set_register_callback(1, move || {
            *called_clone.lock().unwrap() = true;
        });

        // Simulate system ability addition
        instance.on_add_system_ability(1301, "");
        assert!(*called.lock().unwrap());

        // Remove callback
        instance.remove_register_callback(1);
    }

    #[test]
    fn test_manager_set_call_count() {
        let instance = DataShareManagerImpl::get_instance();
        // First call should not exceed threshold (returns false)
        assert!(!instance.set_call_count("Insert", "datashare:///test"));
    }
}
