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

//! IPC Proxy for ISharedResultSet (client-side stub).
//!
//! Equivalent to C++ `ishared_result_set_proxy.cpp`.
//!
//! Sends IPC requests to a remote `ISharedResultSetStub` and caches
//! row count and column names for performance.

use std::sync::Mutex;

use ipc::parcel::MsgParcel;
use ipc::remote::RemoteObj;
use ipc::{IpcResult, IpcStatusCode};

use super::stub::{SharedResultCode, ISHARED_RESULT_SET_DESCRIPTOR};

/// Error code: success.
const E_OK: i32 = 0;
/// Error code: invalid parcel data.
const E_INVALID_PARCEL: i32 = -4;
/// Error code: invalid file descriptor / descriptor mismatch.
const INVALID_FD: i32 = -1;

/// Client-side proxy for ISharedResultSet.
///
/// Equivalent to C++ `ISharedResultSetProxy`. Wraps a `RemoteObj` and
/// sends IPC requests to the server-side stub.
///
/// Caches `row_count` and `column_names` to avoid repeated IPC calls,
/// matching C++ behavior.
pub struct ISharedResultSetProxy {
    remote: RemoteObj,
    /// Cached row count. -1 means not cached.
    row_count: Mutex<i32>,
    /// Cached column names. Empty vec means not cached.
    column_names: Mutex<Vec<String>>,
}

impl ISharedResultSetProxy {
    /// Creates a new proxy wrapping the given remote object.
    pub fn new(remote: RemoteObj) -> Self {
        Self {
            remote,
            row_count: Mutex::new(-1),
            column_names: Mutex::new(Vec::new()),
        }
    }

    /// Get a reference to the underlying remote object.
    /// Used by FFI to extract the raw IRemoteObject pointer for C++ bridging.
    pub fn remote(&self) -> &RemoteObj {
        &self.remote
    }

    /// Helper: create a request MsgParcel with interface token.
    fn create_request(&self) -> Result<MsgParcel, i32> {
        let mut request = MsgParcel::new();
        request
            .write_interface_token(ISHARED_RESULT_SET_DESCRIPTOR)
            .map_err(|_| INVALID_FD)?;
        Ok(request)
    }

    /// Helper: send a request and return the reply.
    fn send_request(&self, code: u32, request: &mut MsgParcel) -> Result<MsgParcel, i32> {
        self.remote
            .send_request(code, request)
            .map_err(|e| match e {
                IpcStatusCode::ServiceDied => -29189,
                _ => -1,
            })
    }

    /// Get row count from remote. Caches the result after first call.
    ///
    /// Equivalent to C++ `ISharedResultSetProxy::GetRowCount`.
    pub fn get_row_count(&self) -> Result<i32, i32> {
        // Check cache
        {
            let cached = self.row_count.lock().unwrap();
            if *cached >= 0 {
                return Ok(*cached);
            }
        }

        let mut request = self.create_request()?;
        let mut reply =
            self.send_request(SharedResultCode::FuncGetRowCount as u32, &mut request)?;

        let err_code: i32 = reply.read().map_err(|_| E_INVALID_PARCEL)?;
        if err_code != E_OK {
            return Err(err_code);
        }

        let count: i32 = reply.read().map_err(|_| E_INVALID_PARCEL)?;
        // Cache it
        *self.row_count.lock().unwrap() = count;
        Ok(count)
    }

    /// Get all column names from remote. Caches the result after first call.
    ///
    /// Equivalent to C++ `ISharedResultSetProxy::GetAllColumnNames`.
    pub fn get_all_column_names(&self) -> Result<Vec<String>, i32> {
        // Check cache
        {
            let cached = self.column_names.lock().unwrap();
            if !cached.is_empty() {
                return Ok(cached.clone());
            }
        }

        let mut request = self.create_request()?;
        let mut reply =
            self.send_request(SharedResultCode::FuncGetAllColumnNames as u32, &mut request)?;

        let err_code: i32 = reply.read().map_err(|_| E_INVALID_PARCEL)?;
        if err_code != E_OK {
            return Err(err_code);
        }

        let names: Vec<String> = reply.read().map_err(|_| E_INVALID_PARCEL)?;
        // Cache it
        *self.column_names.lock().unwrap() = names.clone();
        Ok(names)
    }

    /// Request OnGo (pagination) from remote.
    ///
    /// Returns the cached index on success, or `false` indicator via Err.
    /// Equivalent to C++ `ISharedResultSetProxy::OnGo`.
    pub fn on_go(&self, old_row_index: i32, new_row_index: i32) -> Result<i32, i32> {
        let mut request = self.create_request()?;
        request.write(&old_row_index).map_err(|_| INVALID_FD)?;
        request.write(&new_row_index).map_err(|_| INVALID_FD)?;

        let mut reply = self.send_request(SharedResultCode::FuncOnGo as u32, &mut request)?;

        let cached_index: i32 = reply.read().map_err(|_| E_INVALID_PARCEL)?;
        if cached_index < 0 {
            Err(-1) // OnGo failed
        } else {
            Ok(cached_index)
        }
    }

    /// Request Close from remote.
    ///
    /// Equivalent to C++ `ISharedResultSetProxy::Close`.
    pub fn close(&self) -> Result<i32, i32> {
        let mut request = self.create_request()?;
        let mut reply = self.send_request(SharedResultCode::FuncClose as u32, &mut request)?;

        let err_code: i32 = reply.read().map_err(|_| E_INVALID_PARCEL)?;
        Ok(err_code)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use ipc::remote::RemoteStub;

    // Helper: create a stub-backed remote for testing
    fn create_test_proxy() -> ISharedResultSetProxy {
        use crate::ipc::stub::ISharedResultSetStub;
        use crate::result_set::DataShareResultSet;
        use crate::shared_block::SharedBlock;

        let mut rs = DataShareResultSet::new();
        let mut block = SharedBlock::create("test", 4096).unwrap();
        block.set_column_num(2).unwrap();
        block.alloc_row().unwrap();
        block.put_long(0, 0, 42).unwrap();
        block.put_string(0, 1, "hello").unwrap();
        rs.set_block(block);
        rs.set_column_names(vec!["id".to_string(), "name".to_string()]);

        let stub = ISharedResultSetStub::new(rs);
        let remote = RemoteObj::from_stub(stub).unwrap();
        ISharedResultSetProxy::new(remote)
    }

    #[test]
    fn test_proxy_get_row_count() {
        let proxy = create_test_proxy();
        let count = proxy.get_row_count().unwrap();
        assert_eq!(count, 1);

        // Second call should use cache
        let count2 = proxy.get_row_count().unwrap();
        assert_eq!(count2, 1);
    }

    #[test]
    fn test_proxy_get_all_column_names() {
        let proxy = create_test_proxy();
        let names = proxy.get_all_column_names().unwrap();
        assert_eq!(names, vec!["id", "name"]);

        // Second call should use cache
        let names2 = proxy.get_all_column_names().unwrap();
        assert_eq!(names2, vec!["id", "name"]);
    }

    #[test]
    fn test_proxy_close() {
        let proxy = create_test_proxy();
        let result = proxy.close().unwrap();
        assert_eq!(result, E_OK);
    }

    #[test]
    fn test_proxy_on_go_no_bridge() {
        let proxy = create_test_proxy();
        // No bridge, so OnGo returns -1
        let result = proxy.on_go(0, 0);
        assert!(result.is_err() || result.unwrap() < 0);
    }
}
