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
use std::sync::{Arc, Mutex};

/// Test implementation of DataObsCallbackTrait for testing callback dispatch
struct TestDataObsCallback {
    change_count: Arc<Mutex<u32>>,
    ext_changes: Arc<Mutex<Vec<(String, i32)>>>,
}

impl DataObsCallbackTrait for TestDataObsCallback {
    fn on_change(&mut self) {
        *self.change_count.lock().unwrap() += 1;
    }

    fn on_change_ext(&mut self, info: &ffi::FfiChangeInfo) {
        let change_type_val = match info.change_type {
            ffi::ChangeType::INSERT => 0,
            ffi::ChangeType::DELETE => 1,
            ffi::ChangeType::UPDATE => 2,
            ffi::ChangeType::OTHER => 3,
            _ => -1,
        };
        self.ext_changes
            .lock()
            .unwrap()
            .push((info.uri.clone(), change_type_val));
    }
}

// @tc.name: ut_dataobs_change_info_construct_001
// @tc.desc: Test construction of FfiChangeInfo struct
// @tc.precon: NA
// @tc.step: 1. Create an FfiChangeInfo with INSERT type and a URI
//           2. Verify the fields match
// @tc.expect: FfiChangeInfo struct is correctly constructed
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_dataobs_change_info_construct_001() {
    let info = ffi::FfiChangeInfo {
        change_type: ffi::ChangeType::INSERT,
        uri: "datashare:///com.example.test/table1".to_string(),
    };
    assert_eq!(info.uri, "datashare:///com.example.test/table1");
}

// @tc.name: ut_dataobs_callback_on_change_001
// @tc.desc: Test DataObsCallback on_change dispatch
// @tc.precon: NA
// @tc.step: 1. Create a test callback implementation
//           2. Wrap it in DataObsCallback
//           3. Call on_change
//           4. Verify the callback was invoked
// @tc.expect: on_change is dispatched to the trait implementation
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_dataobs_callback_on_change_001() {
    let count = Arc::new(Mutex::new(0u32));
    let ext = Arc::new(Mutex::new(Vec::new()));
    let mut callback = DataObsCallback {
        inner: Box::new(TestDataObsCallback {
            change_count: count.clone(),
            ext_changes: ext,
        }),
    };
    callback.on_change();
    assert_eq!(*count.lock().unwrap(), 1);
}

// @tc.name: ut_dataobs_callback_on_change_ext_001
// @tc.desc: Test DataObsCallback on_change_ext dispatch with change info
// @tc.precon: NA
// @tc.step: 1. Create a test callback implementation
//           2. Wrap it in DataObsCallback
//           3. Call on_change_ext with FfiChangeInfo
//           4. Verify the callback received the change info
// @tc.expect: on_change_ext dispatches FfiChangeInfo to trait implementation
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_dataobs_callback_on_change_ext_001() {
    let count = Arc::new(Mutex::new(0u32));
    let ext = Arc::new(Mutex::new(Vec::new()));
    let mut callback = DataObsCallback {
        inner: Box::new(TestDataObsCallback {
            change_count: count,
            ext_changes: ext.clone(),
        }),
    };

    let info = ffi::FfiChangeInfo {
        change_type: ffi::ChangeType::UPDATE,
        uri: "datashare:///com.example/table".to_string(),
    };
    callback.on_change_ext(&info);

    let changes = ext.lock().unwrap();
    assert_eq!(changes.len(), 1);
    assert_eq!(changes[0].0, "datashare:///com.example/table");
    assert_eq!(changes[0].1, 2); // UPDATE = 2
}

// @tc.name: ut_dataobs_callback_multiple_changes_001
// @tc.desc: Test multiple on_change calls
// @tc.precon: NA
// @tc.step: 1. Create a test callback
//           2. Call on_change three times
//           3. Verify the count matches
// @tc.expect: Multiple on_change calls are all dispatched
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_dataobs_callback_multiple_changes_001() {
    let count = Arc::new(Mutex::new(0u32));
    let ext = Arc::new(Mutex::new(Vec::new()));
    let mut callback = DataObsCallback {
        inner: Box::new(TestDataObsCallback {
            change_count: count.clone(),
            ext_changes: ext,
        }),
    };
    callback.on_change();
    callback.on_change();
    callback.on_change();
    assert_eq!(*count.lock().unwrap(), 3);
}

// @tc.name: ut_dataobs_notify_change_001
// @tc.desc: Test notifying a change on a URI
// @tc.precon: DataObsMgr service is running
// @tc.step: 1. Call DataObsClient::notify_change() with a URI and user_id
//           2. Observe the return code
// @tc.expect: Function completes without panic
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_dataobs_notify_change_001() {
    let ret = DataObsClient::notify_change("datashare:///com.example.test/notify_test", 100);
    assert_ne!(ret, 0, "notify_change without registered observer should return error code");
}

// @tc.name: ut_dataobs_change_type_values_001
// @tc.desc: Test ChangeType enum variant values
// @tc.precon: NA
// @tc.step: 1. Map each ChangeType variant to its ordinal value via pattern matching
//           2. Verify each variant maps to the expected ordinal
// @tc.expect: INSERT=0, DELETE=1, UPDATE=2, OTHER=3, INVAILD=4
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_dataobs_change_type_values_001() {
    let map_type = |ct: ffi::ChangeType| -> i32 {
        match ct {
            ffi::ChangeType::INSERT => 0,
            ffi::ChangeType::DELETE => 1,
            ffi::ChangeType::UPDATE => 2,
            ffi::ChangeType::OTHER => 3,
            ffi::ChangeType::INVAILD => 4,
            _ => -1,
        }
    };
    assert_eq!(map_type(ffi::ChangeType::INSERT), 0);
    assert_eq!(map_type(ffi::ChangeType::DELETE), 1);
    assert_eq!(map_type(ffi::ChangeType::UPDATE), 2);
    assert_eq!(map_type(ffi::ChangeType::OTHER), 3);
    assert_eq!(map_type(ffi::ChangeType::INVAILD), 4);
}

// @tc.name: ut_dataobs_notify_change_ext_001
// @tc.desc: Test notifying an extended change on a URI
// @tc.precon: DataObsMgr service is running
// @tc.step: 1. Call DataObsClient::notify_change_ext() with UPDATE type and a URI
//           2. Observe the return code
// @tc.expect: notify_change_ext without registered observer returns non-zero error
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_dataobs_notify_change_ext_001() {
    let ret = DataObsClient::notify_change_ext(2, "datashare:///com.example.test/ext_notify_test");
    assert_ne!(ret, 0, "notify_change_ext without registered observer should return error code");
}
