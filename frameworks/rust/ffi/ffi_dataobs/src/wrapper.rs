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

//! CXX bridge wrapper for DataObsMgr (Data Observer Manager).
//!
//! Provides Rust bindings for `OHOS::AAFwk::DataObsMgrClient` and related types:
//! - `register_observer` / `unregister_observer` / `notify_change`
//! - Observer callback wrapper for Rust -> C++ notification

/// FFI declarations for DataObsMgr observer operations.
#[cxx::bridge(namespace = "ffi_dataobs")]
pub mod ffi {
    /// Change type enum matching C++ ChangeInfo::ChangeType.
    enum ChangeType {
        /// Data insertion.
        INSERT = 0,
        /// Data deletion.
        DELETE,
        /// Data update.
        UPDATE,
        /// Other change type.
        OTHER,
        /// Invalid change type.
        INVAILD,
    }

    /// Simplified change info for cross-FFI use.
    struct FfiChangeInfo {
        /// The type of data change.
        change_type: ChangeType,
        /// The URI of the changed data.
        uri: String,
    }

    extern "Rust" {
        /// Rust-side callback for receiving data observer notifications.
        type DataObsCallback;

        /// Called when data at the observed URI changes.
        fn on_change(self: &mut DataObsCallback);
        /// Called when data changes with extended change information.
        fn on_change_ext(self: &mut DataObsCallback, info: &FfiChangeInfo);
    }

    unsafe extern "C++" {
        include!("ffi_dataobs_bridge.h");

        /// Registers an observer. Returns a positive handle on success, negative on error.
        fn register_observer(uri: &str, callback: Box<DataObsCallback>, user_id: i32) -> i64;
        /// Unregisters an observer by handle returned from register_observer.
        fn unregister_observer(handle: i64) -> i32;
        /// Notifies a change on the given URI.
        fn notify_change(uri: &str, user_id: i32) -> i32;

        /// Registers an extended observer that also observes descendant URIs.
        fn register_observer_ext(
            uri: &str,
            callback: Box<DataObsCallback>,
            is_descendants: bool,
        ) -> i64;
        /// Unregisters an extended observer by handle.
        fn unregister_observer_ext(handle: i64) -> i32;
        /// Notifies observers of an extended change with change type and URI.
        fn notify_change_ext(change_type: u32, uri: &str) -> i32;
    }
}

/// Safe wrapper for DataObs operations
pub struct DataObsClient;

impl DataObsClient {
    /// Registers a data change observer for the given URI.
    pub fn register_observer(
        uri: &str,
        callback: Box<dyn DataObsCallbackTrait>,
        user_id: i32,
    ) -> Result<i64, i32> {
        let handle = ffi::register_observer(
            uri,
            Box::new(DataObsCallback { inner: callback }),
            user_id,
        );
        if handle > 0 {
            Ok(handle)
        } else {
            Err(handle as i32)
        }
    }

    /// Unregisters a data change observer by handle.
    pub fn unregister_observer(handle: i64) -> i32 {
        ffi::unregister_observer(handle)
    }

    /// Notifies observers of a data change on the given URI.
    pub fn notify_change(uri: &str, user_id: i32) -> i32 {
        ffi::notify_change(uri, user_id)
    }

    /// Registers an extended data change observer that also observes descendant URIs.
    pub fn register_observer_ext(
        uri: &str,
        callback: Box<dyn DataObsCallbackTrait>,
        is_descendants: bool,
    ) -> Result<i64, i32> {
        let handle = ffi::register_observer_ext(
            uri,
            Box::new(DataObsCallback { inner: callback }),
            is_descendants,
        );
        if handle > 0 {
            Ok(handle)
        } else {
            Err(handle as i32)
        }
    }

    /// Unregisters an extended data change observer by handle.
    pub fn unregister_observer_ext(handle: i64) -> i32 {
        ffi::unregister_observer_ext(handle)
    }

    /// Notifies observers of an extended change with change type and URI.
    pub fn notify_change_ext(change_type: u32, uri: &str) -> i32 {
        ffi::notify_change_ext(change_type, uri)
    }
}

/// Trait that Rust side implements to receive observer callbacks
pub trait DataObsCallbackTrait: Send {
    /// Called when observed data changes.
    fn on_change(&mut self);
    /// Called when observed data changes with extended change information.
    fn on_change_ext(&mut self, _info: &ffi::FfiChangeInfo) {}
}

/// Opaque callback holder used by cxx bridge
pub struct DataObsCallback {
    inner: Box<dyn DataObsCallbackTrait>,
}

impl DataObsCallback {
    /// Called when observed data changes.
    pub fn on_change(&mut self) {
        self.inner.on_change();
    }

    /// Called when observed data changes with extended info.
    pub fn on_change_ext(&mut self, info: &ffi::FfiChangeInfo) {
        self.inner.on_change_ext(info);
    }
}

#[cfg(test)]
mod ut_wrapper {
    include!("../tests/ut/ut_wrapper.rs");
}
