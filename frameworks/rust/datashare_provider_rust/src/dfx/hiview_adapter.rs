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

//! HiViewFaultAdapter — HiSysEvent fault reporting for DataShare.
//!
//! Corresponds to C++ `HiViewFaultAdapter` in
//! `frameworks/native/dfx/src/hiview_datashare.cpp` (97 lines).
//!
//! Reports data faults to HiSysEvent and resolves caller names
//! from access tokens. Shared between datashare_provider and
//! datashare_permission targets.

/// DataShare fault information.
///
/// Corresponds to C++ `DataShareFaultInfo` struct.
#[derive(Debug, Clone, Default)]
pub struct DataShareFaultInfo {
    /// Fault type identifier.
    pub fault_type: String,
    /// Bundle name of the faulting component.
    pub bundle_name: String,
    /// Module name.
    pub module_name: String,
    /// Store name.
    pub store_name: String,
    /// Business type.
    pub business_type: String,
    /// Error code.
    pub error_code: i32,
    /// Additional information.
    pub appendix: String,
}

/// HiViewFaultAdapter — fault reporting adapter.
///
/// Corresponds to C++ `HiViewFaultAdapter`.
pub struct HiViewFaultAdapter;

impl HiViewFaultAdapter {
    /// Report a data fault to HiSysEvent.
    ///
    /// Corresponds to C++ `HiViewFaultAdapter::ReportDataFault()`.
    ///
    /// TODO: Implement with OH_HiSysEvent_Write
    /// Reports DISTRIBUTED_DATA_SHARE_FAULT event with 8 parameters:
    /// FAULT_TIME, FAULT_TYPE, BUNDLE_NAME, MODULE_NAME,
    /// STORE_NAME, BUSINESS_TYPE, ERROR_CODE, APPENDIX
    pub fn report_data_fault(fault_info: &DataShareFaultInfo) {
        // TODO: Format timestamp
        // TODO: Build HiSysEventParam array
        // TODO: Call OH_HiSysEvent_Write(DOMAIN, EVENT_NAME, HISYSEVENT_FAULT, params, 8)
        let _ = fault_info;
    }

    /// Get the caller name from a token ID.
    ///
    /// Corresponds to C++ `HiViewFaultAdapter::GetCallingName()`.
    ///
    /// TODO: Implement with AccessTokenKit
    /// Returns (caller_name, result_code) pair.
    pub fn get_calling_name(calling_token_id: u32) -> (String, i32) {
        // TODO: Check token type via AccessTokenKit::GetTokenTypeFlag
        // TODO: For HAP: GetHapTokenInfo → bundleName
        // TODO: For NATIVE/SHELL: GetNativeTokenInfo → processName
        let _ = calling_token_id;
        (String::new(), -1)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_fault_info_default() {
        let info = DataShareFaultInfo::default();
        assert!(info.fault_type.is_empty());
        assert!(info.bundle_name.is_empty());
        assert_eq!(info.error_code, 0);
    }

    #[test]
    fn test_fault_info_creation() {
        let info = DataShareFaultInfo {
            fault_type: "READ_FAULT".to_string(),
            bundle_name: "com.test.app".to_string(),
            module_name: "entry".to_string(),
            store_name: "test_store".to_string(),
            business_type: "query".to_string(),
            error_code: -1,
            appendix: "test error".to_string(),
        };
        assert_eq!(info.fault_type, "READ_FAULT");
        assert_eq!(info.error_code, -1);
    }

    #[test]
    fn test_report_data_fault() {
        let info = DataShareFaultInfo::default();
        HiViewFaultAdapter::report_data_fault(&info);
    }

    #[test]
    fn test_get_calling_name() {
        let (name, code) = HiViewFaultAdapter::get_calling_name(0);
        assert!(name.is_empty());
        assert_eq!(code, -1);
    }

    #[test]
    fn test_get_calling_name_with_token() {
        let (name, code) = HiViewFaultAdapter::get_calling_name(12345);
        assert!(name.is_empty());
        assert_eq!(code, -1);
    }
}
