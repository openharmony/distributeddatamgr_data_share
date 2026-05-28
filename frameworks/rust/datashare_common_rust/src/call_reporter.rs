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

//! Call frequency reporter for DataShare operations.
//!
//! Monitors function call frequency and logs warnings when thresholds are exceeded.
//! Equivalent to C++ `call_reporter.cpp`.

use std::collections::HashMap;
use std::sync::Mutex;

use crate::utils::string_utils::StringUtils;

/// Threshold: reset per-function counter every N calls
const RESET_COUNT_THRESHOLD: i32 = 100;
/// Threshold: max calls in a 30s window before rate limiting
const ACCESS_COUNT_THRESHOLD: i32 = 3000;
/// Time window for access threshold in milliseconds (30 seconds)
const TIME_THRESHOLD: i64 = 30000;

/// Call statistics for a single function
#[derive(Debug, Clone, Default)]
struct CallInfo {
    /// Counter within current RESET_COUNT_THRESHOLD window
    count: i32,
    /// Total count within current 30s window (for access threshold)
    total_count: i32,
    /// Timestamp of first call in current RESET_COUNT_THRESHOLD window (ms)
    first_time: i64,
    /// Timestamp of start of current 30s window (ms)
    start_time: i64,
    /// Whether error log has been printed for current threshold breach
    log_print_flag: bool,
}

/// Reports call frequency and checks if thresholds are exceeded.
///
/// Equivalent to C++ `DataShareCallReporter`.
pub struct CallReporter {
    call_counts: Mutex<HashMap<String, CallInfo>>,
}

impl Default for CallReporter {
    fn default() -> Self {
        Self::new()
    }
}

impl CallReporter {
    pub fn new() -> Self {
        Self {
            call_counts: Mutex::new(HashMap::new()),
        }
    }

    /// Count a function call and check if the call rate exceeds thresholds.
    ///
    /// Returns `true` if the access count threshold (3000 calls/30s) is exceeded.
    pub fn count(&self, func_name: &str, uri: &str) -> bool {
        let mut over_count = 0i32;
        let mut first_call_time = 0i64;
        let mut is_over_threshold = false;

        // Hold the lock for both update_call_counts and threshold logging
        // to avoid TOCTOU race (matches C++ Compute lambda atomicity)
        {
            let mut counts = self.call_counts.lock().unwrap();
            Self::update_call_counts_locked(
                &mut counts,
                func_name,
                &mut over_count,
                &mut first_call_time,
                &mut is_over_threshold,
            );

            // Error log for over threshold, print only once per window (inside same lock)
            if is_over_threshold {
                if let Some(call_info) = counts.get_mut(func_name) {
                    if !call_info.log_print_flag {
                        let _now = get_boot_time_ms();
                        call_info.log_print_flag = true;
                        let _anonymous_uri = StringUtils::anonymous(uri);
                        crate::log::ds_error!(
                            "Over threshold, func: {}, first:{}ms, now:{}ms, uri:{}",
                            func_name,
                            call_info.start_time,
                            _now,
                            &_anonymous_uri
                        );
                    }
                }
            }
        }

        let now = get_boot_time_ms();
        if now < 0 {
            return is_over_threshold;
        }

        let _anonymous_uri = StringUtils::anonymous(uri);
        if over_count > 0 {
            crate::log::ds_warn!(
                "Call the threshold, func: {}, first:{}ms, now:{}ms, uri:{}",
                func_name,
                first_call_time,
                now,
                &_anonymous_uri
            );
        }
        if over_count > 1 {
            crate::log::ds_warn!(
                "Call too frequently, func: {}, first:{}ms, now:{}ms, uri:{}",
                func_name,
                first_call_time,
                now,
                &_anonymous_uri
            );
        }

        is_over_threshold
    }

    fn update_call_counts_locked(
        counts: &mut HashMap<String, CallInfo>,
        func_name: &str,
        over_count: &mut i32,
        first_call_time: &mut i64,
        is_over_threshold: &mut bool,
    ) {
        let curr_time = get_boot_time_ms();
        // get boot time failed: remove entry to match C++ Compute return false semantics
        if curr_time < 0 {
            counts.remove(func_name);
            return;
        }

        let call_info = counts.entry(func_name.to_string()).or_default();

        let mut call_count = call_info.count;
        let mut total_call_count = call_info.total_count;

        if call_count == 0 {
            call_info.first_time = curr_time;
        }

        call_count += 1;
        if call_count % RESET_COUNT_THRESHOLD == 0 {
            let first = call_info.first_time;
            *over_count += 1;
            *first_call_time = first;
            if curr_time - first <= TIME_THRESHOLD {
                *over_count += 1;
            }
            call_count = 0;
        }
        call_info.count = call_count;

        // Update access control count
        if total_call_count == 0 {
            call_info.start_time = curr_time;
        }
        let threshold_start_time = call_info.start_time;

        // Reset call_info when time >= 30s or curr_time < threshold_start_time
        if curr_time - threshold_start_time >= TIME_THRESHOLD || curr_time < threshold_start_time {
            call_info.start_time = curr_time;
            call_info.total_count = 0;
            call_info.log_print_flag = false;
            total_call_count = 0;
        }

        total_call_count += 1;
        // is_over_threshold returns true when total_call_count >= 3000 in 30s
        if total_call_count >= ACCESS_COUNT_THRESHOLD {
            *is_over_threshold = true;
        }
        call_info.total_count = total_call_count;
    }
}

/// Get system boot time in milliseconds.
///
/// Uses `clock_gettime(CLOCK_BOOTTIME)` on Linux/OpenHarmony.
fn get_boot_time_ms() -> i64 {
    #[cfg(target_os = "linux")]
    {
        #[repr(C)]
        struct Timespec {
            tv_sec: core::ffi::c_long,
            tv_nsec: core::ffi::c_long,
        }

        extern "C" {
            fn clock_gettime(clk_id: i32, tp: *mut Timespec) -> i32;
        }

        let mut ts = Timespec {
            tv_sec: 0,
            tv_nsec: 0,
        };
        let ret = unsafe { clock_gettime(7, &mut ts as *mut Timespec) };
        if ret != 0 {
            return -1;
        }
        (ts.tv_sec as i64) * 1000 + (ts.tv_nsec as i64) / 1_000_000
    }
    #[cfg(not(target_os = "linux"))]
    {
        use std::time::{SystemTime, UNIX_EPOCH};
        match SystemTime::now().duration_since(UNIX_EPOCH) {
            Ok(d) => d.as_millis() as i64,
            Err(_) => -1,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_call_reporter_basic() {
        let reporter = CallReporter::new();
        // First call should not exceed threshold
        let result = reporter.count("testFunc", "content://com.example/test");
        assert!(!result);
    }

    #[test]
    fn test_call_reporter_under_threshold() {
        let reporter = CallReporter::new();
        // Call 99 times - should still be under threshold
        for _ in 0..99 {
            let result = reporter.count("testFunc", "content://com.example/test");
            assert!(!result);
        }
    }

    #[test]
    fn test_call_reporter_access_threshold() {
        let reporter = CallReporter::new();
        // Call ACCESS_COUNT_THRESHOLD times within same time window
        let mut exceeded = false;
        for _ in 0..3001 {
            if reporter.count("testFunc", "content://com.example/test") {
                exceeded = true;
                break;
            }
        }
        assert!(exceeded, "Should exceed access threshold after 3000 calls");
    }

    #[test]
    fn test_call_reporter_multiple_functions() {
        let reporter = CallReporter::new();
        // Different function names should have independent counters
        reporter.count("func1", "content://com.example/test");
        reporter.count("func2", "content://com.example/test");
        reporter.count("func1", "content://com.example/test");

        // Verify counters are independent by checking internal state
        let counts = reporter.call_counts.lock().unwrap();
        assert_eq!(counts.get("func1").unwrap().count, 2);
        assert_eq!(counts.get("func2").unwrap().count, 1);
    }

    #[test]
    fn test_call_reporter_reset_count_threshold() {
        let reporter = CallReporter::new();
        // Call exactly RESET_COUNT_THRESHOLD times
        for _ in 0..RESET_COUNT_THRESHOLD {
            reporter.count("testFunc", "content://com.example/test");
        }
        // After 100 calls, count should be reset to 0
        let counts = reporter.call_counts.lock().unwrap();
        assert_eq!(counts.get("testFunc").unwrap().count, 0);
    }
}
