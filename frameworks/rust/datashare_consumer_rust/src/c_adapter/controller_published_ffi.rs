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

//! FFI functions for PublishedDataController.

use std::ffi::c_void;

use ipc::parcel::MsgParcel;

use datashare_common::types::{Data, OperationResult, PublishedDataChangeNode};

use super::types_ffi::c_str_to_rust;
use crate::controller::published_data::PublishedDataController;

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlPublishedPublish(
    data_parcel: *mut c_void,
    bundle_ptr: *const u8,
    bundle_len: u32,
) -> *mut c_void {
    let bundle = match c_str_to_rust(bundle_ptr, bundle_len) {
        Some(s) => s,
        None => return std::ptr::null_mut(),
    };
    let mut dp = MsgParcel::from_ptr(data_parcel as *mut _);
    let data: Data = match dp.read() {
        Ok(d) => d,
        Err(_) => return std::ptr::null_mut(),
    };
    let ctl = PublishedDataController::new();
    let results = ctl.publish(&data, bundle);
    operation_results_to_parcel(results)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlPublishedGetPublishedData(
    bundle_ptr: *const u8,
    bundle_len: u32,
    out_result_code: *mut i32,
) -> *mut c_void {
    let bundle = match c_str_to_rust(bundle_ptr, bundle_len) {
        Some(s) => s,
        None => return std::ptr::null_mut(),
    };
    let ctl = PublishedDataController::new();
    let (data, result_code) = ctl.get_published_data(bundle);
    if !out_result_code.is_null() {
        *out_result_code = result_code;
    }
    let mut parcel = MsgParcel::new();
    if parcel.write(&data).is_err() {
        return std::ptr::null_mut();
    }
    parcel.into_raw() as *mut c_void
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlPublishedSubscribe(
    subscriber: u64,
    uris_parcel: *mut c_void,
    subscriber_id: i64,
    callback_ctx: *mut c_void,
    callback: Option<unsafe extern "C" fn(*mut c_void, *mut c_void)>,
) -> *mut c_void {
    let mut up = MsgParcel::from_ptr(uris_parcel as *mut _);
    let uris: Vec<String> = up.read().unwrap_or_default();
    let cb = callback;
    let ctx = callback_ctx as usize;
    let rust_callback: Box<dyn Fn(&PublishedDataChangeNode) + Send + Sync> =
        Box::new(move |change_node: &PublishedDataChangeNode| {
            if let Some(cb_fn) = cb {
                let mut parcel = MsgParcel::new();
                let _ = parcel.write(&change_node.owner_bundle_name);
                let _ = parcel.write(&(change_node.datas.len() as i32));
                for item in &change_node.datas {
                    let _ = parcel.write(item);
                }
                let parcel_ptr = parcel.into_raw() as *mut c_void;
                unsafe {
                    cb_fn(ctx as *mut c_void, parcel_ptr);
                }
            }
        });
    let ctl = PublishedDataController::new();
    let results = ctl.subscribe_published_data(subscriber, &uris, subscriber_id, rust_callback);
    operation_results_to_parcel(results)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlPublishedUnSubscribe(
    subscriber: u64,
    uris_parcel: *mut c_void,
    subscriber_id: i64,
) -> *mut c_void {
    let mut up = MsgParcel::from_ptr(uris_parcel as *mut _);
    let uris: Vec<String> = up.read().unwrap_or_default();
    let ctl = PublishedDataController::new();
    let results = ctl.unsubscribe_published_data(subscriber, &uris, subscriber_id);
    operation_results_to_parcel(results)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlPublishedEnable(
    subscriber: u64,
    uris_parcel: *mut c_void,
    subscriber_id: i64,
) -> *mut c_void {
    let mut up = MsgParcel::from_ptr(uris_parcel as *mut _);
    let uris: Vec<String> = up.read().unwrap_or_default();
    let ctl = PublishedDataController::new();
    let results = ctl.enable_subscribe_published_data(subscriber, &uris, subscriber_id);
    operation_results_to_parcel(results)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlPublishedDisable(
    subscriber: u64,
    uris_parcel: *mut c_void,
    subscriber_id: i64,
) -> *mut c_void {
    let mut up = MsgParcel::from_ptr(uris_parcel as *mut _);
    let uris: Vec<String> = up.read().unwrap_or_default();
    let ctl = PublishedDataController::new();
    let results = ctl.disable_subscribe_published_data(subscriber, &uris, subscriber_id);
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
