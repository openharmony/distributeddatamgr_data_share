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

//! IPC command codes for DataShare interfaces.
//!
//! Corresponds to C++ enums in
//! `frameworks/native/common/include/distributeddata_data_share_ipc_interface_code.h`.

/// IDataShare interface command codes (extension-side IPC).
///
/// Corresponds to C++ `enum class IDataShareInterfaceCode`.
#[repr(u32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum IDataShareCmd {
    GetFileTypes = 1,
    OpenFile = 2,
    OpenRawFile = 3,
    Insert = 4,
    Update = 5,
    Delete = 6,
    Query = 7,
    GetType = 8,
    BatchInsert = 9,
    RegisterObserver = 10,
    UnregisterObserver = 11,
    NotifyChange = 12,
    NormalizeUri = 13,
    DenormalizeUri = 14,
    ExecuteBatch = 15,
    InsertExt = 16,
    BatchUpdate = 17,
    InsertEx = 18,
    UpdateEx = 19,
    DeleteEx = 20,
    UserDefineFunc = 21,
    RegisterObserverExtProvider = 22,
    UnregisterObserverExtProvider = 23,
    NotifyChangeExtProvider = 24,
    OpenFileWithErrCode = 25,
}

/// DataShareService interface command codes (service-side IPC).
///
/// Corresponds to C++ `enum class DataShareServiceInterfaceCode`.
#[repr(u32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DataShareServiceCmd {
    Query = 0,
    AddTemplate = 1,
    DelTemplate = 2,
    Publish = 3,
    GetData = 4,
    SubscribeRdb = 5,
    UnsubscribeRdb = 6,
    EnableSubscribeRdb = 7,
    DisableSubscribeRdb = 8,
    SubscribePublished = 9,
    UnsubscribePublished = 10,
    EnableSubscribePublished = 11,
    DisableSubscribePublished = 12,
    Notify = 13,
    NotifyObservers = 14,
    SetSilentSwitch = 15,
    GetSilentProxyStatus = 16,
    RegisterObserver = 17,
    UnregisterObserver = 18,
    InsertEx = 19,
    DeleteEx = 20,
    UpdateEx = 21,
    ProxyPublish = 22,
    ProxyDelete = 23,
    ProxyGet = 24,
    SubscribeProxyData = 25,
    UnsubscribeProxyData = 26,
    GetConnectionInterfaceInfo = 27,
}

/// System app command code offset.
///
/// Corresponds to C++ `DATA_SHARE_CMD_SYSTEM_CODE = 100`.
pub const SYSTEM_CODE_OFFSET: u32 = 100;

impl DataShareServiceCmd {
    /// Convert to system app command code.
    pub fn to_system_code(self) -> u32 {
        (self as u32) + SYSTEM_CODE_OFFSET
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_idatashare_cmd_values() {
        assert_eq!(IDataShareCmd::GetFileTypes as u32, 1);
        assert_eq!(IDataShareCmd::Insert as u32, 4);
        assert_eq!(IDataShareCmd::Query as u32, 7);
        assert_eq!(IDataShareCmd::OpenFileWithErrCode as u32, 25);
    }

    #[test]
    fn test_service_cmd_values() {
        assert_eq!(DataShareServiceCmd::Query as u32, 0);
        assert_eq!(DataShareServiceCmd::Publish as u32, 3);
        assert_eq!(DataShareServiceCmd::GetSilentProxyStatus as u32, 16);
    }

    #[test]
    fn test_system_code_offset() {
        assert_eq!(SYSTEM_CODE_OFFSET, 100);
        assert_eq!(DataShareServiceCmd::Query.to_system_code(), 100);
        assert_eq!(DataShareServiceCmd::Publish.to_system_code(), 103);
    }
}
