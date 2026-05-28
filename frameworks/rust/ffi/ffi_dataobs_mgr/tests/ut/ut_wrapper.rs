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
const TEST_OBSERVER_ID: u64 = 0x1234;

// ==================== 基础接口测试 ====================

/// 测试 register_observer 弱符号 stub 返回 -1。
#[test]
fn test_register_observer_returns_stub_value() {
    let ret = DataObsMgrClient::register_observer(TEST_URI, TEST_OBSERVER_ID);
    assert_eq!(ret, STUB_RETURN);
}

/// 测试 unregister_observer 弱符号 stub 返回 -1。
#[test]
fn test_unregister_observer_returns_stub_value() {
    let ret = DataObsMgrClient::unregister_observer(TEST_URI, TEST_OBSERVER_ID);
    assert_eq!(ret, STUB_RETURN);
}

/// 测试 notify_change 弱符号 stub 返回 -1。
#[test]
fn test_notify_change_returns_stub_value() {
    let ret = DataObsMgrClient::notify_change(TEST_URI);
    assert_eq!(ret, STUB_RETURN);
}

// ==================== 扩展接口测试 ====================

/// 测试 register_observer_ext 弱符号 stub 返回 -1。
#[test]
fn test_register_observer_ext_returns_stub_value() {
    let ret = DataObsMgrClient::register_observer_ext(TEST_URI, TEST_OBSERVER_ID, true);
    assert_eq!(ret, STUB_RETURN);
}

/// 测试 register_observer_ext is_descendants=false 路径。
#[test]
fn test_register_observer_ext_no_descendants() {
    let ret = DataObsMgrClient::register_observer_ext(TEST_URI, TEST_OBSERVER_ID, false);
    assert_eq!(ret, STUB_RETURN);
}

/// 测试 unregister_observer_ext 弱符号 stub 返回 -1。
#[test]
fn test_unregister_observer_ext_returns_stub_value() {
    let ret = DataObsMgrClient::unregister_observer_ext(TEST_URI, TEST_OBSERVER_ID);
    assert_eq!(ret, STUB_RETURN);
}

/// 测试 notify_change_ext 弱符号 stub 返回 -1。
#[test]
fn test_notify_change_ext_returns_stub_value() {
    let change_info: &[u8] = b"\x00\x01\x02\x03";
    let ret = DataObsMgrClient::notify_change_ext(change_info);
    assert_eq!(ret, STUB_RETURN);
}

/// 测试 notify_change_ext 空数据。
#[test]
fn test_notify_change_ext_empty_data() {
    let ret = DataObsMgrClient::notify_change_ext(&[]);
    assert_eq!(ret, STUB_RETURN);
}

// ==================== WithOption 接口测试 ====================

/// 测试 register_observer_ext_with_option 弱符号 stub 返回 -1。
#[test]
fn test_register_observer_ext_with_option_returns_stub_value() {
    let ret = DataObsMgrClient::register_observer_ext_with_option(
        TEST_URI,
        TEST_OBSERVER_ID,
        true,
        false,
    );
    assert_eq!(ret, STUB_RETURN);
}

/// 测试 register_observer_ext_with_option is_system=true 路径。
#[test]
fn test_register_observer_ext_with_option_system() {
    let ret = DataObsMgrClient::register_observer_ext_with_option(
        TEST_URI,
        TEST_OBSERVER_ID,
        false,
        true,
    );
    assert_eq!(ret, STUB_RETURN);
}

/// 测试 unregister_observer_ext_with_option 弱符号 stub 返回 -1。
#[test]
fn test_unregister_observer_ext_with_option_returns_stub_value() {
    let ret =
        DataObsMgrClient::unregister_observer_ext_with_option(TEST_URI, TEST_OBSERVER_ID, false);
    assert_eq!(ret, STUB_RETURN);
}

/// 测试 unregister_observer_ext_with_option is_system=true 路径。
#[test]
fn test_unregister_observer_ext_with_option_system() {
    let ret =
        DataObsMgrClient::unregister_observer_ext_with_option(TEST_URI, TEST_OBSERVER_ID, true);
    assert_eq!(ret, STUB_RETURN);
}

/// 测试 notify_change_ext_with_option 弱符号 stub 返回 -1。
#[test]
fn test_notify_change_ext_with_option_returns_stub_value() {
    let change_info: &[u8] = b"\x00\x01\x02\x03";
    let ret = DataObsMgrClient::notify_change_ext_with_option(change_info, false);
    assert_eq!(ret, STUB_RETURN);
}

/// 测试 notify_change_ext_with_option is_system=true 路径。
#[test]
fn test_notify_change_ext_with_option_system() {
    let change_info: &[u8] = b"\x00\x01\x02\x03";
    let ret = DataObsMgrClient::notify_change_ext_with_option(change_info, true);
    assert_eq!(ret, STUB_RETURN);
}
