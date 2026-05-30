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

/// DataShare error codes, mapped from C++ error codes
#[repr(i32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum DataShareError {
    /// Ok = 0
    Ok = 0,
    /// General error = -1
    GeneralError = -1,
    /// E_ERROR = 1001
    Error = 1001,
    /// E_REGISTERED_REPEATED = 1002
    RegisteredRepeated = 1002,
    /// E_UNREGISTERED_EMPTY = 1003
    UnregisteredEmpty = 1003,
    /// E_INVALID_STATEMENT = 1007
    InvalidStatement = 1007,
    /// E_INVALID_COLUMN_INDEX = 1008
    InvalidColumnIndex = 1008,
    /// E_INVALID_OBJECT_TYPE = 1020
    InvalidObjectType = 1020,
    /// E_INVALID_PARCEL = 1042
    InvalidParcel = 1042,
    /// E_VERSION_NOT_NEWER = 1045
    VersionNotNewer = 1045,
    /// E_TEMPLATE_NOT_EXIST = 1046
    TemplateNotExist = 1046,
    /// E_SUBSCRIBER_NOT_EXIST = 1047
    SubscriberNotExist = 1047,
    /// E_URI_NOT_EXIST = 1048
    UriNotExist = 1048,
    /// E_BUNDLE_NAME_NOT_EXIST = 1049
    BundleNameNotExist = 1049,
    /// E_BMS_NOT_READY = 1050
    BmsNotReady = 1050,
    /// E_METADATA_NOT_EXISTS = 1051
    MetadataNotExists = 1051,
    /// E_SILENT_PROXY_DISABLE = 1052
    SilentProxyDisable = 1052,
    /// E_TOKEN_EMPTY = 1053
    TokenEmpty = 1053,
    /// E_EXT_URI_INVALID = 1054
    ExtUriInvalid = 1054,
    /// E_DATA_SHARE_NOT_READY = 1055
    DataShareNotReady = 1055,
    /// E_DB_ERROR = 1056
    DbError = 1056,
    /// E_DATA_SUPPLIER_ERROR = 1057
    DataSupplierError = 1057,
    /// E_MARSHAL_ERROR = 1058
    MarshalError = 1058,
    /// E_UNMARSHAL_ERROR = 1059
    UnmarshalError = 1059,
    /// E_WRITE_TO_PARCE_ERROR = 1060
    WriteToParcelError = 1060,
    /// E_RESULTSET_BUSY = 1061
    ResultSetBusy = 1061,
    /// E_APPINDEX_INVALID = 1062
    AppIndexInvalid = 1062,
    /// E_NULL_OBSERVER = 1063
    NullObserver = 1063,
    /// E_HELPER_DIED = 1064
    HelperDied = 1064,
    /// E_DATA_OBS_NOT_READY = 1065
    DataObsNotReady = 1065,
    /// E_PROVIDER_NOT_CONNECTED = 1066
    ProviderNotConnected = 1066,
    /// E_PROVIDER_CONN_NULL = 1067
    ProviderConnNull = 1067,
    /// E_INVALID_USER_ID = 1068
    InvalidUserId = 1068,
    /// E_NOT_SYSTEM_APP = 1069
    NotSystemApp = 1069,
    /// E_REGISTER_ERROR = 1070
    RegisterError = 1070,
    /// E_NOTIFYCHANGE_ERROR = 1071
    NotifyChangeError = 1071,
    /// E_TIMEOUT_ERROR = 1072
    TimeoutError = 1072,
    /// E_ERROR_OVER_LIMIT_TASK = 1073
    ErrorOverLimitTask = 1073,
    /// E_EXECUTOR_POOL_IS_NULL = 1074
    ExecutorPoolIsNull = 1074,
    /// E_NOT_HAP = 1075
    NotHap = 1075,
    /// E_GET_BUNDLEINFO_FAILED = 1076
    GetBundleInfoFailed = 1076,
    /// E_NOT_DATASHARE_EXTENSION = 1077
    NotDataShareExtension = 1077,
    /// E_DATASHARE_INVALID_URI = 1078
    DataShareInvalidUri = 1078,
    /// E_VERIFY_FAILED = 1079
    VerifyFailed = 1079,
    /// E_DATASHARE_PERMISSION_DENIED = 1080
    PermissionDenied = 1080,
    /// E_EMPTY_URI = 1081
    EmptyUri = 1081,
    /// E_NOT_IN_TRUSTS = 1082
    NotInTrusts = 1082,
    /// E_GET_CALLER_NAME_FAILED = 1083
    GetCallerNameFailed = 1083,
    /// E_NULL_OBSERVER_CLIENT = 1084
    NullObserverClient = 1084,
    /// E_UNIMPLEMENT = 1085
    Unimplement = 1085,
    /// E_DATASHARE_TYPE = 1086
    DataShareType = 1086,
    /// E_FIELD_ILLEGAL = 1087
    FieldIllegal = 1087,
    /// E_FIELD_INVALID = 1088
    FieldInvalid = 1088,
    /// E_SYSTEM_ABILITY_OPERATE_FAILED = 1089
    SystemAbilityOperateFailed = 1089,
    // Internal Rust error variants (not mapped from C++)
    /// Invalid parameter
    InvalidParameter = 1090,
    /// Out of memory
    OutOfMemory = 1091,
    /// Invalid operation
    InvalidOperation = 1092,
    /// Invalid column name
    InvalidColumnName = 1093,
    /// Invalid row index
    InvalidRowIndex = 1094,
}

impl From<i32> for DataShareError {
    fn from(code: i32) -> Self {
        match code {
            0 => DataShareError::Ok,
            -1 => DataShareError::GeneralError,
            1001 => DataShareError::Error,
            1002 => DataShareError::RegisteredRepeated,
            1003 => DataShareError::UnregisteredEmpty,
            1007 => DataShareError::InvalidStatement,
            1008 => DataShareError::InvalidColumnIndex,
            1020 => DataShareError::InvalidObjectType,
            1042 => DataShareError::InvalidParcel,
            1045 => DataShareError::VersionNotNewer,
            1046 => DataShareError::TemplateNotExist,
            1047 => DataShareError::SubscriberNotExist,
            1048 => DataShareError::UriNotExist,
            1049 => DataShareError::BundleNameNotExist,
            1050 => DataShareError::BmsNotReady,
            1051 => DataShareError::MetadataNotExists,
            1052 => DataShareError::SilentProxyDisable,
            1053 => DataShareError::TokenEmpty,
            1054 => DataShareError::ExtUriInvalid,
            1055 => DataShareError::DataShareNotReady,
            1056 => DataShareError::DbError,
            1057 => DataShareError::DataSupplierError,
            1058 => DataShareError::MarshalError,
            1059 => DataShareError::UnmarshalError,
            1060 => DataShareError::WriteToParcelError,
            1061 => DataShareError::ResultSetBusy,
            1062 => DataShareError::AppIndexInvalid,
            1063 => DataShareError::NullObserver,
            1064 => DataShareError::HelperDied,
            1065 => DataShareError::DataObsNotReady,
            1066 => DataShareError::ProviderNotConnected,
            1067 => DataShareError::ProviderConnNull,
            1068 => DataShareError::InvalidUserId,
            1069 => DataShareError::NotSystemApp,
            1070 => DataShareError::RegisterError,
            1071 => DataShareError::NotifyChangeError,
            1072 => DataShareError::TimeoutError,
            1073 => DataShareError::ErrorOverLimitTask,
            1074 => DataShareError::ExecutorPoolIsNull,
            1075 => DataShareError::NotHap,
            1076 => DataShareError::GetBundleInfoFailed,
            1077 => DataShareError::NotDataShareExtension,
            1078 => DataShareError::DataShareInvalidUri,
            1079 => DataShareError::VerifyFailed,
            1080 => DataShareError::PermissionDenied,
            1081 => DataShareError::EmptyUri,
            1082 => DataShareError::NotInTrusts,
            1083 => DataShareError::GetCallerNameFailed,
            1084 => DataShareError::NullObserverClient,
            1085 => DataShareError::Unimplement,
            1086 => DataShareError::DataShareType,
            1087 => DataShareError::FieldIllegal,
            1088 => DataShareError::FieldInvalid,
            1089 => DataShareError::SystemAbilityOperateFailed,
            1090 => DataShareError::InvalidParameter,
            1091 => DataShareError::OutOfMemory,
            1092 => DataShareError::InvalidOperation,
            1093 => DataShareError::InvalidColumnName,
            1094 => DataShareError::InvalidRowIndex,
            _ => DataShareError::GeneralError,
        }
    }
}

impl From<DataShareError> for i32 {
    fn from(err: DataShareError) -> i32 {
        err as i32
    }
}

pub type Result<T> = std::result::Result<T, DataShareError>;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_error_code_conversion() {
        assert_eq!(i32::from(DataShareError::Ok), 0);
        assert_eq!(i32::from(DataShareError::GeneralError), -1);
        assert_eq!(i32::from(DataShareError::Error), 1001);
        assert_eq!(i32::from(DataShareError::PermissionDenied), 1080);
    }

    #[test]
    fn test_error_from_i32() {
        assert_eq!(DataShareError::from(0), DataShareError::Ok);
        assert_eq!(DataShareError::from(1001), DataShareError::Error);
        assert_eq!(DataShareError::from(1080), DataShareError::PermissionDenied);
        assert_eq!(DataShareError::from(9999), DataShareError::GeneralError);
    }
}
