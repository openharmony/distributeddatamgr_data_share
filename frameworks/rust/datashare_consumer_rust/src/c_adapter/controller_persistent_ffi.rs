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

//! FFI functions for PersistentDataController.

use std::ffi::c_void;

use ipc::parcel::MsgParcel;

use datashare_common::template::{Template, TemplateId};
use datashare_common::types::{OperationResult, RdbChangeNode};

use super::types_ffi::c_str_to_rust;
use crate::controller::persistent_data::PersistentDataController;

const INVALID_VALUE: i32 = -1;

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlPersistentAddQueryTemplate(
    uri_ptr: *const u8,
    uri_len: u32,
    subscriber_id: i64,
    tpl_parcel: *mut c_void,
) -> i32 {
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return INVALID_VALUE,
    };
    let mut parcel = MsgParcel::from_ptr(tpl_parcel as *mut _);
    let tpl: Template = match parcel.read() {
        Ok(t) => t,
        Err(_) => return INVALID_VALUE,
    };
    let ctl = PersistentDataController::new();
    ctl.add_query_template(uri, subscriber_id, &tpl)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlPersistentDelQueryTemplate(
    uri_ptr: *const u8,
    uri_len: u32,
    subscriber_id: i64,
) -> i32 {
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return INVALID_VALUE,
    };
    let ctl = PersistentDataController::new();
    ctl.del_query_template(uri, subscriber_id)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlPersistentSubscribeRdbData(
    subscriber: u64,
    uris_parcel: *mut c_void,
    template_id_parcel: *mut c_void,
    callback_ctx: *mut c_void,
    callback: Option<unsafe extern "C" fn(*mut c_void, *mut c_void)>,
) -> *mut c_void {
    let mut up = MsgParcel::from_ptr(uris_parcel as *mut _);
    let uris: Vec<String> = up.read().unwrap_or_default();
    let mut tp = MsgParcel::from_ptr(template_id_parcel as *mut _);
    let template_id: TemplateId = match tp.read() {
        Ok(t) => t,
        Err(_) => return std::ptr::null_mut(),
    };
    let cb = callback;
    let ctx = callback_ctx as usize;
    let rust_callback: Box<dyn Fn(&RdbChangeNode) + Send + Sync> =
        Box::new(move |change_node: &RdbChangeNode| {
            if let Some(cb_fn) = cb {
                let mut parcel = MsgParcel::new();
                let _ = parcel.write(change_node);
                let parcel_ptr = parcel.into_raw() as *mut c_void;
                unsafe {
                    cb_fn(ctx as *mut c_void, parcel_ptr);
                }
            }
        });
    let ctl = PersistentDataController::new();
    let results = ctl.subscribe_rdb_data(subscriber, &uris, &template_id, rust_callback);
    operation_results_to_parcel(results)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlPersistentUnSubscribeRdbData(
    subscriber: u64,
    uris_parcel: *mut c_void,
    template_id_parcel: *mut c_void,
) -> *mut c_void {
    let mut up = MsgParcel::from_ptr(uris_parcel as *mut _);
    let uris: Vec<String> = up.read().unwrap_or_default();
    let mut tp = MsgParcel::from_ptr(template_id_parcel as *mut _);
    let template_id: TemplateId = match tp.read() {
        Ok(t) => t,
        Err(_) => return std::ptr::null_mut(),
    };
    let ctl = PersistentDataController::new();
    let results = ctl.unsubscribe_rdb_data(subscriber, &uris, &template_id);
    operation_results_to_parcel(results)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlPersistentEnableSubscribeRdbData(
    subscriber: u64,
    uris_parcel: *mut c_void,
    template_id_parcel: *mut c_void,
) -> *mut c_void {
    let mut up = MsgParcel::from_ptr(uris_parcel as *mut _);
    let uris: Vec<String> = up.read().unwrap_or_default();
    let mut tp = MsgParcel::from_ptr(template_id_parcel as *mut _);
    let template_id: TemplateId = match tp.read() {
        Ok(t) => t,
        Err(_) => return std::ptr::null_mut(),
    };
    let ctl = PersistentDataController::new();
    let results = ctl.enable_subscribe_rdb_data(subscriber, &uris, &template_id);
    operation_results_to_parcel(results)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlPersistentDisableSubscribeRdbData(
    subscriber: u64,
    uris_parcel: *mut c_void,
    template_id_parcel: *mut c_void,
) -> *mut c_void {
    let mut up = MsgParcel::from_ptr(uris_parcel as *mut _);
    let uris: Vec<String> = up.read().unwrap_or_default();
    let mut tp = MsgParcel::from_ptr(template_id_parcel as *mut _);
    let template_id: TemplateId = match tp.read() {
        Ok(t) => t,
        Err(_) => return std::ptr::null_mut(),
    };
    let ctl = PersistentDataController::new();
    let results = ctl.disable_subscribe_rdb_data(subscriber, &uris, &template_id);
    operation_results_to_parcel(results)
}

fn operation_results_to_parcel(results: Vec<OperationResult>) -> *mut c_void {
    let mut parcel = MsgParcel::new();
    if parcel.write(&(results.len() as i32)).is_err() {
        return std::ptr::null_mut();
    }
    for r in &results {
        if parcel.write(r).is_err() {
            return std::ptr::null_mut();
        }
    }
    parcel.into_raw() as *mut c_void
}
