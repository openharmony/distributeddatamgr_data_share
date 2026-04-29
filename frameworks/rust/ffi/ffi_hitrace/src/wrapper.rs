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

//! CXX bridge wrapper for HiTrace.

/// FFI declarations for HiTrace operations.
#[cxx::bridge(namespace = "OHOS::HiviewDFX")]
pub mod ffi {
    /// Trace identifier containing chain, span, and parent span IDs.
    struct FfiHiTraceId {
        /// The trace chain ID.
        chain_id: u64,
        /// The current span ID.
        span_id: u64,
        /// The parent span ID.
        parent_span_id: u64,
        /// Trace flags.
        flags: u32,
        /// Whether the trace ID is valid.
        is_valid: bool,
    }

    unsafe extern "C++" {
        include!("ffi_hitrace_bridge.h");

        /// Begins a trace section and returns the trace ID.
        fn begin_section(name: &str) -> FfiHiTraceId;
        /// Ends the current trace section.
        fn end_section();
        /// Creates a new trace ID with the given name.
        fn create_trace_id(name: &str) -> FfiHiTraceId;
        /// Returns the current trace ID.
        fn get_current_trace_id() -> FfiHiTraceId;
        /// Returns whether tracing is currently enabled.
        fn is_trace_enabled() -> bool;
    }
}

pub use ffi::FfiHiTraceId as HiTraceId;

/// Wrapper for HiTrace instrumentation.
pub struct HiTrace;

impl HiTrace {
    /// Begins a new trace section with the given name.
    pub fn begin_section(name: &str) -> ffi::FfiHiTraceId {
        ffi::begin_section(name)
    }

    /// Ends the current trace section.
    pub fn end_section() {
        ffi::end_section();
    }

    /// Returns whether tracing is currently enabled.
    pub fn is_trace_enabled() -> bool {
        ffi::is_trace_enabled()
    }

    /// Returns the current active trace ID.
    pub fn get_current_trace_id() -> ffi::FfiHiTraceId {
        ffi::get_current_trace_id()
    }
}

/// RAII guard for trace sections
pub struct TraceGuard {
    _name: String,
}

impl TraceGuard {
    /// Creates a new instance.
    pub fn new(name: &str) -> Self {
        ffi::begin_section(name);
        Self { _name: name.to_string() }
    }
}

impl Drop for TraceGuard {
    fn drop(&mut self) {
        ffi::end_section();
    }
}

#[cfg(test)]
mod ut_wrapper {
    include!("../tests/ut/ut_wrapper.rs");
}
