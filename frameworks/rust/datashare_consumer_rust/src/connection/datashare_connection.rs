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

//! DataShareConnection — ability extension connection lifecycle manager.
//!
//! Corresponds to C++ `DataShareConnection` in
//! `frameworks/native/consumer/src/datashare_connection.cpp` (311 lines).
//!
//! This class manages the lifecycle of connections to DataShare extensions.
//! It inherits from `AbilityConnectionStub` in C++ and handles:
//! - Connection establishment via AbilityManager
//! - Automatic reconnection on disconnect (up to MAX_RECONNECT attempts)
//! - Observer re-registration on reconnect
//! - DataShareProxy creation from remote object
//!
//! In Rust, the ability connection callbacks are modeled as trait methods.

use std::collections::HashMap;
use std::ffi::c_void;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::{Arc, Condvar, Mutex};
use std::time::Duration;

use ipc::remote::RemoteObj;

use crate::ffi::ability_mgr_ffi;
use crate::proxy::DataShareProxy;

/// Maximum number of reconnection threads.
const MAX_THREADS: usize = 2;

/// Minimum number of reconnection threads.
const MIN_THREADS: usize = 0;

/// Maximum number of reconnection attempts.
const MAX_RECONNECT: i32 = 6;

/// Time interval between reconnection attempts (milliseconds).
const RECONNECT_TIME_INTERVAL_MS: u64 = 10_000;

/// Maximum time window for reconnection attempts (milliseconds).
const MAX_RECONNECT_TIME_INTERVAL_MS: u64 = 70_000;

/// Connection timing threshold for warnings (milliseconds).
const TIME_THRESHOLD_MS: u64 = 200;

/// Information about reconnection attempts.
#[derive(Debug, Default)]
struct ConnectionInfo {
    count: i32,
    first_time: i64,
    prev_time: i64,
}

/// Observer parameter for re-registration.
#[derive(Debug, Clone)]
struct ObserverParam {
    uri: String,
    is_descendants: bool,
}

/// DataShareConnection — manages the connection lifecycle to DataShare extensions.
///
/// Corresponds to C++ `DataShareConnection : public AbilityConnectionStub`.
///
/// Key features:
/// - Connects to DataShare extension via AbilityManager (AbilityMgrProxy)
/// - Creates DataShareProxy when connection is established
/// - Handles automatic reconnection on unexpected disconnect
/// - Re-registers observers after successful reconnection
/// - Thread-safe via internal mutex
pub struct DataShareConnection {
    /// URI for the extension
    uri: String,

    /// Remote object token (caller's identity)
    token: Option<RemoteObj>,

    /// Raw pointer to the caller token's IRemoteObject (for FFI calls)
    token_raw: *mut c_void,

    /// Wait time for connection (seconds)
    wait_time: i32,

    /// The proxy to the DataShare extension
    proxy: Mutex<Option<Arc<DataShareProxy>>>,

    /// Condition variable for connection waiting
    connect_cond: Condvar,

    /// Whether the connection has been invalidated
    is_invalid: AtomicBool,

    /// Whether a reconnection is in progress
    is_reconnect: AtomicBool,

    /// Whether callbacks are still active (cleared in drop)
    callback_active: AtomicBool,

    /// Reconnection tracking information
    reconnects: Mutex<ConnectionInfo>,

    /// Observer map for re-registration: observer_id → list of params
    observer_exts_provider: Mutex<HashMap<u64, Vec<ObserverParam>>>,

    /// Connection stub handle returned by C++ FFI (AbilityConnectionStub*)
    connection_handle: Mutex<*mut c_void>,

    /// Raw IRemoteObject* pointer stored during on_connect_ffi callback.
    /// Used by FFI to return the pointer to C++ for constructing C++ DataShareProxy.
    proxy_remote_raw: Mutex<*mut c_void>,
}

// Safety: connection_handle and token_raw are only accessed under mutex / in FFI calls
unsafe impl Send for DataShareConnection {}
unsafe impl Sync for DataShareConnection {}

impl DataShareConnection {
    /// Create a new DataShareConnection.
    ///
    /// Corresponds to C++ constructor `DataShareConnection(const Uri& uri, const sptr<IRemoteObject>& token, int32_t waitTime)`.
    pub fn new(uri: String, token: Option<RemoteObj>, wait_time: i32) -> Self {
        Self {
            uri,
            token,
            token_raw: std::ptr::null_mut(),
            wait_time,
            proxy: Mutex::new(None),
            connect_cond: Condvar::new(),
            is_invalid: AtomicBool::new(false),
            is_reconnect: AtomicBool::new(false),
            callback_active: AtomicBool::new(true),
            reconnects: Mutex::new(ConnectionInfo::default()),
            observer_exts_provider: Mutex::new(HashMap::new()),
            connection_handle: Mutex::new(std::ptr::null_mut()),
            proxy_remote_raw: Mutex::new(std::ptr::null_mut()),
        }
    }

    /// Create with a raw token pointer for FFI use.
    pub fn new_with_token_raw(
        uri: String,
        token: Option<RemoteObj>,
        token_raw: *mut c_void,
        wait_time: i32,
    ) -> Self {
        Self {
            uri,
            token,
            token_raw,
            wait_time,
            proxy: Mutex::new(None),
            connect_cond: Condvar::new(),
            is_invalid: AtomicBool::new(false),
            is_reconnect: AtomicBool::new(false),
            callback_active: AtomicBool::new(true),
            reconnects: Mutex::new(ConnectionInfo::default()),
            observer_exts_provider: Mutex::new(HashMap::new()),
            connection_handle: Mutex::new(std::ptr::null_mut()),
            proxy_remote_raw: Mutex::new(std::ptr::null_mut()),
        }
    }

    /// Create with default wait time of 2 seconds.
    pub fn with_default_wait(uri: String, token: Option<RemoteObj>) -> Self {
        Self::new(uri, token, 2)
    }

    /// Get the URI.
    pub fn uri(&self) -> &str {
        &self.uri
    }

    /// Get the token.
    pub fn token(&self) -> Option<&RemoteObj> {
        self.token.as_ref()
    }

    /// Called when the ability connection is established.
    ///
    /// Corresponds to C++ `OnAbilityConnectDone()`.
    /// Creates a DataShareProxy from the remote object.
    pub fn on_ability_connect_done(&self, remote_object: RemoteObj, _result_code: i32) {
        let proxy = Arc::new(DataShareProxy::new(remote_object));
        {
            let mut p = self.proxy.lock().unwrap();
            *p = Some(proxy);
        }
        // Wake up any thread waiting in connect_data_share_ext_ability
        self.connect_cond.notify_all();

        if self.is_invalid.load(Ordering::SeqCst) {
            self.disconnect();
            return;
        }
        if self.is_reconnect.load(Ordering::SeqCst) {
            self.re_register_observer_ext_provider();
            self.is_reconnect.store(false, Ordering::SeqCst);
            let mut reconnects = self.reconnects.lock().unwrap();
            *reconnects = ConnectionInfo::default();
        }
    }

    /// Called when the ability connection is unexpectedly lost.
    ///
    /// Corresponds to C++ `OnAbilityDisconnectDone()`.
    /// Clears the proxy and initiates reconnection.
    pub fn on_ability_disconnect_done(&self, _result_code: i32) {
        {
            let mut proxy = self.proxy.lock().unwrap();
            *proxy = None;
        }
        *self.proxy_remote_raw.lock().unwrap() = std::ptr::null_mut();
        let uri = self.uri.clone();
        self.reconnect_ext_ability(&uri);
    }

    /// Get the DataShareProxy, connecting if necessary.
    ///
    /// Corresponds to C++ `GetDataShareProxy(const Uri& uri, const sptr<IRemoteObject>& token)`.
    pub fn get_data_share_proxy(&self) -> Option<Arc<DataShareProxy>> {
        // If already connected, return cached proxy
        if let Some(proxy) = self.get_proxy() {
            return Some(proxy);
        }
        self.connect_data_share_ext_ability()
    }

    /// Get the current proxy without connecting.
    pub fn get_proxy(&self) -> Option<Arc<DataShareProxy>> {
        let proxy = self.proxy.lock().unwrap();
        proxy.clone()
    }

    /// Inject a proxy from an existing remote object (established by C++ side).
    /// Used when C++ has already connected to the ext ability and passes
    /// the remote object to Rust to avoid duplicate connection.
    pub fn set_proxy_from_remote(&self, remote: RemoteObj) {
        let proxy = Arc::new(DataShareProxy::new(remote));
        let mut p = self.proxy.lock().unwrap();
        *p = Some(proxy);
    }

    /// Connect to the DataShare extension.
    ///
    /// Corresponds to C++ `ConnectDataShareExtAbility()`.
    /// If already connected, returns the existing proxy.
    /// Otherwise, connects via AbilityManager and waits for the callback.
    fn connect_data_share_ext_ability(&self) -> Option<Arc<DataShareProxy>> {
        // Check if already connected
        if let Some(proxy) = self.get_proxy() {
            return Some(proxy);
        }

        // Connect via AbilityConnectionStub bridge — the C++ side creates an
        // AbilityConnectionStub that calls back on_connect_ffi / on_disconnect_ffi.
        let self_ptr = self as *const Self as *mut c_void;
        let cbs = ability_mgr_ffi::DataShareConnectionCallbacks {
            on_connect: Self::on_connect_ffi,
            on_disconnect: Self::on_disconnect_ffi,
        };
        let handle = unsafe {
            ability_mgr_ffi::DataShareConnectionConnectExt(
                self.uri.as_ptr(),
                self.uri.len() as u32,
                self.token_raw,
                self_ptr,
                &cbs,
            )
        };
        if !handle.is_null() {
            *self.connection_handle.lock().unwrap() = handle;
        }

        // Wait for on_ability_connect_done callback
        let proxy = self.proxy.lock().unwrap();
        let timeout = Duration::from_secs(self.wait_time as u64);
        let (proxy, _) = self.connect_cond.wait_timeout(proxy, timeout).unwrap();
        proxy.clone()
    }

    /// FFI callback: ability connection established.
    unsafe extern "C" fn on_connect_ffi(
        context: *mut c_void,
        remote_object: *mut c_void,
        result_code: i32,
    ) {
        let conn = &*(context as *const Self);
        if !conn.callback_active.load(Ordering::SeqCst) {
            return;
        }
        *conn.proxy_remote_raw.lock().unwrap() = remote_object;
        if let Some(remote) = RemoteObj::from_ciremote(remote_object as *mut _) {
            conn.on_ability_connect_done(remote, result_code);
        }
    }

    /// FFI callback: ability connection lost.
    unsafe extern "C" fn on_disconnect_ffi(context: *mut c_void, result_code: i32) {
        let conn = &*(context as *const Self);
        if !conn.callback_active.load(Ordering::SeqCst) {
            return;
        }
        conn.on_ability_disconnect_done(result_code);
    }

    /// Disconnect from the DataShare extension.
    ///
    /// Corresponds to C++ `DisconnectDataShareExtAbility()`.
    pub fn disconnect(&self) {
        {
            let proxy = self.proxy.lock().unwrap();
            if proxy.is_none() {
                return;
            }
        }

        *self.proxy_remote_raw.lock().unwrap() = std::ptr::null_mut();
        let mut handle = self.connection_handle.lock().unwrap();
        if !(*handle).is_null() {
            unsafe {
                ability_mgr_ffi::DataShareConnectionDisconnect(*handle);
            }
            *handle = std::ptr::null_mut();
        }
    }

    /// Get the raw IRemoteObject* pointer for FFI return to C++.
    pub fn get_proxy_remote_raw(&self) -> *mut c_void {
        *self.proxy_remote_raw.lock().unwrap()
    }

    /// Mark the connection as invalid.
    ///
    /// Corresponds to C++ `SetConnectInvalid()`.
    pub fn set_connect_invalid(&self) {
        self.is_invalid.store(true, Ordering::SeqCst);
    }

    /// Store observer info for re-registration on reconnect.
    ///
    /// Corresponds to C++ `UpdateObserverExtsProviderMap()`.
    pub fn update_observer_exts_provider_map(
        &self,
        uri: &str,
        observer_id: u64,
        is_descendants: bool,
    ) {
        let mut observers = self.observer_exts_provider.lock().unwrap();
        observers
            .entry(observer_id)
            .or_insert_with(Vec::new)
            .push(ObserverParam {
                uri: uri.to_string(),
                is_descendants,
            });
    }

    /// Remove observer info after unregistration.
    ///
    /// Corresponds to C++ `DeleteObserverExtsProviderMap()`.
    pub fn delete_observer_exts_provider_map(&self, uri: &str, observer_id: u64) {
        let mut observers = self.observer_exts_provider.lock().unwrap();
        if let Some(params) = observers.get_mut(&observer_id) {
            params.retain(|p| p.uri != uri);
            if params.is_empty() {
                observers.remove(&observer_id);
            }
        }
    }

    /// Attempt to reconnect to the extension.
    ///
    /// Corresponds to C++ `ReconnectExtAbility()`.
    fn reconnect_ext_ability(&self, _uri: &str) {
        let mut reconnects = self.reconnects.lock().unwrap();
        if reconnects.count == 0 {
            reconnects.count = 1;
            self.is_reconnect.store(true, Ordering::SeqCst);
            drop(reconnects);
            // First reconnection attempt — connect immediately
            let self_ptr = self as *const Self as *mut c_void;
            let cbs = ability_mgr_ffi::DataShareConnectionCallbacks {
                on_connect: Self::on_connect_ffi,
                on_disconnect: Self::on_disconnect_ffi,
            };
            let handle = unsafe {
                ability_mgr_ffi::DataShareConnectionConnectExt(
                    self.uri.as_ptr(),
                    self.uri.len() as u32,
                    self.token_raw,
                    self_ptr,
                    &cbs,
                )
            };
            if !handle.is_null() {
                *self.connection_handle.lock().unwrap() = handle;
            }
            return;
        }
        drop(reconnects);
        self.delay_connect_ext_ability(_uri);
    }

    /// Schedule a delayed reconnection attempt.
    ///
    /// Corresponds to C++ `DelayConnectExtAbility()`.
    fn delay_connect_ext_ability(&self, _uri: &str) {
        let mut reconnects = self.reconnects.lock().unwrap();
        if reconnects.count >= MAX_RECONNECT {
            return;
        }
        reconnects.count += 1;
        self.is_reconnect.store(true, Ordering::SeqCst);
        drop(reconnects);

        // Schedule delayed reconnection in a background thread
        let uri = self.uri.clone();
        let token_raw = self.token_raw;
        let self_ptr = self as *const Self as *mut c_void;
        let cbs = ability_mgr_ffi::DataShareConnectionCallbacks {
            on_connect: Self::on_connect_ffi,
            on_disconnect: Self::on_disconnect_ffi,
        };
        // Safety: token_raw and self_ptr are valid for the lifetime of the connection.
        // We use a helper to bypass the Send check on raw pointers.
        let task = move || {
            std::thread::sleep(Duration::from_millis(RECONNECT_TIME_INTERVAL_MS));
            let handle = unsafe {
                ability_mgr_ffi::DataShareConnectionConnectExt(
                    uri.as_ptr(),
                    uri.len() as u32,
                    token_raw,
                    self_ptr,
                    &cbs,
                )
            };
            if !handle.is_null() {
                let conn = unsafe { &*(self_ptr as *const Self) };
                if conn.callback_active.load(Ordering::SeqCst) {
                    *conn.connection_handle.lock().unwrap() = handle;
                }
            }
        };
        // Safety: raw pointers are valid for the connection's lifetime
        unsafe {
            let boxed: Box<dyn FnOnce() + 'static> = Box::new(task);
            let send_boxed: Box<dyn FnOnce() + Send + 'static> = std::mem::transmute(boxed);
            std::thread::spawn(send_boxed);
        }
    }

    /// Re-register all stored observers after reconnection.
    ///
    /// Corresponds to C++ `ReRegisterObserverExtProvider()`.
    fn re_register_observer_ext_provider(&self) {
        let mut observers = self.observer_exts_provider.lock().unwrap();
        let old_observers: HashMap<u64, Vec<ObserverParam>> = std::mem::take(&mut *observers);
        drop(observers);

        for (observer_id, params) in old_observers {
            for param in &params {
                let ret = ffi_dataobs_mgr::DataObsMgrClient::register_observer_ext(
                    &param.uri,
                    observer_id,
                    param.is_descendants,
                );
                if ret == 0 {
                    self.update_observer_exts_provider_map(
                        &param.uri,
                        observer_id,
                        param.is_descendants,
                    );
                }
            }
        }
    }
}

impl Drop for DataShareConnection {
    fn drop(&mut self) {
        // Deactivate callbacks before releasing the connection stub
        self.callback_active.store(false, Ordering::SeqCst);
        let handle = *self.connection_handle.lock().unwrap();
        if !handle.is_null() {
            unsafe {
                ability_mgr_ffi::DataShareConnectionDisconnect(handle);
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_connection_construction() {
        let conn = DataShareConnection::new("datashare:///test".to_string(), None, 2);
        assert_eq!(conn.uri(), "datashare:///test");
        assert!(conn.token().is_none());
    }

    #[test]
    fn test_connection_default_wait() {
        let conn = DataShareConnection::with_default_wait("datashare:///test".to_string(), None);
        assert_eq!(conn.wait_time, 2);
    }

    #[test]
    fn test_get_proxy_initially_none() {
        let conn = DataShareConnection::new("datashare:///test".to_string(), None, 2);
        assert!(conn.get_proxy().is_none());
    }

    #[test]
    fn test_set_connect_invalid() {
        let conn = DataShareConnection::new("datashare:///test".to_string(), None, 2);
        assert!(!conn.is_invalid.load(Ordering::SeqCst));
        conn.set_connect_invalid();
        assert!(conn.is_invalid.load(Ordering::SeqCst));
    }

    #[test]
    fn test_observer_map_update_and_delete() {
        let conn = DataShareConnection::new("datashare:///test".to_string(), None, 2);
        conn.update_observer_exts_provider_map("uri1", 1, false);
        conn.update_observer_exts_provider_map("uri2", 1, true);

        {
            let observers = conn.observer_exts_provider.lock().unwrap();
            assert_eq!(observers.get(&1).unwrap().len(), 2);
        }

        conn.delete_observer_exts_provider_map("uri1", 1);
        {
            let observers = conn.observer_exts_provider.lock().unwrap();
            assert_eq!(observers.get(&1).unwrap().len(), 1);
        }

        conn.delete_observer_exts_provider_map("uri2", 1);
        {
            let observers = conn.observer_exts_provider.lock().unwrap();
            assert!(observers.get(&1).is_none());
        }
    }
}
