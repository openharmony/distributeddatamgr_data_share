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

//! FFI bridge for observer stubs (RdbObserverStub, PublishedDataObserverStub, ProxyDataObserverStub).
//!
//! Each stub has a per-instance handle. The Rust side validates the IPC descriptor/code,
//! then calls back into C++ for unmarshal + user callback dispatch.

use ipc::parcel::MsgParcel;
use std::ffi::c_void;

const ERR_INVALID_STATE: i32 = -5;
const REQUEST_CODE: u32 = 0;

const RDB_DESCRIPTOR: &str = "OHOS.DataShare.IDataProxyRdbObserver";
const PUB_DESCRIPTOR: &str = "OHOS.DataShare.IDataProxyPublishedDataObserver";

const PROXY_DESCRIPTOR: &str = "OHOS.DataShare.IProxyDataObserver";

type OnChangeFn = unsafe extern "C" fn(*mut c_void, *mut c_void);

struct ObserverHandle {
    on_change: Option<OnChangeFn>,
    ctx: *mut c_void,
}

unsafe impl Send for ObserverHandle {}
unsafe impl Sync for ObserverHandle {}

unsafe fn do_remote_request(handle: *mut c_void, code: u32, data_parcel: *mut c_void) -> i32 {
    if handle.is_null() || data_parcel.is_null() {
        return ERR_INVALID_STATE;
    }
    let h = &*(handle as *const ObserverHandle);
    let mut data = MsgParcel::from_ptr(data_parcel as *mut _);
    if data.read_interface_token().is_err() {
        return ERR_INVALID_STATE;
    }
    if code != REQUEST_CODE {
        return ERR_INVALID_STATE;
    }
    if let Some(on_change) = h.on_change {
        (on_change)(h.ctx, data_parcel);
    }
    0
}

// ===== RdbObserverStub =====

#[no_mangle]
pub unsafe extern "C" fn DataShareRdbObserverCreate(
    on_change: OnChangeFn,
    ctx: *mut c_void,
) -> *mut c_void {
    let handle = Box::new(ObserverHandle {
        on_change: Some(on_change),
        ctx,
    });
    Box::into_raw(handle) as *mut c_void
}

#[no_mangle]
pub unsafe extern "C" fn DataShareRdbObserverDestroy(handle: *mut c_void) {
    if !handle.is_null() {
        drop(Box::from_raw(handle as *mut ObserverHandle));
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareRdbObserverOnRemoteRequest(
    handle: *mut c_void,
    code: u32,
    data_parcel: *mut c_void,
    _reply_parcel: *mut c_void,
) -> i32 {
    do_remote_request(handle, code, data_parcel)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareRdbObserverClearCallback(handle: *mut c_void) {
    if !handle.is_null() {
        let h = &mut *(handle as *mut ObserverHandle);
        h.on_change = None;
    }
}

// ===== PublishedDataObserverStub =====

#[no_mangle]
pub unsafe extern "C" fn DataSharePubObserverCreate(
    on_change: OnChangeFn,
    ctx: *mut c_void,
) -> *mut c_void {
    let handle = Box::new(ObserverHandle {
        on_change: Some(on_change),
        ctx,
    });
    Box::into_raw(handle) as *mut c_void
}

#[no_mangle]
pub unsafe extern "C" fn DataSharePubObserverDestroy(handle: *mut c_void) {
    if !handle.is_null() {
        drop(Box::from_raw(handle as *mut ObserverHandle));
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataSharePubObserverOnRemoteRequest(
    handle: *mut c_void,
    code: u32,
    data_parcel: *mut c_void,
    _reply_parcel: *mut c_void,
) -> i32 {
    do_remote_request(handle, code, data_parcel)
}

#[no_mangle]
pub unsafe extern "C" fn DataSharePubObserverClearCallback(handle: *mut c_void) {
    if !handle.is_null() {
        let h = &mut *(handle as *mut ObserverHandle);
        h.on_change = None;
    }
}

// ===== ProxyDataObserverStub =====

#[no_mangle]
pub unsafe extern "C" fn DataShareProxyDataObserverCreate(
    on_change: OnChangeFn,
    ctx: *mut c_void,
) -> *mut c_void {
    let handle = Box::new(ObserverHandle {
        on_change: Some(on_change),
        ctx,
    });
    Box::into_raw(handle) as *mut c_void
}

#[no_mangle]
pub unsafe extern "C" fn DataShareProxyDataObserverDestroy(handle: *mut c_void) {
    if !handle.is_null() {
        drop(Box::from_raw(handle as *mut ObserverHandle));
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareProxyDataObserverOnRemoteRequest(
    handle: *mut c_void,
    code: u32,
    data_parcel: *mut c_void,
    _reply_parcel: *mut c_void,
) -> i32 {
    do_remote_request(handle, code, data_parcel)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareProxyDataObserverClearCallback(handle: *mut c_void) {
    if !handle.is_null() {
        let h = &mut *(handle as *mut ObserverHandle);
        h.on_change = None;
    }
}
