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

// @tc.name: ut_bundlemgr_bundle_info_construct_001
// @tc.desc: Test construction of FfiBundleInfo struct
// @tc.precon: NA
// @tc.step: 1. Create a FfiBundleInfo with test values
//           2. Verify all fields match
// @tc.expect: FfiBundleInfo struct is correctly constructed
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_bundlemgr_bundle_info_construct_001() {
    let info = ffi::FfiBundleInfo {
        name: "com.example.test".to_string(),
        uid: 20010001,
        is_system_app: false,
        app_id: String::new(),
        signature_info: String::new(),
    };
    assert_eq!(info.name, "com.example.test");
    assert_eq!(info.uid, 20010001);
    assert!(!info.is_system_app);
}

// @tc.name: ut_bundlemgr_bundle_info_system_app_001
// @tc.desc: Test FfiBundleInfo with system app flag
// @tc.precon: NA
// @tc.step: 1. Create a FfiBundleInfo with is_system_app = true
//           2. Verify the flag is set correctly
// @tc.expect: System app flag is correctly stored
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_bundlemgr_bundle_info_system_app_001() {
    let info = ffi::FfiBundleInfo {
        name: "com.ohos.settings".to_string(),
        uid: 1000,
        is_system_app: true,
        app_id: String::new(),
        signature_info: String::new(),
    };
    assert!(info.is_system_app);
    assert_eq!(info.name, "com.ohos.settings");
}

// @tc.name: ut_bundlemgr_get_uid_for_bundle_001
// @tc.desc: Test getting UID for a bundle name
// @tc.precon: Bundle manager service is running
// @tc.step: 1. Call BundleMgrClient::get_uid_for_bundle() with a test bundle name
//           2. Observe the returned UID
// @tc.expect: Function completes without panic
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_bundlemgr_get_uid_for_bundle_001() {
    let uid = BundleMgrClient::get_uid_for_bundle("com.example.nonexistent");
    assert!(uid <= 0, "Nonexistent bundle should not have a valid UID, got {}", uid);
}

// @tc.name: ut_bundlemgr_get_bundle_name_for_uid_001
// @tc.desc: Test getting bundle name for a UID
// @tc.precon: Bundle manager service is running
// @tc.step: 1. Call BundleMgrClient::get_bundle_name_for_uid() with UID 0
//           2. Observe the returned bundle name
// @tc.expect: Function completes without panic
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_bundlemgr_get_bundle_name_for_uid_001() {
    let name = BundleMgrClient::get_bundle_name_for_uid(0);
    assert!(name.is_empty(), "UID 0 should not map to a bundle name, got '{}'", name);
}

// @tc.name: ut_bundlemgr_is_system_app_001
// @tc.desc: Test checking if a bundle is a system app
// @tc.precon: Bundle manager service is running
// @tc.step: 1. Call BundleMgrClient::is_system_app() with a nonexistent bundle
//           2. Verify it returns false
// @tc.expect: Nonexistent bundle should not be identified as system app
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_bundlemgr_is_system_app_001() {
    let result = BundleMgrClient::is_system_app("com.example.nonexistent");
    assert!(!result, "Nonexistent bundle should not be a system app");
}

// @tc.name: ut_bundlemgr_get_bundle_info_001
// @tc.desc: Test getting full bundle info for a bundle name
// @tc.precon: Bundle manager service is running
// @tc.step: 1. Call BundleMgrClient::get_bundle_info() with a nonexistent bundle
//           2. Observe the returned FfiBundleInfo
// @tc.expect: Function completes without panic
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_bundlemgr_get_bundle_info_001() {
    let info = BundleMgrClient::get_bundle_info("com.example.nonexistent");
    // Nonexistent bundle should return default/empty info
    assert!(!info.is_system_app);
}

// @tc.name: ut_bundlemgr_bundle_info_full_construct_001
// @tc.desc: Test construction of FfiBundleInfo with all five fields
// @tc.precon: NA
// @tc.step: 1. Create a FfiBundleInfo with all fields including app_id and signature_info
//           2. Verify all fields match
// @tc.expect: FfiBundleInfo struct is correctly constructed with all fields
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_bundlemgr_bundle_info_full_construct_001() {
    let info = ffi::FfiBundleInfo {
        name: "com.example.full".to_string(),
        uid: 20010002,
        is_system_app: false,
        app_id: "app_id_123".to_string(),
        signature_info: "sig_abc".to_string(),
    };
    assert_eq!(info.name, "com.example.full");
    assert_eq!(info.uid, 20010002);
    assert!(!info.is_system_app);
    assert_eq!(info.app_id, "app_id_123");
    assert_eq!(info.signature_info, "sig_abc");
}

// @tc.name: ut_bundlemgr_get_bundle_info_with_flags_001
// @tc.desc: Test getting bundle info with flags for a nonexistent bundle
// @tc.precon: Bundle manager service is running
// @tc.step: 1. Call BundleMgrClient::get_bundle_info_with_flags() with nonexistent bundle
//           2. Verify the returned info has default values
// @tc.expect: Nonexistent bundle returns empty/default bundle info
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_bundlemgr_get_bundle_info_with_flags_001() {
    let info = BundleMgrClient::get_bundle_info_with_flags("com.example.nonexistent", 0, 100);
    assert!(
        info.name.is_empty() || info.uid <= 0,
        "Nonexistent bundle should return empty name or non-positive uid"
    );
}

// @tc.name: ut_bundlemgr_get_bundle_info_for_self_001
// @tc.desc: Test getting bundle info for the calling process
// @tc.precon: Bundle manager service is running
// @tc.step: 1. Call BundleMgrClient::get_bundle_info_for_self() with flags 0
//           2. Verify the returned info reflects the test process
// @tc.expect: Test process should not be identified as a system app
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_bundlemgr_get_bundle_info_for_self_001() {
    let info = BundleMgrClient::get_bundle_info_for_self(0);
    // In unit test environment, calling process is not a bundle
    assert!(!info.is_system_app, "Test process should not be a system app");
}
