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

//! CXX bridge wrapper for DataObsMgrClient observer management.
//!
//! Provides type-safe Rust bindings for the 9 DataObsMgr operations:
//! - Basic: `register_observer` / `unregister_observer` / `notify_change`
//! - Extended: `register_observer_ext` / `unregister_observer_ext` / `notify_change_ext`
//! - WithOption: `register_observer_ext_with_option` / `unregister_observer_ext_with_option`
//!   / `notify_change_ext_with_option`

/// CXX bridge 声明，映射到 C++ 侧的 OHOS::DataShare::FfiDataObsMgr 命名空间。
#[cxx::bridge(namespace = "OHOS::DataShare::FfiDataObsMgr")]
pub mod ffi {
    unsafe extern "C++" {
        include!("ffi_dataobs_mgr_bridge.h");

        /// 注册基础数据观察者。
        #[cxx_name = "RegisterObserver"]
        fn register_observer(uri: &str, observer_id: u64) -> i32;
        /// 注销基础数据观察者。
        #[cxx_name = "UnregisterObserver"]
        fn unregister_observer(uri: &str, observer_id: u64) -> i32;
        /// 通知基础数据变更。
        #[cxx_name = "NotifyChange"]
        fn notify_change(uri: &str) -> i32;

        /// 注册扩展数据观察者（支持 descendants 标志）。
        #[cxx_name = "RegisterObserverExt"]
        fn register_observer_ext(uri: &str, observer_id: u64, is_descendants: bool) -> i32;
        /// 注销扩展数据观察者。
        #[cxx_name = "UnregisterObserverExt"]
        fn unregister_observer_ext(uri: &str, observer_id: u64) -> i32;
        /// 通知扩展数据变更（序列化的 ChangeInfo）。
        #[cxx_name = "NotifyChangeExt"]
        fn notify_change_ext(change_info: &[u8]) -> i32;

        /// 注册带 DataObsOption 的扩展数据观察者。
        #[cxx_name = "RegisterObserverExtWithOption"]
        fn register_observer_ext_with_option(
            uri: &str,
            observer_id: u64,
            is_descendants: bool,
            is_system: bool,
        ) -> i32;
        /// 注销带 DataObsOption 的扩展数据观察者。
        #[cxx_name = "UnregisterObserverExtWithOption"]
        fn unregister_observer_ext_with_option(uri: &str, observer_id: u64, is_system: bool)
            -> i32;
        /// 通知带 DataObsOption 的扩展数据变更。
        #[cxx_name = "NotifyChangeExtWithOption"]
        fn notify_change_ext_with_option(change_info: &[u8], is_system: bool) -> i32;
    }
}

/// DataObsMgrClient 的安全封装，提供静态方法调用 CXX bridge 函数。
pub struct DataObsMgrClient;

impl DataObsMgrClient {
    /// 注册基础数据观察者。
    pub fn register_observer(uri: &str, observer_id: u64) -> i32 {
        ffi::register_observer(uri, observer_id)
    }

    /// 注销基础数据观察者。
    pub fn unregister_observer(uri: &str, observer_id: u64) -> i32 {
        ffi::unregister_observer(uri, observer_id)
    }

    /// 通知基础数据变更。
    pub fn notify_change(uri: &str) -> i32 {
        ffi::notify_change(uri)
    }

    /// 注册扩展数据观察者（支持 descendants 标志）。
    pub fn register_observer_ext(uri: &str, observer_id: u64, is_descendants: bool) -> i32 {
        ffi::register_observer_ext(uri, observer_id, is_descendants)
    }

    /// 注销扩展数据观察者。
    pub fn unregister_observer_ext(uri: &str, observer_id: u64) -> i32 {
        ffi::unregister_observer_ext(uri, observer_id)
    }

    /// 通知扩展数据变更（序列化的 ChangeInfo）。
    pub fn notify_change_ext(change_info: &[u8]) -> i32 {
        ffi::notify_change_ext(change_info)
    }

    /// 注册带 DataObsOption 的扩展数据观察者。
    pub fn register_observer_ext_with_option(
        uri: &str,
        observer_id: u64,
        is_descendants: bool,
        is_system: bool,
    ) -> i32 {
        ffi::register_observer_ext_with_option(uri, observer_id, is_descendants, is_system)
    }

    /// 注销带 DataObsOption 的扩展数据观察者。
    pub fn unregister_observer_ext_with_option(
        uri: &str,
        observer_id: u64,
        is_system: bool,
    ) -> i32 {
        ffi::unregister_observer_ext_with_option(uri, observer_id, is_system)
    }

    /// 通知带 DataObsOption 的扩展数据变更。
    pub fn notify_change_ext_with_option(change_info: &[u8], is_system: bool) -> i32 {
        ffi::notify_change_ext_with_option(change_info, is_system)
    }
}

#[cfg(test)]
mod ut_wrapper {
    include!("../tests/ut/ut_wrapper.rs");
}
