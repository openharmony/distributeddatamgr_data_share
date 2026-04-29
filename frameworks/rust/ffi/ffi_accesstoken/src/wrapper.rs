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

//! CXX bridge wrapper for AccessTokenKit (Access Token Kit).
//!
//! Provides Rust bindings for:
//! - `GetCallingTokenID()` — get current caller's token ID
//! - `GetCallingProcessTokenID()` — get process-level token ID
//! - `VerifyAccessToken()` — verify permission for token
//! - `GetTokenTypeEx()` — get token type

/// FFI declarations for AccessTokenKit operations.
#[cxx::bridge(namespace = "OHOS::Security::AccessToken")]
pub mod ffi {
    /// Calling identity information containing both token IDs.
    struct CallingInfo {
        /// The token ID of the calling process.
        token_id: u64,
        /// The process-level token ID.
        process_token_id: u64,
    }

    /// HAP token information returned by GetHapTokenInfo.
    struct FfiHapTokenInfo {
        /// The bundle name of the HAP.
        bundle_name: String,
        /// The instance index of the HAP.
        inst_index: i32,
        /// The DLP type of the HAP.
        dlp_type: i32,
        /// The user ID that owns the HAP.
        user_id: i32,
    }

    /// Native token information returned by GetNativeTokenInfo.
    struct FfiNativeTokenInfo {
        /// The process name of the native token.
        process_name: String,
        /// The APL (Ability Privilege Level) of the native token.
        apl: i32,
    }

    unsafe extern "C++" {
        include!("ffi_accesstoken_bridge.h");

        /// Returns the token ID of the calling process.
        fn get_calling_token_id() -> u64;
        /// Returns the process-level token ID.
        fn get_calling_process_token_id() -> u64;
        /// Verifies if the given token has the specified permission.
        fn verify_permission(token_id: u64, permission_name: &str) -> i32;
        /// Returns the type of the token (HAP, NATIVE, etc.).
        fn get_token_type(token_id: u64) -> i32;
        /// Returns the bundle name associated with a token.
        fn get_bundle_name(token_id: u64) -> String;
        /// Returns both token IDs as a CallingInfo struct.
        fn get_calling_info() -> CallingInfo;
        /// Returns HAP token information for the given token ID.
        fn get_hap_token_info(token_id: u64) -> FfiHapTokenInfo;
        /// Returns native token information for the given token ID.
        fn get_native_token_info(token_id: u64) -> FfiNativeTokenInfo;
        /// Returns the token ID for a HAP given (userId, bundleName, appIndex).
        fn get_hap_token_id(user_id: i32, bundle_name: &str, app_index: i32) -> u64;
    }
}

/// Rust-safe wrapper for OpenHarmony AccessTokenKit APIs.
pub struct AccessTokenKit;

impl AccessTokenKit {
    /// Get the token ID of the calling process
    pub fn get_calling_token_id() -> u64 {
        ffi::get_calling_token_id()
    }

    /// Get the process-level token ID
    pub fn get_calling_process_token_id() -> u64 {
        ffi::get_calling_process_token_id()
    }

    /// Verify if the given token has the specified permission
    /// Returns 0 (PERMISSION_GRANTED) or -1 (PERMISSION_DENIED)
    pub fn verify_permission(token_id: u64, permission_name: &str) -> i32 {
        ffi::verify_permission(token_id, permission_name)
    }

    /// Get the type of the token (HAP, NATIVE, etc.)
    pub fn get_token_type(token_id: u64) -> i32 {
        ffi::get_token_type(token_id)
    }

    /// Get the bundle name associated with a token
    pub fn get_bundle_name(token_id: u64) -> String {
        ffi::get_bundle_name(token_id)
    }

    /// Get both token IDs at once
    pub fn get_calling_info() -> ffi::CallingInfo {
        ffi::get_calling_info()
    }

    /// Get HAP token information for the given token ID
    pub fn get_hap_token_info(token_id: u64) -> ffi::FfiHapTokenInfo {
        ffi::get_hap_token_info(token_id)
    }

    /// Get native token information for the given token ID
    pub fn get_native_token_info(token_id: u64) -> ffi::FfiNativeTokenInfo {
        ffi::get_native_token_info(token_id)
    }

    /// Get the HAP token ID for a given (userId, bundleName, appIndex) triple.
    pub fn get_hap_token_id(user_id: i32, bundle_name: &str, app_index: i32) -> u64 {
        ffi::get_hap_token_id(user_id, bundle_name, app_index)
    }
}

#[cfg(test)]
mod ut_wrapper {
    include!("../tests/ut/ut_wrapper.rs");
}
