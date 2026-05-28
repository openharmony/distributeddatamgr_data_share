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

//! DataShareHelperImpl — concrete implementation of DataShareHelper.
//!
//! Corresponds to C++ `DataShareHelperImpl` in
//! `frameworks/native/consumer/src/datashare_helper_impl.cpp` (814 lines).
//!
//! Routes all DataShare operations to the appropriate controllers:
//! - `GeneralController` for CRUD + observer operations
//! - `ExtSpecialController` for file/batch/extension operations
//! - `PersistentDataController` for RDB template subscriptions
//! - `PublishedDataController` for publish/subscribe data

use std::ffi::c_void;
use std::sync::{
    atomic::{AtomicBool, AtomicU64, Ordering},
    Arc,
};

use ipc::remote::RemoteObj;

use datashare_common::observer::ChangeInfo;
use datashare_common::predicates::DataSharePredicates;
use datashare_common::template::{Template, TemplateId};
use datashare_common::types::{
    Data, DataShareOption, OperationResult, PublishedDataChangeNode, RdbChangeNode,
    UpdateOperations,
};
use datashare_common::values_bucket::DataShareValuesBucket;

use datashare_resultset::ipc::proxy::ISharedResultSetProxy;

use crate::connection::DataShareConnection;
use crate::controller::general_controller::{DatashareBusinessError, DATA_SHARE_ERROR};
use crate::controller::GeneralController;
use crate::controller::{
    ExtSpecialController, GeneralControllerProviderImpl, GeneralControllerServiceImpl,
    PersistentDataController, PublishedDataController,
};
use crate::helper::datashare_helper::DataShareHelper;

/// Monotonic counter for generating unique helper IDs (analogous to C++ `this` pointer).
static NEXT_HELPER_ID: AtomicU64 = AtomicU64::new(1);

/// DataShareHelperImpl — concrete implementation routing to controllers.
///
/// Corresponds to C++ `DataShareHelperImpl : public DataShareHelper`.
///
/// Two construction modes:
/// - Non-silent: with DataShareConnection (provider path)
/// - Silent (service): only ext_uri string (service path)
pub struct DataShareHelperImpl {
    /// Unique ID for this helper instance, used as subscriber identity in
    /// RdbSubscriberManager / PublishedDataSubscriberManager.
    /// In C++ the `this` pointer serves this role.
    helper_id: u64,

    /// Extension special controller (file ops, batch ops)
    ext_sp_ctl: Option<ExtSpecialController>,

    /// General controller (CRUD + observer)
    general_ctl: Option<Box<dyn GeneralController>>,

    /// Persistent data controller (RDB template subscription)
    persistent_data_ctl: Option<PersistentDataController>,

    /// Published data controller (publish/subscribe)
    published_data_ctl: Option<PublishedDataController>,

    /// Whether this is a system app
    is_system: bool,

    /// Helper type: SILENT(0) or NON_SILENT(1), set at construction
    helper_type: i32,

    /// Whether release() has been called
    released: AtomicBool,

    /// Connection to ext ability (non-silent path only, for proxy injection)
    connection: Option<Arc<DataShareConnection>>,
}

impl DataShareHelperImpl {
    /// Create a non-silent (provider path) helper.
    pub fn new_provider(
        uri: &str,
        token: Option<RemoteObj>,
        token_raw: *mut c_void,
        is_system: bool,
    ) -> Self {
        let connection = Arc::new(DataShareConnection::new_with_token_raw(
            uri.to_string(),
            token,
            token_raw,
            2,
        ));
        Self {
            helper_id: NEXT_HELPER_ID.fetch_add(1, Ordering::Relaxed),
            ext_sp_ctl: Some(ExtSpecialController::new(
                uri.to_string(),
                connection.clone(),
            )),
            general_ctl: Some(Box::new(GeneralControllerProviderImpl::new(
                connection.clone(),
                uri.to_string(),
                uri.to_string(),
            ))),
            persistent_data_ctl: Some(PersistentDataController::new()),
            published_data_ctl: Some(PublishedDataController::new()),
            is_system,
            helper_type: 1, // NON_SILENT
            released: AtomicBool::new(false),
            connection: Some(connection),
        }
    }

    /// Create a silent (service path) helper.
    pub fn new_service(ext_uri: &str, is_system: bool) -> Self {
        Self {
            helper_id: NEXT_HELPER_ID.fetch_add(1, Ordering::Relaxed),
            ext_sp_ctl: None,
            general_ctl: Some(Box::new(GeneralControllerServiceImpl::new(
                ext_uri.to_string(),
            ))),
            persistent_data_ctl: Some(PersistentDataController::new()),
            published_data_ctl: Some(PublishedDataController::new()),
            is_system,
            helper_type: 0, // SILENT
            released: AtomicBool::new(false),
            connection: None,
        }
    }

    /// Query and return the raw IPC reply MsgParcel (FFI bridge).
    /// Bypasses Rust ISharedResultSet so C++ can call ReadFromParcel and
    /// receive the SharedBlock fd.
    pub fn query_raw_for_ffi(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        columns: &[String],
        option: &DataShareOption,
        business_error: &mut DatashareBusinessError,
    ) -> Option<ipc::parcel::MsgParcel> {
        let ctl = self.general_ctl.as_ref()?;
        ctl.query_raw(uri, predicates, columns, business_error, option)
    }

    /// Inject an already-connected ext ability proxy from C++ side.
    /// C++ establishes the connection and passes the remote object to Rust.
    pub fn set_ext_proxy(&self, remote: RemoteObj) {
        if let Some(conn) = &self.connection {
            conn.set_proxy_from_remote(remote);
        }
    }
}

impl DataShareHelper for DataShareHelperImpl {
    fn release(&self) -> bool {
        self.released.store(true, Ordering::Release);
        true
    }

    fn insert(&self, uri: &str, value: &DataShareValuesBucket) -> i32 {
        if let Some(ctl) = &self.general_ctl {
            return ctl.insert(uri, value);
        }
        DATA_SHARE_ERROR
    }

    fn update(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> i32 {
        if let Some(ctl) = &self.general_ctl {
            return ctl.update(uri, predicates, value);
        }
        DATA_SHARE_ERROR
    }

    fn delete(&self, uri: &str, predicates: &DataSharePredicates) -> i32 {
        if let Some(ctl) = &self.general_ctl {
            return ctl.delete(uri, predicates);
        }
        DATA_SHARE_ERROR
    }

    fn query(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        columns: &[String],
        business_error: &mut DatashareBusinessError,
    ) -> Option<ISharedResultSetProxy> {
        let option = DataShareOption { timeout: 0 };
        if let Some(ctl) = &self.general_ctl {
            return ctl.query(uri, predicates, columns, business_error, &option);
        }
        None
    }

    fn query_with_option(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        columns: &[String],
        option: &DataShareOption,
        business_error: &mut DatashareBusinessError,
    ) -> Option<ISharedResultSetProxy> {
        if let Some(ctl) = &self.general_ctl {
            return ctl.query(uri, predicates, columns, business_error, option);
        }
        None
    }

    fn insert_ex(&self, uri: &str, value: &DataShareValuesBucket) -> (i32, i32) {
        if let Some(ctl) = &self.general_ctl {
            return ctl.insert_ex(uri, value);
        }
        (DATA_SHARE_ERROR, 0)
    }

    fn update_ex(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> (i32, i32) {
        if let Some(ctl) = &self.general_ctl {
            return ctl.update_ex(uri, predicates, value);
        }
        (DATA_SHARE_ERROR, 0)
    }

    fn delete_ex(&self, uri: &str, predicates: &DataSharePredicates) -> (i32, i32) {
        if let Some(ctl) = &self.general_ctl {
            return ctl.delete_ex(uri, predicates);
        }
        (DATA_SHARE_ERROR, 0)
    }

    fn open_file(&self, uri: &str, mode: &str) -> i32 {
        if let Some(ctl) = &self.ext_sp_ctl {
            return ctl.open_file(uri, mode);
        }
        DATA_SHARE_ERROR
    }

    fn open_file_with_err_code(&self, uri: &str, mode: &str) -> (i32, i32) {
        if let Some(ctl) = &self.ext_sp_ctl {
            return ctl.open_file_with_err_code(uri, mode);
        }
        (DATA_SHARE_ERROR, 0)
    }

    fn open_raw_file(&self, uri: &str, mode: &str) -> i32 {
        if let Some(ctl) = &self.ext_sp_ctl {
            return ctl.open_raw_file(uri, mode);
        }
        DATA_SHARE_ERROR
    }

    fn get_type(&self, uri: &str) -> String {
        if let Some(ctl) = &self.ext_sp_ctl {
            return ctl.get_type(uri);
        }
        String::new()
    }

    fn get_file_types(&self, uri: &str, mime_type_filter: &str) -> Vec<String> {
        if let Some(ctl) = &self.ext_sp_ctl {
            return ctl.get_file_types(uri, mime_type_filter);
        }
        Vec::new()
    }

    fn batch_insert(&self, uri: &str, values: &[DataShareValuesBucket]) -> i32 {
        if let Some(ctl) = &self.ext_sp_ctl {
            return ctl.batch_insert(uri, values);
        }
        DATA_SHARE_ERROR
    }

    fn batch_update(&self, operations: &UpdateOperations) -> i32 {
        if let Some(ctl) = &self.ext_sp_ctl {
            return ctl.batch_update(operations);
        }
        DATA_SHARE_ERROR
    }

    fn register_observer(&self, uri: &str, observer_id: u64) -> i32 {
        if let Some(ctl) = &self.general_ctl {
            return ctl.register_observer(uri, observer_id);
        }
        DATA_SHARE_ERROR
    }

    fn unregister_observer(&self, uri: &str, observer_id: u64) -> i32 {
        if let Some(ctl) = &self.general_ctl {
            return ctl.unregister_observer(uri, observer_id);
        }
        DATA_SHARE_ERROR
    }

    fn notify_change(&self, uri: &str) {
        if let Some(ctl) = &self.general_ctl {
            ctl.notify_change(uri);
        }
    }

    fn normalize_uri(&self, uri: &str) -> String {
        if let Some(ctl) = &self.ext_sp_ctl {
            return ctl.normalize_uri(uri);
        }
        String::new()
    }

    fn denormalize_uri(&self, uri: &str) -> String {
        if let Some(ctl) = &self.ext_sp_ctl {
            return ctl.denormalize_uri(uri);
        }
        String::new()
    }

    fn add_query_template(&self, uri: &str, subscriber_id: i64, tpl: &Template) -> i32 {
        if let Some(ctl) = &self.persistent_data_ctl {
            return ctl.add_query_template(uri, subscriber_id, tpl);
        }
        DATA_SHARE_ERROR
    }

    fn del_query_template(&self, uri: &str, subscriber_id: i64) -> i32 {
        if let Some(ctl) = &self.persistent_data_ctl {
            return ctl.del_query_template(uri, subscriber_id);
        }
        DATA_SHARE_ERROR
    }

    fn publish(&self, data: &Data, bundle_name: &str) -> Vec<OperationResult> {
        if let Some(ctl) = &self.published_data_ctl {
            return ctl.publish(data, bundle_name);
        }
        Vec::new()
    }

    fn get_published_data(&self, bundle_name: &str) -> (Data, i32) {
        if let Some(ctl) = &self.published_data_ctl {
            return ctl.get_published_data(bundle_name);
        }
        (Data::default(), DATA_SHARE_ERROR)
    }

    fn subscribe_rdb_data(
        &self,
        uris: &[String],
        template_id: &TemplateId,
        callback: Arc<dyn Fn(&RdbChangeNode) + Send + Sync>,
    ) -> Vec<OperationResult> {
        if let Some(ctl) = &self.persistent_data_ctl {
            return ctl.subscribe_rdb_data(
                self.helper_id,
                uris,
                template_id,
                Box::new(move |n| callback(n)),
            );
        }
        Vec::new()
    }

    fn unsubscribe_rdb_data(
        &self,
        uris: &[String],
        template_id: &TemplateId,
    ) -> Vec<OperationResult> {
        if let Some(ctl) = &self.persistent_data_ctl {
            return ctl.unsubscribe_rdb_data(self.helper_id, uris, template_id);
        }
        Vec::new()
    }

    fn enable_rdb_subs(&self, uris: &[String], template_id: &TemplateId) -> Vec<OperationResult> {
        if let Some(ctl) = &self.persistent_data_ctl {
            return ctl.enable_subscribe_rdb_data(self.helper_id, uris, template_id);
        }
        Vec::new()
    }

    fn disable_rdb_subs(&self, uris: &[String], template_id: &TemplateId) -> Vec<OperationResult> {
        if let Some(ctl) = &self.persistent_data_ctl {
            return ctl.disable_subscribe_rdb_data(self.helper_id, uris, template_id);
        }
        Vec::new()
    }

    fn subscribe_published_data(
        &self,
        uris: &[String],
        subscriber_id: i64,
        callback: Arc<dyn Fn(&PublishedDataChangeNode) + Send + Sync>,
    ) -> Vec<OperationResult> {
        if let Some(ctl) = &self.published_data_ctl {
            return ctl.subscribe_published_data(
                self.helper_id,
                uris,
                subscriber_id,
                Box::new(move |n| callback(n)),
            );
        }
        Vec::new()
    }

    fn unsubscribe_published_data(
        &self,
        uris: &[String],
        subscriber_id: i64,
    ) -> Vec<OperationResult> {
        if let Some(ctl) = &self.published_data_ctl {
            return ctl.unsubscribe_published_data(self.helper_id, uris, subscriber_id);
        }
        Vec::new()
    }

    fn enable_pub_subs(&self, uris: &[String], subscriber_id: i64) -> Vec<OperationResult> {
        if let Some(ctl) = &self.published_data_ctl {
            return ctl.enable_subscribe_published_data(self.helper_id, uris, subscriber_id);
        }
        Vec::new()
    }

    fn disable_pub_subs(&self, uris: &[String], subscriber_id: i64) -> Vec<OperationResult> {
        if let Some(ctl) = &self.published_data_ctl {
            return ctl.disable_subscribe_published_data(self.helper_id, uris, subscriber_id);
        }
        Vec::new()
    }
}

// ---- Extension-specific methods (not on the DataShareHelper trait) ----
// These are only available on the provider path (non-silent).

impl DataShareHelperImpl {
    /// Get the extension special controller (for FFI direct proxy access).
    pub fn get_ext_sp_ctl(&self) -> Option<&ExtSpecialController> {
        self.ext_sp_ctl.as_ref()
    }

    /// Extension insert — returns (error_code, string_result).
    /// Only available on provider path (uses ext_sp_ctl).
    pub fn insert_ext_str(&self, uri: &str, value: &DataShareValuesBucket) -> (i32, String) {
        if let Some(ctl) = &self.ext_sp_ctl {
            return ctl.insert_ext(uri, value);
        }
        (DATA_SHARE_ERROR, String::new())
    }

    /// Execute a batch of operations.
    /// Only available on provider path (uses ext_sp_ctl).
    pub fn execute_batch(
        &self,
        statements: &[datashare_common::operation::OperationStatement],
    ) -> i32 {
        if let Some(ctl) = &self.ext_sp_ctl {
            return ctl.execute_batch(statements);
        }
        DATA_SHARE_ERROR
    }

    /// Execute a batch of operations using pre-serialized binary blob.
    pub fn execute_batch_blob(&self, blob: &[u8]) -> i32 {
        if let Some(ctl) = &self.ext_sp_ctl {
            return ctl.execute_batch_blob(blob);
        }
        DATA_SHARE_ERROR
    }

    /// Register an observer at the provider level (non-silent only).
    pub fn register_observer_ext_provider(
        &self,
        uri: &str,
        observer_id: u64,
        is_descendants: bool,
    ) -> i32 {
        if let Some(ctl) = &self.general_ctl {
            return ctl.register_observer_ext_provider(uri, observer_id, is_descendants);
        }
        DATA_SHARE_ERROR
    }

    /// Unregister an observer at the provider level (non-silent only).
    pub fn unregister_observer_ext_provider(&self, uri: &str, observer_id: u64) -> i32 {
        if let Some(ctl) = &self.general_ctl {
            return ctl.unregister_observer_ext_provider(uri, observer_id);
        }
        DATA_SHARE_ERROR
    }

    /// Notify change at the provider level (non-silent only).
    pub fn notify_change_ext_provider(
        &self,
        change_info: &datashare_common::observer::ChangeInfo,
    ) -> i32 {
        if let Some(ctl) = &self.general_ctl {
            return ctl.notify_change_ext_provider(change_info);
        }
        DATA_SHARE_ERROR
    }

    /// User-defined function dispatch.
    /// Only available on provider path (uses ext_sp_ctl).
    pub fn user_define_func(
        &self,
        data: &mut ipc::parcel::MsgParcel,
    ) -> Option<ipc::parcel::MsgParcel> {
        if let Some(ctl) = &self.ext_sp_ctl {
            return ctl.user_define_func(data);
        }
        None
    }

    /// Set the extension URI on the general controller.
    pub fn set_ext_uri(&self, ext_uri: &str) -> i32 {
        if let Some(ctl) = &self.general_ctl {
            return ctl.set_ext_uri(ext_uri);
        }
        DATA_SHARE_ERROR
    }

    /// Get the helper type: SILENT(0) or NON_SILENT(1).
    pub fn get_helper_type(&self) -> i32 {
        self.helper_type
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_helper_impl_service_construction() {
        let helper = DataShareHelperImpl::new_service("datashare:///test", false);
        assert!(!helper.is_system);
        assert!(helper.ext_sp_ctl.is_none()); // No ext controller for service path
        assert!(helper.general_ctl.is_some());
    }

    #[test]
    fn test_helper_impl_provider_construction() {
        let helper = DataShareHelperImpl::new_provider(
            "datashare:///test",
            None,
            std::ptr::null_mut(),
            false,
        );
        assert!(helper.ext_sp_ctl.is_some());
        assert!(helper.general_ctl.is_some());
    }

    #[test]
    fn test_helper_impl_insert() {
        let helper = DataShareHelperImpl::new_service("datashare:///test", false);
        let vb = DataShareValuesBucket::new();
        let result = helper.insert("datashare:///test/table", &vb);
        assert_eq!(result, DATA_SHARE_ERROR);
    }

    #[test]
    fn test_helper_impl_query() {
        let helper = DataShareHelperImpl::new_service("datashare:///test", false);
        let predicates = DataSharePredicates::new();
        let mut err = DatashareBusinessError::new();
        let result = helper.query("datashare:///test/table", &predicates, &[], &mut err);
        assert!(result.is_none());
    }

    #[test]
    fn test_helper_impl_release() {
        let helper = DataShareHelperImpl::new_service("datashare:///test", false);
        assert!(helper.release());
    }
}
