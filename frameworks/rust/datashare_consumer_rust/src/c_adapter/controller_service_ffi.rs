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

//! FFI functions for GeneralControllerServiceImpl.
//!
//! Corresponds to C++ `general_controller_service_impl.cpp` `#ifdef DATASHARE_USE_RUST_IMPL` path.

use std::ffi::c_void;

use ipc::parcel::MsgParcel;

use datashare_common::predicates::DataSharePredicates;
use datashare_common::types::DataShareOption;
use datashare_common::values_bucket::DataShareValuesBucket;

use super::types_ffi::c_str_to_rust;
use crate::controller::general_controller::{DatashareBusinessError, GeneralController};
use crate::controller::service_impl::GeneralControllerServiceImpl;

const DATA_SHARE_ERROR: i32 = -1;

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceCreate(
    ext_uri_ptr: *const u8,
    ext_uri_len: u32,
) -> *mut c_void {
    let ext_uri = match c_str_to_rust(ext_uri_ptr, ext_uri_len) {
        Some(s) => s.to_string(),
        None => return std::ptr::null_mut(),
    };
    let ctl = GeneralControllerServiceImpl::new(ext_uri);
    Box::into_raw(Box::new(ctl)) as *mut c_void
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceDestroy(handle: *mut c_void) {
    if !handle.is_null() {
        drop(Box::from_raw(handle as *mut GeneralControllerServiceImpl));
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceInsert(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    value_parcel: *mut c_void,
) -> i32 {
    let ctl = &*(handle as *const GeneralControllerServiceImpl);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut parcel = MsgParcel::from_ptr(value_parcel as *mut _);
    let value: DataShareValuesBucket = match parcel.read() {
        Ok(v) => v,
        Err(_) => return DATA_SHARE_ERROR,
    };
    ctl.insert(uri, &value)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceUpdate(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    pred_parcel: *mut c_void,
    value_parcel: *mut c_void,
) -> i32 {
    let ctl = &*(handle as *const GeneralControllerServiceImpl);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut pp = MsgParcel::from_ptr(pred_parcel as *mut _);
    let predicates: DataSharePredicates = match pp.read() {
        Ok(p) => p,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let mut vp = MsgParcel::from_ptr(value_parcel as *mut _);
    let value: DataShareValuesBucket = match vp.read() {
        Ok(v) => v,
        Err(_) => return DATA_SHARE_ERROR,
    };
    ctl.update(uri, &predicates, &value)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceDelete(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    pred_parcel: *mut c_void,
) -> i32 {
    let ctl = &*(handle as *const GeneralControllerServiceImpl);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut pp = MsgParcel::from_ptr(pred_parcel as *mut _);
    let predicates: DataSharePredicates = match pp.read() {
        Ok(p) => p,
        Err(_) => return DATA_SHARE_ERROR,
    };
    ctl.delete(uri, &predicates)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceQuery(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    pred_parcel: *mut c_void,
    columns_parcel: *mut c_void,
    timeout: i32,
    out_err_code: *mut i32,
) -> *mut c_void {
    let ctl = &*(handle as *const GeneralControllerServiceImpl);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return std::ptr::null_mut(),
    };
    let mut pp = MsgParcel::from_ptr(pred_parcel as *mut _);
    let predicates: DataSharePredicates = match pp.read() {
        Ok(p) => p,
        Err(_) => return std::ptr::null_mut(),
    };
    let mut cp = MsgParcel::from_ptr(columns_parcel as *mut _);
    let columns: Vec<String> = cp.read().unwrap_or_default();
    let option = DataShareOption {
        timeout: timeout as u32,
    };
    let mut business_error = DatashareBusinessError::new();
    let result = ctl.query_raw(uri, &predicates, &columns, &mut business_error, &option);
    if !out_err_code.is_null() {
        *out_err_code = business_error.get_code();
    }
    match result {
        Some(parcel) => parcel.into_raw() as *mut c_void,
        None => std::ptr::null_mut(),
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceInsertEx(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    value_parcel: *mut c_void,
    out_second: *mut i32,
) -> i32 {
    let ctl = &*(handle as *const GeneralControllerServiceImpl);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut vp = MsgParcel::from_ptr(value_parcel as *mut _);
    let value: DataShareValuesBucket = match vp.read() {
        Ok(v) => v,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let (first, second) = ctl.insert_ex(uri, &value);
    if !out_second.is_null() {
        *out_second = second;
    }
    first
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceUpdateEx(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    pred_parcel: *mut c_void,
    value_parcel: *mut c_void,
    out_second: *mut i32,
) -> i32 {
    let ctl = &*(handle as *const GeneralControllerServiceImpl);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut pp = MsgParcel::from_ptr(pred_parcel as *mut _);
    let predicates: DataSharePredicates = match pp.read() {
        Ok(p) => p,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let mut vp = MsgParcel::from_ptr(value_parcel as *mut _);
    let value: DataShareValuesBucket = match vp.read() {
        Ok(v) => v,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let (first, second) = ctl.update_ex(uri, &predicates, &value);
    if !out_second.is_null() {
        *out_second = second;
    }
    first
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceDeleteEx(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    pred_parcel: *mut c_void,
    out_second: *mut i32,
) -> i32 {
    let ctl = &*(handle as *const GeneralControllerServiceImpl);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut pp = MsgParcel::from_ptr(pred_parcel as *mut _);
    let predicates: DataSharePredicates = match pp.read() {
        Ok(p) => p,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let (first, second) = ctl.delete_ex(uri, &predicates);
    if !out_second.is_null() {
        *out_second = second;
    }
    first
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceRegisterObserver(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    observer_id: u64,
) -> i32 {
    let ctl = &*(handle as *const GeneralControllerServiceImpl);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    ctl.register_observer(uri, observer_id)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceUnregisterObserver(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    observer_id: u64,
) -> i32 {
    let ctl = &*(handle as *const GeneralControllerServiceImpl);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    ctl.unregister_observer(uri, observer_id)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceNotifyChange(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
) {
    let ctl = &*(handle as *const GeneralControllerServiceImpl);
    if let Some(uri) = c_str_to_rust(uri_ptr, uri_len) {
        ctl.notify_change(uri);
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceSetExtUri(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
) -> i32 {
    let ctl = &*(handle as *const GeneralControllerServiceImpl);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    ctl.set_ext_uri(uri)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlServiceSetRegisterCallback(handle: *mut c_void) {
    if handle.is_null() {
        return;
    }
    let ctl = &*(handle as *const GeneralControllerServiceImpl);
    ctl.set_register_callback();
}
