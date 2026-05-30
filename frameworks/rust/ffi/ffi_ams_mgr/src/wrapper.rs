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

//! CXX bridge wrapper for AbilityMgrProxy connection management.

/// CXX bridge 声明，映射到 C++ 侧的 OHOS::DataShare::FfiAbilityMgr 命名空间。
#[cxx::bridge(namespace = "OHOS::DataShare::FfiAbilityMgr")]
pub mod ffi {
    extern "Rust" {
        /// Rust 回调类型，C++ 侧通过 `rust::Box<AbilityMgrCallback>` 持有。
        type AbilityMgrCallback;

        /// ability 连接建立时的回调。
        fn on_connect(self: &mut AbilityMgrCallback, remote_object: u64, result_code: i32);

        /// ability 连接断开时的回调。
        fn on_disconnect(self: &mut AbilityMgrCallback, result_code: i32);
    }

    unsafe extern "C++" {
        include!("ffi_ability_mgr_bridge.h");

        /// C++ 不透明连接句柄类型。
        type ConnectionHandle;

        /// 通过 AbilityManager 连接 DataShare extension ability。
        #[cxx_name = "AbilityMgrConnect"]
        fn ability_mgr_connect(uri: &str, connect_remote: u64, caller_token: u64) -> i32;

        /// 断开 AbilityManager 连接。
        #[cxx_name = "AbilityMgrDisconnect"]
        fn ability_mgr_disconnect(connect_remote: u64) -> i32;

        /// 创建 AbilityConnectionStub 并发起连接，返回连接句柄。
        #[cxx_name = "ConnectionConnectExt"]
        fn connection_connect_ext(
            uri: &str,
            caller_token: u64,
            callback: Box<AbilityMgrCallback>,
        ) -> UniquePtr<ConnectionHandle>;

        /// 断开并释放连接句柄。
        #[cxx_name = "ConnectionDisconnect"]
        fn connection_disconnect(handle: &ConnectionHandle);
    }
}

/// AbilityMgr 回调 trait，调用者需实现此 trait 以接收连接/断开通知。
pub trait AbilityMgrCallbackTrait: Send {
    /// ability 连接建立时调用。
    /// `remote_object` 为 `IRemoteObject*` 的 u64 地址表示。
    fn on_connect(&mut self, remote_object: u64, result_code: i32);

    /// ability 连接断开时调用。
    fn on_disconnect(&mut self, result_code: i32);
}

/// CXX bridge 回调类型，内部持有 trait 对象。
pub struct AbilityMgrCallback {
    inner: Box<dyn AbilityMgrCallbackTrait>,
}

impl AbilityMgrCallback {
    /// 从 trait 对象创建回调。
    pub fn new(inner: Box<dyn AbilityMgrCallbackTrait>) -> Self {
        Self { inner }
    }

    fn on_connect(&mut self, remote_object: u64, result_code: i32) {
        self.inner.on_connect(remote_object, result_code);
    }

    fn on_disconnect(&mut self, result_code: i32) {
        self.inner.on_disconnect(result_code);
    }
}

/// AbilityMgrClient 安全封装，提供静态方法调用 CXX bridge 函数。
pub struct AbilityMgrClient;

impl AbilityMgrClient {
    /// 通过 AbilityManager 连接 DataShare extension ability。
    pub fn connect(uri: &str, connect_remote: u64, caller_token: u64) -> i32 {
        ffi::ability_mgr_connect(uri, connect_remote, caller_token)
    }

    /// 断开 AbilityManager 连接。
    pub fn disconnect(connect_remote: u64) -> i32 {
        ffi::ability_mgr_disconnect(connect_remote)
    }

    /// 创建 AbilityConnectionStub 并发起连接。
    /// 返回 `ConnectionGuard`（RAII），drop 时自动断开。
    pub fn connect_ext(
        uri: &str,
        caller_token: u64,
        callback: Box<dyn AbilityMgrCallbackTrait>,
    ) -> Option<ConnectionGuard> {
        let cb = Box::new(AbilityMgrCallback::new(callback));
        let handle = ffi::connection_connect_ext(uri, caller_token, cb);
        if handle.is_null() {
            None
        } else {
            Some(ConnectionGuard { handle })
        }
    }
}

/// RAII 连接守卫，drop 时自动断开连接。
pub struct ConnectionGuard {
    handle: cxx::UniquePtr<ffi::ConnectionHandle>,
}

impl ConnectionGuard {
    /// 获取底层连接句柄的引用。
    pub fn handle_ref(&self) -> &ffi::ConnectionHandle {
        self.handle.as_ref().unwrap()
    }

    /// 主动断开连接（不等待 drop）。
    pub fn disconnect(&self) {
        if let Some(h) = self.handle.as_ref() {
            ffi::connection_disconnect(h);
        }
    }
}

impl Drop for ConnectionGuard {
    fn drop(&mut self) {
        // UniquePtr<ConnectionHandle> 的 C++ 析构函数会自动断开连接
        // 这里不需要额外操作，drop UniquePtr 即可
    }
}

#[cfg(test)]
mod ut_wrapper {
    include!("../tests/ut/ut_wrapper.rs");
}
