// Copyright (c) 2026 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

use super::*;

// @tc.name: ut_accesstoken_calling_info_construct_001
// @tc.desc: Test construction of CallingInfo struct
// @tc.precon: NA
// @tc.step: 1. Create a CallingInfo with token_id and process_token_id
//           2. Verify fields match
// @tc.expect: CallingInfo struct is correctly constructed
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_accesstoken_calling_info_construct_001() {
    let info = ffi::CallingInfo {
        token_id: 12345,
        process_token_id: 67890,
    };
    assert_eq!(info.token_id, 12345);
    assert_eq!(info.process_token_id, 67890);
}

// @tc.name: ut_accesstoken_calling_info_zero_001
// @tc.desc: Test CallingInfo with zero values
// @tc.precon: NA
// @tc.step: 1. Create a CallingInfo with zero token IDs
//           2. Verify fields are zero
// @tc.expect: CallingInfo allows zero values
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_accesstoken_calling_info_zero_001() {
    let info = ffi::CallingInfo {
        token_id: 0,
        process_token_id: 0,
    };
    assert_eq!(info.token_id, 0);
    assert_eq!(info.process_token_id, 0);
}

// @tc.name: ut_accesstoken_get_calling_info_001
// @tc.desc: Test getting the full CallingInfo struct
// @tc.precon: NA
// @tc.step: 1. Call AccessTokenKit::get_calling_info()
//           2. Verify the struct fields are consistent with individual calls
// @tc.expect: CallingInfo token_id matches get_calling_token_id result
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_accesstoken_get_calling_info_001() {
    let info = AccessTokenKit::get_calling_info();
    let token_id = AccessTokenKit::get_calling_token_id();
    assert_eq!(
        info.token_id, token_id,
        "CallingInfo token_id should match get_calling_token_id"
    );
}

// @tc.name: ut_accesstoken_verify_permission_001
// @tc.desc: Test verifying a permission for an invalid token
// @tc.precon: NA
// @tc.step: 1. Call verify_permission with token_id 0 and a test permission
//           2. Verify the result is not PERMISSION_GRANTED (0)
// @tc.expect: Invalid token should not have any permission
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_accesstoken_verify_permission_001() {
    let result = AccessTokenKit::verify_permission(0, "ohos.permission.TEST");
    // Token ID 0 should not have any permission
    assert_ne!(result, 0, "Invalid token should not have permission");
}

// @tc.name: ut_accesstoken_hap_token_info_construct_001
// @tc.desc: Test construction of FfiHapTokenInfo struct
// @tc.precon: NA
// @tc.step: 1. Create a FfiHapTokenInfo with test values
//           2. Verify all fields match
// @tc.expect: FfiHapTokenInfo struct is correctly constructed
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_accesstoken_hap_token_info_construct_001() {
    let info = ffi::FfiHapTokenInfo {
        bundle_name: "com.example.test".to_string(),
        inst_index: 1,
        dlp_type: 0,
        user_id: 100,
    };
    assert_eq!(info.bundle_name, "com.example.test");
    assert_eq!(info.inst_index, 1);
    assert_eq!(info.dlp_type, 0);
    assert_eq!(info.user_id, 100);
}

// @tc.name: ut_accesstoken_native_token_info_construct_001
// @tc.desc: Test construction of FfiNativeTokenInfo struct
// @tc.precon: NA
// @tc.step: 1. Create a FfiNativeTokenInfo with test values
//           2. Verify all fields match
// @tc.expect: FfiNativeTokenInfo struct is correctly constructed
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_accesstoken_native_token_info_construct_001() {
    let info = ffi::FfiNativeTokenInfo {
        process_name: "foundation".to_string(),
        apl: 3,
    };
    assert_eq!(info.process_name, "foundation");
    assert_eq!(info.apl, 3);
}

// @tc.name: ut_accesstoken_get_hap_token_info_invalid_001
// @tc.desc: Test getting HAP token info with invalid token ID
// @tc.precon: NA
// @tc.step: 1. Call AccessTokenKit::get_hap_token_info() with token_id 0
//           2. Verify the returned info has empty bundle_name
// @tc.expect: Invalid token should return empty/default HAP token info
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_accesstoken_get_hap_token_info_invalid_001() {
    let info = AccessTokenKit::get_hap_token_info(0);
    assert!(
        info.bundle_name.is_empty(),
        "Invalid token should return empty bundle_name, got '{}'",
        info.bundle_name
    );
}

// @tc.name: ut_accesstoken_get_native_token_info_invalid_001
// @tc.desc: Test getting native token info with invalid token ID
// @tc.precon: NA
// @tc.step: 1. Call AccessTokenKit::get_native_token_info() with token_id 0
//           2. Verify the returned info has empty process_name
// @tc.expect: Invalid token should return empty/default native token info
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_accesstoken_get_native_token_info_invalid_001() {
    let info = AccessTokenKit::get_native_token_info(0);
    assert!(
        info.process_name.is_empty(),
        "Invalid token should return empty process_name, got '{}'",
        info.process_name
    );
}

