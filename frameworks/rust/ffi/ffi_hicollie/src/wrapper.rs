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

//! CXX bridge wrapper for HiCollie XCollie SetTimer/CancelTimer API.

/// FFI declarations for XCollie timer-based watchdog operations.
#[cxx::bridge(namespace = "OHOS::HiviewDFX")]
pub mod ffi {
    unsafe extern "C++" {
        include!("ffi_hicollie_bridge.h");

        /// Sets a watchdog timer and returns the timer ID.
        /// `tag` identifies the timer, `timeout_seconds` is the timeout duration,
        /// `flags` controls behavior (e.g., XCOLLIE_FLAG_LOG | XCOLLIE_FLAG_RECOVERY).
        fn xcollie_set_timer(tag: &str, timeout_seconds: u32, flags: u32) -> i32;

        /// Cancels a previously set watchdog timer by its ID.
        fn xcollie_cancel_timer(id: i32);
    }
}

/// XCOLLIE_FLAG_LOG: generate log file on timeout.
pub const XCOLLIE_FLAG_LOG: u32 = 1 << 0;

/// XCOLLIE_FLAG_RECOVERY: trigger recovery on timeout.
pub const XCOLLIE_FLAG_RECOVERY: u32 = 1 << 1;

/// Default flags: do all callback functions.
pub const XCOLLIE_FLAG_DEFAULT: u32 = !0;

/// No-op flags: do nothing but caller-defined function.
pub const XCOLLIE_FLAG_NOOP: u32 = 0;

/// Default timeout in seconds used by datamgr_service.
const DEFAULT_TIMEOUT_SECONDS: u32 = 30;

/// RAII guard for XCollie timer. Automatically cancels the timer on drop.
///
/// Usage matches the C++ RAII pattern:
/// ```
/// let _guard = XCollieGuard::new("MyFunction", XCOLLIE_FLAG_LOG | XCOLLIE_FLAG_RECOVERY);
/// // ... do work ...
/// // timer is automatically cancelled when _guard goes out of scope
/// ```
pub struct XCollieGuard {
    id: i32,
}

impl XCollieGuard {
    /// Creates a new XCollie guard with default timeout (30 seconds).
    pub fn new(tag: &str, flags: u32) -> Self {
        let id = ffi::xcollie_set_timer(tag, DEFAULT_TIMEOUT_SECONDS, flags);
        Self { id }
    }

    /// Creates a new XCollie guard with a custom timeout.
    pub fn with_timeout(tag: &str, timeout_seconds: u32, flags: u32) -> Self {
        let id = ffi::xcollie_set_timer(tag, timeout_seconds, flags);
        Self { id }
    }
}

impl Drop for XCollieGuard {
    fn drop(&mut self) {
        ffi::xcollie_cancel_timer(self.id);
    }
}

#[cfg(test)]
mod ut_wrapper {
    include!("../tests/ut/ut_wrapper.rs");
}
