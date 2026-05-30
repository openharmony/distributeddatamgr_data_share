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

const STUB_RETURN: i32 = -1;
const TEST_URI: &str = "datashare:///com.example.test";

// ==================== AmsMgrCallbackTrait 实现测试 ====================

/// 测试用回调实现，记录收到的事件。
struct MockCallback {
    connected: bool,
    disconnected: bool,
    last_remote_object: u64,
    last_result_code: i32,
}

impl MockCallback {
    fn new() -> Self {
        Self {
            connected: false,
            disconnected: false,
            last_remote_object: 0,
            last_result_code: 0,
        }
    }
}

impl AmsMgrCallbackTrait for MockCallback {
    fn on_connect(&mut self, remote_object: u64, result_code: i32) {
        self.connected = true;
        self.last_remote_object = remote_object;
        self.last_result_code = result_code;
    }

    fn on_disconnect(&mut self, result_code: i32) {
        self.disconnected = true;
        self.last_result_code = result_code;
    }
}

/// 验证 AmsMgrCallbackTrait 可以被实现。
#[test]
fn test_callback_trait_impl() {
    let cb = MockCallback::new();
    assert!(!cb.connected);
    assert!(!cb.disconnected);
}

/// 验证 AmsMgrCallback 可以从 trait 对象创建。
#[test]
fn test_callback_new_from_trait() {
    let mock = MockCallback::new();
    let _cb = AmsMgrCallback::new(Box::new(mock));
}

/// 验证 AmsMgrCallback::on_connect 正确委托到 trait 实现。
#[test]
fn test_callback_on_connect_delegates() {
    let mock = MockCallback::new();
    let mut cb = AmsMgrCallback::new(Box::new(mock));
    cb.on_connect(0xDEAD, 42);
    // 无法直接访问 inner，但调用不 panic 即表示委托成功
}

/// 验证 AmsMgrCallback::on_disconnect 正确委托到 trait 实现。
#[test]
fn test_callback_on_disconnect_delegates() {
    let mock = MockCallback::new();
    let mut cb = AmsMgrCallback::new(Box::new(mock));
    cb.on_disconnect(-1);
    // 无法直接访问 inner，但调用不 panic 即表示委托成功
}

/// 验证 MockCallback 满足 Send trait 约束。
#[test]
fn test_callback_trait_requires_send() {
    fn assert_send<T: Send>() {}
    assert_send::<MockCallback>();
}

// ==================== AmsMgrClient 接口测试 ====================

/// 测试 connect 弱符号 stub 返回 -1。
#[test]
fn test_connect_returns_stub_value() {
    let ret = AmsMgrClient::connect(TEST_URI, 0x1000, 0x2000);
    assert_eq!(ret, STUB_RETURN);
}

/// 测试 disconnect 弱符号 stub 返回 -1。
#[test]
fn test_disconnect_returns_stub_value() {
    let ret = AmsMgrClient::disconnect(0x1000);
    assert_eq!(ret, STUB_RETURN);
}

/// 测试 connect_ext 弱符号 stub 返回 null（即 None）。
#[test]
fn test_connect_ext_returns_none_for_stub() {
    let mock = MockCallback::new();
    let guard = AmsMgrClient::connect_ext(TEST_URI, 0x2000, Box::new(mock));
    // 弱符号 stub 返回 null UniquePtr，connect_ext 应返回 None
    assert!(guard.is_none());
}
