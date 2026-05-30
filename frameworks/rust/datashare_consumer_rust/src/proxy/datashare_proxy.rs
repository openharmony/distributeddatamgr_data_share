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

//! DataShareProxy — IPC proxy to DataShare extension.
//!
//! Corresponds to C++ `DataShareProxy` in
//! `frameworks/native/consumer/src/datashare_proxy.cpp` (758 lines).
//!
//! This is the IPC proxy that implements the `IDataShare` interface.
//! All method calls are serialized via `MessageParcel` and sent to the
//! remote DataShare extension via `SendRequest`.
//!
//! Methods:
//! - CRUD: Insert, Update, Delete, Query
//! - Extended CRUD: InsertEx, UpdateEx, DeleteEx
//! - File: OpenFile, OpenRawFile, GetType, GetFileTypes
//! - Batch: BatchInsert, BatchUpdate, ExecuteBatch
//! - Observer: RegisterObserver, UnregisterObserver, NotifyChange
//! - Extension observer: RegisterObserverExtProvider, UnregisterObserverExtProvider, NotifyChangeExtProvider
//! - URI: NormalizeUri, DenormalizeUri
//! - Misc: InsertExt, UserDefineFunc

use ipc::parcel::MsgParcel;
use ipc::remote::RemoteObj;

use datashare_common::ipc::itypes_utils;
use datashare_common::observer::ChangeInfo;
use datashare_common::predicates::DataSharePredicates;
use datashare_common::types::{BatchUpdateResult, ExecResultSet, UpdateOperations};
use datashare_common::values_bucket::DataShareValuesBucket;

use crate::controller::general_controller::DatashareBusinessError;
use crate::ipc::codes::IDataShareCmd;

// Ensure parcel_impl Serialize/Deserialize impls are linked in.
#[allow(unused_imports)]
use datashare_common::ipc::parcel_impl;

/// Maximum IPC transfer size (900KB).
const MTU_SIZE: usize = 921_600;

/// Maximum number of items in batch operations.
const MAX_SIZE: usize = 4000;

/// Permission error result from IPC.
const PERMISSION_ERR: i32 = 1;

/// Permission error code returned to caller.
const PERMISSION_ERR_CODE: i32 = -2;

/// Invalid return value.
const INVALID_VALUE: i32 = -1;

/// IPC interface descriptor for IDataShare.
const DESCRIPTOR: &str = "OHOS.DataShare.IDataShare";

/// DataShareProxy — IPC proxy to a DataShare extension.
///
/// Corresponds to C++ `DataShareProxy : public IRemoteProxy<IDataShare>`.
pub struct DataShareProxy {
    remote: RemoteObj,
}

impl DataShareProxy {
    /// Create a new DataShareProxy with the given remote object.
    pub fn new(remote: RemoteObj) -> Self {
        Self { remote }
    }

    /// Create a MsgParcel with the interface token written.
    fn create_request(&self) -> Option<MsgParcel> {
        let mut data = MsgParcel::new();
        if data.set_max_capacity(MTU_SIZE).is_err() {
            return None;
        }
        data.write_interface_token(DESCRIPTOR).ok()?;
        Some(data)
    }

    /// Send an IPC request and return the reply parcel.
    fn send(&self, code: u32, data: &mut MsgParcel) -> Option<MsgParcel> {
        self.remote.send_request(code, data).ok()
    }

    /// Write a URI as Parcelable(Uri): int32(1) + String16.
    /// Matches C++ WriteParcelable(&Uri(str)) which writes int32(1) + WriteString16(str).
    fn write_uri(data: &mut MsgParcel, uri: &str) -> bool {
        data.write::<i32>(&1).is_ok() && data.write_string16(uri).is_ok()
    }

    /// Read a URI from Parcelable(Uri): int32 (null indicator) + String16.
    fn read_uri(reply: &mut MsgParcel) -> String {
        let indicator = reply.read::<i32>().unwrap_or(0);
        if indicator == 0 {
            return String::new();
        }
        reply.read_string16().unwrap_or_default()
    }

    /// Get file types matching the MIME filter.
    pub fn get_file_types(&self, uri: &str, mime_type_filter: &str) -> Vec<String> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Vec::new(),
        };
        if !Self::write_uri(&mut data, uri) {
            return Vec::new();
        }
        if data.write(&mime_type_filter.to_string()).is_err() {
            return Vec::new();
        }

        let mut reply = match self.send(IDataShareCmd::GetFileTypes as u32, &mut data) {
            Some(r) => r,
            None => return Vec::new(),
        };

        let count: i32 = reply.read().unwrap_or(0);
        let mut types = Vec::new();
        for _ in 0..count {
            if let Ok(s) = reply.read::<String>() {
                types.push(s);
            }
        }
        types
    }

    /// Open a file — internal helper for OpenFile and OpenFileWithErrCode.
    fn open_file_inner(&self, uri: &str, mode: &str, code: u32) -> (i32, i32) {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return (INVALID_VALUE, 0),
        };
        if !Self::write_uri(&mut data, uri) {
            return (INVALID_VALUE, 0);
        }
        if data.write(&mode.to_string()).is_err() {
            return (INVALID_VALUE, 0);
        }

        match self.remote.send_request(code, &mut data) {
            Ok(mut reply) => {
                let fd = unsafe { reply.read_raw_fd() };
                (fd, 0)
            }
            Err(e) => (INVALID_VALUE, e as i32),
        }
    }

    /// Open a file at the given URI.
    pub fn open_file(&self, uri: &str, mode: &str) -> i32 {
        self.open_file_inner(uri, mode, IDataShareCmd::OpenFile as u32)
            .0
    }

    /// Open a file with error code output.
    pub fn open_file_with_err_code(&self, uri: &str, mode: &str) -> (i32, i32) {
        self.open_file_inner(uri, mode, IDataShareCmd::OpenFileWithErrCode as u32)
    }

    /// Open a raw file at the given URI.
    pub fn open_raw_file(&self, uri: &str, mode: &str) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return INVALID_VALUE,
        };
        if !Self::write_uri(&mut data, uri) {
            return INVALID_VALUE;
        }
        if data.write(&mode.to_string()).is_err() {
            return INVALID_VALUE;
        }

        let mut reply = match self.send(IDataShareCmd::OpenRawFile as u32, &mut data) {
            Some(r) => r,
            None => return INVALID_VALUE,
        };
        reply.read::<i32>().unwrap_or(INVALID_VALUE)
    }

    /// Insert a row.
    pub fn insert(&self, uri: &str, value: &DataShareValuesBucket) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return INVALID_VALUE,
        };
        if !Self::write_uri(&mut data, uri) {
            return INVALID_VALUE;
        }
        if data.write(value).is_err() {
            return INVALID_VALUE;
        }

        let mut reply = match self.send(IDataShareCmd::Insert as u32, &mut data) {
            Some(r) => r,
            None => return INVALID_VALUE,
        };
        reply.read::<i32>().unwrap_or(INVALID_VALUE)
    }

    /// Extended insert with result string.
    pub fn insert_ext(&self, uri: &str, value: &DataShareValuesBucket) -> (i32, String) {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return (INVALID_VALUE, String::new()),
        };
        if !Self::write_uri(&mut data, uri) {
            return (INVALID_VALUE, String::new());
        }
        if data.write(value).is_err() {
            return (INVALID_VALUE, String::new());
        }

        let mut reply = match self.send(IDataShareCmd::InsertExt as u32, &mut data) {
            Some(r) => r,
            None => return (INVALID_VALUE, String::new()),
        };
        let index = reply.read::<i32>().unwrap_or(INVALID_VALUE);
        let result = reply.read::<String>().unwrap_or_default();
        (index, result)
    }

    /// Update rows matching predicates.
    pub fn update(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return INVALID_VALUE,
        };
        if !Self::write_uri(&mut data, uri) {
            return INVALID_VALUE;
        }
        if data.write(predicates).is_err() {
            return INVALID_VALUE;
        }
        if data.write(value).is_err() {
            return INVALID_VALUE;
        }

        let mut reply = match self.send(IDataShareCmd::Update as u32, &mut data) {
            Some(r) => r,
            None => return INVALID_VALUE,
        };
        reply.read::<i32>().unwrap_or(INVALID_VALUE)
    }

    /// Batch update operations.
    pub fn batch_update(
        &self,
        operations: &UpdateOperations,
        results: &mut Vec<BatchUpdateResult>,
    ) -> i32 {
        // Check total size
        let total: usize = operations.iter().map(|(_, v)| v.len()).sum();
        if total > MAX_SIZE {
            return INVALID_VALUE;
        }

        let mut data = match self.create_request() {
            Some(d) => d,
            None => return INVALID_VALUE,
        };
        if data.write(operations).is_err() {
            return INVALID_VALUE;
        }

        let mut reply = match self.send(IDataShareCmd::BatchUpdate as u32, &mut data) {
            Some(r) => r,
            None => return INVALID_VALUE,
        };

        let count: i32 = reply.read().unwrap_or(0);
        for _ in 0..count {
            if let Ok(r) = reply.read::<BatchUpdateResult>() {
                results.push(r);
            }
        }
        0
    }

    /// Delete rows matching predicates.
    pub fn delete(&self, uri: &str, predicates: &DataSharePredicates) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return INVALID_VALUE,
        };
        if !Self::write_uri(&mut data, uri) {
            return INVALID_VALUE;
        }
        if data.write(predicates).is_err() {
            return INVALID_VALUE;
        }

        let mut reply = match self.send(IDataShareCmd::Delete as u32, &mut data) {
            Some(r) => r,
            None => return INVALID_VALUE,
        };
        reply.read::<i32>().unwrap_or(INVALID_VALUE)
    }

    /// Query rows. Returns opaque result set handle.
    /// Note: In C++ this returns shared_ptr<DataShareResultSet> via ISharedResultSet::ReadFromParcel.
    /// The Rust side returns the raw reply MsgParcel for the caller to extract the result set.
    pub fn query(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        columns: &[String],
        business_error: &mut DatashareBusinessError,
    ) -> Option<MsgParcel> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => {
                business_error.set_code(-1);
                return None;
            }
        };
        if !Self::write_uri(&mut data, uri) {
            business_error.set_code(-1);
            return None;
        }
        // Write columns
        if data.write(&(columns.len() as i32)).is_err() {
            return None;
        }
        for col in columns {
            if data.write(col).is_err() {
                return None;
            }
        }
        // Write predicates as raw buffer (matching C++ MarshalPredicates format)
        // C++ writes: WriteInt32(size) + WriteRawData(data, size)
        // WriteRawData internally writes: WriteInt32(size) + WriteUnpadBuffer(data, size)
        // So total on wire: [outer_size][inner_size][unpadded_data]
        if let Some((size, buf)) = itypes_utils::marshal_predicates_raw(predicates) {
            if data.write(&size).is_err() {
                return None;
            }
            if data.write(&size).is_err() {
                return None;
            }
            if data.write_buffer(&buf).is_err() {
                return None;
            }
        } else {
            business_error.set_code(-1);
            return None;
        }

        let mut reply = match self.send(IDataShareCmd::Query as u32, &mut data) {
            Some(r) => r,
            None => {
                business_error.set_code(-1);
                return None;
            }
        };

        // Read business error from reply (after result set data)
        // The result set is read by the caller from the reply parcel
        Some(reply)
    }

    /// Get the MIME type of the resource.
    pub fn get_type(&self, uri: &str) -> String {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return String::new(),
        };
        if !Self::write_uri(&mut data, uri) {
            return String::new();
        }

        let mut reply = match self.send(IDataShareCmd::GetType as u32, &mut data) {
            Some(r) => r,
            None => return String::new(),
        };
        reply.read::<String>().unwrap_or_default()
    }

    /// Batch insert multiple rows.
    pub fn batch_insert(&self, uri: &str, values: &[DataShareValuesBucket]) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return INVALID_VALUE,
        };
        if !Self::write_uri(&mut data, uri) {
            return INVALID_VALUE;
        }
        // Write values as raw buffer (matching C++ WriteRawData format)
        if let Some((size, buf)) = itypes_utils::marshal_values_bucket_vec_raw(values) {
            if data.write(&size).is_err() {
                return INVALID_VALUE;
            }
            if data.write(&size).is_err() {
                return INVALID_VALUE;
            }
            if data.write_buffer(&buf).is_err() {
                return INVALID_VALUE;
            }
        } else {
            return INVALID_VALUE;
        }

        let mut reply = match self.send(IDataShareCmd::BatchInsert as u32, &mut data) {
            Some(r) => r,
            None => return INVALID_VALUE,
        };
        reply.read::<i32>().unwrap_or(INVALID_VALUE)
    }

    /// Execute a batch of operation statements.
    pub fn execute_batch(
        &self,
        statements: &[datashare_common::operation::OperationStatement],
        result: &mut ExecResultSet,
    ) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return INVALID_VALUE,
        };
        if data.write(&(statements.len() as i32)).is_err() {
            return INVALID_VALUE;
        }
        for stmt in statements {
            if data.write(stmt).is_err() {
                return INVALID_VALUE;
            }
        }

        let mut reply = match self.send(IDataShareCmd::ExecuteBatch as u32, &mut data) {
            Some(r) => r,
            None => return INVALID_VALUE,
        };
        if let Ok(r) = reply.read::<ExecResultSet>() {
            *result = r;
            return 0;
        }
        INVALID_VALUE
    }

    /// Execute a batch of operations using pre-serialized binary blob.
    ///
    /// The blob is the output of C++ `MarshalOperationStatementVecToBuffer` or
    /// Rust `marshal_operation_statement_vec`. Written to IPC as
    /// `WriteInt32(size) + WriteRawData(blob)`.
    pub fn execute_batch_blob(&self, blob: &[u8], result: &mut ExecResultSet) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return INVALID_VALUE,
        };
        let size = blob.len() as i32;
        if data.write(&size).is_err() {
            return INVALID_VALUE;
        }
        if data.write(&size).is_err() {
            return INVALID_VALUE;
        }
        if data.write_buffer(blob).is_err() {
            return INVALID_VALUE;
        }

        let mut reply = match self.send(IDataShareCmd::ExecuteBatch as u32, &mut data) {
            Some(r) => r,
            None => return INVALID_VALUE,
        };
        if let Ok(r) = reply.read::<ExecResultSet>() {
            *result = r;
            return 0;
        }
        INVALID_VALUE
    }

    /// Register an observer for data changes.
    pub fn register_observer(&self, uri: &str, observer: &RemoteObj) -> bool {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return false,
        };
        if !Self::write_uri(&mut data, uri) {
            return false;
        }
        if data.write_remote(observer.clone()).is_err() {
            return false;
        }

        let mut reply = match self.send(IDataShareCmd::RegisterObserver as u32, &mut data) {
            Some(r) => r,
            None => return false,
        };
        reply.read::<i32>().unwrap_or(0) != 0
    }

    /// Unregister an observer.
    pub fn unregister_observer(&self, uri: &str, observer: &RemoteObj) -> bool {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return false,
        };
        if !Self::write_uri(&mut data, uri) {
            return false;
        }
        if data.write_remote(observer.clone()).is_err() {
            return false;
        }

        let mut reply = match self.send(IDataShareCmd::UnregisterObserver as u32, &mut data) {
            Some(r) => r,
            None => return false,
        };
        reply.read::<i32>().unwrap_or(0) != 0
    }

    /// Notify observers of a data change.
    pub fn notify_change(&self, uri: &str) -> bool {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return false,
        };
        if !Self::write_uri(&mut data, uri) {
            return false;
        }

        self.send(IDataShareCmd::NotifyChange as u32, &mut data)
            .is_some()
    }

    /// Register a provider-level observer.
    pub fn register_observer_ext_provider(
        &self,
        uri: &str,
        observer: &RemoteObj,
        is_descendants: bool,
        is_reconnect: bool,
    ) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return INVALID_VALUE,
        };
        if !Self::write_uri(&mut data, uri) {
            return INVALID_VALUE;
        }
        if data.write_remote(observer.clone()).is_err() {
            return INVALID_VALUE;
        }
        if data.write(&is_descendants).is_err() {
            return INVALID_VALUE;
        }
        if data.write(&is_reconnect).is_err() {
            return INVALID_VALUE;
        }

        let mut reply =
            match self.send(IDataShareCmd::RegisterObserverExtProvider as u32, &mut data) {
                Some(r) => r,
                None => return INVALID_VALUE,
            };
        reply.read::<i32>().unwrap_or(INVALID_VALUE)
    }

    /// Unregister a provider-level observer.
    pub fn unregister_observer_ext_provider(&self, uri: &str, observer: &RemoteObj) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return INVALID_VALUE,
        };
        if !Self::write_uri(&mut data, uri) {
            return INVALID_VALUE;
        }
        if data.write_remote(observer.clone()).is_err() {
            return INVALID_VALUE;
        }

        let mut reply = match self.send(
            IDataShareCmd::UnregisterObserverExtProvider as u32,
            &mut data,
        ) {
            Some(r) => r,
            None => return INVALID_VALUE,
        };
        reply.read::<i32>().unwrap_or(INVALID_VALUE)
    }

    /// Notify provider-level change.
    pub fn notify_change_ext_provider(&self, change_info: &ChangeInfo) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return INVALID_VALUE,
        };
        if data.write(change_info).is_err() {
            return INVALID_VALUE;
        }

        let mut reply = match self.send(IDataShareCmd::NotifyChangeExtProvider as u32, &mut data) {
            Some(r) => r,
            None => return INVALID_VALUE,
        };
        reply.read::<i32>().unwrap_or(INVALID_VALUE)
    }

    /// Normalize a URI.
    pub fn normalize_uri(&self, uri: &str) -> String {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return String::new(),
        };
        if !Self::write_uri(&mut data, uri) {
            return String::new();
        }

        let mut reply = match self.send(IDataShareCmd::NormalizeUri as u32, &mut data) {
            Some(r) => r,
            None => return String::new(),
        };
        Self::read_uri(&mut reply)
    }

    /// Denormalize a URI.
    pub fn denormalize_uri(&self, uri: &str) -> String {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return String::new(),
        };
        if !Self::write_uri(&mut data, uri) {
            return String::new();
        }

        let mut reply = match self.send(IDataShareCmd::DenormalizeUri as u32, &mut data) {
            Some(r) => r,
            None => return String::new(),
        };
        Self::read_uri(&mut reply)
    }

    /// Extended insert.
    pub fn insert_ex(&self, uri: &str, value: &DataShareValuesBucket) -> (i32, i32) {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return (INVALID_VALUE, 0),
        };
        if !Self::write_uri(&mut data, uri) {
            return (INVALID_VALUE, 0);
        }
        if data.write(value).is_err() {
            return (INVALID_VALUE, 0);
        }

        let mut reply = match self.send(IDataShareCmd::InsertEx as u32, &mut data) {
            Some(r) => r,
            None => return (INVALID_VALUE, 0),
        };
        let err_code = reply.read::<i32>().unwrap_or(INVALID_VALUE);
        let result = reply.read::<i32>().unwrap_or(0);
        (err_code, result)
    }

    /// Extended update.
    pub fn update_ex(
        &self,
        uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> (i32, i32) {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return (INVALID_VALUE, 0),
        };
        if !Self::write_uri(&mut data, uri) {
            return (INVALID_VALUE, 0);
        }
        if data.write(predicates).is_err() {
            return (INVALID_VALUE, 0);
        }
        if data.write(value).is_err() {
            return (INVALID_VALUE, 0);
        }

        let mut reply = match self.send(IDataShareCmd::UpdateEx as u32, &mut data) {
            Some(r) => r,
            None => return (INVALID_VALUE, 0),
        };
        let err_code = reply.read::<i32>().unwrap_or(INVALID_VALUE);
        let result = reply.read::<i32>().unwrap_or(0);
        (err_code, result)
    }

    /// Extended delete.
    pub fn delete_ex(&self, uri: &str, predicates: &DataSharePredicates) -> (i32, i32) {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return (INVALID_VALUE, 0),
        };
        if !Self::write_uri(&mut data, uri) {
            return (INVALID_VALUE, 0);
        }
        if data.write(predicates).is_err() {
            return (INVALID_VALUE, 0);
        }

        let mut reply = match self.send(IDataShareCmd::DeleteEx as u32, &mut data) {
            Some(r) => r,
            None => return (INVALID_VALUE, 0),
        };
        let err_code = reply.read::<i32>().unwrap_or(INVALID_VALUE);
        let result = reply.read::<i32>().unwrap_or(0);
        (err_code, result)
    }

    /// User-defined function dispatch.
    /// Takes raw data/reply parcels, forwarding directly to the remote.
    pub fn user_define_func(&self, data: &mut MsgParcel) -> Option<MsgParcel> {
        self.send(IDataShareCmd::UserDefineFunc as u32, data)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_proxy_constants() {
        assert_eq!(MTU_SIZE, 921_600);
        assert_eq!(MAX_SIZE, 4000);
        assert_eq!(PERMISSION_ERR, 1);
        assert_eq!(PERMISSION_ERR_CODE, -2);
        assert_eq!(INVALID_VALUE, -1);
        assert_eq!(DESCRIPTOR, "OHOS.DataShare.IDataShare");
    }
}
