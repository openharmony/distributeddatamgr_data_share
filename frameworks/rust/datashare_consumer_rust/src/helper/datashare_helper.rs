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

//! DataShareHelper trait — abstract interface for DataShare operations.
//!
//! Corresponds to C++ `DataShareHelper` in
//! `interfaces/inner_api/consumer/include/datashare_helper.h` (590 lines).
//!
//! This is the main user-facing API for DataShare consumers.
//! Applications use `DataShareHelper::create()` to obtain an instance,
//! then call CRUD, observer, and subscription methods.
//!
//! Two access paths:
//! - Silent (service) path: via DataShareService, no connection to extension
//! - Non-silent (provider) path: direct connection to DataShare extension

use std::sync::Arc;

use datashare_common::observer::ChangeInfo;
use datashare_common::predicates::DataSharePredicates;
use datashare_common::template::{Template, TemplateId};
use datashare_common::types::{
    Data, DataShareOption, OperationResult, PublishedDataChangeNode, RdbChangeNode,
    UpdateOperations,
};
use datashare_common::values_bucket::DataShareValuesBucket;

use crate::connection::DataShareManagerImpl;
use crate::controller::general_controller::DatashareBusinessError;
use crate::helper::datashare_helper_impl::DataShareHelperImpl;
use crate::proxy::DataShareServiceProxy;

use datashare_resultset::ipc::proxy::ISharedResultSetProxy;

/// DataShareHelper — abstract interface for DataShare operations.
///
/// Corresponds to C++ `DataShareHelper` abstract class.
///
/// Implementations:
/// - `DataShareHelperImpl` — routes to appropriate controllers
pub trait DataShareHelper: Send + Sync {
    /// Release the client resource.
    fn release(&self) -> bool;

    // ---- CRUD Operations ----

    /// Insert a row. Returns number of rows inserted or error code.
    fn insert(&self, uri: &str, value: &DataShareValuesBucket) -> i32;

    /// Update rows. Returns number of rows updated or error code.
    fn update(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> i32;

    /// Delete rows. Returns number of rows deleted or error code.
    fn delete(&self, uri: &str, predicates: &DataSharePredicates) -> i32;

    /// Query rows. Returns result set proxy for reading data.
    fn query(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        columns: &[String],
        business_error: &mut DatashareBusinessError,
    ) -> Option<ISharedResultSetProxy>;

    /// Query with timeout option.
    fn query_with_option(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        columns: &[String],
        option: &DataShareOption,
        business_error: &mut DatashareBusinessError,
    ) -> Option<ISharedResultSetProxy>;

    // ---- Extended CRUD ----

    /// Extended insert. Returns (error_code, result_value).
    fn insert_ex(&self, uri: &str, value: &DataShareValuesBucket) -> (i32, i32);

    /// Extended update. Returns (error_code, result_value).
    fn update_ex(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> (i32, i32);

    /// Extended delete. Returns (error_code, result_value).
    fn delete_ex(&self, uri: &str, predicates: &DataSharePredicates) -> (i32, i32);

    // ---- File Operations ----

    /// Open a file. Returns file descriptor.
    fn open_file(&self, uri: &str, mode: &str) -> i32;

    /// Open a file with error code. Returns (fd, error_code).
    fn open_file_with_err_code(&self, uri: &str, mode: &str) -> (i32, i32);

    /// Open a raw file. Returns file descriptor.
    fn open_raw_file(&self, uri: &str, mode: &str) -> i32;

    /// Get MIME type.
    fn get_type(&self, uri: &str) -> String;

    /// Get file types matching filter.
    fn get_file_types(&self, uri: &str, mime_type_filter: &str) -> Vec<String>;

    // ---- Batch Operations ----

    /// Batch insert.
    fn batch_insert(&self, uri: &str, values: &[DataShareValuesBucket]) -> i32;

    /// Batch update.
    fn batch_update(&self, operations: &UpdateOperations) -> i32;

    // ---- Observer Operations ----

    /// Register an observer.
    fn register_observer(&self, uri: &str, observer_id: u64) -> i32;

    /// Unregister an observer.
    fn unregister_observer(&self, uri: &str, observer_id: u64) -> i32;

    /// Notify data change.
    fn notify_change(&self, uri: &str);

    // ---- URI Operations ----

    /// Normalize a URI.
    fn normalize_uri(&self, uri: &str) -> String;

    /// Denormalize a URI.
    fn denormalize_uri(&self, uri: &str) -> String;

    // ---- Template Operations ----

    /// Add a query template.
    fn add_query_template(&self, uri: &str, subscriber_id: i64, tpl: &Template) -> i32;

    /// Delete a query template.
    fn del_query_template(&self, uri: &str, subscriber_id: i64) -> i32;

    // ---- Publish Operations ----

    /// Publish data.
    fn publish(&self, data: &Data, bundle_name: &str) -> Vec<OperationResult>;

    /// Get published data.
    fn get_published_data(&self, bundle_name: &str) -> (Data, i32);

    // ---- RDB Subscription ----

    /// Subscribe to RDB data changes.
    fn subscribe_rdb_data(
        &self,
        uris: &[String],
        template_id: &TemplateId,
        callback: Arc<dyn Fn(&RdbChangeNode) + Send + Sync>,
    ) -> Vec<OperationResult>;

    /// Unsubscribe from RDB data changes.
    fn unsubscribe_rdb_data(
        &self,
        uris: &[String],
        template_id: &TemplateId,
    ) -> Vec<OperationResult>;

    /// Enable RDB subscriptions.
    fn enable_rdb_subs(&self, uris: &[String], template_id: &TemplateId) -> Vec<OperationResult>;

    /// Disable RDB subscriptions.
    fn disable_rdb_subs(&self, uris: &[String], template_id: &TemplateId) -> Vec<OperationResult>;

    // ---- Published Data Subscription ----

    /// Subscribe to published data changes.
    fn subscribe_published_data(
        &self,
        uris: &[String],
        subscriber_id: i64,
        callback: Arc<dyn Fn(&PublishedDataChangeNode) + Send + Sync>,
    ) -> Vec<OperationResult>;

    /// Unsubscribe from published data changes.
    fn unsubscribe_published_data(
        &self,
        uris: &[String],
        subscriber_id: i64,
    ) -> Vec<OperationResult>;

    /// Enable published data subscriptions.
    fn enable_pub_subs(&self, uris: &[String], subscriber_id: i64) -> Vec<OperationResult>;

    /// Disable published data subscriptions.
    fn disable_pub_subs(&self, uris: &[String], subscriber_id: i64) -> Vec<OperationResult>;
}

/// Create a DataShareHelper instance (silent path, priority).
///
/// Corresponds to C++ static `DataShareHelper::Create()`.
/// Returns (error_code, helper_instance).
pub fn create_helper(
    _token: u64,
    str_uri: &str,
    ext_uri: &str,
    _wait_time: i32,
) -> (i32, Option<Arc<dyn DataShareHelper>>) {
    // Try silent (service) path first via GetSilentProxyStatus
    if let Some(proxy) = DataShareManagerImpl::get_service_proxy() {
        let status = proxy.get_silent_proxy_status(str_uri);
        if status == 0 {
            let helper =
                DataShareHelperImpl::new_service(ext_uri, DataShareServiceProxy::is_system());
            return (0, Some(Arc::new(helper)));
        }
    }
    // Fall back to non-silent (provider) path
    let remote_token = None; // Token conversion handled by caller via FFI
    let helper = DataShareHelperImpl::new_provider(
        str_uri,
        remote_token,
        std::ptr::null_mut(),
        DataShareServiceProxy::is_system(),
    );
    (0, Some(Arc::new(helper)))
}

/// Set silent switch for a URI.
///
/// Corresponds to C++ static `DataShareHelper::SetSilentSwitch()`.
pub fn set_silent_switch(uri: &str, enable: bool, is_system: bool) -> i32 {
    DataShareServiceProxy::set_system(is_system);
    let proxy = match DataShareManagerImpl::get_service_proxy() {
        Some(p) => p,
        None => return -1,
    };
    proxy.set_silent_switch(uri, enable)
}

/// Get silent proxy status for a URI.
///
/// Corresponds to C++ static `DataShareHelper::GetSilentProxyStatus()`.
pub fn get_silent_proxy_status(uri: &str, is_system: bool) -> i32 {
    let proxy = match DataShareManagerImpl::get_service_proxy() {
        Some(p) => p,
        None => return -1,
    };
    DataShareServiceProxy::set_system(is_system);
    let res = proxy.get_silent_proxy_status(uri);
    DataShareServiceProxy::clean_system();
    res
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_create_helper_placeholder() {
        let (code, helper) = create_helper(0, "datashare:///test", "", 2);
        assert_eq!(code, 0);
        assert!(helper.is_some());
    }

    #[test]
    fn test_set_silent_switch_placeholder() {
        let result = set_silent_switch("datashare:///test", true, false);
        // Returns -1 when service proxy is unavailable (no SA in test env)
        assert_eq!(result, -1);
    }
}
