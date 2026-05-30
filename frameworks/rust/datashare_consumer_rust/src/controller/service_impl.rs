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

//! GeneralControllerServiceImpl — silent access path controller.
//!
//! Corresponds to C++ `GeneralControllerServiceImpl` in
//! `frameworks/native/consumer/controller/service/src/general_controller_service_impl.cpp` (337 lines).
//!
//! Data flow: App → DataShareHelper → GeneralControllerServiceImpl
//!     → DataShareServiceProxy → IPC → DataShareService
//!
//! Features:
//! - Call count threshold checking via DataShareManagerImpl
//! - Timed query with executor pool and timeout
//! - Retry logic for busy result sets (up to MAX_RETRY_COUNT)
//! - Observer re-registration on service restart

use std::sync::{Arc, Condvar, Mutex, RwLock};
use std::time::Duration;

use datashare_common::observer::ChangeInfo;
use datashare_common::predicates::DataSharePredicates;
use datashare_common::types::{DataShareOption, UriInfo};
use datashare_common::values_bucket::DataShareValuesBucket;

use datashare_resultset::ipc::proxy::ISharedResultSetProxy;

use super::general_controller::{DatashareBusinessError, GeneralController, DATA_SHARE_ERROR};
use crate::connection::DataShareManagerImpl;

/// Maximum number of retry attempts when result set is busy.
const MAX_RETRY_COUNT: i32 = 3;

/// Minimum random delay for retry (milliseconds).
const RANDOM_MIN: u64 = 50;

/// Maximum random delay for retry (milliseconds).
const RANDOM_MAX: u64 = 150;

/// Timed query result container.
/// Corresponds to C++ `TimedQueryResult` struct.
pub struct TimedQueryResult {
    pub is_finish: bool,
    pub business_error: DatashareBusinessError,
    pub result_set: Option<ISharedResultSetProxy>,
}

impl std::fmt::Debug for TimedQueryResult {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("TimedQueryResult")
            .field("is_finish", &self.is_finish)
            .field("business_error", &self.business_error)
            .field(
                "result_set",
                &self.result_set.as_ref().map(|_| "ISharedResultSetProxy"),
            )
            .finish()
    }
}

impl TimedQueryResult {
    pub fn new(is_finish: bool, business_error: DatashareBusinessError) -> Self {
        Self {
            is_finish,
            business_error,
            result_set: None,
        }
    }
}

/// GeneralControllerServiceImpl — controller for silent (service) access path.
///
/// Corresponds to C++ class `GeneralControllerServiceImpl : public GeneralController`.
///
/// All CRUD operations are routed through `DataShareServiceProxy` via IPC,
/// with call count checking and optional timed execution.
pub struct GeneralControllerServiceImpl {
    /// Extension URI string
    ext_uri: RwLock<String>,

    /// Observer map: observer_id → list of URIs (Vec for small N, avoids HashMap monomorphization)
    observers: Mutex<Vec<(u64, Vec<String>)>>,

    /// Unique controller ID for register callback
    controller_id: u64,
}

impl GeneralControllerServiceImpl {
    /// Create a new service controller with the given extension URI.
    ///
    /// Corresponds to C++ constructor `GeneralControllerServiceImpl(const string& ext)`.
    /// Creates an executor pool with MAX_THREADS/MIN_THREADS for timed queries.
    pub fn new(ext_uri: String) -> Self {
        static NEXT_ID: std::sync::atomic::AtomicU64 = std::sync::atomic::AtomicU64::new(1);
        Self {
            ext_uri: RwLock::new(ext_uri),
            observers: Mutex::new(Vec::new()),
            controller_id: NEXT_ID.fetch_add(1, std::sync::atomic::Ordering::Relaxed),
        }
    }

    /// Get the extension URI.
    pub fn ext_uri(&self) -> String {
        self.ext_uri.read().unwrap().clone()
    }

    /// Re-register all observers after service restart.
    ///
    /// Corresponds to C++ `ReRegisterObserver()`.
    /// Moves out existing observer map, clears it, and re-registers each observer.
    fn re_register_observer(&self) {
        let mut observers = self.observers.lock().unwrap();
        let old_observers: Vec<(u64, Vec<String>)> = std::mem::take(&mut *observers);
        drop(observers);

        for (observer_id, uris) in old_observers {
            for uri in &uris {
                self.register_observer(uri, observer_id);
            }
        }
    }

    /// Set the register callback for service restart re-registration.
    ///
    /// Corresponds to C++ `SetRegisterCallback()`.
    /// When the distributed data service restarts, it triggers re-registration
    /// of all observers.
    pub fn set_register_callback(&self) {
        let instance = DataShareManagerImpl::get_instance();
        let controller_id = self.controller_id;
        // Safety: The callback is removed in Drop before self is deallocated,
        // matching C++ where ~GeneralControllerServiceImpl calls RemoveRegisterCallback(this).
        let self_ptr = self as *const Self as usize;
        instance.set_register_callback(controller_id, move || unsafe {
            let controller = &*(self_ptr as *const GeneralControllerServiceImpl);
            controller.re_register_observer();
        });
    }

    /// Execute a timed query with timeout.
    ///
    /// Corresponds to C++ `TimedQuery()`.
    /// Spawns a task in the executor pool that calls proxy->Query with retry logic.
    /// If the task does not finish within `timeout_ms`, returns E_TIMEOUT_ERROR.
    fn timed_query(
        &self,
        uri_info: &UriInfo,
        predicates: &DataSharePredicates,
        columns: &[String],
    ) -> (Option<ISharedResultSetProxy>, DatashareBusinessError) {
        let timeout_ms = uri_info.option.timeout as u64;
        let uri = uri_info.uri.clone();
        let ext_uri = uri_info.ext_uri.clone();
        let preds = predicates.clone();
        let cols: Vec<String> = columns.to_vec();

        let (tx, rx) = oneshot_channel();

        std::thread::spawn(move || {
            let mut business_error = DatashareBusinessError::new();
            let mut result_set = None;

            for attempt in 0..MAX_RETRY_COUNT {
                let proxy = match DataShareManagerImpl::get_service_proxy() {
                    Some(p) => p,
                    None => break,
                };
                let reply = proxy.query(&uri, &ext_uri, &preds, &cols, &mut business_error);
                if let Some(mut r) = reply {
                    result_set = datashare_resultset::ipc::interface::read_from_parcel(&mut r);
                    break;
                }
                if attempt < MAX_RETRY_COUNT - 1 {
                    let delay = RANDOM_MIN
                        + (attempt as u64) * (RANDOM_MAX - RANDOM_MIN) / (MAX_RETRY_COUNT as u64);
                    std::thread::sleep(Duration::from_millis(delay));
                }
            }

            tx.send((result_set, business_error));
        });

        match rx.recv_timeout(Duration::from_millis(timeout_ms)) {
            Some((result_set, err)) => (result_set, err),
            None => {
                let mut err = DatashareBusinessError::new();
                err.set_code(1072); // E_TIMEOUT_ERROR
                err.set_message("timed query timeout".to_string());
                (None, err)
            }
        }
    }

    fn timed_query_raw(
        &self,
        uri_info: &UriInfo,
        predicates: &DataSharePredicates,
        columns: &[String],
    ) -> (Option<ipc::parcel::MsgParcel>, DatashareBusinessError) {
        let timeout_ms = uri_info.option.timeout as u64;
        let uri = uri_info.uri.clone();
        let ext_uri = uri_info.ext_uri.clone();
        let preds = predicates.clone();
        let cols: Vec<String> = columns.to_vec();

        let (tx, rx) = oneshot_channel();

        std::thread::spawn(move || {
            let mut business_error = DatashareBusinessError::new();
            let proxy = match DataShareManagerImpl::get_service_proxy() {
                Some(p) => p,
                None => {
                    business_error.set_code(DATA_SHARE_ERROR);
                    tx.send((None, business_error));
                    return;
                }
            };
            let reply = proxy.query(&uri, &ext_uri, &preds, &cols, &mut business_error);
            tx.send((reply, business_error));
        });

        match rx.recv_timeout(Duration::from_millis(timeout_ms)) {
            Some((reply, err)) => (reply, err),
            None => {
                let mut err = DatashareBusinessError::new();
                err.set_code(1072); // E_TIMEOUT_ERROR
                err.set_message("timed query timeout".to_string());
                (None, err)
            }
        }
    }
}

fn oneshot_channel<T>() -> (OneShotTx<T>, OneShotRx<T>) {
    let shared = Arc::new((Mutex::new(None::<T>), Condvar::new()));
    (OneShotTx(shared.clone()), OneShotRx(shared))
}

struct OneShotTx<T>(Arc<(Mutex<Option<T>>, Condvar)>);
struct OneShotRx<T>(Arc<(Mutex<Option<T>>, Condvar)>);

impl<T> OneShotTx<T> {
    fn send(self, val: T) {
        let (lock, cvar) = &*self.0;
        *lock.lock().unwrap() = Some(val);
        cvar.notify_one();
    }
}

impl<T> OneShotRx<T> {
    fn recv_timeout(&self, timeout: Duration) -> Option<T> {
        let (lock, cvar) = &*self.0;
        let mut guard = lock.lock().unwrap();
        if guard.is_some() {
            return guard.take();
        }
        let (mut guard, _) = cvar.wait_timeout(guard, timeout).unwrap();
        guard.take()
    }
}

/// Validate that a URI is a valid extension URI.
/// Must start with "datashare:///" and not contain "?".
fn is_ext_uri(uri: &str) -> bool {
    if uri.contains('?') {
        return false;
    }
    let prefix = "datashare:///";
    if !uri.starts_with(prefix) {
        return false;
    }
    uri.len() > prefix.len()
}

impl Drop for GeneralControllerServiceImpl {
    fn drop(&mut self) {
        let instance = DataShareManagerImpl::get_instance();
        instance.remove_register_callback(self.controller_id);
    }
}

impl GeneralController for GeneralControllerServiceImpl {
    fn insert(&self, uri: &str, value: &DataShareValuesBucket) -> i32 {
        let instance = DataShareManagerImpl::get_instance();
        if instance.set_call_count("Insert", uri) {
            return DATA_SHARE_ERROR;
        }
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return DATA_SHARE_ERROR,
        };
        let ext_uri = self.ext_uri.read().unwrap();
        let r = proxy.insert(uri, &ext_uri, value);
        r
    }

    fn update(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> i32 {
        let instance = DataShareManagerImpl::get_instance();
        if instance.set_call_count("Update", uri) {
            return DATA_SHARE_ERROR;
        }
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return DATA_SHARE_ERROR,
        };
        let ext_uri = self.ext_uri.read().unwrap();
        proxy.update(uri, &ext_uri, predicates, value)
    }

    fn delete(&self, uri: &str, predicates: &DataSharePredicates) -> i32 {
        let instance = DataShareManagerImpl::get_instance();
        if instance.set_call_count("Delete", uri) {
            return DATA_SHARE_ERROR;
        }
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return DATA_SHARE_ERROR,
        };
        let ext_uri = self.ext_uri.read().unwrap();
        proxy.delete(uri, &ext_uri, predicates)
    }

    fn query(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        columns: &[String],
        business_error: &mut DatashareBusinessError,
        option: &DataShareOption,
    ) -> Option<ISharedResultSetProxy> {
        let instance = DataShareManagerImpl::get_instance();
        if instance.set_call_count("Query", uri) {
            return None;
        }

        if option.timeout > 0 {
            let uri_info = UriInfo {
                uri: uri.to_string(),
                ext_uri: self.ext_uri.read().unwrap().clone(),
                option: DataShareOption {
                    timeout: option.timeout,
                },
            };
            let (result_set, err) = self.timed_query(&uri_info, predicates, columns);
            *business_error = err;
            return result_set;
        }

        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => {
                business_error.set_code(DATA_SHARE_ERROR);
                return None;
            }
        };
        let ext_uri = self.ext_uri.read().unwrap();
        let mut reply = proxy.query(uri, &ext_uri, predicates, columns, business_error)?;
        datashare_resultset::ipc::interface::read_from_parcel(&mut reply)
    }

    fn query_raw(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        columns: &[String],
        business_error: &mut DatashareBusinessError,
        option: &DataShareOption,
    ) -> Option<ipc::parcel::MsgParcel> {
        let instance = DataShareManagerImpl::get_instance();
        if instance.set_call_count("Query", uri) {
            return None;
        }

        if option.timeout > 0 {
            let uri_info = UriInfo {
                uri: uri.to_string(),
                ext_uri: self.ext_uri.read().unwrap().clone(),
                option: DataShareOption {
                    timeout: option.timeout,
                },
            };
            let (result, err) = self.timed_query_raw(&uri_info, predicates, columns);
            *business_error = err;
            return result;
        }

        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => {
                business_error.set_code(DATA_SHARE_ERROR);
                return None;
            }
        };
        let ext_uri = self.ext_uri.read().unwrap();
        proxy.query(uri, &ext_uri, predicates, columns, business_error)
    }

    fn register_observer(&self, uri: &str, observer_id: u64) -> i32 {
        let mut observers = self.observers.lock().unwrap();
        if let Some((_, uris)) = observers.iter_mut().find(|(id, _)| *id == observer_id) {
            uris.push(uri.to_string());
        } else {
            observers.push((observer_id, vec![uri.to_string()]));
        }
        drop(observers);

        unsafe {
            crate::ffi::ability_mgr_ffi::DataShareDataObsMgrRegisterObserverSilent(
                uri.as_ptr(),
                uri.len() as u32,
                observer_id as *mut std::ffi::c_void,
            )
        }
    }

    fn unregister_observer(&self, uri: &str, observer_id: u64) -> i32 {
        let mut observers = self.observers.lock().unwrap();
        if let Some(pos) = observers.iter().position(|(id, _)| *id == observer_id) {
            observers[pos].1.retain(|u| u != uri);
            if observers[pos].1.is_empty() {
                observers.swap_remove(pos);
            }
        }
        drop(observers);

        unsafe {
            crate::ffi::ability_mgr_ffi::DataShareDataObsMgrUnregisterObserverSilent(
                uri.as_ptr(),
                uri.len() as u32,
                observer_id as *mut std::ffi::c_void,
            )
        }
    }

    fn notify_change(&self, uri: &str) {
        if let Some(proxy) = DataShareManagerImpl::get_service_proxy() {
            proxy.notify(uri);
        }
    }

    fn register_observer_ext_provider(
        &self,
        _uri: &str,
        _observer_id: u64,
        _is_descendants: bool,
    ) -> i32 {
        // Not supported for service (silent) path
        DATA_SHARE_ERROR
    }

    fn unregister_observer_ext_provider(&self, _uri: &str, _observer_id: u64) -> i32 {
        // Not supported for service (silent) path
        DATA_SHARE_ERROR
    }

    fn notify_change_ext_provider(&self, _change_info: &ChangeInfo) -> i32 {
        // Not supported for service (silent) path
        DATA_SHARE_ERROR
    }

    fn insert_ex(&self, uri: &str, value: &DataShareValuesBucket) -> (i32, i32) {
        let instance = DataShareManagerImpl::get_instance();
        if instance.set_call_count("InsertEx", uri) {
            return (DATA_SHARE_ERROR, 0);
        }
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return (DATA_SHARE_ERROR, 0),
        };
        let ext_uri = self.ext_uri.read().unwrap();
        proxy.insert_ex(uri, &ext_uri, value)
    }

    fn update_ex(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> (i32, i32) {
        let instance = DataShareManagerImpl::get_instance();
        if instance.set_call_count("UpdateEx", uri) {
            return (DATA_SHARE_ERROR, 0);
        }
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return (DATA_SHARE_ERROR, 0),
        };
        let ext_uri = self.ext_uri.read().unwrap();
        proxy.update_ex(uri, &ext_uri, predicates, value)
    }

    fn delete_ex(&self, uri: &str, predicates: &DataSharePredicates) -> (i32, i32) {
        let instance = DataShareManagerImpl::get_instance();
        if instance.set_call_count("DeleteEx", uri) {
            return (DATA_SHARE_ERROR, 0);
        }
        let proxy = match DataShareManagerImpl::get_service_proxy() {
            Some(p) => p,
            None => return (DATA_SHARE_ERROR, 0),
        };
        let ext_uri = self.ext_uri.read().unwrap();
        proxy.delete_ex(uri, &ext_uri, predicates)
    }

    fn set_ext_uri(&self, ext_uri: &str) -> i32 {
        if !is_ext_uri(ext_uri) {
            return 1078; // E_DATASHARE_INVALID_URI
        }
        *self.ext_uri.write().unwrap() = ext_uri.to_string();
        0
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_service_impl_construction() {
        let controller = GeneralControllerServiceImpl::new("datashare:///test".to_string());
        assert_eq!(controller.ext_uri(), "datashare:///test");
    }

    #[test]
    fn test_service_impl_ext_provider_not_supported() {
        let controller = GeneralControllerServiceImpl::new("datashare:///test".to_string());
        assert_eq!(
            controller.register_observer_ext_provider("uri", 1, false),
            DATA_SHARE_ERROR
        );
        assert_eq!(
            controller.unregister_observer_ext_provider("uri", 1),
            DATA_SHARE_ERROR
        );

        let change_info = ChangeInfo::new(datashare_common::observer::ChangeType::Insert);
        assert_eq!(
            controller.notify_change_ext_provider(&change_info),
            DATA_SHARE_ERROR
        );
    }

    #[test]
    fn test_service_impl_insert_ex_placeholder() {
        let controller = GeneralControllerServiceImpl::new("datashare:///test".to_string());
        let vb = DataShareValuesBucket::new();
        let (code, val) = controller.insert_ex("uri", &vb);
        assert_eq!(code, DATA_SHARE_ERROR);
        assert_eq!(val, 0);
    }

    #[test]
    fn test_constants() {
        assert_eq!(MAX_RETRY_COUNT, 3);
        assert!(RANDOM_MIN < RANDOM_MAX);
    }
}
