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

//! GeneralController trait — abstract interface for CRUD + observer operations.
//!
//! Corresponds to C++ `GeneralController` abstract class in
//! `frameworks/native/consumer/controller/common/general_controller.h`.
//!
//! Two concrete implementations:
//! - `GeneralControllerServiceImpl` — silent path via DataShareService
//! - `GeneralControllerProviderImpl` — non-silent path via provider extension

use datashare_common::observer::ChangeInfo;
use datashare_common::predicates::DataSharePredicates;
use datashare_common::types::{DataShareOption, OperationResult};
use datashare_common::values_bucket::DataShareValuesBucket;

use datashare_resultset::ipc::proxy::ISharedResultSetProxy;

use ipc::parcel::MsgParcel;

/// Business error type for DataShare operations.
/// Corresponds to C++ `DatashareBusinessError`.
#[derive(Debug, Clone, Default)]
pub struct DatashareBusinessError {
    code: i32,
    message: String,
}

impl DatashareBusinessError {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn get_code(&self) -> i32 {
        self.code
    }

    pub fn set_code(&mut self, code: i32) {
        self.code = code;
    }

    pub fn set_code_from_str(&mut self, code: &str) {
        if !code.is_empty() {
            self.code = code.parse::<i32>().unwrap_or(0);
        }
    }

    pub fn get_message(&self) -> &str {
        &self.message
    }

    pub fn set_message(&mut self, message: String) {
        self.message = message;
    }
}

/// DB_NOT_EXIST_ERR constant from C++
pub const DB_NOT_EXIST_ERR: i32 = 14800045;

/// DATA_SHARE_ERROR constant — general error return value
pub const DATA_SHARE_ERROR: i32 = -1;

/// GeneralController trait — abstract interface for CRUD + observer operations.
///
/// This is the Rust equivalent of the C++ abstract class `GeneralController`.
/// Two implementations exist:
/// - `GeneralControllerServiceImpl` for silent (service) path
/// - `GeneralControllerProviderImpl` for non-silent (provider) path
pub trait GeneralController: Send + Sync {
    /// Insert a row. Returns number of rows inserted or error code.
    fn insert(&self, uri: &str, value: &DataShareValuesBucket) -> i32;

    /// Update rows matching predicates. Returns number of rows updated or error code.
    fn update(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> i32;

    /// Delete rows matching predicates. Returns number of rows deleted or error code.
    fn delete(&self, uri: &str, predicates: &DataSharePredicates) -> i32;

    /// Query rows. Returns opaque result set handle.
    ///
    /// `columns` specifies the columns to return.
    /// `business_error` is filled with error details if the query fails.
    /// `option` may specify timeout for timed queries.
    fn query(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        columns: &[String],
        business_error: &mut DatashareBusinessError,
        option: &DataShareOption,
    ) -> Option<ISharedResultSetProxy>;

    /// Register an observer for data changes at the given URI.
    fn register_observer(&self, uri: &str, observer_id: u64) -> i32;

    /// Unregister an observer for data changes at the given URI.
    fn unregister_observer(&self, uri: &str, observer_id: u64) -> i32;

    /// Notify observers of a data change at the given URI.
    fn notify_change(&self, uri: &str);

    /// Register an observer at the provider level (non-silent only).
    fn register_observer_ext_provider(
        &self,
        uri: &str,
        observer_id: u64,
        is_descendants: bool,
    ) -> i32;

    /// Unregister an observer at the provider level (non-silent only).
    fn unregister_observer_ext_provider(&self, uri: &str, observer_id: u64) -> i32;

    /// Notify change at the provider level (non-silent only).
    fn notify_change_ext_provider(&self, change_info: &ChangeInfo) -> i32;

    /// Extended insert — returns (error_code, result_value).
    fn insert_ex(&self, uri: &str, value: &DataShareValuesBucket) -> (i32, i32);

    /// Extended update — returns (error_code, result_value).
    fn update_ex(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> (i32, i32);

    /// Extended delete — returns (error_code, result_value).
    fn delete_ex(&self, uri: &str, predicates: &DataSharePredicates) -> (i32, i32);

    /// Set the extension URI on this controller.
    /// Only supported on the service (silent) path.
    /// Default returns E_DATASHARE_TYPE (1086).
    fn set_ext_uri(&self, _ext_uri: &str) -> i32 {
        1086
    }

    /// Query rows and return the raw IPC reply MsgParcel (for FFI bridge to C++).
    /// This bypasses Rust-side ISharedResultSet construction so C++ can read the
    /// SharedBlock fd via ISharedResultSet::ReadFromParcel.
    /// Default returns None for impls that do not support it.
    fn query_raw(
        &self,
        _uri: &str,
        _predicates: &DataSharePredicates,
        _columns: &[String],
        _business_error: &mut DatashareBusinessError,
        _option: &DataShareOption,
    ) -> Option<MsgParcel> {
        None
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_business_error_default() {
        let err = DatashareBusinessError::new();
        assert_eq!(err.get_code(), 0);
        assert_eq!(err.get_message(), "");
    }

    #[test]
    fn test_business_error_set_code() {
        let mut err = DatashareBusinessError::new();
        err.set_code(1061);
        assert_eq!(err.get_code(), 1061);
    }

    #[test]
    fn test_business_error_set_code_from_str() {
        let mut err = DatashareBusinessError::new();
        err.set_code_from_str("1072");
        assert_eq!(err.get_code(), 1072);

        err.set_code_from_str("");
        assert_eq!(err.get_code(), 1072); // unchanged on empty

        err.set_code_from_str("invalid");
        assert_eq!(err.get_code(), 0); // parsed as 0
    }

    #[test]
    fn test_business_error_message() {
        let mut err = DatashareBusinessError::new();
        err.set_message("query time out".to_string());
        assert_eq!(err.get_message(), "query time out");
    }
}
