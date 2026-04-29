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

#[cxx::bridge(namespace = "OHOS::DataShare")]
mod ffi {
    unsafe extern "C++" {
        include!("ffi_extension_connect_bridge.h");

        fn connect_extension(
            uri: &str,
            bundle_name: &str,
            user_id: i32,
            want_params_ptr: usize,
        ) -> i32;
        fn disconnect_extension(bundle_name: &str) -> i32;
        fn has_active_connection(bundle_name: &str) -> bool;
        fn schedule_disconnect(bundle_name: &str, delay_secs: i32);
        fn init_executor(executor_ptr: usize);
        fn build_corruption_want_params(bundle_name: &str, store_name: &str) -> usize;
        fn destroy_want_params(ptr: usize);
    }
}

/// Connects to a DataShare extension ability via SAMGR.
///
/// Creates an `AbilityConnectionStub` callback, stores it in the internal
/// callback map, and calls `ConnectAbilityCommon`.  The callback's
/// `OnAbilityConnectDone` calls back into Rust via
/// `ds_on_ability_connect_done`.
///
/// `want_params_ptr` is the address of a C++ `AAFwk::WantParams` object
/// (pass 0 when no extra parameters are needed).
///
/// Returns 0 on success, -1 on error (already connected, SA unavailable, or
/// IPC failure).
pub fn connect_extension(
    uri: &str,
    bundle_name: &str,
    user_id: i32,
    want_params_ptr: usize,
) -> i32 {
    ffi::connect_extension(uri, bundle_name, user_id, want_params_ptr)
}

/// Disconnects a DataShare extension ability by bundle name.
///
/// Removes the callback from the internal map and calls
/// `DisconnectAbility`.  Returns 0 on success, -1 if no active connection
/// exists for the bundle.
pub fn disconnect_extension(bundle_name: &str) -> i32 {
    ffi::disconnect_extension(bundle_name)
}

/// Returns `true` if there is an active connection callback registered for
/// the given bundle name.
pub fn has_active_connection(bundle_name: &str) -> bool {
    ffi::has_active_connection(bundle_name)
}

/// Schedules a delayed disconnection for the given bundle using the C++
/// `ExecutorPool`.
///
/// After `delay_secs` seconds the executor fires `DisconnectInternal`,
/// which removes the callback from the map and calls `DisconnectAbility`.
pub fn schedule_disconnect(bundle_name: &str, delay_secs: i32) {
    ffi::schedule_disconnect(bundle_name, delay_secs)
}

/// Initialises the C++ executor pool from a raw pointer to
/// `std::shared_ptr<ExecutorPool>`.
///
/// Called once from `ds_init_extension_executor` during service startup.
/// The shared pointer is *copied* (refcount incremented), so the caller's
/// original shared_ptr remains valid.
pub fn init_executor(executor_ptr: usize) {
    ffi::init_executor(executor_ptr)
}

/// Allocates a C++ `AAFwk::WantParams` populated with corruption metadata.
///
/// Sets `BundleName`, `StoreName`, and `StoreStatus=1`.  Returns the raw
/// pointer as `usize` (0 on allocation failure).  The caller **must** pass
/// the returned pointer to [`destroy_want_params`] after use.
pub fn build_corruption_want_params(bundle_name: &str, store_name: &str) -> usize {
    ffi::build_corruption_want_params(bundle_name, store_name)
}

/// Frees a `WantParams` object previously allocated by
/// [`build_corruption_want_params`].
pub fn destroy_want_params(ptr: usize) {
    ffi::destroy_want_params(ptr)
}

#[cfg(test)]
mod ut_wrapper {
    include!("../tests/ut/ut_wrapper.rs");
}
