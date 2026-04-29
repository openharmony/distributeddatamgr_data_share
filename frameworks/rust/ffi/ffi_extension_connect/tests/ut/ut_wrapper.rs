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

#[test]
fn test_connect_extension_signature() {
    let _fn_ptr: fn(&str, &str, i32, usize) -> i32 = connect_extension;
}

#[test]
fn test_disconnect_extension_signature() {
    let _fn_ptr: fn(&str) -> i32 = disconnect_extension;
}

#[test]
fn test_has_active_connection_signature() {
    let _fn_ptr: fn(&str) -> bool = has_active_connection;
}

#[test]
fn test_schedule_disconnect_signature() {
    let _fn_ptr: fn(&str, i32) = schedule_disconnect;
}

#[test]
fn test_init_executor_signature() {
    let _fn_ptr: fn(usize) = init_executor;
}
