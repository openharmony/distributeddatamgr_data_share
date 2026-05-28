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

//! FFI bridge for DataShareManagerImpl (singleton, no handle parameter).

use std::ffi::c_void;

use ipc::parcel::MsgParcel;

use crate::connection::manager_impl::DataShareManagerImpl;

unsafe fn slice_to_str(ptr: *const u8, len: u32) -> &'static str {
    std::str::from_utf8_unchecked(std::slice::from_raw_parts(ptr, len as usize))
}

/// Acquire data_share service remote via full SA flow.
/// Returns MsgParcel* containing the RemoteObject (caller must delete), or nullptr.
#[no_mangle]
pub unsafe extern "C" fn DataShareMgrImplAcquireServiceRemote() -> *mut c_void {
    let instance = DataShareManagerImpl::get_instance();
    let remote = match instance.acquire_data_share_remote() {
        Some(r) => r,
        None => return std::ptr::null_mut(),
    };
    let mut parcel = MsgParcel::new();
    if parcel.write_remote(remote).is_err() {
        return std::ptr::null_mut();
    }
    parcel.into_raw() as *mut c_void
}

/// Subscribe to SA 1301 changes.
#[no_mangle]
pub unsafe extern "C" fn DataShareMgrImplSubscribeSA() {
    let instance = DataShareManagerImpl::get_instance();
    instance.subscribe_sa();
}

/// Reset cached service handle.
#[no_mangle]
pub unsafe extern "C" fn DataShareMgrImplResetServiceHandle() {
    let instance = DataShareManagerImpl::get_instance();
    instance.reset_service_handle();
}

#[no_mangle]
pub unsafe extern "C" fn DataShareMgrImplSetBundleName(name: *const u8, len: u32) {
    if name.is_null() {
        return;
    }
    let instance = DataShareManagerImpl::get_instance();
    instance.set_bundle_name(slice_to_str(name, len));
}

#[no_mangle]
pub unsafe extern "C" fn DataShareMgrImplSetCallCount(
    func: *const u8,
    func_len: u32,
    uri: *const u8,
    uri_len: u32,
) -> bool {
    if func.is_null() || uri.is_null() {
        return false;
    }
    let instance = DataShareManagerImpl::get_instance();
    instance.set_call_count(slice_to_str(func, func_len), slice_to_str(uri, uri_len))
}

#[no_mangle]
pub unsafe extern "C" fn DataShareMgrImplSetRegisterCallback(
    controller_id: u64,
    cb: unsafe extern "C" fn(*mut c_void),
    ctx: *mut c_void,
) {
    let instance = DataShareManagerImpl::get_instance();
    let ctx_val = ctx as usize;
    instance.set_register_callback(controller_id, move || {
        unsafe { cb(ctx_val as *mut c_void) };
    });
}

#[no_mangle]
pub unsafe extern "C" fn DataShareMgrImplRemoveRegisterCallback(controller_id: u64) {
    let instance = DataShareManagerImpl::get_instance();
    instance.remove_register_callback(controller_id);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareMgrImplOnAddSystemAbility(sa_id: i32) {
    let instance = DataShareManagerImpl::get_instance();
    instance.on_add_system_ability(sa_id, "");
}

#[no_mangle]
pub unsafe extern "C" fn DataShareMgrImplOnRemoteDied() {
    let instance = DataShareManagerImpl::get_instance();
    instance.on_remote_died();
}

/// Acquire KV service remote (SA 1301) only.
/// Returns MsgParcel* containing the RemoteObject (caller must delete), or nullptr.
/// Corresponds to C++ GetDistributedDataManager().
#[no_mangle]
pub unsafe extern "C" fn DataShareMgrImplAcquireKvServiceRemote() -> *mut c_void {
    let instance = DataShareManagerImpl::get_instance();
    let remote = match instance.acquire_kv_service_remote() {
        Some(r) => r,
        None => return std::ptr::null_mut(),
    };
    let mut parcel = MsgParcel::new();
    if parcel.write_remote(remote).is_err() {
        return std::ptr::null_mut();
    }
    parcel.into_raw() as *mut c_void
}

/// Death recovery: re-register observers after service restart.
/// Subscriber managers (Phase 5-6) will be called here once implemented.
#[no_mangle]
pub unsafe extern "C" fn DataShareMgrImplOnDeathRecovery() {
    // TODO: When Rust subscriber managers are implemented (Phase 5-6):
    // RdbSubscriberManager::get_instance().recover_observers(proxy);
    // PublishedDataSubscriberManager::get_instance().recover_observers(proxy);
    // ProxyDataSubscriberManager::get_instance().recover_observers(proxy);
}
