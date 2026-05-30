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

//! FFI functions for DataShareHelper observer management.
//!
//! Observer callbacks cross the FFI boundary via function pointers.
//! C++ registers a callback function + context pointer; Rust stores them
//! and invokes the callback when data changes occur.

use super::types_ffi::{c_str_to_rust, DataShareHelperHandle};
use crate::helper::datashare_helper::DataShareHelper;

use std::sync::Mutex;

const DATA_SHARE_ERROR: i32 = -1;

/// Observer registry entry: tracks which URIs an observer is registered for.
struct ObserverEntry {
    /// Raw pointer to sptr<ObserverImpl> (stored as u64 for FFI safety).
    observer_id: u64,
    /// URIs this observer is watching.
    uris: Vec<String>,
}

/// Global observer registry. Vec of (key, entry) pairs — small N, avoids HashMap monomorphization.
static OBSERVER_REGISTRY: Mutex<Option<Vec<(u64, ObserverEntry)>>> = Mutex::new(None);

fn with_registry<F, R>(f: F) -> R
where
    F: FnOnce(&mut Vec<(u64, ObserverEntry)>) -> R,
{
    let mut guard = OBSERVER_REGISTRY.lock().unwrap();
    let vec = guard.get_or_insert_with(Vec::new);
    f(vec)
}

/// Register an observer for a URI.
///
/// `observer_id` is a unique identifier for the observer on the C++ side
/// (typically the address of the IDataAbilityObserver sptr).
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperRegisterObserver(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    observer_id: u64,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    (*handle).register_observer(uri, observer_id)
}

/// Unregister an observer for a URI.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperUnregisterObserver(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    observer_id: u64,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    (*handle).unregister_observer(uri, observer_id)
}

/// Notify data change for a URI.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperNotifyChange(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
) {
    if handle.is_null() {
        return;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return,
    };
    (*handle).notify_change(uri);
}

// ---- Static observer ext functions (not tied to a handle) ----
// These correspond to the free functions in datashare_helper.cpp:
// TryRegisterObserverExtInner / TryUnregisterObserverExtInner

/// Register an extended observer with DataObsOption(isSystem, true).
/// This is a static function — not tied to a DataShareHelper handle.
///
/// # Safety
/// `observer_id` must be a valid raw pointer to a C++ IDataAbilityObserver.
#[no_mangle]
pub unsafe extern "C" fn DataShareRegisterObserverExtWithOption(
    uri_ptr: *const u8,
    uri_len: u32,
    observer_id: u64,
    is_descendants: bool,
    is_system: bool,
) -> i32 {
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    ffi_dataobs_mgr::DataObsMgrClient::register_observer_ext_with_option(
        uri,
        observer_id,
        is_descendants,
        is_system,
    )
}

/// Unregister an extended observer with DataObsOption(isSystem, true).
/// This is a static function — not tied to a DataShareHelper handle.
///
/// # Safety
/// `observer_id` must be a valid raw pointer to a C++ IDataAbilityObserver.
#[no_mangle]
pub unsafe extern "C" fn DataShareUnregisterObserverExtWithOption(
    uri_ptr: *const u8,
    uri_len: u32,
    observer_id: u64,
    is_system: bool,
) -> i32 {
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    ffi_dataobs_mgr::DataObsMgrClient::unregister_observer_ext_with_option(
        uri,
        observer_id,
        is_system,
    )
}

/// Notify extended data change with DataObsOption(isSystem, true).
/// This is a static function — not tied to a DataShareHelper handle.
///
/// C++ serializes ChangeInfo into raw bytes; this function forwards them
/// to the C++ DataObsMgrClient wrapper for deserialization and dispatch.
///
/// # Safety
/// `change_info_data` must point to valid serialized ChangeInfo bytes.
#[no_mangle]
pub unsafe extern "C" fn DataShareNotifyChangeExtWithOption(
    change_info_data: *const u8,
    change_info_len: u32,
    is_system: bool,
) -> i32 {
    if change_info_data.is_null() || change_info_len == 0 {
        return DATA_SHARE_ERROR;
    }
    let change_info_slice = std::slice::from_raw_parts(change_info_data, change_info_len as usize);
    ffi_dataobs_mgr::DataObsMgrClient::notify_change_ext_with_option(change_info_slice, is_system)
}

// ---- ObserverImpl map management (moved from C++ ConcurrentMap) ----
// C++ still owns sptr<ObserverImpl> for reference counting, but URI tracking
// and lookup logic lives in Rust.

/// Get or create an observer entry. Returns the existing observer_id if found,
/// or 0 if the entry is new (C++ must create ObserverImpl and call
/// DataShareObserverMapSetId to store the id).
///
/// Also adds the URI to the observer's URI list if not already present.
///
/// # Safety
/// `uri_ptr` must point to valid UTF-8 data of `uri_len` bytes.
#[no_mangle]
pub unsafe extern "C" fn DataShareObserverMapGetOrCreate(
    observer_key: u64,
    uri_ptr: *const u8,
    uri_len: u32,
) -> u64 {
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return 0,
    };
    with_registry(|vec| {
        let entry = if let Some((_, entry)) = vec.iter_mut().find(|(k, _)| *k == observer_key) {
            entry
        } else {
            vec.push((
                observer_key,
                ObserverEntry {
                    observer_id: 0,
                    uris: Vec::new(),
                },
            ));
            &mut vec.last_mut().unwrap().1
        };
        if !entry.uris.iter().any(|u| u == uri) {
            entry.uris.push(uri.to_string());
        }
        entry.observer_id
    })
}

/// Set the observer_id for a newly created observer entry.
/// Called by C++ after creating a new ObserverImpl.
#[no_mangle]
pub unsafe extern "C" fn DataShareObserverMapSetId(observer_key: u64, observer_id: u64) {
    with_registry(|vec| {
        if let Some((_, entry)) = vec.iter_mut().find(|(k, _)| *k == observer_key) {
            entry.observer_id = observer_id;
        }
    });
}

/// Check if an observer is registered for a specific URI.
///
/// # Safety
/// `uri_ptr` must point to valid UTF-8 data of `uri_len` bytes.
#[no_mangle]
pub unsafe extern "C" fn DataShareObserverMapFind(
    observer_key: u64,
    uri_ptr: *const u8,
    uri_len: u32,
) -> bool {
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return false,
    };
    with_registry(|vec| {
        vec.iter()
            .find(|(k, _)| *k == observer_key)
            .map(|(_, entry)| entry.uris.iter().any(|u| u == uri))
            .unwrap_or(false)
    })
}

/// Remove a URI from an observer's list. Returns true if the observer
/// still has remaining URIs (should be kept), false if empty (should be removed).
///
/// # Safety
/// `uri_ptr` must point to valid UTF-8 data of `uri_len` bytes.
#[no_mangle]
pub unsafe extern "C" fn DataShareObserverMapRemoveUri(
    observer_key: u64,
    uri_ptr: *const u8,
    uri_len: u32,
) -> bool {
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return false,
    };
    with_registry(|vec| {
        if let Some(pos) = vec.iter().position(|(k, _)| *k == observer_key) {
            vec[pos].1.uris.retain(|u| u != uri);
            if vec[pos].1.uris.is_empty() {
                vec.swap_remove(pos);
                return false;
            }
            return true;
        }
        false
    })
}

/// Convert AAFwk ChangeType (u32) to DataShareObserver ChangeType (u32).
/// Both use the same numeric values (0=Insert, 1=Delete, 2=Update, 3=Other),
/// but Rust validates the range and clamps invalid values to Invalid(4).
#[no_mangle]
pub unsafe extern "C" fn DataShareObserverConvertChangeType(change_type: u32) -> u32 {
    use datashare_common::observer::ChangeType;
    ChangeType::from_u32(change_type) as u32
}
