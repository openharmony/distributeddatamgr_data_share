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

//! IPC Stub for ISharedResultSet (server-side handler).
//!
//! Equivalent to C++ `ishared_result_set_stub.cpp`.
//!
//! Dispatches incoming IPC requests by command code to the appropriate handler:
//! - `FUNC_GET_ROW_COUNT` (0)
//! - `FUNC_GET_ALL_COLUMN_NAMES` (1)
//! - `FUNC_ON_GO` (2)
//! - `FUNC_CLOSE` (3)

use std::sync::Mutex;
use std::time::Instant;

use ipc::parcel::MsgParcel;
use ipc::remote::RemoteStub;

use crate::result_set::{DataShareResultSet, ResultSet};

/// Descriptor for the ISharedResultSet IPC interface.
pub const ISHARED_RESULT_SET_DESCRIPTOR: &str = "OHOS.DataShare.ISharedResultSet";

/// Command codes for ISharedResultSet IPC interface.
///
/// Equivalent to C++ `ISharedResultInterfaceCode` (only the 4 used in stub/proxy).
#[repr(u32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SharedResultCode {
    FuncGetRowCount = 0,
    FuncGetAllColumnNames = 1,
    FuncOnGo = 2,
    FuncClose = 3,
}

/// Total number of valid function codes (C++ FUNC_BUTT equivalent).
const FUNC_BUTT: u32 = 4;

/// Error codes matching C++ `datashare_errno.h`.
pub(crate) const E_OK: i32 = 0;

/// Performance threshold for handler execution (500ms).
const TIME_THRESHOLD_MS: u128 = 500;

/// Server-side IPC handler for DataShareResultSet.
///
/// Equivalent to C++ `ISharedResultSetStub`.
/// Holds a `Mutex<DataShareResultSet>` since some handlers (OnGo, Close) need `&mut self`.
pub struct ISharedResultSetStub {
    result_set: Mutex<DataShareResultSet>,
}

impl ISharedResultSetStub {
    /// Creates a new stub wrapping the given result set.
    pub fn new(result_set: DataShareResultSet) -> Self {
        Self {
            result_set: Mutex::new(result_set),
        }
    }

    /// Handles GetRowCount request.
    /// C++: writes errCode + count to reply.
    fn handle_get_row_count(&self, _data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        let rs = self.result_set.lock().unwrap();
        match rs.get_row_count() {
            Ok(count) => {
                let _ = reply.write(&E_OK);
                let _ = reply.write(&count);
            }
            Err(_) => {
                let _ = reply.write(&(-1i32));
            }
        }
        0 // NO_ERROR
    }

    /// Handles GetAllColumnNames request.
    /// C++: writes errCode + string vector to reply.
    fn handle_get_all_column_names(&self, _data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        let rs = self.result_set.lock().unwrap();
        match rs.get_all_column_names() {
            Ok(names) => {
                let _ = reply.write(&E_OK);
                let _ = reply.write(&names);
            }
            Err(_) => {
                let _ = reply.write(&(-1i32));
            }
        }
        0 // NO_ERROR
    }

    /// Handles OnGo request (pagination).
    /// C++: reads oldRow + newRow from data, calls OnGo, writes cachedIndex to reply.
    fn handle_on_go(&self, data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        let old_row = match data.read::<i32>() {
            Ok(v) => v,
            Err(_) => {
                let _ = reply.write(&(-1i32));
                return 0;
            }
        };
        let new_row = match data.read::<i32>() {
            Ok(v) => v,
            Err(_) => {
                let _ = reply.write(&(-1i32));
                return 0;
            }
        };

        let mut rs = self.result_set.lock().unwrap();
        match rs.on_go(old_row, new_row) {
            Ok(cached_index) => {
                let _ = reply.write(&cached_index);
            }
            Err(_) => {
                let _ = reply.write(&(-1i32));
            }
        }
        0 // NO_ERROR
    }

    /// Handles Close request.
    /// C++: calls Close(), writes errCode to reply.
    fn handle_close(&self, _data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        let mut rs = self.result_set.lock().unwrap();
        match rs.close() {
            Ok(_) => {
                let _ = reply.write(&E_OK);
            }
            Err(_) => {
                let _ = reply.write(&(-1i32));
            }
        }
        0 // NO_ERROR
    }
}

impl RemoteStub for ISharedResultSetStub {
    fn on_remote_request(&self, code: u32, data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        // Validate interface token (matches C++ OnRemoteRequest behavior)
        if let Ok(remote_descriptor) = data.read_interface_token() {
            if remote_descriptor != ISHARED_RESULT_SET_DESCRIPTOR {
                return -1; // INVALID_FD
            }
        } else {
            return -1; // INVALID_FD
        }

        // Validate command code range (C++ checks code >= FUNC_BUTT)
        if code >= FUNC_BUTT {
            return -1;
        }

        let start = Instant::now();

        // Dispatch to handler
        let result = match code {
            0 => self.handle_get_row_count(data, reply),
            1 => self.handle_get_all_column_names(data, reply),
            2 => self.handle_on_go(data, reply),
            3 => self.handle_close(data, reply),
            _ => -1,
        };

        // Performance monitoring (C++ threshold: 500ms)
        let duration = start.elapsed();
        if duration.as_millis() >= TIME_THRESHOLD_MS {
            // In production: hilog warning about slow handler
        }

        result
    }

    fn descriptor(&self) -> &'static str {
        ISHARED_RESULT_SET_DESCRIPTOR
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::shared_block::SharedBlock;

    /// Helper: write interface token to a MsgParcel for direct stub testing.
    fn prepare_request() -> MsgParcel {
        let mut data = MsgParcel::new();
        data.write_interface_token(ISHARED_RESULT_SET_DESCRIPTOR)
            .unwrap();
        data
    }

    fn create_stub_with_data() -> ISharedResultSetStub {
        let mut rs = DataShareResultSet::new();
        let mut block = SharedBlock::create("test", 4096).unwrap();
        block.set_column_num(2).unwrap();
        block.alloc_row().unwrap();
        block.put_long(0, 0, 42).unwrap();
        block.put_string(0, 1, "hello").unwrap();
        rs.set_block(block);
        rs.set_column_names(vec!["id".to_string(), "name".to_string()]);
        ISharedResultSetStub::new(rs)
    }

    #[test]
    fn test_shared_result_code_values() {
        assert_eq!(SharedResultCode::FuncGetRowCount as u32, 0);
        assert_eq!(SharedResultCode::FuncGetAllColumnNames as u32, 1);
        assert_eq!(SharedResultCode::FuncOnGo as u32, 2);
        assert_eq!(SharedResultCode::FuncClose as u32, 3);
    }

    #[test]
    fn test_stub_descriptor() {
        let stub = ISharedResultSetStub::new(DataShareResultSet::new());
        assert_eq!(stub.descriptor(), "OHOS.DataShare.ISharedResultSet");
    }

    #[test]
    fn test_stub_invalid_code() {
        let stub = ISharedResultSetStub::new(DataShareResultSet::new());
        let mut data = prepare_request();
        let mut reply = MsgParcel::new();
        assert_eq!(stub.on_remote_request(99, &mut data, &mut reply), -1);
    }

    #[test]
    fn test_stub_invalid_descriptor() {
        let stub = ISharedResultSetStub::new(DataShareResultSet::new());
        let mut data = MsgParcel::new();
        data.write_interface_token("OHOS.Wrong.Descriptor").unwrap();
        let mut reply = MsgParcel::new();
        assert_eq!(
            stub.on_remote_request(
                SharedResultCode::FuncGetRowCount as u32,
                &mut data,
                &mut reply
            ),
            -1
        );
    }

    #[test]
    fn test_stub_get_row_count() {
        let stub = create_stub_with_data();
        let mut data = prepare_request();
        let mut reply = MsgParcel::new();

        let ret = stub.on_remote_request(
            SharedResultCode::FuncGetRowCount as u32,
            &mut data,
            &mut reply,
        );
        assert_eq!(ret, 0);

        let err_code: i32 = reply.read().unwrap();
        assert_eq!(err_code, E_OK);
        let count: i32 = reply.read().unwrap();
        assert_eq!(count, 1);
    }

    #[test]
    fn test_stub_get_all_column_names() {
        let stub = create_stub_with_data();
        let mut data = prepare_request();
        let mut reply = MsgParcel::new();

        let ret = stub.on_remote_request(
            SharedResultCode::FuncGetAllColumnNames as u32,
            &mut data,
            &mut reply,
        );
        assert_eq!(ret, 0);

        let err_code: i32 = reply.read().unwrap();
        assert_eq!(err_code, E_OK);
        let names: Vec<String> = reply.read().unwrap();
        assert_eq!(names, vec!["id", "name"]);
    }

    #[test]
    fn test_stub_on_go_no_bridge() {
        let stub = create_stub_with_data();
        let mut data = prepare_request();
        data.write(&0i32).unwrap(); // oldRow
        data.write(&0i32).unwrap(); // newRow
        let mut reply = MsgParcel::new();

        let ret = stub.on_remote_request(SharedResultCode::FuncOnGo as u32, &mut data, &mut reply);
        assert_eq!(ret, 0);

        // No bridge, so on_go returns -1
        let cached_index: i32 = reply.read().unwrap();
        assert_eq!(cached_index, -1);
    }

    #[test]
    fn test_stub_close() {
        let stub = ISharedResultSetStub::new(DataShareResultSet::new());
        let mut data = prepare_request();
        let mut reply = MsgParcel::new();

        let ret = stub.on_remote_request(SharedResultCode::FuncClose as u32, &mut data, &mut reply);
        assert_eq!(ret, 0);

        let err_code: i32 = reply.read().unwrap();
        assert_eq!(err_code, E_OK);
    }

    #[test]
    fn test_stub_close_twice() {
        let stub = ISharedResultSetStub::new(DataShareResultSet::new());

        // First close succeeds
        let mut data1 = prepare_request();
        let mut reply1 = MsgParcel::new();
        stub.on_remote_request(SharedResultCode::FuncClose as u32, &mut data1, &mut reply1);
        let err_code: i32 = reply1.read().unwrap();
        assert_eq!(err_code, E_OK);

        // Second close fails (already closed)
        let mut data2 = prepare_request();
        let mut reply2 = MsgParcel::new();
        stub.on_remote_request(SharedResultCode::FuncClose as u32, &mut data2, &mut reply2);
        let err_code2: i32 = reply2.read().unwrap();
        assert_eq!(err_code2, -1);
    }
}
