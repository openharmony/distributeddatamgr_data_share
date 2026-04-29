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

//! CXX bridge wrapper for BundleMgr (Bundle Manager).
//!
//! Provides Rust bindings for bundle information queries:
//! - Get application UID from bundle name
//! - Query bundle name from UID
//! - Check if an application is a system app

/// FFI declarations for BundleMgr operations.
#[cxx::bridge(namespace = "OHOS::AppExecFwk")]
pub mod ffi {
    /// Bundle information returned from bundle manager queries.
    struct FfiBundleInfo {
        /// The bundle name.
        name: String,
        /// The UID of the bundle.
        uid: i32,
        /// Whether the bundle is a system application.
        is_system_app: bool,
        /// The application ID.
        app_id: String,
        /// The signature fingerprint.
        signature_info: String,
    }

    unsafe extern "C++" {
        include!("ffi_bundlemgr_bridge.h");

        /// Returns the UID for the given bundle name.
        fn get_uid_for_bundle(bundle_name: &str) -> i32;
        /// Returns the bundle name for the given UID.
        fn get_bundle_name_for_uid(uid: i32) -> String;
        /// Returns whether the given bundle is a system application.
        fn is_system_app(bundle_name: &str) -> bool;
        /// Returns the bundle information for the given bundle name.
        fn get_bundle_info(bundle_name: &str) -> FfiBundleInfo;
        /// Returns bundle information with the specified flags and user ID.
        fn get_bundle_info_with_flags(bundle_name: &str, flags: i32, user_id: i32) -> FfiBundleInfo;
        /// Returns bundle information for the calling process with the specified flags.
        fn get_bundle_info_for_self(flags: i32) -> FfiBundleInfo;
    }
}

pub use ffi::FfiBundleInfo;

/// Safe wrapper for BundleMgr queries
pub struct BundleMgrClient;

impl BundleMgrClient {
    /// Returns the UID for the given bundle name.
    pub fn get_uid_for_bundle(bundle_name: &str) -> i32 {
        ffi::get_uid_for_bundle(bundle_name)
    }

    /// Returns the bundle name for the given UID.
    pub fn get_bundle_name_for_uid(uid: i32) -> String {
        ffi::get_bundle_name_for_uid(uid)
    }

    /// Returns whether the given bundle is a system application.
    pub fn is_system_app(bundle_name: &str) -> bool {
        ffi::is_system_app(bundle_name)
    }

    /// Returns bundle information for the given bundle name.
    pub fn get_bundle_info(bundle_name: &str) -> FfiBundleInfo {
        ffi::get_bundle_info(bundle_name)
    }

    /// Returns bundle information with specified flags and user ID.
    pub fn get_bundle_info_with_flags(bundle_name: &str, flags: i32, user_id: i32) -> FfiBundleInfo {
        ffi::get_bundle_info_with_flags(bundle_name, flags, user_id)
    }

    /// Returns bundle information for the calling process with specified flags.
    pub fn get_bundle_info_for_self(flags: i32) -> FfiBundleInfo {
        ffi::get_bundle_info_for_self(flags)
    }
}

#[cfg(test)]
mod ut_wrapper {
    include!("../tests/ut/ut_wrapper.rs");
}
