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

//! ExtSpecialController — extension-specific operations controller.
//!
//! Corresponds to C++ `ExtSpecialController` in
//! `frameworks/native/consumer/controller/provider/src/ext_special_controller.cpp` (211 lines).
//!
//! Data flow: App → DataShareHelper → ExtSpecialController
//!     → DataShareConnection → DataShareProxy → IPC → DataShare extension
//!
//! Features:
//! - File operations (OpenFile, OpenRawFile, GetType, GetFileTypes)
//! - Batch operations (BatchInsert, BatchUpdate, ExecuteBatch)
//! - URI normalization/denormalization
//! - Extension-specific insert (InsertExt)
//! - User-defined function dispatch

use std::sync::Arc;

use ipc::parcel::MsgParcel;

use datashare_common::operation::OperationStatement;
use datashare_common::types::UpdateOperations;
use datashare_common::values_bucket::DataShareValuesBucket;

use crate::connection::DataShareConnection;

const INVALID_VALUE: i32 = -1;

pub struct ExtSpecialController {
    uri: String,
    connection: Arc<DataShareConnection>,
}

impl ExtSpecialController {
    pub fn new(uri: String, connection: Arc<DataShareConnection>) -> Self {
        Self { uri, connection }
    }

    pub fn uri(&self) -> &str {
        &self.uri
    }

    /// Get the underlying connection (for direct proxy access in FFI).
    pub fn connection(&self) -> &Arc<DataShareConnection> {
        &self.connection
    }

    pub fn open_file(&self, uri: &str, mode: &str) -> i32 {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return INVALID_VALUE,
        };
        proxy.open_file(uri, mode)
    }

    pub fn open_file_with_err_code(&self, uri: &str, mode: &str) -> (i32, i32) {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return (INVALID_VALUE, 0),
        };
        proxy.open_file_with_err_code(uri, mode)
    }

    pub fn open_raw_file(&self, uri: &str, mode: &str) -> i32 {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return INVALID_VALUE,
        };
        proxy.open_raw_file(uri, mode)
    }

    pub fn get_type(&self, uri: &str) -> String {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return String::new(),
        };
        proxy.get_type(uri)
    }

    pub fn batch_insert(&self, uri: &str, values: &[DataShareValuesBucket]) -> i32 {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return INVALID_VALUE,
        };
        proxy.batch_insert(uri, values)
    }

    pub fn batch_update(&self, operations: &UpdateOperations) -> i32 {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return INVALID_VALUE,
        };
        let mut results = Vec::new();
        proxy.batch_update(operations, &mut results)
    }

    pub fn batch_update_ex(
        &self,
        operations: &UpdateOperations,
        results: &mut Vec<datashare_common::types::BatchUpdateResult>,
    ) -> i32 {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return INVALID_VALUE,
        };
        proxy.batch_update(operations, results)
    }

    pub fn insert_ext(&self, uri: &str, value: &DataShareValuesBucket) -> (i32, String) {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return (INVALID_VALUE, String::new()),
        };
        proxy.insert_ext(uri, value)
    }

    pub fn execute_batch(&self, statements: &[OperationStatement]) -> i32 {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return INVALID_VALUE,
        };
        let mut result = datashare_common::types::ExecResultSet {
            error_code: datashare_common::types::ExecErrorCode::Success,
            results: Vec::new(),
        };
        proxy.execute_batch(statements, &mut result)
    }

    pub fn execute_batch_ex(
        &self,
        statements: &[OperationStatement],
        result: &mut datashare_common::types::ExecResultSet,
    ) -> i32 {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return INVALID_VALUE,
        };
        proxy.execute_batch(statements, result)
    }

    pub fn execute_batch_blob(&self, blob: &[u8]) -> i32 {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return INVALID_VALUE,
        };
        let mut result = datashare_common::types::ExecResultSet {
            error_code: datashare_common::types::ExecErrorCode::Success,
            results: Vec::new(),
        };
        proxy.execute_batch_blob(blob, &mut result)
    }

    pub fn normalize_uri(&self, uri: &str) -> String {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return String::new(),
        };
        proxy.normalize_uri(uri)
    }

    pub fn denormalize_uri(&self, uri: &str) -> String {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return String::new(),
        };
        proxy.denormalize_uri(uri)
    }

    pub fn get_file_types(&self, uri: &str, mime_type_filter: &str) -> Vec<String> {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return Vec::new(),
        };
        proxy.get_file_types(uri, mime_type_filter)
    }

    pub fn user_define_func(&self, data: &mut MsgParcel) -> Option<MsgParcel> {
        let proxy = match self.connection.get_data_share_proxy() {
            Some(p) => p,
            None => return None,
        };
        proxy.user_define_func(data)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn make_controller() -> ExtSpecialController {
        let conn = Arc::new(DataShareConnection::new(
            "datashare:///test".to_string(),
            None,
            2,
        ));
        ExtSpecialController::new("datashare:///test".to_string(), conn)
    }

    #[test]
    fn test_ext_special_construction() {
        let controller = make_controller();
        assert_eq!(controller.uri(), "datashare:///test");
    }
}
