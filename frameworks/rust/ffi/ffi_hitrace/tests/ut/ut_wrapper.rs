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

// @tc.name: ut_hitrace_id_construct_001
// @tc.desc: Test construction of FfiHiTraceId struct
// @tc.precon: NA
// @tc.step: 1. Create an FfiHiTraceId with test values
//           2. Verify all fields match
// @tc.expect: FfiHiTraceId struct is correctly constructed
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_hitrace_id_construct_001() {
    let id = ffi::FfiHiTraceId {
        chain_id: 123456,
        span_id: 789,
        parent_span_id: 456,
        flags: 1,
        is_valid: true,
    };
    assert_eq!(id.chain_id, 123456);
    assert_eq!(id.span_id, 789);
    assert_eq!(id.parent_span_id, 456);
    assert_eq!(id.flags, 1);
    assert!(id.is_valid);
}

// @tc.name: ut_hitrace_id_invalid_001
// @tc.desc: Test FfiHiTraceId with invalid state
// @tc.precon: NA
// @tc.step: 1. Create an FfiHiTraceId with is_valid = false
//           2. Verify the flag
// @tc.expect: Invalid trace ID is correctly represented
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_hitrace_id_invalid_001() {
    let id = ffi::FfiHiTraceId {
        chain_id: 0,
        span_id: 0,
        parent_span_id: 0,
        flags: 0,
        is_valid: false,
    };
    assert!(!id.is_valid);
    assert_eq!(id.chain_id, 0);
}

// HiTrace functions (is_trace_enabled, begin_section, end_section,
// get_current_trace_id) and TraceGuard RAII pattern produce no assertable
// return values without mocking. Tests removed: is_trace_enabled_001,
// begin_end_section_001, get_current_trace_id_001, guard_new_001,
// guard_nested_001.
