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

//! CXX bridge wrapper for AccountDelegate.

#[cxx::bridge(namespace = "OHOS::DistributedData")]
mod ffi {
    unsafe extern "C++" {
        include!("ffi_account_bridge.h");

        fn is_deactivating(user_id: i32) -> bool;
        fn is_verified(user_id: i32) -> bool;
        fn get_user_by_token(token_id: u32) -> i32;
        fn query_foreground_user_id() -> i32;
        fn is_caller_system_app() -> bool;
    }
}

/// Returns whether the given user account is currently deactivating.
pub fn is_deactivating(user_id: i32) -> bool {
    ffi::is_deactivating(user_id)
}

/// Returns whether the given user account has been verified.
pub fn is_verified(user_id: i32) -> bool {
    ffi::is_verified(user_id)
}

/// Resolves an access token ID to its owning user ID.
pub fn get_user_by_token(token_id: u32) -> i32 {
    ffi::get_user_by_token(token_id)
}

/// Returns the foreground user ID, or -1 on failure.
pub fn query_foreground_user_id() -> i32 {
    ffi::query_foreground_user_id()
}

/// Returns whether the IPC caller is a system application.
pub fn is_caller_system_app() -> bool {
    ffi::is_caller_system_app()
}
