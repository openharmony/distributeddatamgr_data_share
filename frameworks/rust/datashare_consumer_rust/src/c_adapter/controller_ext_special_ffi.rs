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

//! FFI functions for ExtSpecialController.

use std::ffi::c_void;

use ipc::parcel::MsgParcel;

use datashare_common::types::UpdateOperations;
use datashare_common::values_bucket::DataShareValuesBucket;

use super::connection_ffi::connection_arc_clone;
use super::types_ffi::c_str_to_rust;
use crate::controller::ext_special::ExtSpecialController;
use crate::ffi::ability_mgr_ffi;
use crate::ipc::codes::IDataShareCmd;

const INVALID_VALUE: i32 = -1;

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialCreate(
    conn_handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
) -> *mut c_void {
    let conn = match connection_arc_clone(conn_handle) {
        Some(c) => c,
        None => return std::ptr::null_mut(),
    };
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s.to_string(),
        None => return std::ptr::null_mut(),
    };
    let ctl = ExtSpecialController::new(uri, conn);
    Box::into_raw(Box::new(ctl)) as *mut c_void
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialDestroy(handle: *mut c_void) {
    if !handle.is_null() {
        drop(Box::from_raw(handle as *mut ExtSpecialController));
    }
}

unsafe fn rust_string_to_c_alloc(s: &str, out_ptr: *mut *mut u8, out_len: *mut u32) {
    if out_ptr.is_null() || out_len.is_null() {
        return;
    }
    if s.is_empty() {
        *out_ptr = std::ptr::null_mut();
        *out_len = 0;
        return;
    }
    let mut v = s.as_bytes().to_vec();
    v.shrink_to_fit();
    let ptr = v.as_mut_ptr();
    let len = v.len();
    std::mem::forget(v);
    *out_ptr = ptr;
    *out_len = len as u32;
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialOpenFile(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    mode_ptr: *const u8,
    mode_len: u32,
) -> i32 {
    let ctl = &*(handle as *const ExtSpecialController);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return INVALID_VALUE,
    };
    let mode = match c_str_to_rust(mode_ptr, mode_len) {
        Some(s) => s,
        None => return INVALID_VALUE,
    };
    ctl.open_file(uri, mode)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialOpenFileWithErrCode(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    mode_ptr: *const u8,
    mode_len: u32,
    out_err_code: *mut i32,
) -> i32 {
    let ctl = &*(handle as *const ExtSpecialController);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return INVALID_VALUE,
    };
    let mode = match c_str_to_rust(mode_ptr, mode_len) {
        Some(s) => s,
        None => return INVALID_VALUE,
    };
    let (fd, err_code) = ctl.open_file_with_err_code(uri, mode);
    if !out_err_code.is_null() {
        *out_err_code = err_code;
    }
    fd
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialOpenRawFile(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    mode_ptr: *const u8,
    mode_len: u32,
) -> i32 {
    let ctl = &*(handle as *const ExtSpecialController);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return INVALID_VALUE,
    };
    let mode = match c_str_to_rust(mode_ptr, mode_len) {
        Some(s) => s,
        None => return INVALID_VALUE,
    };
    ctl.open_raw_file(uri, mode)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialGetType(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    out_type: *mut *mut u8,
    out_type_len: *mut u32,
) {
    let ctl = &*(handle as *const ExtSpecialController);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => {
            rust_string_to_c_alloc("", out_type, out_type_len);
            return;
        }
    };
    let result = ctl.get_type(uri);
    rust_string_to_c_alloc(&result, out_type, out_type_len);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialBatchInsert(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    values_parcel: *mut c_void,
) -> i32 {
    let ctl = &*(handle as *const ExtSpecialController);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return INVALID_VALUE,
    };
    let mut parcel = MsgParcel::from_ptr(values_parcel as *mut _);
    let count: i32 = parcel.read().unwrap_or(0);
    let mut values = Vec::with_capacity(count.max(0) as usize);
    for _ in 0..count {
        match parcel.read::<DataShareValuesBucket>() {
            Ok(v) => values.push(v),
            Err(_) => return INVALID_VALUE,
        }
    }
    ctl.batch_insert(uri, &values)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialBatchUpdate(
    handle: *mut c_void,
    operations_parcel: *mut c_void,
    results_parcel: *mut c_void,
) -> i32 {
    let ctl = &*(handle as *const ExtSpecialController);
    let mut op_parcel = MsgParcel::from_ptr(operations_parcel as *mut _);
    let operations: UpdateOperations = match op_parcel.read() {
        Ok(o) => o,
        Err(_) => return INVALID_VALUE,
    };
    let mut results = Vec::new();
    let ret = ctl.batch_update_ex(&operations, &mut results);
    let mut res_parcel = MsgParcel::from_ptr(results_parcel as *mut _);
    let _ = res_parcel.write(&(results.len() as i32));
    for r in &results {
        let _ = res_parcel.write(r);
    }
    ret
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialInsertExt(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    value_parcel: *mut c_void,
    out_result: *mut *mut u8,
    out_result_len: *mut u32,
) -> i32 {
    let ctl = &*(handle as *const ExtSpecialController);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return INVALID_VALUE,
    };
    let mut vp = MsgParcel::from_ptr(value_parcel as *mut _);
    let value: DataShareValuesBucket = match vp.read() {
        Ok(v) => v,
        Err(_) => return INVALID_VALUE,
    };
    let (ret, result_str) = ctl.insert_ext(uri, &value);
    rust_string_to_c_alloc(&result_str, out_result, out_result_len);
    ret
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialExecuteBatch(
    handle: *mut c_void,
    statements_parcel: *mut c_void,
    result_parcel: *mut c_void,
) -> i32 {
    let ctl = &*(handle as *const ExtSpecialController);
    let mut sp = MsgParcel::from_ptr(statements_parcel as *mut _);
    let count: i32 = match sp.read() {
        Ok(c) => c,
        Err(_) => return INVALID_VALUE,
    };
    let mut statements = Vec::with_capacity(count.max(0) as usize);
    for _ in 0..count {
        match sp.read::<datashare_common::operation::OperationStatement>() {
            Ok(s) => statements.push(s),
            Err(_) => return INVALID_VALUE,
        }
    }
    let mut result = datashare_common::types::ExecResultSet {
        error_code: datashare_common::types::ExecErrorCode::Success,
        results: Vec::new(),
    };
    let ret = ctl.execute_batch_ex(&statements, &mut result);
    let mut rp = MsgParcel::from_ptr(result_parcel as *mut _);
    let _ = rp.write(&result);
    ret
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialNormalizeUri(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    out_uri: *mut *mut u8,
    out_uri_len: *mut u32,
) {
    let ctl = &*(handle as *const ExtSpecialController);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => {
            rust_string_to_c_alloc("", out_uri, out_uri_len);
            return;
        }
    };
    let result = ctl.normalize_uri(uri);
    rust_string_to_c_alloc(&result, out_uri, out_uri_len);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialDenormalizeUri(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    out_uri: *mut *mut u8,
    out_uri_len: *mut u32,
) {
    let ctl = &*(handle as *const ExtSpecialController);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => {
            rust_string_to_c_alloc("", out_uri, out_uri_len);
            return;
        }
    };
    let result = ctl.denormalize_uri(uri);
    rust_string_to_c_alloc(&result, out_uri, out_uri_len);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialGetFileTypes(
    handle: *mut c_void,
    uri_ptr: *const u8,
    uri_len: u32,
    mime_ptr: *const u8,
    mime_len: u32,
    result_parcel: *mut c_void,
) {
    let ctl = &*(handle as *const ExtSpecialController);
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return,
    };
    let mime = match c_str_to_rust(mime_ptr, mime_len) {
        Some(s) => s,
        None => return,
    };
    let result = ctl.get_file_types(uri, mime);
    let mut rp = MsgParcel::from_ptr(result_parcel as *mut _);
    let _ = rp.write(&result);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareCtlExtSpecialUserDefineFunc(
    handle: *mut c_void,
    data_parcel: *mut c_void,
    reply_parcel: *mut c_void,
) -> i32 {
    let ctl = &*(handle as *const ExtSpecialController);
    let conn = ctl.connection();
    // Ensure proxy is connected (triggers connect if needed).
    if conn.get_data_share_proxy().is_none() {
        return INVALID_VALUE;
    }
    let remote = conn.get_proxy_remote_raw();
    if remote.is_null() {
        return INVALID_VALUE;
    }
    ability_mgr_ffi::DataShareRemoteObjSendRequest(
        remote,
        IDataShareCmd::UserDefineFunc as u32,
        data_parcel,
        reply_parcel,
    )
}
