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

//! FFI bridge for DataShareServiceProxy.
//!
//! Implements the `extern "C"` functions declared in `datashare_proxy_rust_ffi.h`.
//! Each function delegates to `DataShareServiceProxy` methods in `proxy/service_proxy.rs`.

use std::ffi::c_void;

use ipc::parcel::MsgParcel;
use ipc::remote::RemoteObj;

use datashare_common::predicates::DataSharePredicates;
use datashare_common::template::Template;
use datashare_common::types::{
    Data, DataProxyConfig, DataProxyGetResult, DataProxyResult, DataShareProxyData,
    OperationResult, PublishedDataItem,
};
use datashare_common::values_bucket::DataShareValuesBucket;

use crate::controller::general_controller::DatashareBusinessError;
use crate::proxy::service_proxy::DataShareServiceProxy;

const DATA_SHARE_ERROR: i32 = -1;

unsafe fn as_proxy<'a>(handle: *mut c_void) -> Option<&'a DataShareServiceProxy> {
    if handle.is_null() {
        None
    } else {
        Some(&*(handle as *const DataShareServiceProxy))
    }
}

unsafe fn slice_to_str(ptr: *const u8, len: u32) -> &'static str {
    std::str::from_utf8_unchecked(std::slice::from_raw_parts(ptr, len as usize))
}

fn read_string_vec(parcel: &mut MsgParcel) -> Vec<String> {
    let count: i32 = parcel.read().unwrap_or(0);
    (0..count)
        .filter_map(|_| parcel.read::<String>().ok())
        .collect()
}

fn write_operation_results(parcel: &mut MsgParcel, results: &[OperationResult]) {
    let _ = parcel.write(&(results.len() as i32));
    for r in results {
        let _ = parcel.write(r);
    }
}

fn write_proxy_results(parcel: &mut MsgParcel, results: &[DataProxyResult]) {
    let _ = parcel.write(&(results.len() as i32));
    for r in results {
        let _ = parcel.write(r);
    }
}

fn write_proxy_get_results(parcel: &mut MsgParcel, results: &[DataProxyGetResult]) {
    let _ = parcel.write(&(results.len() as i32));
    for r in results {
        let _ = parcel.write(r);
    }
}

// =====================================================================
// Lifecycle
// =====================================================================

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyCreate(remote_obj: *mut c_void) -> *mut c_void {
    if remote_obj.is_null() {
        return std::ptr::null_mut();
    }
    let remote = match RemoteObj::from_ciremote(remote_obj as *mut _) {
        Some(r) => r,
        None => return std::ptr::null_mut(),
    };
    let proxy = DataShareServiceProxy::new(remote);
    Box::into_raw(Box::new(proxy)) as *mut c_void
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyDestroy(handle: *mut c_void) {
    if !handle.is_null() {
        let _ = Box::from_raw(handle as *mut DataShareServiceProxy);
    }
}

// =====================================================================
// CRUD
// =====================================================================

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyInsertEx(
    handle: *mut c_void,
    uri: *const u8,
    uri_len: u32,
    ext_uri: *const u8,
    ext_uri_len: u32,
    value_parcel: *mut c_void,
    out_err: *mut i32,
    out_result: *mut i32,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => {
            *out_err = DATA_SHARE_ERROR;
            *out_result = 0;
            return;
        }
    };
    let uri_str = slice_to_str(uri, uri_len);
    let ext_uri_str = slice_to_str(ext_uri, ext_uri_len);
    let mut parcel = MsgParcel::from_ptr(value_parcel as *mut _);
    let value: DataShareValuesBucket = match parcel.read() {
        Ok(v) => v,
        Err(_) => {
            *out_err = DATA_SHARE_ERROR;
            *out_result = 0;
            return;
        }
    };
    let (err, result) = proxy.insert_ex(uri_str, ext_uri_str, &value);
    *out_err = err;
    *out_result = result;
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyUpdateEx(
    handle: *mut c_void,
    uri: *const u8,
    uri_len: u32,
    ext_uri: *const u8,
    ext_uri_len: u32,
    input_parcel: *mut c_void,
    out_err: *mut i32,
    out_result: *mut i32,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => {
            *out_err = DATA_SHARE_ERROR;
            *out_result = 0;
            return;
        }
    };
    let uri_str = slice_to_str(uri, uri_len);
    let ext_uri_str = slice_to_str(ext_uri, ext_uri_len);
    let mut parcel = MsgParcel::from_ptr(input_parcel as *mut _);
    let predicates: DataSharePredicates = match parcel.read() {
        Ok(v) => v,
        Err(_) => {
            *out_err = DATA_SHARE_ERROR;
            *out_result = 0;
            return;
        }
    };
    let value: DataShareValuesBucket = match parcel.read() {
        Ok(v) => v,
        Err(_) => {
            *out_err = DATA_SHARE_ERROR;
            *out_result = 0;
            return;
        }
    };
    let (err, result) = proxy.update_ex(uri_str, ext_uri_str, &predicates, &value);
    *out_err = err;
    *out_result = result;
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyDeleteEx(
    handle: *mut c_void,
    uri: *const u8,
    uri_len: u32,
    ext_uri: *const u8,
    ext_uri_len: u32,
    preds_parcel: *mut c_void,
    out_err: *mut i32,
    out_result: *mut i32,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => {
            *out_err = DATA_SHARE_ERROR;
            *out_result = 0;
            return;
        }
    };
    let uri_str = slice_to_str(uri, uri_len);
    let ext_uri_str = slice_to_str(ext_uri, ext_uri_len);
    let mut parcel = MsgParcel::from_ptr(preds_parcel as *mut _);
    let predicates: DataSharePredicates = match parcel.read() {
        Ok(v) => v,
        Err(_) => {
            *out_err = DATA_SHARE_ERROR;
            *out_result = 0;
            return;
        }
    };
    let (err, result) = proxy.delete_ex(uri_str, ext_uri_str, &predicates);
    *out_err = err;
    *out_result = result;
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyQuery(
    handle: *mut c_void,
    uri: *const u8,
    uri_len: u32,
    ext_uri: *const u8,
    ext_uri_len: u32,
    input_parcel: *mut c_void,
    out_reply_parcel: *mut *mut c_void,
    out_business_err: *mut i32,
) -> i32 {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return DATA_SHARE_ERROR,
    };
    let uri_str = slice_to_str(uri, uri_len);
    let ext_uri_str = slice_to_str(ext_uri, ext_uri_len);
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let predicates: DataSharePredicates = match inp.read() {
        Ok(v) => v,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let columns: Vec<String> = read_string_vec(&mut inp);

    let mut business_error = DatashareBusinessError::new();
    let reply_opt = proxy.query(
        uri_str,
        ext_uri_str,
        &predicates,
        &columns,
        &mut business_error,
    );
    *out_business_err = business_error.get_code();

    if let Some(reply) = reply_opt {
        *out_reply_parcel = reply.into_raw() as *mut c_void;
        0
    } else {
        *out_reply_parcel = std::ptr::null_mut();
        business_error.get_code()
    }
}
// =====================================================================
// Template Management
// =====================================================================

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyAddQueryTemplate(
    handle: *mut c_void,
    uri: *const u8,
    uri_len: u32,
    subscriber_id: i64,
    tpl_parcel: *mut c_void,
) -> i32 {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return DATA_SHARE_ERROR,
    };
    let uri_str = slice_to_str(uri, uri_len);
    let mut parcel = MsgParcel::from_ptr(tpl_parcel as *mut _);
    let update: String = parcel.read().unwrap_or_default();
    let pred_count: i32 = parcel.read().unwrap_or(0);
    let mut predicates = Vec::new();
    for _ in 0..pred_count {
        let key: String = match parcel.read() {
            Ok(k) => k,
            Err(_) => break,
        };
        let select_sql: String = match parcel.read() {
            Ok(s) => s,
            Err(_) => break,
        };
        predicates.push(datashare_common::template::PredicateTemplateNode { key, select_sql });
    }
    let scheduler: String = parcel.read().unwrap_or_default();
    let tpl = Template {
        update,
        predicates,
        scheduler,
    };
    proxy.add_query_template(uri_str, subscriber_id, &tpl)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyDelQueryTemplate(
    handle: *mut c_void,
    uri: *const u8,
    uri_len: u32,
    subscriber_id: i64,
) -> i32 {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return DATA_SHARE_ERROR,
    };
    proxy.del_query_template(slice_to_str(uri, uri_len), subscriber_id)
}

// =====================================================================
// Publish / GetPublishedData
// =====================================================================

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyPublish(
    handle: *mut c_void,
    _bundle: *const u8,
    _bundle_len: u32,
    data_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(data_parcel as *mut _);
    let datas_count: i32 = inp.read().unwrap_or(0);
    let mut datas = Vec::new();
    for _ in 0..datas_count {
        if let Ok(item) = inp.read::<PublishedDataItem>() {
            datas.push(item);
        }
    }
    let version: i32 = inp.read().unwrap_or(0);
    let bundle_name: String = inp.read().unwrap_or_default();
    let pub_data = Data { datas, version };

    let results = proxy.publish(&pub_data, &bundle_name);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyGetPublishedData(
    handle: *mut c_void,
    bundle: *const u8,
    bundle_len: u32,
    result_parcel: *mut c_void,
    out_result_code: *mut i32,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => {
            *out_result_code = DATA_SHARE_ERROR;
            return;
        }
    };
    let bundle_name = slice_to_str(bundle, bundle_len);
    let (data, result_code) = proxy.get_published_data(bundle_name);
    *out_result_code = result_code;
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    let _ = out.write(&(data.datas.len() as i32));
    for item in &data.datas {
        let _ = out.write(item);
    }
    let _ = out.write(&result_code);
}

// =====================================================================
// RDB Subscription
// =====================================================================

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxySubscribeRdbData(
    handle: *mut c_void,
    input_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let uris = read_string_vec(&mut inp);
    let subscriber_id: i64 = inp.read().unwrap_or(0);
    let bundle_name: String = inp.read().unwrap_or_default();
    // 读取 subscribeOption (map<string, bool>)，传递给 proxy
    let option_count: i32 = inp.read().unwrap_or(0);
    for _ in 0..option_count {
        let _key: String = inp.read().unwrap_or_default();
        let _val: bool = inp.read().unwrap_or(false);
    }
    let observer = match inp.read_remote() {
        Ok(r) => r,
        Err(_) => return,
    };
    let results = proxy.subscribe_rdb_data(&uris, subscriber_id, &bundle_name, &observer);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyUnSubscribeRdbData(
    handle: *mut c_void,
    input_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let uris = read_string_vec(&mut inp);
    let subscriber_id: i64 = inp.read().unwrap_or(0);
    let bundle_name: String = inp.read().unwrap_or_default();
    let results = proxy.unsubscribe_rdb_data(&uris, subscriber_id, &bundle_name);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyEnableSubscribeRdbData(
    handle: *mut c_void,
    input_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let uris = read_string_vec(&mut inp);
    let subscriber_id: i64 = inp.read().unwrap_or(0);
    let bundle_name: String = inp.read().unwrap_or_default();
    let results = proxy.enable_subscribe_rdb_data(&uris, subscriber_id, &bundle_name);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyDisableSubscribeRdbData(
    handle: *mut c_void,
    input_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let uris = read_string_vec(&mut inp);
    let subscriber_id: i64 = inp.read().unwrap_or(0);
    let bundle_name: String = inp.read().unwrap_or_default();
    let results = proxy.disable_subscribe_rdb_data(&uris, subscriber_id, &bundle_name);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
}

// =====================================================================
// Published Data Subscription
// =====================================================================

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxySubscribePublishedData(
    handle: *mut c_void,
    input_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let uris = read_string_vec(&mut inp);
    let subscriber_id: i64 = inp.read().unwrap_or(0);
    let observer = match inp.read_remote() {
        Ok(r) => r,
        Err(_) => return,
    };
    let results = proxy.subscribe_published_data(&uris, subscriber_id, &observer);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyUnSubscribePublishedData(
    handle: *mut c_void,
    input_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let uris = read_string_vec(&mut inp);
    let subscriber_id: i64 = inp.read().unwrap_or(0);
    let results = proxy.unsubscribe_published_data(&uris, subscriber_id);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyEnableSubscribePublishedData(
    handle: *mut c_void,
    input_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let uris = read_string_vec(&mut inp);
    let subscriber_id: i64 = inp.read().unwrap_or(0);
    let results = proxy.enable_subscribe_published_data(&uris, subscriber_id);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyDisableSubscribePublishedData(
    handle: *mut c_void,
    input_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let uris = read_string_vec(&mut inp);
    let subscriber_id: i64 = inp.read().unwrap_or(0);
    let results = proxy.disable_subscribe_published_data(&uris, subscriber_id);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
}

// =====================================================================
// Notify / Silent Switch / Observer Registration
// =====================================================================

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyNotify(
    handle: *mut c_void,
    uri: *const u8,
    uri_len: u32,
) {
    if let Some(proxy) = as_proxy(handle) {
        proxy.notify(slice_to_str(uri, uri_len));
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxySetSilentSwitch(
    handle: *mut c_void,
    uri: *const u8,
    uri_len: u32,
    enable: bool,
) -> i32 {
    match as_proxy(handle) {
        Some(proxy) => proxy.set_silent_switch(slice_to_str(uri, uri_len), enable),
        None => DATA_SHARE_ERROR,
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyGetSilentProxyStatus(
    handle: *mut c_void,
    uri: *const u8,
    uri_len: u32,
) -> i32 {
    match as_proxy(handle) {
        Some(proxy) => proxy.get_silent_proxy_status(slice_to_str(uri, uri_len)),
        None => DATA_SHARE_ERROR,
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyRegisterObserver(
    handle: *mut c_void,
    input_parcel: *mut c_void,
) -> i32 {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return DATA_SHARE_ERROR,
    };
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let uri: String = inp.read().unwrap_or_default();
    let observer = match inp.read_remote() {
        Ok(r) => r,
        Err(_) => return DATA_SHARE_ERROR,
    };
    proxy.register_observer(&uri, &observer)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyUnRegisterObserver(
    handle: *mut c_void,
    input_parcel: *mut c_void,
) -> i32 {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return DATA_SHARE_ERROR,
    };
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let uri: String = inp.read().unwrap_or_default();
    let observer = match inp.read_remote() {
        Ok(r) => r,
        Err(_) => return DATA_SHARE_ERROR,
    };
    proxy.unregister_observer(&uri, &observer)
}

// =====================================================================
// Proxy Data Operations
// =====================================================================

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyPublishProxyData(
    handle: *mut c_void,
    data_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(data_parcel as *mut _);
    let count: i32 = inp.read().unwrap_or(0);
    let mut proxy_datas = Vec::new();
    for _ in 0..count {
        if let Ok(item) = inp.read::<DataShareProxyData>() {
            proxy_datas.push(item);
        }
    }
    let proxy_config: DataProxyConfig = match inp.read() {
        Ok(c) => c,
        Err(_) => return,
    };
    let results = proxy.publish_proxy_data(&proxy_datas, &proxy_config);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_proxy_results(&mut out, &results);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyDeleteProxyData(
    handle: *mut c_void,
    data_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(data_parcel as *mut _);
    let uris = read_string_vec(&mut inp);
    let proxy_config: DataProxyConfig = match inp.read() {
        Ok(c) => c,
        Err(_) => return,
    };
    let results = proxy.delete_proxy_data(&uris, &proxy_config);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_proxy_results(&mut out, &results);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyGetProxyData(
    handle: *mut c_void,
    data_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(data_parcel as *mut _);
    let uris = read_string_vec(&mut inp);
    let proxy_config: DataProxyConfig = match inp.read() {
        Ok(c) => c,
        Err(_) => return,
    };
    let results = proxy.get_proxy_data(&uris, &proxy_config);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_proxy_get_results(&mut out, &results);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxySubscribeProxyData(
    handle: *mut c_void,
    data_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(data_parcel as *mut _);
    let uris = read_string_vec(&mut inp);
    let observer = match inp.read_remote() {
        Ok(r) => r,
        Err(_) => {
            let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
            write_proxy_results(&mut out, &[]);
            return;
        }
    };
    let results = proxy.subscribe_proxy_data(&uris, &observer);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_proxy_results(&mut out, &results);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyUnsubscribeProxyData(
    handle: *mut c_void,
    data_parcel: *mut c_void,
    result_parcel: *mut c_void,
) {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return,
    };
    let mut inp = MsgParcel::from_ptr(data_parcel as *mut _);
    let uris = read_string_vec(&mut inp);
    let results = proxy.unsubscribe_proxy_data(&uris);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_proxy_results(&mut out, &results);
}

// =====================================================================
// Connection Interface Info
// =====================================================================

#[no_mangle]
pub unsafe extern "C" fn DataShareSvcProxyGetConnectionInterfaceInfo(
    handle: *mut c_void,
    sa_id: i32,
    wait_time: u32,
    result_parcel: *mut c_void,
) -> i32 {
    let proxy = match as_proxy(handle) {
        Some(p) => p,
        None => return DATA_SHARE_ERROR,
    };
    match proxy.get_connection_interface_info(sa_id, wait_time) {
        Some((err_code, code, descriptor)) => {
            let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
            let _ = out.write(&err_code);
            let _ = out.write_string16(&descriptor);
            let _ = out.write(&code);
            0
        }
        None => DATA_SHARE_ERROR,
    }
}
