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

//! CXX bridge wrapper for HiSysEvent.

/// FFI declarations for HiSysEvent operations.
#[cxx::bridge(namespace = "OHOS::HiviewDFX")]
pub mod ffi {
    /// Event type classification for system events.
    enum EventType {
        /// User behavior event.
        BEHAVIOR,
        /// Security-related event.
        SECURITY,
        /// Statistical data event.
        STATISTIC,
        /// Fault or error event.
        FAULT,
    }

    /// Key-value pair for event parameters.
    struct KeyValue {
        /// The parameter key.
        key: String,
        /// The parameter value.
        value: String,
    }

    unsafe extern "C++" {
        include!("ffi_hisysevent_bridge.h");

        /// Writes a system event with the given domain, name, type, and parameters.
        fn write_sys_event(
            domain: &str,
            name: &str,
            event_type: EventType,
            key_values: &[KeyValue],
        );
    }
}

/// Wrapper for HiSysEvent logging.
pub struct HiSysEvent;

impl HiSysEvent {
    /// Writes a system event with the given parameters.
    pub fn write(
        domain: &str,
        name: &str,
        event_type: ffi::EventType,
        key_values: &[(&str, &str)],
    ) {
        let pairs: Vec<ffi::KeyValue> = key_values
            .iter()
            .map(|(k, v)| ffi::KeyValue {
                key: k.to_string(),
                value: v.to_string(),
            })
            .collect();
        ffi::write_sys_event(domain, name, event_type, &pairs);
    }
}

#[cfg(test)]
mod ut_wrapper {
    include!("../tests/ut/ut_wrapper.rs");
}
