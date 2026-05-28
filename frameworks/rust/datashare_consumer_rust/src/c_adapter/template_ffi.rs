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

//! FFI functions for DataShareHelper template and subscription operations.

use std::ffi::c_void;
use std::sync::Arc;

use super::types_ffi::{c_str_array_to_vec, c_str_to_rust, DataShareHelperHandle};
use crate::helper::datashare_helper::DataShareHelper;
use datashare_common::template::{Template, TemplateId};
use datashare_common::types::{PublishedDataChangeNode, RdbChangeNode};

const DATA_SHARE_ERROR: i32 = -1;

/// Add a query template.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperAddQueryTemplate(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    subscriber_id: i64,
    tpl: *const Template,
) -> i32 {
    if handle.is_null() || tpl.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    (*handle).add_query_template(uri, subscriber_id, &*tpl)
}

/// Delete a query template.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperDelQueryTemplate(
    handle: DataShareHelperHandle,
    uri_ptr: *const u8,
    uri_len: u32,
    subscriber_id: i64,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uri = match c_str_to_rust(uri_ptr, uri_len) {
        Some(s) => s,
        None => return DATA_SHARE_ERROR,
    };
    (*handle).del_query_template(uri, subscriber_id)
}

/// C callback type for RDB data change notifications.
pub type RdbChangeCallback = extern "C" fn(context: *mut c_void, change_node: *const RdbChangeNode);

/// Subscribe to RDB data changes.
///
/// `callback` is a C function pointer invoked on data change.
/// `context` is an opaque pointer passed back to the callback (typically the C++ observer).
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperSubscribeRdbData(
    handle: DataShareHelperHandle,
    uris_ptrs: *const *const u8,
    uris_lens: *const u32,
    uris_count: u32,
    template_id: *const TemplateId,
    callback: RdbChangeCallback,
    context: *mut c_void,
) -> i32 {
    if handle.is_null() || template_id.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uris = c_str_array_to_vec(uris_ptrs, uris_lens, uris_count);
    // Wrap the C callback in a Rust closure.
    // Safety: context pointer must remain valid for the lifetime of the subscription.
    let ctx = context as usize; // Convert to usize for Send+Sync
    let cb: Arc<dyn Fn(&RdbChangeNode) + Send + Sync> = Arc::new(move |node: &RdbChangeNode| {
        callback(ctx as *mut c_void, node as *const RdbChangeNode);
    });
    let results = (*handle).subscribe_rdb_data(&uris, &*template_id, cb);
    if results.is_empty() {
        DATA_SHARE_ERROR
    } else {
        0
    }
}

/// Unsubscribe from RDB data changes.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperUnsubscribeRdbData(
    handle: DataShareHelperHandle,
    uris_ptrs: *const *const u8,
    uris_lens: *const u32,
    uris_count: u32,
    template_id: *const TemplateId,
) -> i32 {
    if handle.is_null() || template_id.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uris = c_str_array_to_vec(uris_ptrs, uris_lens, uris_count);
    let results = (*handle).unsubscribe_rdb_data(&uris, &*template_id);
    if results.is_empty() {
        DATA_SHARE_ERROR
    } else {
        0
    }
}

/// Enable RDB subscriptions.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperEnableRdbSubs(
    handle: DataShareHelperHandle,
    uris_ptrs: *const *const u8,
    uris_lens: *const u32,
    uris_count: u32,
    template_id: *const TemplateId,
) -> i32 {
    if handle.is_null() || template_id.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uris = c_str_array_to_vec(uris_ptrs, uris_lens, uris_count);
    let results = (*handle).enable_rdb_subs(&uris, &*template_id);
    if results.is_empty() {
        DATA_SHARE_ERROR
    } else {
        0
    }
}

/// Disable RDB subscriptions.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperDisableRdbSubs(
    handle: DataShareHelperHandle,
    uris_ptrs: *const *const u8,
    uris_lens: *const u32,
    uris_count: u32,
    template_id: *const TemplateId,
) -> i32 {
    if handle.is_null() || template_id.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uris = c_str_array_to_vec(uris_ptrs, uris_lens, uris_count);
    let results = (*handle).disable_rdb_subs(&uris, &*template_id);
    if results.is_empty() {
        DATA_SHARE_ERROR
    } else {
        0
    }
}

/// C callback type for published data change notifications.
pub type PublishedDataChangeCallback =
    extern "C" fn(context: *mut c_void, change_node: *const PublishedDataChangeNode);

/// Subscribe to published data changes.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperSubscribePublishedData(
    handle: DataShareHelperHandle,
    uris_ptrs: *const *const u8,
    uris_lens: *const u32,
    uris_count: u32,
    subscriber_id: i64,
    callback: PublishedDataChangeCallback,
    context: *mut c_void,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uris = c_str_array_to_vec(uris_ptrs, uris_lens, uris_count);
    let ctx = context as usize;
    let cb: Arc<dyn Fn(&PublishedDataChangeNode) + Send + Sync> =
        Arc::new(move |node: &PublishedDataChangeNode| {
            callback(ctx as *mut c_void, node as *const PublishedDataChangeNode);
        });
    let results = (*handle).subscribe_published_data(&uris, subscriber_id, cb);
    if results.is_empty() {
        DATA_SHARE_ERROR
    } else {
        0
    }
}

/// Unsubscribe from published data changes.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperUnsubscribePublishedData(
    handle: DataShareHelperHandle,
    uris_ptrs: *const *const u8,
    uris_lens: *const u32,
    uris_count: u32,
    subscriber_id: i64,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uris = c_str_array_to_vec(uris_ptrs, uris_lens, uris_count);
    let results = (*handle).unsubscribe_published_data(&uris, subscriber_id);
    if results.is_empty() {
        DATA_SHARE_ERROR
    } else {
        0
    }
}

/// Enable published data subscriptions.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperEnablePubSubs(
    handle: DataShareHelperHandle,
    uris_ptrs: *const *const u8,
    uris_lens: *const u32,
    uris_count: u32,
    subscriber_id: i64,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uris = c_str_array_to_vec(uris_ptrs, uris_lens, uris_count);
    let results = (*handle).enable_pub_subs(&uris, subscriber_id);
    if results.is_empty() {
        DATA_SHARE_ERROR
    } else {
        0
    }
}

/// Disable published data subscriptions.
#[no_mangle]
pub unsafe extern "C" fn DataShareHelperDisablePubSubs(
    handle: DataShareHelperHandle,
    uris_ptrs: *const *const u8,
    uris_lens: *const u32,
    uris_count: u32,
    subscriber_id: i64,
) -> i32 {
    if handle.is_null() {
        return DATA_SHARE_ERROR;
    }
    let uris = c_str_array_to_vec(uris_ptrs, uris_lens, uris_count);
    let results = (*handle).disable_pub_subs(&uris, subscriber_id);
    if results.is_empty() {
        DATA_SHARE_ERROR
    } else {
        0
    }
}
