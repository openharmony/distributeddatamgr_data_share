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

//! C FFI adapter for DataShareCallReporter.
//!
//! Provides C-compatible functions for the call frequency reporter.
//! Equivalent to C++ `DataShareCallReporter` class.

use crate::call_reporter::CallReporter;

type OpaqueCallReporter = CallReporter;

/// Create a new CallReporter.
#[no_mangle]
pub extern "C" fn DataShareCallReporterNew() -> *mut OpaqueCallReporter {
    Box::into_raw(Box::new(CallReporter::new()))
}

/// Count a function call and check if it exceeds the rate threshold.
///
/// Returns `true` if the access count threshold (3000 calls/30s) is exceeded.
#[no_mangle]
pub extern "C" fn DataShareCallReporterCount(
    reporter: *const OpaqueCallReporter,
    func_name: *const u8,
    func_name_len: u32,
    uri: *const u8,
    uri_len: u32,
) -> bool {
    if reporter.is_null() || func_name.is_null() || uri.is_null() {
        return false;
    }
    unsafe {
        let name = std::str::from_utf8_unchecked(std::slice::from_raw_parts(
            func_name,
            func_name_len as usize,
        ));
        let u = std::str::from_utf8_unchecked(std::slice::from_raw_parts(uri, uri_len as usize));
        (*reporter).count(name, u)
    }
}

/// Free a CallReporter.
#[no_mangle]
pub extern "C" fn DataShareCallReporterFree(reporter: *mut OpaqueCallReporter) {
    if !reporter.is_null() {
        unsafe {
            let _ = Box::from_raw(reporter);
        }
    }
}
