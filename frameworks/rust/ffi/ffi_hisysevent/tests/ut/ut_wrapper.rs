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

// @tc.name: ut_hisysevent_keyvalue_construct_001
// @tc.desc: Test construction of KeyValue struct
// @tc.precon: NA
// @tc.step: 1. Create a KeyValue with key and value
//           2. Verify the fields match
// @tc.expect: KeyValue struct is correctly constructed
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_hisysevent_keyvalue_construct_001() {
    let kv = ffi::KeyValue {
        key: "EVENT_KEY".to_string(),
        value: "event_value".to_string(),
    };
    assert_eq!(kv.key, "EVENT_KEY");
    assert_eq!(kv.value, "event_value");
}

// @tc.name: ut_hisysevent_keyvalue_empty_001
// @tc.desc: Test KeyValue with empty strings
// @tc.precon: NA
// @tc.step: 1. Create a KeyValue with empty key and value
//           2. Verify fields are empty
// @tc.expect: KeyValue allows empty strings
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_hisysevent_keyvalue_empty_001() {
    let kv = ffi::KeyValue {
        key: String::new(),
        value: String::new(),
    };
    assert!(kv.key.is_empty());
    assert!(kv.value.is_empty());
}

// HiSysEvent::write() is void. No meaningful assertions can be made without
// mocking the underlying C++ implementation. Tests for write_behavior,
// write_fault, write_empty_params, and write_security have been removed.
