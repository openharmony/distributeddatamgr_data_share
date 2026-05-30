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

//! Observer stubs for IPC callbacks.
//!
//! Corresponds to C++ `data_proxy_observer_stub.h/cpp` (243 lines).
//!
//! These stubs receive IPC callbacks from the DataShareService when
//! subscribed data changes. Each stub type handles a different kind
//! of data change notification:
//! - `RdbObserverStub` — RDB data changes
//! - `PublishedDataObserverStub` — Published data changes
//! - `ProxyDataObserverStub` — Proxy data changes
//!
//! In C++, these inherit from `IRemoteStub<IDataProxyXxxObserver>`.
//! In Rust, they wrap a callback function and dispatch to it.

use std::sync::Mutex;

use ipc::parcel::MsgParcel;
use ipc::remote::{RemoteObj, RemoteStub};

use datashare_common::types::{DataProxyChangeInfo, PublishedDataChangeNode, RdbChangeNode};

const MAX_VEC_SIZE: i32 = 1024;

#[allow(unused_imports)]
use datashare_common::ipc::parcel_impl;

/// Callback type for RDB data change notifications.
pub type RdbCallback = Box<dyn Fn(RdbChangeNode) + Send + Sync>;

/// Callback type for published data change notifications.
pub type PublishedDataCallback = Box<dyn Fn(&PublishedDataChangeNode) + Send + Sync>;

/// Callback type for proxy data change notifications.
pub type ProxyDataCallback = Box<dyn Fn(&[DataProxyChangeInfo]) + Send + Sync>;

/// RDB observer stub descriptor.
const RDB_OBSERVER_DESCRIPTOR: &str = "OHOS.DataShare.IDataProxyRdbObserver";

/// Published data observer stub descriptor.
const PUBLISHED_OBSERVER_DESCRIPTOR: &str = "OHOS.DataShare.IDataProxyPublishedDataObserver";

/// Proxy data observer stub descriptor.
const PROXY_OBSERVER_DESCRIPTOR: &str = "OHOS.DataShare.IProxyDataObserver";

/// RDB observer stub — receives IPC callbacks for RDB data changes.
///
/// Corresponds to C++ `RdbObserverStub : public IRemoteStub<IDataProxyRdbObserver>`.
pub struct RdbObserverStub {
    callback: Mutex<Option<RdbCallback>>,
}

fn recover_rdb_change_node_data(node: &mut RdbChangeNode) {
    if !node.is_shared_memory {
        return;
    }
    let _ = deserialize_data_from_ashmem(node);
    if let Some(ref ashmem) = node.memory {
        ashmem.unmap_ashmem();
        ashmem.close_ashmem();
    }
    node.memory = None;
    node.is_shared_memory = false;
    node.size = 0;
}

fn deserialize_data_from_ashmem(node: &mut RdbChangeNode) -> Option<()> {
    let ashmem = node.memory.as_ref()?;
    if !ashmem.map_read_write_ashmem() {
        ashmem.close_ashmem();
        node.memory = None;
        return None;
    }
    let int_len = 4i32;
    let mut offset = 0i32;
    let ptr = unsafe { ashmem.read_from_ashmem(int_len, offset) };
    if ptr.is_null() {
        return None;
    }
    let vec_len = unsafe { *(ptr as *const i32) };
    offset += int_len;
    if !(0..=MAX_VEC_SIZE).contains(&vec_len) {
        return None;
    }
    for _ in 0..vec_len {
        let ptr = unsafe { ashmem.read_from_ashmem(int_len, offset) };
        if ptr.is_null() {
            return None;
        }
        let data_len = unsafe { *(ptr as *const i32) };
        offset += int_len;
        let ptr = unsafe { ashmem.read_from_ashmem(data_len, offset) };
        if ptr.is_null() {
            return None;
        }
        let bytes = unsafe { std::slice::from_raw_parts(ptr as *const u8, data_len as usize) };
        node.data.push(String::from_utf8_lossy(bytes).into_owned());
        offset += data_len;
    }
    Some(())
}

impl RdbObserverStub {
    pub fn new(callback: RdbCallback) -> Self {
        Self {
            callback: Mutex::new(Some(callback)),
        }
    }

    pub fn on_change_from_rdb(&self, mut change_node: RdbChangeNode) {
        recover_rdb_change_node_data(&mut change_node);
        let cb = self.callback.lock().unwrap();
        if let Some(callback) = cb.as_ref() {
            callback(change_node);
        }
    }

    pub fn clear_callback(&self) {
        let mut cb = self.callback.lock().unwrap();
        *cb = None;
    }

    /// Convert this stub into a RemoteObj for passing to proxy methods.
    pub fn into_remote(self) -> Option<RemoteObj> {
        RemoteObj::from_stub(self)
    }
}

impl RemoteStub for RdbObserverStub {
    fn descriptor(&self) -> &'static str {
        RDB_OBSERVER_DESCRIPTOR
    }

    fn on_remote_request(&self, code: u32, data: &mut MsgParcel, _reply: &mut MsgParcel) -> i32 {
        if data.read_interface_token().is_err() {
            return -1;
        }
        if code != 0 {
            return -1;
        }
        match data.read::<RdbChangeNode>() {
            Ok(node) => {
                self.on_change_from_rdb(node);
                0
            }
            Err(_) => -1,
        }
    }
}

/// Published data observer stub — receives IPC callbacks for published data changes.
pub struct PublishedDataObserverStub {
    callback: Mutex<Option<PublishedDataCallback>>,
}

impl PublishedDataObserverStub {
    pub fn new(callback: PublishedDataCallback) -> Self {
        Self {
            callback: Mutex::new(Some(callback)),
        }
    }

    pub fn on_change_from_published_data(&self, change_node: &PublishedDataChangeNode) {
        let cb = self.callback.lock().unwrap();
        if let Some(callback) = cb.as_ref() {
            callback(change_node);
        }
    }

    pub fn clear_callback(&self) {
        let mut cb = self.callback.lock().unwrap();
        *cb = None;
    }

    pub fn into_remote(self) -> Option<RemoteObj> {
        RemoteObj::from_stub(self)
    }
}

impl RemoteStub for PublishedDataObserverStub {
    fn descriptor(&self) -> &'static str {
        PUBLISHED_OBSERVER_DESCRIPTOR
    }

    fn on_remote_request(&self, _code: u32, data: &mut MsgParcel, _reply: &mut MsgParcel) -> i32 {
        if data.read_interface_token().is_err() {
            return -1;
        }
        let owner_bundle_name: String = data.read().unwrap_or_default();
        let count: i32 = data.read().unwrap_or(0);
        let mut datas = Vec::new();
        for _ in 0..count {
            if let Ok(item) = data.read::<datashare_common::types::PublishedDataItem>() {
                datas.push(item);
            }
        }
        let node = PublishedDataChangeNode {
            owner_bundle_name,
            datas,
        };
        self.on_change_from_published_data(&node);
        0
    }
}

/// Proxy data observer stub — receives IPC callbacks for proxy data changes.
pub struct ProxyDataObserverStub {
    callback: Mutex<Option<ProxyDataCallback>>,
}

impl ProxyDataObserverStub {
    pub fn new(callback: ProxyDataCallback) -> Self {
        Self {
            callback: Mutex::new(Some(callback)),
        }
    }

    pub fn on_change_from_proxy_data(&self, change_info: &[DataProxyChangeInfo]) {
        let cb = self.callback.lock().unwrap();
        if let Some(callback) = cb.as_ref() {
            callback(change_info);
        }
    }

    pub fn clear_callback(&self) {
        let mut cb = self.callback.lock().unwrap();
        *cb = None;
    }

    pub fn into_remote(self) -> Option<RemoteObj> {
        RemoteObj::from_stub(self)
    }
}

impl RemoteStub for ProxyDataObserverStub {
    fn descriptor(&self) -> &'static str {
        PROXY_OBSERVER_DESCRIPTOR
    }

    fn on_remote_request(&self, _code: u32, data: &mut MsgParcel, _reply: &mut MsgParcel) -> i32 {
        if data.read_interface_token().is_err() {
            return -1;
        }
        let count: i32 = data.read().unwrap_or(0);
        let mut changes = Vec::new();
        for _ in 0..count {
            let change_type_val: u32 = data.read().unwrap_or(0);
            let uri: String = data.read().unwrap_or_default();
            let value: datashare_common::types::DataProxyValue = data.read().unwrap_or_default();
            changes.push(DataProxyChangeInfo {
                change_type: datashare_common::observer::ChangeType::from_u32(change_type_val),
                uri,
                value,
            });
        }
        self.on_change_from_proxy_data(&changes);
        0
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::Arc;

    #[test]
    fn test_rdb_observer_stub() {
        let called = Arc::new(Mutex::new(false));
        let called_clone = called.clone();
        let stub = RdbObserverStub::new(Box::new(move |_| {
            *called_clone.lock().unwrap() = true;
        }));

        let node = RdbChangeNode::default();
        stub.on_change_from_rdb(node);
        assert!(*called.lock().unwrap());
    }

    #[test]
    fn test_rdb_observer_stub_clear() {
        let called = Arc::new(Mutex::new(false));
        let called_clone = called.clone();
        let stub = RdbObserverStub::new(Box::new(move |_| {
            *called_clone.lock().unwrap() = true;
        }));

        stub.clear_callback();
        let node = RdbChangeNode::default();
        stub.on_change_from_rdb(node);
        assert!(!*called.lock().unwrap());
    }

    #[test]
    fn test_published_data_observer_stub() {
        let called = Arc::new(Mutex::new(false));
        let called_clone = called.clone();
        let stub = PublishedDataObserverStub::new(Box::new(move |_| {
            *called_clone.lock().unwrap() = true;
        }));

        let node = PublishedDataChangeNode::default();
        stub.on_change_from_published_data(&node);
        assert!(*called.lock().unwrap());
    }

    #[test]
    fn test_proxy_data_observer_stub() {
        let called = Arc::new(Mutex::new(false));
        let called_clone = called.clone();
        let stub = ProxyDataObserverStub::new(Box::new(move |_| {
            *called_clone.lock().unwrap() = true;
        }));

        stub.on_change_from_proxy_data(&[]);
        assert!(*called.lock().unwrap());
    }
}
