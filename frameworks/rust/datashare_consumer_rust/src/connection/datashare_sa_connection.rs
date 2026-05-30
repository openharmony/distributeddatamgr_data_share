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

//! DataShareSAConnection — system-ability-based provider connection.
//!
//! Corresponds to C++ `DataShareSAConnection` in
//! `frameworks/native/consumer/src/datashare_sa_connection.cpp`.
//!
//! Unlike `DataShareConnection` (which connects via AbilityMgr extension ability),
//! this variant resolves the provider through `samgr_rust`:
//! 1. Asks the DataShare service for the SA's interface descriptor + IPC code.
//! 2. Loads (or fetches) the target SA via `SystemAbilityManager`.
//! 3. Sends a single IPC request to obtain the provider proxy IRemoteObject.
//! 4. Registers a death recipient that clears the cached proxy.

use std::any::Any;
use std::sync::Mutex;

use ipc::parcel::MsgParcel;
use ipc::remote::RemoteObj;
use samgr::manage::SystemAbilityManager;

use crate::connection::manager_impl::DataShareManagerImpl;

/// Sentinel for "interface descriptor / code not yet fetched".
/// Mirrors C++ `INVALID_INTERFACE_CODE = UINT32_MAX`.
const INVALID_INTERFACE_CODE: u32 = u32::MAX;

/// Generic data_share error code (matches C++ `DATA_SHARE_ERROR = -1`).
pub const DATA_SHARE_ERROR: i32 = -1;

/// E_OK from datashare_errno.h.
const E_OK: i32 = 0;

/// Cached interface info for a single SA.
#[derive(Default, Clone)]
struct InterfaceInfo {
    code: u32,
    descriptor: String,
}

impl InterfaceInfo {
    fn new() -> Self {
        Self {
            code: INVALID_INTERFACE_CODE,
            descriptor: String::new(),
        }
    }
}

/// Connection to a system-ability-hosted DataShare provider.
///
/// Lifetime: created and destroyed via FFI from C++ `DataShareSAConnection`.
/// Internal state is mutated under `Mutex` to support concurrent access from
/// the C++ side (which itself takes a mutex around the cached proxy).
pub struct DataShareSAConnection {
    uri: String,
    sa_id: i32,
    wait_time: i32,

    /// Cached interface descriptor + code (looked up once, then reused).
    interface_info: Mutex<InterfaceInfo>,

    /// Cached proxy IRemoteObject — kept alive while the connection is in use,
    /// also used to detect "already connected" on subsequent fetches.
    proxy: Mutex<Option<RemoteObj>>,

    /// Holder for the death recipient registration; dropped on destroy.
    death_handle: Mutex<Option<Box<dyn Any>>>,
}

unsafe impl Send for DataShareSAConnection {}
unsafe impl Sync for DataShareSAConnection {}

impl DataShareSAConnection {
    pub fn new(uri: String, sa_id: i32, wait_time: i32) -> Self {
        let wait_time = if wait_time < 0 { 0 } else { wait_time };
        Self {
            uri,
            sa_id,
            wait_time,
            interface_info: Mutex::new(InterfaceInfo::new()),
            proxy: Mutex::new(None),
            death_handle: Mutex::new(None),
        }
    }

    /// Connects if needed, then writes the proxy IRemoteObject into `out_parcel`
    /// for the C++ caller to read back via `MessageParcel::ReadRemoteObject()`.
    /// Returns 0 on success, negative on failure.
    pub fn write_proxy_remote(&self, out_parcel: &mut MsgParcel) -> i32 {
        let remote = match self.get_or_connect() {
            Some(r) => r,
            None => return DATA_SHARE_ERROR,
        };
        if out_parcel.write_remote(remote).is_err() {
            return DATA_SHARE_ERROR;
        }
        0
    }

    /// Clear the cached proxy so the next `get_or_connect()` reconnects.
    /// Called from the C++ side when a death notification is observed.
    pub fn on_remote_died(&self) {
        *self.proxy.lock().unwrap() = None;
        *self.death_handle.lock().unwrap() = None;
    }

    fn get_or_connect(&self) -> Option<RemoteObj> {
        if let Some(p) = self.proxy.lock().unwrap().clone() {
            return Some(p);
        }
        self.connect_to_provider()
    }

    fn connect_to_provider(&self) -> Option<RemoteObj> {
        let info = self.ensure_interface_info()?;

        let remote = match SystemAbilityManager::check_system_ability(self.sa_id) {
            Some(r) => r,
            None => SystemAbilityManager::load_system_ability(self.sa_id, self.wait_time)?,
        };

        let mut data = MsgParcel::new();
        if data.write_interface_token(&info.descriptor).is_err() {
            return None;
        }
        if data.write(&self.uri).is_err() {
            return None;
        }

        let mut reply = remote.send_request(info.code, &mut data).ok()?;

        let err_code: i32 = reply.read().ok()?;
        if err_code != E_OK {
            return None;
        }
        let proxy_remote = reply.read_remote().ok()?;

        // Register a death recipient that clears the cached proxy. The closure
        // must not hold a `&self` reference (which would tie the recipient's
        // lifetime to a borrow); drop the cached proxy by clearing the mutex.
        let proxy_slot = self.proxy_slot_ptr();
        let handle = proxy_remote.add_death_recipient(move |_| unsafe {
            if let Some(slot) = proxy_slot.as_ref() {
                *slot.lock().unwrap() = None;
            }
        });
        *self.death_handle.lock().unwrap() = handle.map(|h| Box::new(h) as Box<dyn Any>);

        *self.proxy.lock().unwrap() = Some(proxy_remote.clone());
        Some(proxy_remote)
    }

    /// Look up the SA's IPC descriptor + code via DataShareService.
    /// Cached on success.
    fn ensure_interface_info(&self) -> Option<InterfaceInfo> {
        {
            let cur = self.interface_info.lock().unwrap();
            if cur.code != INVALID_INTERFACE_CODE {
                return Some(cur.clone());
            }
        }
        let svc = DataShareManagerImpl::get_service_proxy()?;
        let (err_code, code, descriptor) =
            svc.get_connection_interface_info(self.sa_id, self.wait_time as u32)?;
        if err_code != E_OK || code == INVALID_INTERFACE_CODE {
            return None;
        }
        let info = InterfaceInfo { code, descriptor };
        *self.interface_info.lock().unwrap() = info.clone();
        Some(info)
    }

    /// Store an externally-registered death recipient handle so it is not
    /// dropped prematurely. Used by `DataShareSAConnectionLinkToDeathFFI`.
    pub fn set_external_death_handle(&self, h: Option<Box<dyn Any>>) {
        *self.death_handle.lock().unwrap() = h;
    }

    fn proxy_slot_ptr(&self) -> *const Mutex<Option<RemoteObj>> {
        &self.proxy as *const _
    }
}
