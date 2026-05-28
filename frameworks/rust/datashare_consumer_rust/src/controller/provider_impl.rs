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

//! GeneralControllerProviderImpl — non-silent access path controller.
//!
//! Corresponds to C++ `GeneralControllerProviderImpl` in
//! `frameworks/native/consumer/controller/service/src/general_controller_provider_impl.cpp` (263 lines).
//!
//! Data flow: App → DataShareHelper → GeneralControllerProviderImpl
//!     → DataShareConnection → DataShareProxy → IPC → DataShare extension
//!
//! Features:
//! - All CRUD operations via DataShareProxy obtained from DataShareConnection
//! - Observer registration at provider level (non-silent only)
//! - Support for extension-specific operations

use std::collections::HashMap;
use std::sync::{Arc, Mutex};

use ipc::remote::RemoteObj;

use datashare_common::observer::ChangeInfo;
use datashare_common::predicates::DataSharePredicates;
use datashare_common::types::{DataShareOption, UriInfo};
use datashare_common::values_bucket::DataShareValuesBucket;

use datashare_resultset::ipc::proxy::ISharedResultSetProxy;

use super::general_controller::{DatashareBusinessError, GeneralController, DATA_SHARE_ERROR};
use crate::connection::DataShareConnection;

/// GeneralControllerProviderImpl — controller for non-silent (provider) access path.
///
/// Corresponds to C++ class `GeneralControllerProviderImpl : public GeneralController`.
///
/// All CRUD operations are routed through `DataShareConnection` to obtain a
/// `DataShareProxy`, which communicates with the DataShare extension via IPC.
pub struct GeneralControllerProviderImpl {
    /// Extension URI string
    uri: String,

    /// Extension URI string (may differ from uri in some contexts)
    ext_uri: String,

    /// Connection to the DataShare extension
    connection: Arc<DataShareConnection>,

    /// Observer map: observer_id → list of URIs (for provider-level observers)
    observers: Mutex<HashMap<u64, Vec<String>>>,
}

impl GeneralControllerProviderImpl {
    /// Create a new provider controller with the given connection.
    pub fn new(connection: Arc<DataShareConnection>, uri: String, ext_uri: String) -> Self {
        Self {
            uri,
            ext_uri,
            connection,
            observers: Mutex::new(HashMap::new()),
        }
    }

    /// Get the URI.
    pub fn uri(&self) -> &str {
        &self.uri
    }

    /// Get the extension URI.
    pub fn ext_uri(&self) -> &str {
        &self.ext_uri
    }
}

impl GeneralController for GeneralControllerProviderImpl {
    fn insert(&self, uri: &str, value: &DataShareValuesBucket) -> i32 {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return DATA_SHARE_ERROR,
        };
        proxy.insert(uri, value)
    }

    fn update(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> i32 {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return DATA_SHARE_ERROR,
        };
        proxy.update(uri, predicates, value)
    }

    fn delete(&self, uri: &str, predicates: &DataSharePredicates) -> i32 {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return DATA_SHARE_ERROR,
        };
        proxy.delete(uri, predicates)
    }

    fn query(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        columns: &[String],
        business_error: &mut DatashareBusinessError,
        _option: &DataShareOption,
    ) -> Option<ISharedResultSetProxy> {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => {
                business_error.set_code(DATA_SHARE_ERROR);
                return None;
            }
        };
        let mut reply = proxy.query(uri, predicates, columns, business_error)?;
        datashare_resultset::ipc::interface::read_from_parcel(&mut reply)
    }

    fn query_raw(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        columns: &[String],
        business_error: &mut DatashareBusinessError,
        _option: &DataShareOption,
    ) -> Option<ipc::parcel::MsgParcel> {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => {
                business_error.set_code(DATA_SHARE_ERROR);
                return None;
            }
        };
        proxy.query(uri, predicates, columns, business_error)
    }

    fn register_observer(&self, uri: &str, observer_id: u64) -> i32 {
        let proxy_remote = self.connection.get_proxy_remote_raw();
        if proxy_remote.is_null() {
            return 1066; // E_PROVIDER_NOT_CONNECTED
        }
        let ret = unsafe {
            crate::ffi::ability_mgr_ffi::DataShareProxyRegisterObserver(
                proxy_remote,
                uri.as_ptr(),
                uri.len() as u32,
                observer_id as *mut std::ffi::c_void,
            )
        };
        if ret != 0 {
            0
        } else {
            1070
        }
    }

    fn unregister_observer(&self, uri: &str, observer_id: u64) -> i32 {
        let proxy_remote = self.connection.get_proxy_remote_raw();
        if proxy_remote.is_null() {
            return 1066;
        }
        let ret = unsafe {
            crate::ffi::ability_mgr_ffi::DataShareProxyUnregisterObserver(
                proxy_remote,
                uri.as_ptr(),
                uri.len() as u32,
                observer_id as *mut std::ffi::c_void,
            )
        };
        if ret != 0 {
            0
        } else {
            1070
        }
    }

    fn notify_change(&self, uri: &str) {
        if let Some(proxy) = self.connection.get_data_share_proxy() {
            proxy.notify_change(uri);
        }
    }

    fn register_observer_ext_provider(
        &self,
        uri: &str,
        observer_id: u64,
        is_descendants: bool,
    ) -> i32 {
        let proxy_remote = self.connection.get_proxy_remote_raw();
        if proxy_remote.is_null() {
            return 1066; // E_PROVIDER_NOT_CONNECTED
        }
        let mut flags: u32 = 0;
        if is_descendants {
            flags |= crate::ffi::ability_mgr_ffi::DATASHARE_OBSERVER_FLAG_DESCENDANTS;
        }
        let ret = unsafe {
            crate::ffi::ability_mgr_ffi::DataShareProxyRegisterObserverExtProvider(
                proxy_remote,
                uri.as_ptr(),
                uri.len() as u32,
                observer_id as *mut std::ffi::c_void,
                flags,
            )
        };
        if ret == 0 {
            self.connection
                .update_observer_exts_provider_map(uri, observer_id, is_descendants);
        }
        ret
    }

    fn unregister_observer_ext_provider(&self, uri: &str, observer_id: u64) -> i32 {
        let proxy_remote = self.connection.get_proxy_remote_raw();
        if proxy_remote.is_null() {
            return 1066; // E_PROVIDER_NOT_CONNECTED
        }
        let ret = unsafe {
            crate::ffi::ability_mgr_ffi::DataShareProxyUnregisterObserverExtProvider(
                proxy_remote,
                uri.as_ptr(),
                uri.len() as u32,
                observer_id as *mut std::ffi::c_void,
            )
        };
        if ret == 0 {
            self.connection
                .delete_observer_exts_provider_map(uri, observer_id);
        }
        ret
    }

    fn notify_change_ext_provider(&self, change_info: &ChangeInfo) -> i32 {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return 1066, // E_PROVIDER_NOT_CONNECTED
        };
        proxy.notify_change_ext_provider(change_info)
    }

    fn insert_ex(&self, uri: &str, value: &DataShareValuesBucket) -> (i32, i32) {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return (DATA_SHARE_ERROR, 0),
        };
        proxy.insert_ex(uri, value)
    }

    fn update_ex(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> (i32, i32) {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return (DATA_SHARE_ERROR, 0),
        };
        proxy.update_ex(uri, predicates, value)
    }

    fn delete_ex(&self, uri: &str, predicates: &DataSharePredicates) -> (i32, i32) {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return (DATA_SHARE_ERROR, 0),
        };
        proxy.delete_ex(uri, predicates)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn make_controller() -> GeneralControllerProviderImpl {
        let conn = Arc::new(DataShareConnection::new(
            "datashare:///test".to_string(),
            None,
            2,
        ));
        GeneralControllerProviderImpl::new(
            conn,
            "datashare:///test".to_string(),
            "datashare:///ext".to_string(),
        )
    }

    #[test]
    fn test_provider_impl_construction() {
        let controller = make_controller();
        assert_eq!(controller.uri(), "datashare:///test");
        assert_eq!(controller.ext_uri(), "datashare:///ext");
    }

    #[test]
    fn test_provider_impl_register_observer_ext() {
        let controller = make_controller();
        let result = controller.register_observer_ext_provider("uri1", 1, false);
        assert_eq!(result, 0);
    }

    #[test]
    fn test_provider_impl_unregister_observer_ext() {
        let controller = make_controller();
        controller.register_observer_ext_provider("uri1", 1, false);
        let result = controller.unregister_observer_ext_provider("uri1", 1);
        assert_eq!(result, 0);
    }
}
