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

//! KV Store Data Service Proxy.
//!
//! Equivalent to C++ `ikvstore_data_service.cpp`.
//!
//! Provides proxy access to the KV Store Data Service via IPC.

use ipc::parcel::MsgParcel;
use ipc::remote::RemoteObj;
use ipc::{IpcResult, IpcStatusCode};

/// Descriptor for the IKvStoreDataService IPC interface.
const IKVSTORE_DATA_SERVICE_DESCRIPTOR: &str = "OHOS.DistributedKv.IKvStoreDataService";

/// Command codes for IKvStoreDataService IPC interface.
///
/// Equivalent to C++ `IKvStoreDataInterfaceCode`.
#[repr(u32)]
#[derive(Debug, Clone, Copy)]
pub enum KvStoreDataInterfaceCode {
    GetFeatureInterface = 0,
    RegisterClientDeathObserver = 1,
}

/// Proxy for the KV Store Data Service.
///
/// Equivalent to C++ `DataShareKvServiceProxy`.
/// Sends IPC requests to the KV Store service.
pub struct DataShareKvServiceProxy {
    remote: RemoteObj,
}

impl DataShareKvServiceProxy {
    /// Creates a new proxy wrapping the given remote object.
    pub fn new(remote: RemoteObj) -> Self {
        Self { remote }
    }

    /// Get a feature interface from the KV Store service.
    ///
    /// Equivalent to C++ `DataShareKvServiceProxy::GetFeatureInterface`.
    /// Returns the remote object for the requested feature.
    pub fn get_feature_interface(&self, name: &str) -> Option<RemoteObj> {
        let mut data = MsgParcel::new();
        data.write_interface_token(IKVSTORE_DATA_SERVICE_DESCRIPTOR)
            .ok()?;
        data.write(&name.to_string()).ok()?;

        let mut reply = self
            .remote
            .send_request(
                KvStoreDataInterfaceCode::GetFeatureInterface as u32,
                &mut data,
            )
            .ok()?;

        let remote_object = reply.read_remote().ok()?;
        Some(remote_object)
    }

    /// Register a client death observer with the KV Store service.
    ///
    /// Equivalent to C++ `DataShareKvServiceProxy::RegisterClientDeathObserver`.
    /// Returns 0 on success, -1 on failure.
    pub fn register_client_death_observer(&self, app_id: &str, observer: RemoteObj) -> u32 {
        let mut data = MsgParcel::new();
        if data
            .write_interface_token(IKVSTORE_DATA_SERVICE_DESCRIPTOR)
            .is_err()
        {
            return u32::MAX; // -1 as unsigned
        }
        if data.write(&app_id.to_string()).is_err() {
            return u32::MAX;
        }
        if data.write_remote(observer).is_err() {
            return u32::MAX;
        }

        match self.remote.send_request(
            KvStoreDataInterfaceCode::RegisterClientDeathObserver as u32,
            &mut data,
        ) {
            Ok(mut reply) => match reply.read::<i32>() {
                Ok(code) => code as u32,
                Err(_) => u32::MAX,
            },
            Err(_) => u32::MAX,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_kvstore_interface_code_values() {
        assert_eq!(KvStoreDataInterfaceCode::GetFeatureInterface as u32, 0);
        assert_eq!(
            KvStoreDataInterfaceCode::RegisterClientDeathObserver as u32,
            1
        );
    }
}
