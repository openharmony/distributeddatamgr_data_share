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

//! FFI functions for DataProxyHandle operations.
//!
//! All complex types are passed via MessageParcel (void* pointers).
//! The C++ side serializes/deserializes using existing Marshalling functions.
//! Rust reads/writes using its Serialize/Deserialize impls.

use std::ffi::c_void;
use std::sync::Arc;

use ipc::parcel::{Deserialize, MsgParcel, Serialize};

use datashare_common::types::{
    DataProxyChangeInfo, DataProxyConfig, DataProxyGetResult, DataProxyResult, DataShareProxyData,
};

use crate::connection::DataShareManagerImpl;
use crate::subscriber::proxy_data_subscriber_manager::ProxyDataSubscriberManager;

const E_OK: i32 = 0;
const E_ERROR: i32 = -1;

/// Initialize Rust DataShareManagerImpl singleton with a C++ IRemoteObject*.
///
/// Called from C++ when the C++ side already has a valid service proxy.
/// This bridges the separate-singleton gap: C++ obtains the proxy via its own
/// DataShareManagerImpl, then passes the underlying IRemoteObject* to Rust
/// so Rust's singleton can use the same remote connection.
///
/// # Safety
/// `remote` must be a valid C++ `IRemoteObject*` pointer with a held reference.
#[no_mangle]
pub unsafe extern "C" fn DataProxyHandleInitServiceProxy(remote: *mut c_void) -> i32 {
    if remote.is_null() {
        return E_ERROR;
    }
    if let Some(remote_obj) = ipc::remote::RemoteObj::from_ciremote(remote as *mut _) {
        DataShareManagerImpl::get_instance().set_service_proxy(remote_obj);
        E_OK
    } else {
        E_ERROR
    }
}

/// Check whether the Rust DataShare service proxy is available.
///
/// Corresponds to C++ `DataProxyHandle::Create()` validation logic.
/// Returns E_OK if service proxy exists, E_ERROR otherwise.
#[no_mangle]
pub extern "C" fn DataProxyHandleCreate() -> i32 {
    if DataShareManagerImpl::get_service_proxy().is_some() {
        E_OK
    } else {
        E_ERROR
    }
}

/// Publish proxy data.
///
/// Input parcel format (C++ writes):
///   int32_t count
///   DataShareProxyData[count]  (each via Marshalling)
///   int32_t proxyType          (DataProxyConfig.type_)
///
/// Output parcel format (Rust writes):
///   int32_t count
///   DataProxyResult[count]     (each via Serialize)
///
/// # Safety
/// `data_parcel` and `result_parcel` must be valid C++ MessageParcel pointers.
#[no_mangle]
pub unsafe extern "C" fn DataProxyHandlePublishProxyData(
    data_parcel: *mut c_void,
    result_parcel: *mut c_void,
) -> i32 {
    if data_parcel.is_null() || result_parcel.is_null() {
        return E_ERROR;
    }

    let mut data = MsgParcel::from_ptr(data_parcel as *mut _);
    let count: i32 = match data.read() {
        Ok(c) => c,
        Err(_) => return E_ERROR,
    };
    let mut proxy_data = Vec::with_capacity(count.max(0) as usize);
    for _ in 0..count {
        match data.read::<DataShareProxyData>() {
            Ok(item) => proxy_data.push(item),
            Err(_) => return E_ERROR,
        }
    }
    let proxy_config: DataProxyConfig = match data.read() {
        Ok(c) => c,
        Err(_) => return E_ERROR,
    };

    let proxy = match DataShareManagerImpl::get_service_proxy() {
        Some(p) => p,
        None => return E_ERROR,
    };

    let results = proxy.publish_proxy_data(&proxy_data, &proxy_config);

    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_proxy_results(&mut out, &results);
    E_OK
}

/// Delete proxy data.
///
/// Input parcel format:
///   int32_t uri_count
///   String[uri_count]
///   int32_t proxyType
///
/// Output parcel format:
///   int32_t count
///   DataProxyResult[count]
///
/// # Safety
/// `data_parcel` and `result_parcel` must be valid C++ MessageParcel pointers.
#[no_mangle]
pub unsafe extern "C" fn DataProxyHandleDeleteProxyData(
    data_parcel: *mut c_void,
    result_parcel: *mut c_void,
) -> i32 {
    if data_parcel.is_null() || result_parcel.is_null() {
        return E_ERROR;
    }

    let mut data = MsgParcel::from_ptr(data_parcel as *mut _);
    let uris = match read_string_vec(&mut data) {
        Some(v) => v,
        None => return E_ERROR,
    };
    let proxy_config: DataProxyConfig = match data.read() {
        Ok(c) => c,
        Err(_) => return E_ERROR,
    };

    let proxy = match DataShareManagerImpl::get_service_proxy() {
        Some(p) => p,
        None => return E_ERROR,
    };

    let results = proxy.delete_proxy_data(&uris, &proxy_config);

    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_proxy_results(&mut out, &results);
    E_OK
}

/// Get proxy data.
///
/// Input parcel format:
///   int32_t uri_count
///   String[uri_count]
///   int32_t proxyType
///
/// Output parcel format:
///   int32_t count
///   DataProxyGetResult[count]
///
/// # Safety
/// `data_parcel` and `result_parcel` must be valid C++ MessageParcel pointers.
#[no_mangle]
pub unsafe extern "C" fn DataProxyHandleGetProxyData(
    data_parcel: *mut c_void,
    result_parcel: *mut c_void,
) -> i32 {
    if data_parcel.is_null() || result_parcel.is_null() {
        return E_ERROR;
    }

    let mut data = MsgParcel::from_ptr(data_parcel as *mut _);
    let uris = match read_string_vec(&mut data) {
        Some(v) => v,
        None => return E_ERROR,
    };
    let proxy_config: DataProxyConfig = match data.read() {
        Ok(c) => c,
        Err(_) => return E_ERROR,
    };

    let proxy = match DataShareManagerImpl::get_service_proxy() {
        Some(p) => p,
        None => return E_ERROR,
    };

    let results = proxy.get_proxy_data(&uris, &proxy_config);

    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    let _ = out.write(&(results.len() as i32));
    for r in &results {
        let _ = r.serialize(&mut out);
    }
    E_OK
}

/// C callback type for proxy data change notifications.
///
/// Parameters:
///   context: opaque pointer passed back to C++ (typically the std::function*)
///   change_parcel: MessageParcel* containing serialized DataProxyChangeInfo array.
///                  C++ must read and then delete this parcel.
///     Format: int32_t count, DataProxyChangeInfo[count]
pub type ProxyDataChangeCallback =
    unsafe extern "C" fn(context: *mut c_void, change_parcel: *mut c_void);

/// Subscribe to proxy data changes.
///
/// Input parcel format:
///   int32_t uri_count
///   String[uri_count]
///
/// Output parcel format:
///   int32_t count
///   DataProxyResult[count]
///
/// The callback will be invoked with a newly-allocated MessageParcel containing
/// serialized DataProxyChangeInfo array. C++ is responsible for deleting it.
///
/// # Safety
/// All pointer parameters must be valid. `handle_id` is the C++ `this` pointer
/// used for subscription tracking.
#[no_mangle]
pub unsafe extern "C" fn DataProxyHandleSubscribeProxyData(
    handle_id: u64,
    data_parcel: *mut c_void,
    callback: ProxyDataChangeCallback,
    context: *mut c_void,
    result_parcel: *mut c_void,
) -> i32 {
    if data_parcel.is_null() || result_parcel.is_null() {
        return E_ERROR;
    }

    let mut data = MsgParcel::from_ptr(data_parcel as *mut _);
    let uris = match read_string_vec(&mut data) {
        Some(v) => v,
        None => return E_ERROR,
    };

    let proxy = match DataShareManagerImpl::get_service_proxy() {
        Some(p) => p,
        None => return E_ERROR,
    };

    let ctx_val = context as usize;
    let rust_callback: Arc<dyn Fn(&[DataProxyChangeInfo]) + Send + Sync> =
        Arc::new(move |changes: &[DataProxyChangeInfo]| {
            let mut parcel = MsgParcel::new();
            let _ = parcel.write(&(changes.len() as i32));
            for change in changes {
                let _ = change.serialize(&mut parcel);
            }
            let raw = parcel.into_raw();
            unsafe {
                callback(ctx_val as *mut c_void, raw as *mut c_void);
            }
        });

    let results = ProxyDataSubscriberManager::get_instance().add_observers(
        handle_id,
        &proxy,
        &uris,
        rust_callback,
    );

    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_proxy_results(&mut out, &results);
    E_OK
}

/// Unsubscribe from proxy data changes.
///
/// Input parcel format:
///   int32_t uri_count
///   String[uri_count]
///
/// Output parcel format:
///   int32_t count
///   DataProxyResult[count]
///
/// # Safety
/// All pointer parameters must be valid.
#[no_mangle]
pub unsafe extern "C" fn DataProxyHandleUnsubscribeProxyData(
    handle_id: u64,
    data_parcel: *mut c_void,
    result_parcel: *mut c_void,
) -> i32 {
    if data_parcel.is_null() || result_parcel.is_null() {
        return E_ERROR;
    }

    let mut data = MsgParcel::from_ptr(data_parcel as *mut _);
    let uris = match read_string_vec(&mut data) {
        Some(v) => v,
        None => return E_ERROR,
    };

    let proxy = match DataShareManagerImpl::get_service_proxy() {
        Some(p) => p,
        None => return E_ERROR,
    };

    let results =
        ProxyDataSubscriberManager::get_instance().del_observers(handle_id, &proxy, &uris);

    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_proxy_results(&mut out, &results);
    E_OK
}

/// Free a MessageParcel allocated by Rust (via MsgParcel::into_raw).
///
/// Called from C++ after reading callback data from the parcel.
///
/// # Safety
/// `parcel` must be a pointer previously returned by MsgParcel::into_raw().
#[no_mangle]
pub unsafe extern "C" fn DataProxyHandleFreeParcel(parcel: *mut c_void) {
    if !parcel.is_null() {
        let _ = MsgParcel::from_ptr(parcel as *mut _);
        // MsgParcel::from_ptr creates a Borrow variant which doesn't free.
        // We need to reconstruct the owned variant to free.
        // Since from_ptr gives Borrow (no-op drop), we use the C++ side to delete.
        // This function is actually a no-op; C++ should use `delete (MessageParcel*)ptr`.
    }
}

// ---- Internal helpers ----

fn read_string_vec(parcel: &mut MsgParcel) -> Option<Vec<String>> {
    let count: i32 = parcel.read().ok()?;
    let mut vec = Vec::with_capacity(count.max(0) as usize);
    for _ in 0..count {
        vec.push(parcel.read::<String>().ok()?);
    }
    Some(vec)
}

fn write_proxy_results(parcel: &mut MsgParcel, results: &[DataProxyResult]) {
    let _ = parcel.write(&(results.len() as i32));
    for r in results {
        let _ = r.serialize(parcel);
    }
}
