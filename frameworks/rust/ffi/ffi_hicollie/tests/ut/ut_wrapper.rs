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

use super::*;

// @tc.name: ut_hicollie_flag_constants_001
// @tc.desc: Verify XCollie flag constants have correct values
// @tc.precon: NA
// @tc.step: 1. Check XCOLLIE_FLAG_LOG equals 1
//           2. Check XCOLLIE_FLAG_RECOVERY equals 2
//           3. Check XCOLLIE_FLAG_DEFAULT equals u32::MAX
//           4. Check XCOLLIE_FLAG_NOOP equals 0
// @tc.expect: All flag constants match their defined values
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_hicollie_flag_constants_001() {
    assert_eq!(XCOLLIE_FLAG_LOG, 1 << 0);
    assert_eq!(XCOLLIE_FLAG_RECOVERY, 1 << 1);
    assert_eq!(XCOLLIE_FLAG_DEFAULT, !0u32);
    assert_eq!(XCOLLIE_FLAG_NOOP, 0u32);
}

// @tc.name: ut_hicollie_default_timeout_001
// @tc.desc: Verify DEFAULT_TIMEOUT_SECONDS constant is 30
// @tc.precon: NA
// @tc.step: 1. Check DEFAULT_TIMEOUT_SECONDS equals 30
// @tc.expect: Default timeout is 30 seconds
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_hicollie_default_timeout_001() {
    assert_eq!(DEFAULT_TIMEOUT_SECONDS, 30u32);
}

// @tc.name: ut_hicollie_flag_combinations_001
// @tc.desc: Verify XCollie flags can be combined with bitwise OR
// @tc.precon: NA
// @tc.step: 1. Combine XCOLLIE_FLAG_LOG and XCOLLIE_FLAG_RECOVERY
//           2. Verify the result is 3
//           3. Verify individual bits are set
// @tc.expect: Combined flags have both bits set
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_hicollie_flag_combinations_001() {
    let combined = XCOLLIE_FLAG_LOG | XCOLLIE_FLAG_RECOVERY;
    assert_eq!(combined, 3u32);
    assert_ne!(combined & XCOLLIE_FLAG_LOG, 0);
    assert_ne!(combined & XCOLLIE_FLAG_RECOVERY, 0);
}

// @tc.name: ut_hicollie_guard_new_001
// @tc.desc: Test creating XCollieGuard with default timeout via new()
// @tc.precon: HiCollie service is available
// @tc.step: 1. Create an XCollieGuard using new() with LOG and RECOVERY flags
//           2. Verify the guard's timer ID is non-negative
// @tc.expect: Guard is created with a valid timer ID (>= 0)
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_hicollie_guard_new_001() {
    let guard = XCollieGuard::new(
        "ut_hicollie_guard_new_001",
        XCOLLIE_FLAG_LOG | XCOLLIE_FLAG_RECOVERY,
    );
    // XCollieGuard.id is private but accessible from this child module.
    // A valid timer ID from SetTimer is >= 0.
    assert!(guard.id >= 0, "XCollieGuard timer id should be non-negative, got {}", guard.id);
}

// @tc.name: ut_hicollie_guard_with_timeout_001
// @tc.desc: Test creating XCollieGuard with custom timeout via with_timeout()
// @tc.precon: HiCollie service is available
// @tc.step: 1. Create an XCollieGuard using with_timeout() with 10s timeout and LOG flag
//           2. Verify the guard's timer ID is non-negative
// @tc.expect: Guard is created with a valid timer ID (>= 0)
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_hicollie_guard_with_timeout_001() {
    let guard = XCollieGuard::with_timeout(
        "ut_hicollie_guard_with_timeout_001",
        10,
        XCOLLIE_FLAG_LOG,
    );
    assert!(guard.id >= 0, "XCollieGuard timer id should be non-negative, got {}", guard.id);
}

// @tc.name: ut_hicollie_guard_noop_flag_001
// @tc.desc: Test creating XCollieGuard with NOOP flag (do nothing on timeout)
// @tc.precon: HiCollie service is available
// @tc.step: 1. Create an XCollieGuard using new() with XCOLLIE_FLAG_NOOP
//           2. Verify the guard's timer ID is non-negative
// @tc.expect: Guard is created even with NOOP flag
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 2
#[test]
fn ut_hicollie_guard_noop_flag_001() {
    let guard = XCollieGuard::new("ut_hicollie_guard_noop_flag_001", XCOLLIE_FLAG_NOOP);
    assert!(guard.id >= 0, "XCollieGuard timer id should be non-negative, got {}", guard.id);
}

// @tc.name: ut_hicollie_set_timer_001
// @tc.desc: Test direct xcollie_set_timer FFI call returns valid timer ID
// @tc.precon: HiCollie service is available
// @tc.step: 1. Call ffi::xcollie_set_timer with a tag, timeout, and flags
//           2. Verify the returned timer ID is non-negative
//           3. Cancel the timer to clean up
// @tc.expect: set_timer returns a valid non-negative timer ID
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_hicollie_set_timer_001() {
    let id = ffi::xcollie_set_timer(
        "ut_hicollie_set_timer_001",
        5,
        XCOLLIE_FLAG_LOG,
    );
    assert!(id >= 0, "xcollie_set_timer should return non-negative id, got {}", id);
    // Clean up the timer
    ffi::xcollie_cancel_timer(id);
}
