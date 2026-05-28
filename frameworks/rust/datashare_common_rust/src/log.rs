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

//! Logging utilities for DataShare Rust components.
//!
//! Wraps `hilog_rust` with the DataShare log domain and tag,
//! matching C++ `datashare_log.h` (domain: 0xD001651, tag: "DataShare").

#[cfg(feature = "hilog")]
pub use hilog_rust::{self, HiLogLabel, LogType};

/// DataShare log domain, matching C++ `datashare_log.h`.
#[cfg(feature = "hilog")]
pub const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD001651,
    tag: "DataShare",
};

/// Log a warning message via hilog.
#[cfg(feature = "hilog")]
macro_rules! ds_warn {
    ($fmt:literal $(, $args:expr)* $(,)?) => {{
        use hilog_rust::hilog;
        use std::ffi::{c_char, CString};
        use $crate::log::LOG_LABEL as DS_LOG_LABEL;
        hilog_rust::warn!(DS_LOG_LABEL, $fmt $(, @public($args))*);
    }};
}

/// Log an error message via hilog.
#[cfg(feature = "hilog")]
macro_rules! ds_error {
    ($fmt:literal $(, $args:expr)* $(,)?) => {{
        use hilog_rust::hilog;
        use std::ffi::{c_char, CString};
        use $crate::log::LOG_LABEL as DS_LOG_LABEL;
        hilog_rust::error!(DS_LOG_LABEL, $fmt $(, @public($args))*);
    }};
}

/// No-op fallback when hilog is not available.
#[cfg(not(feature = "hilog"))]
macro_rules! ds_warn {
    ($($arg:tt)*) => {};
}

/// No-op fallback when hilog is not available.
#[cfg(not(feature = "hilog"))]
macro_rules! ds_error {
    ($($arg:tt)*) => {};
}

pub(crate) use ds_error;
pub(crate) use ds_warn;
