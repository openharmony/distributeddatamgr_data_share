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

/// Test implementation of CommonEventCallbackTrait for testing callback dispatch
struct TestEventCallback {
    events: Arc<Mutex<Vec<String>>>,
}

impl CommonEventCallbackTrait for TestEventCallback {
    fn on_receive(&mut self, event_name: &str) {
        self.events.lock().unwrap().push(event_name.to_string());
    }
}

// @tc.name: ut_commonevent_callback_dispatch_001
// @tc.desc: Test that CommonEventCallback correctly dispatches to trait implementation
// @tc.precon: NA
// @tc.step: 1. Create a test callback trait implementation
//           2. Wrap it in a CommonEventCallback
//           3. Call on_receive with an event name
//           4. Verify the event was received by the trait implementation
// @tc.expect: Callback dispatches event correctly to trait implementation
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_commonevent_callback_dispatch_001() {
    let events = Arc::new(Mutex::new(Vec::new()));
    let mut callback = CommonEventCallback {
        inner: Box::new(TestEventCallback {
            events: events.clone(),
        }),
    };
    callback.on_receive("test.EVENT_ONE");
    let received = events.lock().unwrap();
    assert_eq!(received.len(), 1);
    assert_eq!(received[0], "test.EVENT_ONE");
}

// @tc.name: ut_commonevent_callback_multiple_events_001
// @tc.desc: Test receiving multiple events through the callback
// @tc.precon: NA
// @tc.step: 1. Create a test callback
//           2. Dispatch three different events
//           3. Verify all events were received in order
// @tc.expect: All events are dispatched correctly and in order
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_commonevent_callback_multiple_events_001() {
    let events = Arc::new(Mutex::new(Vec::new()));
    let mut callback = CommonEventCallback {
        inner: Box::new(TestEventCallback {
            events: events.clone(),
        }),
    };
    callback.on_receive("event.FIRST");
    callback.on_receive("event.SECOND");
    callback.on_receive("event.THIRD");

    let received = events.lock().unwrap();
    assert_eq!(received.len(), 3);
    assert_eq!(received[0], "event.FIRST");
    assert_eq!(received[1], "event.SECOND");
    assert_eq!(received[2], "event.THIRD");
}

// @tc.name: ut_commonevent_callback_empty_event_001
// @tc.desc: Test receiving an event with empty name
// @tc.precon: NA
// @tc.step: 1. Create a test callback
//           2. Dispatch an event with empty name
//           3. Verify the empty event was received
// @tc.expect: Empty event name is handled without panic
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 2
#[test]
fn ut_commonevent_callback_empty_event_001() {
    let events = Arc::new(Mutex::new(Vec::new()));
    let mut callback = CommonEventCallback {
        inner: Box::new(TestEventCallback {
            events: events.clone(),
        }),
    };
    callback.on_receive("");
    let received = events.lock().unwrap();
    assert_eq!(received.len(), 1);
    assert_eq!(received[0], "");
}

// @tc.name: ut_commonevent_publish_001
// @tc.desc: Test publishing a common event
// @tc.precon: CommonEvent service is running
// @tc.step: 1. Call CommonEventPublisher::publish() with event name and code
//           2. Observe the result
// @tc.expect: Function completes without panic
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_commonevent_publish_001() {
    // C++ reference: EXPECT_EQ(CommonEventManager::PublishCommonEvent(data), true)
    // When CommonEvent service is running on device, publish returns true.
    let result = CommonEventPublisher::publish("test.EVENT_PUBLISH", 0);
    assert!(result, "Publish should return true when CommonEvent service is running");
}
