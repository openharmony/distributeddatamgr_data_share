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

//! DataShareStub — IPC service-side stub with command dispatcher.
//!
//! Corresponds to C++ `DataShareStub` in
//! `frameworks/native/provider/src/datashare_stub.cpp` (689 lines).
//!
//! Maintains a `stub_func_map` that maps IPC command codes
//! (from `IDataShareInterfaceCode`) to handler methods.
//! `on_remote_request()` dispatches incoming IPC commands
//! to the appropriate handler.

use std::collections::HashMap;
use std::time::Instant;

use crate::ability::ext_ability::DataShareExtAbility;

/// Default error number for inner errors.
pub const DEFAULT_NUMBER: i32 = -1;

/// Permission error number.
pub const PERMISSION_ERROR_NUMBER: i32 = -2;

/// Time threshold for logging slow IPC handlers (milliseconds).
const TIME_THRESHOLD_MS: u128 = 1000;

/// IPC command codes for DataShare interface.
///
/// Corresponds to C++ `IDataShareInterfaceCode` enum.
#[repr(u32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum IDataShareInterfaceCode {
    CmdGetFileTypes = 1,
    CmdOpenFile = 2,
    CmdOpenRawFile = 3,
    CmdInsert = 4,
    CmdUpdate = 5,
    CmdDelete = 6,
    CmdQuery = 7,
    CmdGetType = 8,
    CmdBatchInsert = 9,
    CmdRegisterObserver = 10,
    CmdUnregisterObserver = 11,
    CmdNotifyChange = 12,
    CmdNormalizeUri = 13,
    CmdDenormalizeUri = 14,
    CmdExecuteBatch = 15,
    CmdInsertExt = 16,
    CmdBatchUpdate = 17,
    CmdInsertEx = 18,
    CmdUpdateEx = 19,
    CmdDeleteEx = 20,
    CmdUserDefineFunc = 21,
    CmdRegisterObserverExtProvider = 22,
    CmdUnregisterObserverExtProvider = 23,
    CmdNotifyChangeExtProvider = 24,
    CmdOpenFileWithErrCode = 25,
}

impl IDataShareInterfaceCode {
    /// Convert from u32 to IDataShareInterfaceCode.
    pub fn from_u32(code: u32) -> Option<Self> {
        match code {
            1 => Some(Self::CmdGetFileTypes),
            2 => Some(Self::CmdOpenFile),
            3 => Some(Self::CmdOpenRawFile),
            4 => Some(Self::CmdInsert),
            5 => Some(Self::CmdUpdate),
            6 => Some(Self::CmdDelete),
            7 => Some(Self::CmdQuery),
            8 => Some(Self::CmdGetType),
            9 => Some(Self::CmdBatchInsert),
            10 => Some(Self::CmdRegisterObserver),
            11 => Some(Self::CmdUnregisterObserver),
            12 => Some(Self::CmdNotifyChange),
            13 => Some(Self::CmdNormalizeUri),
            14 => Some(Self::CmdDenormalizeUri),
            15 => Some(Self::CmdExecuteBatch),
            16 => Some(Self::CmdInsertExt),
            17 => Some(Self::CmdBatchUpdate),
            18 => Some(Self::CmdInsertEx),
            19 => Some(Self::CmdUpdateEx),
            20 => Some(Self::CmdDeleteEx),
            21 => Some(Self::CmdUserDefineFunc),
            22 => Some(Self::CmdRegisterObserverExtProvider),
            23 => Some(Self::CmdUnregisterObserverExtProvider),
            24 => Some(Self::CmdNotifyChangeExtProvider),
            25 => Some(Self::CmdOpenFileWithErrCode),
            _ => None,
        }
    }
}

/// Error codes used by the stub.
pub mod err_code {
    pub const E_OK: i32 = 0;
    pub const ERR_INVALID_VALUE: i32 = -1;
    pub const ERR_INVALID_STATE: i32 = -2;
    pub const ERR_INVALID_DATA: i32 = -3;
    pub const ERR_PERMISSION_DENIED: i32 = -4;
    pub const E_UNMARSHAL_ERROR: i32 = -5;
    pub const E_MARSHAL_ERROR: i32 = -6;
}

/// Placeholder for MessageParcel (IPC data parcel).
pub struct MessageParcel {
    _private: (),
}

impl MessageParcel {
    pub fn new() -> Self {
        Self { _private: () }
    }
}

impl Default for MessageParcel {
    fn default() -> Self {
        Self::new()
    }
}

/// Placeholder for MessageOption (IPC options).
pub struct MessageOption {
    _private: (),
}

impl MessageOption {
    pub fn new() -> Self {
        Self { _private: () }
    }
}

impl Default for MessageOption {
    fn default() -> Self {
        Self::new()
    }
}

/// Type alias for stub handler functions.
type StubHandler = fn(&DataShareStub, &MessageParcel, &mut MessageParcel) -> i32;

/// DataShareStub — IPC service-side stub.
///
/// Corresponds to C++ `DataShareStub`.
///
/// Maintains a `stub_func_map` mapping IPC command codes to handler functions.
/// `on_remote_request()` dispatches incoming IPC commands.
pub struct DataShareStub {
    /// Map of IPC command code → handler function.
    stub_func_map: HashMap<u32, StubHandler>,
    /// Extension ability that handles actual business logic.
    _extension: Option<Box<dyn DataShareExtAbility>>,
}

impl DataShareStub {
    /// Create a new DataShareStub with all command handlers registered.
    ///
    /// Corresponds to C++ `DataShareStub::DataShareStub()`.
    pub fn new() -> Self {
        let mut map: HashMap<u32, StubHandler> = HashMap::new();

        map.insert(
            IDataShareInterfaceCode::CmdGetFileTypes as u32,
            Self::cmd_get_file_types,
        );
        map.insert(
            IDataShareInterfaceCode::CmdOpenFile as u32,
            Self::cmd_open_file,
        );
        map.insert(
            IDataShareInterfaceCode::CmdOpenFileWithErrCode as u32,
            Self::cmd_open_file_with_err_code,
        );
        map.insert(
            IDataShareInterfaceCode::CmdOpenRawFile as u32,
            Self::cmd_open_raw_file,
        );
        map.insert(IDataShareInterfaceCode::CmdInsert as u32, Self::cmd_insert);
        map.insert(IDataShareInterfaceCode::CmdUpdate as u32, Self::cmd_update);
        map.insert(IDataShareInterfaceCode::CmdDelete as u32, Self::cmd_delete);
        map.insert(IDataShareInterfaceCode::CmdQuery as u32, Self::cmd_query);
        map.insert(
            IDataShareInterfaceCode::CmdGetType as u32,
            Self::cmd_get_type,
        );
        map.insert(
            IDataShareInterfaceCode::CmdBatchInsert as u32,
            Self::cmd_batch_insert,
        );
        map.insert(
            IDataShareInterfaceCode::CmdRegisterObserver as u32,
            Self::cmd_register_observer,
        );
        map.insert(
            IDataShareInterfaceCode::CmdUnregisterObserver as u32,
            Self::cmd_unregister_observer,
        );
        map.insert(
            IDataShareInterfaceCode::CmdNotifyChange as u32,
            Self::cmd_notify_change,
        );
        map.insert(
            IDataShareInterfaceCode::CmdNormalizeUri as u32,
            Self::cmd_normalize_uri,
        );
        map.insert(
            IDataShareInterfaceCode::CmdDenormalizeUri as u32,
            Self::cmd_denormalize_uri,
        );
        map.insert(
            IDataShareInterfaceCode::CmdExecuteBatch as u32,
            Self::cmd_execute_batch,
        );
        map.insert(
            IDataShareInterfaceCode::CmdInsertExt as u32,
            Self::cmd_insert_ext,
        );
        map.insert(
            IDataShareInterfaceCode::CmdBatchUpdate as u32,
            Self::cmd_batch_update,
        );
        map.insert(
            IDataShareInterfaceCode::CmdInsertEx as u32,
            Self::cmd_insert_ex,
        );
        map.insert(
            IDataShareInterfaceCode::CmdUpdateEx as u32,
            Self::cmd_update_ex,
        );
        map.insert(
            IDataShareInterfaceCode::CmdDeleteEx as u32,
            Self::cmd_delete_ex,
        );
        map.insert(
            IDataShareInterfaceCode::CmdRegisterObserverExtProvider as u32,
            Self::cmd_register_observer_ext_provider,
        );
        map.insert(
            IDataShareInterfaceCode::CmdUnregisterObserverExtProvider as u32,
            Self::cmd_unregister_observer_ext_provider,
        );
        map.insert(
            IDataShareInterfaceCode::CmdNotifyChangeExtProvider as u32,
            Self::cmd_notify_change_ext_provider,
        );

        Self {
            stub_func_map: map,
            _extension: None,
        }
    }

    /// Get the number of registered command handlers.
    pub fn handler_count(&self) -> usize {
        self.stub_func_map.len()
    }

    /// Check if a command code has a registered handler.
    pub fn has_handler(&self, code: u32) -> bool {
        self.stub_func_map.contains_key(&code)
    }

    /// Dispatch an IPC remote request.
    ///
    /// Corresponds to C++ `DataShareStub::OnRemoteRequest()`.
    pub fn on_remote_request(
        &self,
        code: u32,
        data: &MessageParcel,
        reply: &mut MessageParcel,
        _option: &MessageOption,
    ) -> i32 {
        // TODO: Verify interface descriptor token
        // In C++: check descriptor == remoteDescriptor

        let start = Instant::now();
        let mut is_code_valid = false;

        let ret = if let Some(handler) = self.stub_func_map.get(&code) {
            is_code_valid = true;
            handler(self, data, reply)
        } else if code == IDataShareInterfaceCode::CmdUserDefineFunc as u32 {
            is_code_valid = true;
            self.cmd_user_define_func(data, reply, _option)
        } else {
            0
        };

        if is_code_valid {
            let duration_ms = start.elapsed().as_millis();
            if duration_ms >= TIME_THRESHOLD_MS {
                // TODO: Log slow IPC handler
                // LOG_ERROR("extension time over, code:{} cost:{}ms", code, duration_ms);
            }
            return ret;
        }

        // Unhandled command — return to base stub
        // TODO: Call IPCObjectStub::OnRemoteRequest(code, data, reply, option)
        0
    }

    // ========================================================================
    // Command handlers
    // Each corresponds to a C++ CmdXxx method in datashare_stub.cpp
    // ========================================================================

    /// Corresponds to C++ `CmdGetFileTypes`.
    fn cmd_get_file_types(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri, mimeTypeFilter)
        // TODO: Call GetFileTypes(uri, mimeTypeFilter)
        // TODO: Marshal(reply, types)
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdOpenFile`.
    fn cmd_open_file(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Call OpenFileInner(data, reply, fd)
        // TODO: WriteFileDescriptor(fd) to reply
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdOpenFileWithErrCode`.
    fn cmd_open_file_with_err_code(
        &self,
        _data: &MessageParcel,
        _reply: &mut MessageParcel,
    ) -> i32 {
        // TODO: Call OpenFileInner(data, reply, fd)
        // TODO: WriteFileDescriptor(fd) to reply
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdOpenRawFile`.
    fn cmd_open_raw_file(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri, mode)
        // TODO: Call OpenRawFile(uri, mode)
        // TODO: Marshal(reply, fd)
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdInsert`.
    fn cmd_insert(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri, value)
        // TODO: Call Insert(uri, value)
        // TODO: Check DEFAULT_NUMBER / PERMISSION_ERROR_NUMBER
        // TODO: WriteInt32(index) to reply
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdUpdate`.
    fn cmd_update(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri, predicates, value)
        // TODO: Call Update(uri, predicates, value)
        // TODO: Check DEFAULT_NUMBER / PERMISSION_ERROR_NUMBER
        // TODO: WriteInt32(index) to reply
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdDelete`.
    fn cmd_delete(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri, predicates)
        // TODO: Call Delete(uri, predicates)
        // TODO: Check DEFAULT_NUMBER / PERMISSION_ERROR_NUMBER
        // TODO: WriteInt32(index) to reply
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdQuery`.
    fn cmd_query(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri, columns)
        // TODO: UnmarshalPredicates(predicates, data)
        // TODO: Call Query(uri, predicates, columns, businessError)
        // TODO: ISharedResultSet::WriteToParcel(resultSet, reply)
        // TODO: WriteInt32(businessError.GetCode()), WriteString(businessError.GetMessage())
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdGetType`.
    fn cmd_get_type(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri)
        // TODO: Call GetType(uri)
        // TODO: WriteString(type) to reply
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdBatchInsert`.
    fn cmd_batch_insert(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri)
        // TODO: UnmarshalValuesBucketVec(values, data)
        // TODO: Call BatchInsert(uri, values)
        // TODO: WriteInt32(ret) to reply
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdRegisterObserver`.
    fn cmd_register_observer(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri, observer)
        // TODO: iface_cast to IDataAbilityObserver
        // TODO: Call RegisterObserver(uri, obServer)
        // TODO: WriteInt32(ret) to reply
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdUnregisterObserver`.
    fn cmd_unregister_observer(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri, observer)
        // TODO: iface_cast to IDataAbilityObserver
        // TODO: Call UnregisterObserver(uri, obServer)
        // TODO: WriteInt32(ret) to reply
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdNotifyChange`.
    fn cmd_notify_change(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri)
        // TODO: Call NotifyChange(uri)
        // TODO: WriteInt32(ret) to reply
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdNormalizeUri`.
    fn cmd_normalize_uri(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri)
        // TODO: Call NormalizeUri(uri)
        // TODO: Marshal(reply, ret)
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdDenormalizeUri`.
    fn cmd_denormalize_uri(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri)
        // TODO: Call DenormalizeUri(uri)
        // TODO: Marshal(reply, ret)
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdExecuteBatch`.
    fn cmd_execute_batch(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, statements)
        // TODO: Call ExecuteBatch(statements, result)
        // TODO: Marshal(reply, result)
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdInsertExt`.
    fn cmd_insert_ext(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri, value)
        // TODO: Call InsertExt(uri, value, result)
        // TODO: Marshal(reply, index, result)
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdBatchUpdate`.
    fn cmd_batch_update(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, updateOperations)
        // TODO: Call BatchUpdate(updateOperations, results)
        // TODO: Marshal(reply, results)
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdInsertEx`.
    fn cmd_insert_ex(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri, value)
        // TODO: Call InsertEx(uri, value) → (errCode, result)
        // TODO: Marshal(reply, errCode, result)
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdUpdateEx`.
    fn cmd_update_ex(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri, predicates, value)
        // TODO: Call UpdateEx(uri, predicates, value) → (errCode, result)
        // TODO: Marshal(reply, errCode, result)
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdDeleteEx`.
    fn cmd_delete_ex(&self, _data: &MessageParcel, _reply: &mut MessageParcel) -> i32 {
        // TODO: Unmarshal(data, uri, predicates)
        // TODO: Call DeleteEx(uri, predicates) → (errCode, result)
        // TODO: Marshal(reply, errCode, result)
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdRegisterObserverExtProvider`.
    fn cmd_register_observer_ext_provider(
        &self,
        _data: &MessageParcel,
        _reply: &mut MessageParcel,
    ) -> i32 {
        // TODO: Unmarshal(data, uri, observer, isDescendants, option)
        // TODO: Call RegisterObserverExtProvider(uri, obServer, isDescendants, option)
        // TODO: WriteInt32(ret) to reply
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdUnregisterObserverExtProvider`.
    fn cmd_unregister_observer_ext_provider(
        &self,
        _data: &MessageParcel,
        _reply: &mut MessageParcel,
    ) -> i32 {
        // TODO: Unmarshal(data, uri, observer)
        // TODO: Call UnregisterObserverExtProvider(uri, obServer)
        // TODO: WriteInt32(ret) to reply
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdNotifyChangeExtProvider`.
    fn cmd_notify_change_ext_provider(
        &self,
        _data: &MessageParcel,
        _reply: &mut MessageParcel,
    ) -> i32 {
        // TODO: Unmarshalling changeInfo from data
        // TODO: Call NotifyChangeExtProvider(changeInfo)
        // TODO: WriteInt32(ret) to reply
        err_code::E_OK
    }

    /// Corresponds to C++ `CmdUserDefineFunc`.
    fn cmd_user_define_func(
        &self,
        _data: &MessageParcel,
        _reply: &mut MessageParcel,
        _option: &MessageOption,
    ) -> i32 {
        // TODO: Call UserDefineFunc(data, reply, option)
        err_code::E_OK
    }

    // ========================================================================
    // Default virtual method implementations
    // (These are overridden by DataShareStubImpl)
    // ========================================================================

    /// Default ExecuteBatch implementation.
    ///
    /// Corresponds to C++ `DataShareStub::ExecuteBatch()`.
    pub fn execute_batch_default(&self) -> i32 {
        0
    }

    /// Default InsertExt implementation.
    ///
    /// Corresponds to C++ `DataShareStub::InsertExt()`.
    pub fn insert_ext_default(&self) -> i32 {
        0
    }

    /// Default BatchUpdate implementation.
    ///
    /// Corresponds to C++ `DataShareStub::BatchUpdate()`.
    pub fn batch_update_default(&self) -> i32 {
        0
    }

    /// Default InsertEx implementation.
    ///
    /// Corresponds to C++ `DataShareStub::InsertEx()`.
    pub fn insert_ex_default(&self) -> (i32, i32) {
        (0, 0)
    }

    /// Default UpdateEx implementation.
    ///
    /// Corresponds to C++ `DataShareStub::UpdateEx()`.
    pub fn update_ex_default(&self) -> (i32, i32) {
        (0, 0)
    }

    /// Default DeleteEx implementation.
    ///
    /// Corresponds to C++ `DataShareStub::DeleteEx()`.
    pub fn delete_ex_default(&self) -> (i32, i32) {
        (0, 0)
    }

    /// Default UserDefineFunc implementation.
    ///
    /// Corresponds to C++ `DataShareStub::UserDefineFunc()`.
    pub fn user_define_func_default(&self) -> i32 {
        0
    }

    /// Default RegisterObserverExtProvider implementation.
    pub fn register_observer_ext_provider_default(&self) -> i32 {
        0
    }

    /// Default UnregisterObserverExtProvider implementation.
    pub fn unregister_observer_ext_provider_default(&self) -> i32 {
        0
    }

    /// Default NotifyChangeExtProvider implementation.
    pub fn notify_change_ext_provider_default(&self) -> i32 {
        0
    }
}

impl Default for DataShareStub {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_stub_creation() {
        let stub = DataShareStub::new();
        assert_eq!(stub.handler_count(), 24);
    }

    #[test]
    fn test_interface_code_enum() {
        assert_eq!(IDataShareInterfaceCode::CmdInsert as u32, 5);
        assert_eq!(IDataShareInterfaceCode::CmdUpdate as u32, 6);
        assert_eq!(IDataShareInterfaceCode::CmdDelete as u32, 7);
        assert_eq!(IDataShareInterfaceCode::CmdQuery as u32, 8);
    }

    #[test]
    fn test_interface_code_from_u32() {
        assert_eq!(
            IDataShareInterfaceCode::from_u32(5),
            Some(IDataShareInterfaceCode::CmdInsert)
        );
        assert_eq!(IDataShareInterfaceCode::from_u32(99), None);
    }

    #[test]
    fn test_stub_has_all_handlers() {
        let stub = DataShareStub::new();
        for code in 1..=24u32 {
            assert!(stub.has_handler(code), "Missing handler for code {}", code);
        }
        // CMD_USER_DEFINE_FUNC (25) is handled separately, not in the map
        assert!(!stub.has_handler(25));
    }

    #[test]
    fn test_on_remote_request_with_valid_code() {
        let stub = DataShareStub::new();
        let data = MessageParcel::new();
        let mut reply = MessageParcel::new();
        let option = MessageOption::new();
        let ret = stub.on_remote_request(
            IDataShareInterfaceCode::CmdInsert as u32,
            &data,
            &mut reply,
            &option,
        );
        assert_eq!(ret, err_code::E_OK);
    }

    #[test]
    fn test_on_remote_request_with_user_define_func() {
        let stub = DataShareStub::new();
        let data = MessageParcel::new();
        let mut reply = MessageParcel::new();
        let option = MessageOption::new();
        let ret = stub.on_remote_request(
            IDataShareInterfaceCode::CmdUserDefineFunc as u32,
            &data,
            &mut reply,
            &option,
        );
        assert_eq!(ret, err_code::E_OK);
    }

    #[test]
    fn test_on_remote_request_with_unknown_code() {
        let stub = DataShareStub::new();
        let data = MessageParcel::new();
        let mut reply = MessageParcel::new();
        let option = MessageOption::new();
        let ret = stub.on_remote_request(999, &data, &mut reply, &option);
        assert_eq!(ret, 0);
    }

    #[test]
    fn test_default_virtual_methods() {
        let stub = DataShareStub::new();
        assert_eq!(stub.execute_batch_default(), 0);
        assert_eq!(stub.insert_ext_default(), 0);
        assert_eq!(stub.batch_update_default(), 0);
        assert_eq!(stub.insert_ex_default(), (0, 0));
        assert_eq!(stub.update_ex_default(), (0, 0));
        assert_eq!(stub.delete_ex_default(), (0, 0));
        assert_eq!(stub.user_define_func_default(), 0);
    }

    #[test]
    fn test_all_cmd_handlers_return_ok() {
        let stub = DataShareStub::new();
        let data = MessageParcel::new();
        let option = MessageOption::new();
        for code in 1..=24u32 {
            let mut reply = MessageParcel::new();
            let ret = stub.on_remote_request(code, &data, &mut reply, &option);
            assert_eq!(
                ret,
                err_code::E_OK,
                "Handler for code {} should return E_OK",
                code
            );
        }
    }

    #[test]
    fn test_message_parcel_default() {
        let _parcel = MessageParcel::default();
        let _option = MessageOption::default();
    }

    #[test]
    fn test_interface_code_all_variants() {
        let codes = vec![
            (1, IDataShareInterfaceCode::CmdGetFileTypes),
            (2, IDataShareInterfaceCode::CmdOpenFile),
            (3, IDataShareInterfaceCode::CmdOpenRawFile),
            (4, IDataShareInterfaceCode::CmdInsert),
            (5, IDataShareInterfaceCode::CmdUpdate),
            (6, IDataShareInterfaceCode::CmdDelete),
            (7, IDataShareInterfaceCode::CmdQuery),
            (8, IDataShareInterfaceCode::CmdGetType),
            (9, IDataShareInterfaceCode::CmdBatchInsert),
            (10, IDataShareInterfaceCode::CmdRegisterObserver),
            (11, IDataShareInterfaceCode::CmdUnregisterObserver),
            (12, IDataShareInterfaceCode::CmdNotifyChange),
            (13, IDataShareInterfaceCode::CmdNormalizeUri),
            (14, IDataShareInterfaceCode::CmdDenormalizeUri),
            (15, IDataShareInterfaceCode::CmdExecuteBatch),
            (16, IDataShareInterfaceCode::CmdInsertExt),
            (17, IDataShareInterfaceCode::CmdBatchUpdate),
            (18, IDataShareInterfaceCode::CmdInsertEx),
            (19, IDataShareInterfaceCode::CmdUpdateEx),
            (20, IDataShareInterfaceCode::CmdDeleteEx),
            (21, IDataShareInterfaceCode::CmdUserDefineFunc),
            (22, IDataShareInterfaceCode::CmdRegisterObserverExtProvider),
            (
                23,
                IDataShareInterfaceCode::CmdUnregisterObserverExtProvider,
            ),
            (24, IDataShareInterfaceCode::CmdNotifyChangeExtProvider),
            (25, IDataShareInterfaceCode::CmdOpenFileWithErrCode),
        ];
        for (val, code) in codes {
            assert_eq!(code as u32, val);
            assert_eq!(IDataShareInterfaceCode::from_u32(val), Some(code));
        }
    }
}
