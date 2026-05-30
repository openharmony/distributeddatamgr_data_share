/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//! AbilityMgrProxy — Ability Manager Service proxy.
//!
//! Corresponds to C++ `AbilityMgrProxy` in
//! `frameworks/native/proxy/src/ability_mgr_proxy.cpp` (119 lines).
//!
//! Singleton proxy that communicates with the AbilityManagerService
//! to connect/disconnect DataShare extension abilities.

use std::ffi::c_void;
use std::sync::{Mutex, OnceLock};

use ffi_ams_mgr::AbilityMgrClient;

/// AbilityMgrProxy — singleton proxy to AbilityManagerService.
///
/// Corresponds to C++ `AbilityMgrProxy`.
///
/// Used by `DataShareConnection` to:
/// - Connect to a DataShare extension ability
/// - Disconnect from a DataShare extension ability
pub struct AbilityMgrProxy {
    /// Whether connected to the SA
    connected: Mutex<bool>,
}

/// Singleton instance.
static INSTANCE: OnceLock<AbilityMgrProxy> = OnceLock::new();

impl AbilityMgrProxy {
    /// Get the singleton instance.
    ///
    /// Corresponds to C++ `AbilityMgrProxy::GetInstance()`.
    pub fn get_instance() -> &'static AbilityMgrProxy {
        INSTANCE.get_or_init(|| AbilityMgrProxy {
            connected: Mutex::new(false),
        })
    }

    /// Connect to a DataShare extension ability.
    ///
    /// Corresponds to C++ `Connect(const std::string& uri, const sptr<IRemoteObject>& connect, const sptr<IRemoteObject>& callerToken)`.
    pub fn connect(
        &self,
        uri: &str,
        connect_remote: *mut c_void,
        caller_token: *mut c_void,
    ) -> i32 {
        if !self.connect_sa() {
            return -1;
        }
        AbilityMgrClient::connect(uri, connect_remote as u64, caller_token as u64)
    }

    /// Disconnect from a DataShare extension ability.
    ///
    /// Corresponds to C++ `DisConnect(sptr<IRemoteObject> connect)`.
    pub fn disconnect(&self, connect_remote: *mut c_void) -> i32 {
        AbilityMgrClient::disconnect(connect_remote as u64)
    }

    /// Internal: ensure connection to the SA.
    fn connect_sa(&self) -> bool {
        let mut connected = self.connected.lock().unwrap();
        if *connected {
            return true;
        }
        // The C++ FFI wrapper handles SA connection internally via AbilityMgrProxy::GetInstance()
        *connected = true;
        true
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_ability_mgr_proxy_singleton() {
        let instance1 = AbilityMgrProxy::get_instance();
        let instance2 = AbilityMgrProxy::get_instance();
        assert!(std::ptr::eq(instance1, instance2));
    }

    #[test]
    fn test_ability_mgr_proxy_connect_sa() {
        let proxy = AbilityMgrProxy::get_instance();
        // connect_sa sets connected flag
        assert!(proxy.connect_sa());
    }
}
