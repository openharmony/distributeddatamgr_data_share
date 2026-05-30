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

//! Parcel-bridged FFI for DataShareHelper Step 2c.
//!
//! Complex types (DataShareValuesBucket, DataSharePredicates, Template, Data)
//! are passed via C++ MessageParcel pointers. C++ serializes inputs with
//! `ITypesUtil::Marshalling`; Rust reads via `Deserialize`. Outputs flow back
//! the same way.

use std::ffi::c_void;
use std::sync::Arc;

use ipc::parcel::{Deserialize, MsgParcel, Serialize};

use datashare_common::operation::OperationStatement;
use datashare_common::predicates::DataSharePredicates;
use datashare_common::template::{Template, TemplateId};
use datashare_common::types::{
    BatchUpdateResult, Data, DataShareOption, ExecErrorCode, ExecResultSet, OperationResult,
    RdbChangeNode, UpdateOperations,
};
use datashare_common::values_bucket::DataShareValuesBucket;

use super::types_ffi::{c_str_to_rust, rust_str_to_c, CResultPair, DataShareHelperHandle};
use crate::controller::general_controller::DatashareBusinessError;
use crate::helper::datashare_helper::DataShareHelper;

const DATA_SHARE_ERROR: i32 = -1;
const E_OK: i32 = 0;

// ---------------- internal helpers ----------------

unsafe fn read_string_vec(parcel: &mut MsgParcel) -> Option<Vec<String>> {
    let count: i32 = parcel.read().ok()?;
    let mut v = Vec::with_capacity(count.max(0) as usize);
    for _ in 0..count {
        v.push(parcel.read::<String>().ok()?);
    }
    Some(v)
}

unsafe fn write_operation_results(parcel: &mut MsgParcel, results: &[OperationResult]) {
    let _ = parcel.write(&(results.len() as i32));
    for r in results {
        let _ = r.serialize(parcel);
    }
}

// =====================================================================
// Group 1: CRUD (Parcel-based)
// =====================================================================

/// Insert a row.
/// bucket_parcel: DataShareValuesBucket
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperInsertParcel(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    bucket_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || bucket_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut p = MsgParcel::from_ptr(bucket_parcel as *mut _);
    let bucket: DataShareValuesBucket = match p.read() {
        Ok(b) => b,
        Err(_) => return DATA_SHARE_ERROR,
    };
    (*handle).insert(uri, &bucket)
}

/// Extension insert returning string result.
/// bucket_parcel: DataShareValuesBucket
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperInsertExtParcel(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    bucket_parcel: *mut c_void,
    out_buf: *mut u8,
    buf_len: u32,
    out_len: *mut u32,
) -> i32 {
    if handle.is_null() || bucket_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut p = MsgParcel::from_ptr(bucket_parcel as *mut _);
    let bucket: DataShareValuesBucket = match p.read() {
        Ok(b) => b,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let (err_code, result_str) = (*handle).insert_ext_str(uri, &bucket);
    if !out_buf.is_null() {
        rust_str_to_c(&result_str, out_buf, buf_len, out_len);
    } else if !out_len.is_null() {
        *out_len = result_str.len() as u32;
    }
    err_code
}

/// Update rows.
/// input_parcel: DataSharePredicates + DataShareValuesBucket
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperUpdateParcel(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    input_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || input_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut p = MsgParcel::from_ptr(input_parcel as *mut _);
    let preds: DataSharePredicates = match p.read() {
        Ok(v) => v,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let bucket: DataShareValuesBucket = match p.read() {
        Ok(v) => v,
        Err(_) => return DATA_SHARE_ERROR,
    };
    (*handle).update(uri, &preds, &bucket)
}

/// Delete rows.
/// preds_parcel: DataSharePredicates
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperDeleteParcel(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    preds_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || preds_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut p = MsgParcel::from_ptr(preds_parcel as *mut _);
    let preds: DataSharePredicates = match p.read() {
        Ok(v) => v,
        Err(_) => return DATA_SHARE_ERROR,
    };
    (*handle).delete(uri, &preds)
}

/// Extended insert. Returns (err_code, value).
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperInsertExParcel(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    bucket_parcel: *mut c_void,
) -> CResultPair {
    if handle.is_null() || bucket_parcel.is_null() {
        return CResultPair {
            err_code: DATA_SHARE_ERROR,
            value: 0,
        };
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => {
            return CResultPair {
                err_code: DATA_SHARE_ERROR,
                value: 0,
            }
        }
    };
    let mut p = MsgParcel::from_ptr(bucket_parcel as *mut _);
    let bucket: DataShareValuesBucket = match p.read() {
        Ok(v) => v,
        Err(_) => {
            return CResultPair {
                err_code: DATA_SHARE_ERROR,
                value: 0,
            }
        }
    };
    let (err_code, value) = (*handle).insert_ex(uri, &bucket);
    CResultPair { err_code, value }
}

/// Extended update.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperUpdateExParcel(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    input_parcel: *mut c_void,
) -> CResultPair {
    if handle.is_null() || input_parcel.is_null() {
        return CResultPair {
            err_code: DATA_SHARE_ERROR,
            value: 0,
        };
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => {
            return CResultPair {
                err_code: DATA_SHARE_ERROR,
                value: 0,
            }
        }
    };
    let mut p = MsgParcel::from_ptr(input_parcel as *mut _);
    let preds: DataSharePredicates = match p.read() {
        Ok(v) => v,
        Err(_) => {
            return CResultPair {
                err_code: DATA_SHARE_ERROR,
                value: 0,
            }
        }
    };
    let bucket: DataShareValuesBucket = match p.read() {
        Ok(v) => v,
        Err(_) => {
            return CResultPair {
                err_code: DATA_SHARE_ERROR,
                value: 0,
            }
        }
    };
    let (err_code, value) = (*handle).update_ex(uri, &preds, &bucket);
    CResultPair { err_code, value }
}

/// Extended delete.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperDeleteExParcel(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    preds_parcel: *mut c_void,
) -> CResultPair {
    if handle.is_null() || preds_parcel.is_null() {
        return CResultPair {
            err_code: DATA_SHARE_ERROR,
            value: 0,
        };
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => {
            return CResultPair {
                err_code: DATA_SHARE_ERROR,
                value: 0,
            }
        }
    };
    let mut p = MsgParcel::from_ptr(preds_parcel as *mut _);
    let preds: DataSharePredicates = match p.read() {
        Ok(v) => v,
        Err(_) => {
            return CResultPair {
                err_code: DATA_SHARE_ERROR,
                value: 0,
            }
        }
    };
    let (err_code, value) = (*handle).delete_ex(uri, &preds);
    CResultPair { err_code, value }
}

// =====================================================================
// Group 2: Query (Parcel-based)
// =====================================================================

unsafe fn do_query_parcel(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    input_parcel: *mut c_void,
    out_reply_parcel: *mut *mut c_void,
    timeout: i32,
    out_err_code: *mut i32,
) -> i32 {
    if !out_err_code.is_null() {
        *out_err_code = DATA_SHARE_ERROR;
    }
    if !out_reply_parcel.is_null() {
        *out_reply_parcel = std::ptr::null_mut();
    }
    if handle.is_null() || input_parcel.is_null() || out_reply_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let preds: DataSharePredicates = match inp.read() {
        Ok(v) => v,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let columns = match read_string_vec(&mut inp) {
        Some(c) => c,
        None => return DATA_SHARE_ERROR,
    };
    let option = if timeout > 0 {
        DataShareOption {
            timeout: timeout as u32,
        }
    } else {
        DataShareOption { timeout: 0 }
    };
    let mut business_error = DatashareBusinessError::new();
    let reply = (*handle).query_raw_for_ffi(uri, &preds, &columns, &option, &mut business_error);
    if !out_err_code.is_null() {
        *out_err_code = business_error.get_code();
    }
    match reply {
        Some(r) => {
            *out_reply_parcel = r.into_raw() as *mut c_void;
            E_OK
        }
        None => DATA_SHARE_ERROR,
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareHelperQueryParcel(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    input_parcel: *mut c_void,
    out_reply_parcel: *mut *mut c_void,
    out_err_code: *mut i32,
) -> i32 {
    do_query_parcel(
        handle,
        uri_ptr,
        uri_len,
        input_parcel,
        out_reply_parcel,
        0,
        out_err_code,
    )
}

#[no_mangle]
pub unsafe extern "C" fn DataShareHelperQueryWithOptionParcel(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    input_parcel: *mut c_void,
    timeout: i32,
    out_reply_parcel: *mut *mut c_void,
    out_err_code: *mut i32,
) -> i32 {
    let t = if timeout > 0 { timeout } else { 0 };
    do_query_parcel(
        handle,
        uri_ptr,
        uri_len,
        input_parcel,
        out_reply_parcel,
        t,
        out_err_code,
    )
}

// =====================================================================
// Group 3: Batch (Parcel-based)
// =====================================================================

/// Batch insert.
/// values_parcel: i32 count + DataShareValuesBucket[count]
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperBatchInsertParcel(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    values_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || values_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut p = MsgParcel::from_ptr(values_parcel as *mut _);
    let count: i32 = match p.read() {
        Ok(c) => c,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let mut values = Vec::with_capacity(count.max(0) as usize);
    for _ in 0..count {
        match p.read::<DataShareValuesBucket>() {
            Ok(v) => values.push(v),
            Err(_) => return DATA_SHARE_ERROR,
        }
    }
    (*handle).batch_insert(uri, &values)
}

/// Batch update.
/// input_parcel: UpdateOperations
/// result_parcel: i32 count + BatchUpdateResult[count]
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperBatchUpdateParcel(
    handle: DataShareHelperHandle,
    input_parcel: *mut c_void,
    result_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || input_parcel.is_null() || result_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let mut p = MsgParcel::from_ptr(input_parcel as *mut _);
    let ops: UpdateOperations = match p.read() {
        Ok(v) => v,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let mut results: Vec<BatchUpdateResult> = Vec::new();
    let ret = match (*handle).get_ext_sp_ctl() {
        Some(ctl) => ctl.batch_update_ex(&ops, &mut results),
        None => DATA_SHARE_ERROR,
    };
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    let _ = out.write(&(results.len() as i32));
    for r in &results {
        let _ = r.serialize(&mut out);
    }
    ret
}

/// Execute batch.
/// input_parcel: i32 count + OperationStatement[count]
/// result_parcel: ExecResultSet (i32 error_code + i32 count + ExecResult[count])
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperExecuteBatchParcel(
    handle: DataShareHelperHandle,
    input_parcel: *mut c_void,
    result_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || input_parcel.is_null() || result_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let mut p = MsgParcel::from_ptr(input_parcel as *mut _);
    let count: i32 = match p.read() {
        Ok(c) => c,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let mut stmts = Vec::with_capacity(count.max(0) as usize);
    for _ in 0..count {
        match p.read::<OperationStatement>() {
            Ok(s) => stmts.push(s),
            Err(_) => return DATA_SHARE_ERROR,
        }
    }
    let mut result = ExecResultSet {
        error_code: ExecErrorCode::Success,
        results: Vec::new(),
    };
    let ret = match (*handle).get_ext_sp_ctl() {
        Some(ctl) => ctl.execute_batch_ex(&stmts, &mut result),
        None => DATA_SHARE_ERROR,
    };
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    let _ = result.serialize(&mut out);
    ret
}

// =====================================================================
// Group 4: Template + RDB Subscription
// =====================================================================

#[no_mangle]
pub unsafe extern "C" fn DataShareHelperAddQueryTemplateParcel(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    subscriber_id: i64,
    tpl_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || tpl_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut p = MsgParcel::from_ptr(tpl_parcel as *mut _);
    let tpl: Template = match p.read() {
        Ok(v) => v,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let res = (*handle).add_query_template(uri, subscriber_id, &tpl);
    res
}

/// C callback type for RDB change notifications via parcel.
/// `change_parcel` is a `MsgParcel` allocated by Rust via `into_raw()`;
/// C++ owns it after the callback and must `delete` the underlying
/// `MessageParcel*`.
pub type RdbChangeParcelCallback = extern "C" fn(context: *mut c_void, change_parcel: *mut c_void);

unsafe fn read_uris_and_template(parcel: &mut MsgParcel) -> Option<(Vec<String>, TemplateId)> {
    let uris = read_string_vec(parcel)?;
    let tid: TemplateId = parcel.read().ok()?;
    Some((uris, tid))
}

#[no_mangle]
pub unsafe extern "C" fn DataShareHelperSubscribeRdbDataParcel(
    handle: DataShareHelperHandle,
    input_parcel: *mut c_void,
    callback: RdbChangeParcelCallback,
    context: *mut c_void,
    result_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || input_parcel.is_null() || result_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let (uris, tid) = match read_uris_and_template(&mut inp) {
        Some(v) => v,
        None => return DATA_SHARE_ERROR,
    };
    let ctx_val = context as usize;
    let cb: Arc<dyn Fn(&RdbChangeNode) + Send + Sync> = Arc::new(move |node: &RdbChangeNode| {
        let mut parcel = MsgParcel::new();
        let _ = node.serialize(&mut parcel);
        let raw = parcel.into_raw();
        callback(ctx_val as *mut c_void, raw as *mut c_void);
    });
    let results = (*handle).subscribe_rdb_data(&uris, &tid, cb);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
    E_OK
}

#[no_mangle]
pub unsafe extern "C" fn DataShareHelperUnsubscribeRdbDataParcel(
    handle: DataShareHelperHandle,
    input_parcel: *mut c_void,
    result_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || input_parcel.is_null() || result_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let (uris, tid) = match read_uris_and_template(&mut inp) {
        Some(v) => v,
        None => return DATA_SHARE_ERROR,
    };
    let results = (*handle).unsubscribe_rdb_data(&uris, &tid);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
    E_OK
}

#[no_mangle]
pub unsafe extern "C" fn DataShareHelperEnableRdbSubsParcel(
    handle: DataShareHelperHandle,
    input_parcel: *mut c_void,
    result_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || input_parcel.is_null() || result_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let (uris, tid) = match read_uris_and_template(&mut inp) {
        Some(v) => v,
        None => return DATA_SHARE_ERROR,
    };
    let results = (*handle).enable_rdb_subs(&uris, &tid);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
    E_OK
}

#[no_mangle]
pub unsafe extern "C" fn DataShareHelperDisableRdbSubsParcel(
    handle: DataShareHelperHandle,
    input_parcel: *mut c_void,
    result_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || input_parcel.is_null() || result_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let mut inp = MsgParcel::from_ptr(input_parcel as *mut _);
    let (uris, tid) = match read_uris_and_template(&mut inp) {
        Some(v) => v,
        None => return DATA_SHARE_ERROR,
    };
    let results = (*handle).disable_rdb_subs(&uris, &tid);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
    E_OK
}

// =====================================================================
// Group 5: Publish
// =====================================================================

#[no_mangle]
pub unsafe extern "C" fn DataShareHelperPublishParcel(
    handle: DataShareHelperHandle,
    bundle_ptr: *const u8,
    bundle_len: u32,
    data_parcel: *mut c_void,
    result_parcel: *mut c_void,
) -> i32 {
    if handle.is_null() || data_parcel.is_null() || result_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let bundle = match c_str_to_rust(bundle_ptr, bundle_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let mut inp = MsgParcel::from_ptr(data_parcel as *mut _);
    let data: Data = match inp.read() {
        Ok(d) => d,
        Err(_) => return DATA_SHARE_ERROR,
    };
    let results = (*handle).publish(&data, bundle);
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    write_operation_results(&mut out, &results);
    E_OK
}

#[no_mangle]
pub unsafe extern "C" fn DataShareHelperGetPublishedDataParcel(
    handle: DataShareHelperHandle,
    bundle_ptr: *const u8,
    bundle_len: u32,
    result_parcel: *mut c_void,
    out_result_code: *mut i32,
) -> i32 {
    if handle.is_null() || result_parcel.is_null() {
        return DATA_SHARE_ERROR;
    }
    let bundle = match c_str_to_rust(bundle_ptr, bundle_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    let (data, result_code) = (*handle).get_published_data(bundle);
    if !out_result_code.is_null() {
        *out_result_code = result_code;
    }
    let mut out = MsgParcel::from_ptr(result_parcel as *mut _);
    if data.serialize(&mut out).is_err() {
        return DATA_SHARE_ERROR;
    }
    E_OK
}
