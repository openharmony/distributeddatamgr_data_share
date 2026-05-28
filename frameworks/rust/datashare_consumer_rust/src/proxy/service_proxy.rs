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

//! DataShareServiceProxy — IPC proxy to DataShareService.
//!
//! Corresponds to C++ `DataShareServiceProxy` in
//! `frameworks/native/proxy/src/data_share_service_proxy.cpp`.

use std::sync::atomic::{AtomicBool, Ordering};

use ipc::parcel::MsgParcel;
use ipc::remote::RemoteObj;

use datashare_common::predicates::DataSharePredicates;
use datashare_common::template::Template;
use datashare_common::types::{
    Data, DataProxyConfig, DataProxyErrorCode, DataProxyGetResult, DataProxyResult, DataProxyValue,
    DataShareProxyData, OperationResult,
};
use datashare_common::values_bucket::DataShareValuesBucket;

use crate::controller::general_controller::DatashareBusinessError;
use crate::ipc::codes::{DataShareServiceCmd, SYSTEM_CODE_OFFSET};

#[allow(unused_imports)]
use datashare_common::ipc::parcel_impl;

const INVALID_VALUE: i32 = -1;
const DATA_SHARE_ERROR: i32 = -1;
const DESCRIPTOR: &str = "OHOS.DataShare.IDataShareService";

static IS_SYSTEM: AtomicBool = AtomicBool::new(false);

pub struct DataShareServiceProxy {
    remote: RemoteObj,
}

impl DataShareServiceProxy {
    pub fn new(remote: RemoteObj) -> Self {
        Self { remote }
    }

    /// Get the underlying remote object.
    pub fn remote(&self) -> &RemoteObj {
        &self.remote
    }

    fn create_request(&self) -> Option<MsgParcel> {
        let mut data = MsgParcel::new();
        data.write_interface_token(DESCRIPTOR).ok()?;
        Some(data)
    }

    /// Apply CastIPCCode: adds SYSTEM_CODE_OFFSET when IsSystem().
    fn cast_ipc_code(&self, cmd: DataShareServiceCmd) -> u32 {
        if Self::is_system() {
            (cmd as u32) + SYSTEM_CODE_OFFSET
        } else {
            cmd as u32
        }
    }

    /// Send with CastIPCCode (system-aware offset).
    fn send(&self, cmd: DataShareServiceCmd, data: &mut MsgParcel) -> Option<MsgParcel> {
        let code = self.cast_ipc_code(cmd);
        match self.remote.send_request(code, data) {
            Ok(reply) => Some(reply),
            Err(_) => None,
        }
    }

    /// Send without system offset (static_cast path).
    fn send_direct(&self, cmd: DataShareServiceCmd, data: &mut MsgParcel) -> Option<MsgParcel> {
        self.remote.send_request(cmd as u32, data).ok()
    }

    /// Read a Vec<OperationResult> from reply.
    fn read_operation_results(reply: &mut MsgParcel) -> Vec<OperationResult> {
        let count: i32 = reply.read().unwrap_or(0);
        let mut results = Vec::new();
        for _ in 0..count {
            if let Ok(r) = reply.read::<OperationResult>() {
                results.push(r);
            }
        }
        results
    }

    /// Read a Vec<DataProxyResult> from reply.
    fn read_proxy_results(reply: &mut MsgParcel) -> Vec<DataProxyResult> {
        let count: i32 = reply.read().unwrap_or(0);
        let mut results = Vec::new();
        for _ in 0..count {
            if let Ok(r) = reply.read::<DataProxyResult>() {
                results.push(r);
            }
        }
        results
    }

    /// 构造包含 InnerError 的错误结果 Vec（从 proxy_data 提取 URI）
    fn make_error_results_from_proxy_data(
        proxy_data: &[DataShareProxyData],
    ) -> Vec<DataProxyResult> {
        proxy_data
            .iter()
            .map(|d| DataProxyResult::new(d.uri.clone(), DataProxyErrorCode::InnerError))
            .collect()
    }

    /// 构造包含 InnerError 的错误结果 Vec（从 URI 列表）
    fn make_error_results_from_uris(uris: &[String]) -> Vec<DataProxyResult> {
        uris.iter()
            .map(|u| DataProxyResult::new(u.clone(), DataProxyErrorCode::InnerError))
            .collect()
    }

    /// 构造包含 InnerError 的 GetResult 错误 Vec（从 URI 列表）
    fn make_error_get_results_from_uris(uris: &[String]) -> Vec<DataProxyGetResult> {
        uris.iter()
            .map(|u| DataProxyGetResult {
                uri: u.clone(),
                result: DataProxyErrorCode::InnerError,
                ..Default::default()
            })
            .collect()
    }

    // ---- CRUD Operations (delegate to Ex variants) ----

    pub fn insert(&self, uri: &str, ext_uri: &str, value: &DataShareValuesBucket) -> i32 {
        let (err_code, status) = self.insert_ex(uri, ext_uri, value);
        if err_code == 0 {
            status
        } else if err_code < 0 {
            err_code
        } else {
            DATA_SHARE_ERROR
        }
    }

    pub fn update(
        &self,
        uri: &str,
        ext_uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> i32 {
        let (err_code, status) = self.update_ex(uri, ext_uri, predicates, value);
        if err_code == 0 {
            status
        } else if err_code < 0 {
            err_code
        } else {
            DATA_SHARE_ERROR
        }
    }

    pub fn delete(&self, uri: &str, ext_uri: &str, predicates: &DataSharePredicates) -> i32 {
        let (err_code, status) = self.delete_ex(uri, ext_uri, predicates);
        if err_code == 0 {
            status
        } else if err_code < 0 {
            err_code
        } else {
            DATA_SHARE_ERROR
        }
    }

    // ---- Extended CRUD ----

    pub fn insert_ex(&self, uri: &str, ext_uri: &str, value: &DataShareValuesBucket) -> (i32, i32) {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return (INVALID_VALUE, 0),
        };
        if data.write(&uri.to_string()).is_err() {
            return (INVALID_VALUE, 0);
        }
        if data.write(&ext_uri.to_string()).is_err() {
            return (INVALID_VALUE, 0);
        }
        if data.write(value).is_err() {
            return (INVALID_VALUE, 0);
        }

        let mut reply = match self.send(DataShareServiceCmd::InsertEx, &mut data) {
            Some(r) => r,
            None => return (DATA_SHARE_ERROR, 0),
        };
        let err_code = reply.read::<i32>().unwrap_or(INVALID_VALUE);
        let result = reply.read::<i32>().unwrap_or(0);
        (err_code, result)
    }

    pub fn update_ex(
        &self,
        uri: &str,
        ext_uri: &str,
        predicates: &DataSharePredicates,
        value: &DataShareValuesBucket,
    ) -> (i32, i32) {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return (INVALID_VALUE, 0),
        };
        if data.write(&uri.to_string()).is_err() {
            return (INVALID_VALUE, 0);
        }
        if data.write(&ext_uri.to_string()).is_err() {
            return (INVALID_VALUE, 0);
        }
        if data.write(predicates).is_err() {
            return (INVALID_VALUE, 0);
        }
        if data.write(value).is_err() {
            return (INVALID_VALUE, 0);
        }

        let mut reply = match self.send(DataShareServiceCmd::UpdateEx, &mut data) {
            Some(r) => r,
            None => return (DATA_SHARE_ERROR, 0),
        };
        let err_code = reply.read::<i32>().unwrap_or(INVALID_VALUE);
        let result = reply.read::<i32>().unwrap_or(0);
        (err_code, result)
    }

    pub fn delete_ex(
        &self,
        uri: &str,
        ext_uri: &str,
        predicates: &DataSharePredicates,
    ) -> (i32, i32) {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return (INVALID_VALUE, 0),
        };
        if data.write(&uri.to_string()).is_err() {
            return (INVALID_VALUE, 0);
        }
        if data.write(&ext_uri.to_string()).is_err() {
            return (INVALID_VALUE, 0);
        }
        if data.write(predicates).is_err() {
            return (INVALID_VALUE, 0);
        }

        let mut reply = match self.send(DataShareServiceCmd::DeleteEx, &mut data) {
            Some(r) => r,
            None => return (DATA_SHARE_ERROR, 0),
        };
        let err_code = reply.read::<i32>().unwrap_or(INVALID_VALUE);
        let result = reply.read::<i32>().unwrap_or(0);
        (err_code, result)
    }

    // ---- Query ----

    pub fn query(
        &self,
        uri: &str,
        ext_uri: &str,
        predicates: &DataSharePredicates,
        columns: &[String],
        business_error: &mut DatashareBusinessError,
    ) -> Option<MsgParcel> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => {
                business_error.set_code(INVALID_VALUE);
                return None;
            }
        };
        if data.write(&uri.to_string()).is_err() {
            business_error.set_code(INVALID_VALUE);
            return None;
        }
        if data.write(&ext_uri.to_string()).is_err() {
            business_error.set_code(INVALID_VALUE);
            return None;
        }
        if data.write(predicates).is_err() {
            business_error.set_code(INVALID_VALUE);
            return None;
        }
        if data.write(&(columns.len() as i32)).is_err() {
            return None;
        }
        for col in columns {
            if data.write(col).is_err() {
                return None;
            }
        }

        let mut reply = match self.send(DataShareServiceCmd::Query, &mut data) {
            Some(r) => r,
            None => {
                business_error.set_code(DATA_SHARE_ERROR);
                return None;
            }
        };
        // Result set is read by caller; business error code follows
        Some(reply)
    }

    // ---- Template Management ----

    pub fn add_query_template(&self, uri: &str, subscriber_id: i64, tpl: &Template) -> i32 {
        if !tpl.update.is_empty() && !tpl.update.to_ascii_uppercase().starts_with("UPDATE") {
            return DATA_SHARE_ERROR;
        }
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return DATA_SHARE_ERROR,
        };
        if data.write(&uri.to_string()).is_err() {
            return DATA_SHARE_ERROR;
        }
        if data.write(&subscriber_id).is_err() {
            return DATA_SHARE_ERROR;
        }
        // Marshal template fields individually (matching C++ ITypesUtil::Marshal)
        if data.write(&tpl.update).is_err() {
            return DATA_SHARE_ERROR;
        }
        // Write predicates as count + (key, select_sql) pairs
        if data.write(&(tpl.predicates.len() as i32)).is_err() {
            return DATA_SHARE_ERROR;
        }
        for pred in &tpl.predicates {
            if data.write(&pred.key).is_err() {
                return DATA_SHARE_ERROR;
            }
            if data.write(&pred.select_sql).is_err() {
                return DATA_SHARE_ERROR;
            }
        }
        if data.write(&tpl.scheduler).is_err() {
            return DATA_SHARE_ERROR;
        }

        let mut reply = match self.send(DataShareServiceCmd::AddTemplate, &mut data) {
            Some(r) => r,
            None => return DATA_SHARE_ERROR,
        };
        reply.read::<i32>().unwrap_or(DATA_SHARE_ERROR)
    }

    pub fn del_query_template(&self, uri: &str, subscriber_id: i64) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return DATA_SHARE_ERROR,
        };
        if data.write(&uri.to_string()).is_err() {
            return DATA_SHARE_ERROR;
        }
        if data.write(&subscriber_id).is_err() {
            return DATA_SHARE_ERROR;
        }

        let mut reply = match self.send(DataShareServiceCmd::DelTemplate, &mut data) {
            Some(r) => r,
            None => return DATA_SHARE_ERROR,
        };
        reply.read::<i32>().unwrap_or(DATA_SHARE_ERROR)
    }

    // ---- Publish/Subscribe ----

    pub fn publish(&self, pub_data: &Data, bundle_name: &str) -> Vec<OperationResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Vec::new(),
        };
        // Marshal Data: write datas count + each PublishedDataItem (uses Serialize impl)
        if data.write(&(pub_data.datas.len() as i32)).is_err() {
            return Vec::new();
        }
        for item in &pub_data.datas {
            if data.write(item).is_err() {
                return Vec::new();
            }
        }
        if data.write(&pub_data.version).is_err() {
            return Vec::new();
        }
        if data.write(&bundle_name.to_string()).is_err() {
            return Vec::new();
        }

        let mut reply = match self.send(DataShareServiceCmd::Publish, &mut data) {
            Some(r) => r,
            None => return Vec::new(),
        };
        Self::read_operation_results(&mut reply)
    }

    pub fn get_published_data(&self, bundle_name: &str) -> (Data, i32) {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return (Data::default(), INVALID_VALUE),
        };
        if data.write(&bundle_name.to_string()).is_err() {
            return (Data::default(), INVALID_VALUE);
        }

        let mut reply = match self.send(DataShareServiceCmd::GetData, &mut data) {
            Some(r) => r,
            None => return (Data::default(), INVALID_VALUE),
        };
        // Unmarshal: datas_ vector + resultCode (no version_ field)
        let count: i32 = reply.read().unwrap_or(0);
        let mut result = Data::default();
        for _ in 0..count {
            if let Ok(item) = reply.read::<datashare_common::types::PublishedDataItem>() {
                result.datas.push(item);
            }
        }
        let result_code: i32 = reply.read().unwrap_or(INVALID_VALUE);
        (result, result_code)
    }

    // ---- RDB Subscription ----

    /// Helper: write uris + template_id fields for subscription methods.
    fn write_subscription_header(
        data: &mut MsgParcel,
        uris: &[String],
        subscriber_id: i64,
        bundle_name: &str,
    ) -> bool {
        if data.write(&(uris.len() as i32)).is_err() {
            return false;
        }
        for uri in uris {
            if data.write(uri).is_err() {
                return false;
            }
        }
        if data.write(&subscriber_id).is_err() {
            return false;
        }
        if data.write(&bundle_name.to_string()).is_err() {
            return false;
        }
        true
    }

    pub fn subscribe_rdb_data(
        &self,
        uris: &[String],
        subscriber_id: i64,
        bundle_name: &str,
        observer: &RemoteObj,
    ) -> Vec<OperationResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Vec::new(),
        };
        if !Self::write_subscription_header(&mut data, uris, subscriber_id, bundle_name) {
            return Vec::new();
        }
        // 写入空的 subscribeOption (map<string, bool>)，与 C++ 服务端 Unmarshal 格式对齐
        if data.write(&0i32).is_err() {
            return Vec::new();
        }
        if data.write_remote(observer.clone()).is_err() {
            return Vec::new();
        }

        let mut reply = match self.send(DataShareServiceCmd::SubscribeRdb, &mut data) {
            Some(r) => r,
            None => return Vec::new(),
        };
        Self::read_operation_results(&mut reply)
    }

    pub fn unsubscribe_rdb_data(
        &self,
        uris: &[String],
        subscriber_id: i64,
        bundle_name: &str,
    ) -> Vec<OperationResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Vec::new(),
        };
        if !Self::write_subscription_header(&mut data, uris, subscriber_id, bundle_name) {
            return Vec::new();
        }

        let mut reply = match self.send(DataShareServiceCmd::UnsubscribeRdb, &mut data) {
            Some(r) => r,
            None => return Vec::new(),
        };
        Self::read_operation_results(&mut reply)
    }

    pub fn enable_subscribe_rdb_data(
        &self,
        uris: &[String],
        subscriber_id: i64,
        bundle_name: &str,
    ) -> Vec<OperationResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Vec::new(),
        };
        if !Self::write_subscription_header(&mut data, uris, subscriber_id, bundle_name) {
            return Vec::new();
        }
        let mut reply = match self.send(DataShareServiceCmd::EnableSubscribeRdb, &mut data) {
            Some(r) => r,
            None => return Vec::new(),
        };
        Self::read_operation_results(&mut reply)
    }

    pub fn disable_subscribe_rdb_data(
        &self,
        uris: &[String],
        subscriber_id: i64,
        bundle_name: &str,
    ) -> Vec<OperationResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Vec::new(),
        };
        if !Self::write_subscription_header(&mut data, uris, subscriber_id, bundle_name) {
            return Vec::new();
        }
        let mut reply = match self.send(DataShareServiceCmd::DisableSubscribeRdb, &mut data) {
            Some(r) => r,
            None => return Vec::new(),
        };
        Self::read_operation_results(&mut reply)
    }

    // ---- Published Data Subscription ----

    /// Helper: write uris + subscriber_id for published data subscription.
    fn write_published_sub_header(
        data: &mut MsgParcel,
        uris: &[String],
        subscriber_id: i64,
    ) -> bool {
        if data.write(&(uris.len() as i32)).is_err() {
            return false;
        }
        for uri in uris {
            if data.write(uri).is_err() {
                return false;
            }
        }
        if data.write(&subscriber_id).is_err() {
            return false;
        }
        true
    }

    pub fn subscribe_published_data(
        &self,
        uris: &[String],
        subscriber_id: i64,
        observer: &RemoteObj,
    ) -> Vec<OperationResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Vec::new(),
        };
        if !Self::write_published_sub_header(&mut data, uris, subscriber_id) {
            return Vec::new();
        }
        if data.write_remote(observer.clone()).is_err() {
            return Vec::new();
        }

        let mut reply = match self.send(DataShareServiceCmd::SubscribePublished, &mut data) {
            Some(r) => r,
            None => return Vec::new(),
        };
        Self::read_operation_results(&mut reply)
    }

    pub fn unsubscribe_published_data(
        &self,
        uris: &[String],
        subscriber_id: i64,
    ) -> Vec<OperationResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Vec::new(),
        };
        if !Self::write_published_sub_header(&mut data, uris, subscriber_id) {
            return Vec::new();
        }

        let mut reply = match self.send(DataShareServiceCmd::UnsubscribePublished, &mut data) {
            Some(r) => r,
            None => return Vec::new(),
        };
        Self::read_operation_results(&mut reply)
    }

    pub fn enable_subscribe_published_data(
        &self,
        uris: &[String],
        subscriber_id: i64,
    ) -> Vec<OperationResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Vec::new(),
        };
        if !Self::write_published_sub_header(&mut data, uris, subscriber_id) {
            return Vec::new();
        }
        let mut reply = match self.send(DataShareServiceCmd::EnableSubscribePublished, &mut data) {
            Some(r) => r,
            None => return Vec::new(),
        };
        Self::read_operation_results(&mut reply)
    }

    pub fn disable_subscribe_published_data(
        &self,
        uris: &[String],
        subscriber_id: i64,
    ) -> Vec<OperationResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Vec::new(),
        };
        if !Self::write_published_sub_header(&mut data, uris, subscriber_id) {
            return Vec::new();
        }
        let mut reply = match self.send(DataShareServiceCmd::DisableSubscribePublished, &mut data) {
            Some(r) => r,
            None => return Vec::new(),
        };
        Self::read_operation_results(&mut reply)
    }

    // ---- Observer Registration & Notify ----

    pub fn notify(&self, uri: &str) {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return,
        };
        if data.write(&uri.to_string()).is_err() {
            return;
        }
        let _ = self.send(DataShareServiceCmd::NotifyObservers, &mut data);
    }

    /// Register observer. C++ uses static_cast (no system offset).
    pub fn register_observer(&self, uri: &str, observer: &RemoteObj) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return DATA_SHARE_ERROR,
        };
        if data.write(&uri.to_string()).is_err() {
            return DATA_SHARE_ERROR;
        }
        if data.write_remote(observer.clone()).is_err() {
            return DATA_SHARE_ERROR;
        }

        let mut reply = match self.send_direct(DataShareServiceCmd::RegisterObserver, &mut data) {
            Some(r) => r,
            None => return DATA_SHARE_ERROR,
        };
        reply.read::<i32>().unwrap_or(DATA_SHARE_ERROR)
    }

    /// Unregister observer. C++ uses static_cast (no system offset).
    pub fn unregister_observer(&self, uri: &str, observer: &RemoteObj) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return DATA_SHARE_ERROR,
        };
        if data.write(&uri.to_string()).is_err() {
            return DATA_SHARE_ERROR;
        }
        if data.write_remote(observer.clone()).is_err() {
            return DATA_SHARE_ERROR;
        }

        let mut reply = match self.send_direct(DataShareServiceCmd::UnregisterObserver, &mut data) {
            Some(r) => r,
            None => return DATA_SHARE_ERROR,
        };
        reply.read::<i32>().unwrap_or(DATA_SHARE_ERROR)
    }

    // ---- Silent Switch ----

    pub fn set_silent_switch(&self, uri: &str, enable: bool) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return DATA_SHARE_ERROR,
        };
        if data.write(&uri.to_string()).is_err() {
            return DATA_SHARE_ERROR;
        }
        if data.write(&enable).is_err() {
            return DATA_SHARE_ERROR;
        }

        let mut reply = match self.send(DataShareServiceCmd::SetSilentSwitch, &mut data) {
            Some(r) => r,
            None => return DATA_SHARE_ERROR,
        };
        reply.read::<i32>().unwrap_or(DATA_SHARE_ERROR)
    }

    pub fn get_silent_proxy_status(&self, uri: &str) -> i32 {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return DATA_SHARE_ERROR,
        };
        if data.write(&uri.to_string()).is_err() {
            return DATA_SHARE_ERROR;
        }

        let mut reply = match self.send(DataShareServiceCmd::GetSilentProxyStatus, &mut data) {
            Some(r) => r,
            None => return DATA_SHARE_ERROR,
        };
        reply.read::<i32>().unwrap_or(DATA_SHARE_ERROR)
    }

    // ---- SA Connection Interface Info ----

    pub fn get_connection_interface_info(
        &self,
        sa_id: i32,
        wait_time: u32,
    ) -> Option<(i32, u32, String)> {
        let mut data = self.create_request()?;
        if data.write(&sa_id).is_err() {
            return None;
        }
        if data.write(&wait_time).is_err() {
            return None;
        }

        let mut reply =
            self.send_direct(DataShareServiceCmd::GetConnectionInterfaceInfo, &mut data)?;
        let err_code: i32 = reply.read().ok()?;
        let descriptor: String = reply.read_string16().ok()?;
        let code: u32 = reply.read().ok()?;
        Some((err_code, code, descriptor))
    }

    // ---- Proxy Data Operations (all use static_cast, no system offset) ----

    pub fn publish_proxy_data(
        &self,
        proxy_data: &[DataShareProxyData],
        proxy_config: &DataProxyConfig,
    ) -> Vec<DataProxyResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Self::make_error_results_from_proxy_data(proxy_data),
        };
        if !write_proxy_data_vec_to_parcel(proxy_data, &mut data) {
            return Self::make_error_results_from_proxy_data(proxy_data);
        }
        if data.write(proxy_config).is_err() {
            return Self::make_error_results_from_proxy_data(proxy_data);
        }

        let mut reply = match self.send_direct(DataShareServiceCmd::ProxyPublish, &mut data) {
            Some(r) => r,
            None => return Self::make_error_results_from_proxy_data(proxy_data),
        };
        Self::read_proxy_results(&mut reply)
    }

    pub fn delete_proxy_data(
        &self,
        uris: &[String],
        proxy_config: &DataProxyConfig,
    ) -> Vec<DataProxyResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Self::make_error_results_from_uris(uris),
        };
        if data.write(&(uris.len() as i32)).is_err() {
            return Self::make_error_results_from_uris(uris);
        }
        for uri in uris {
            if data.write(uri).is_err() {
                return Self::make_error_results_from_uris(uris);
            }
        }
        if data.write(proxy_config).is_err() {
            return Self::make_error_results_from_uris(uris);
        }

        let mut reply = match self.send_direct(DataShareServiceCmd::ProxyDelete, &mut data) {
            Some(r) => r,
            None => return Self::make_error_results_from_uris(uris),
        };
        Self::read_proxy_results(&mut reply)
    }

    pub fn get_proxy_data(
        &self,
        uris: &[String],
        proxy_config: &DataProxyConfig,
    ) -> Vec<DataProxyGetResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Self::make_error_get_results_from_uris(uris),
        };
        if data.write(&(uris.len() as i32)).is_err() {
            return Self::make_error_get_results_from_uris(uris);
        }
        for uri in uris {
            if data.write(uri).is_err() {
                return Self::make_error_get_results_from_uris(uris);
            }
        }
        if data.write(proxy_config).is_err() {
            return Self::make_error_get_results_from_uris(uris);
        }

        let mut reply = match self.send_direct(DataShareServiceCmd::ProxyGet, &mut data) {
            Some(r) => r,
            None => return Self::make_error_get_results_from_uris(uris),
        };
        read_proxy_get_result_vec_from_parcel(&mut reply)
    }

    pub fn subscribe_proxy_data(
        &self,
        uris: &[String],
        observer: &RemoteObj,
    ) -> Vec<DataProxyResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Self::make_error_results_from_uris(uris),
        };
        if data.write(&(uris.len() as i32)).is_err() {
            return Self::make_error_results_from_uris(uris);
        }
        for uri in uris {
            if data.write(uri).is_err() {
                return Self::make_error_results_from_uris(uris);
            }
        }
        if data.write_remote(observer.clone()).is_err() {
            return Self::make_error_results_from_uris(uris);
        }

        let mut reply = match self.send_direct(DataShareServiceCmd::SubscribeProxyData, &mut data) {
            Some(r) => r,
            None => return Self::make_error_results_from_uris(uris),
        };
        Self::read_proxy_results(&mut reply)
    }

    pub fn unsubscribe_proxy_data(&self, uris: &[String]) -> Vec<DataProxyResult> {
        let mut data = match self.create_request() {
            Some(d) => d,
            None => return Self::make_error_results_from_uris(uris),
        };
        if data.write(&(uris.len() as i32)).is_err() {
            return Self::make_error_results_from_uris(uris);
        }
        for uri in uris {
            if data.write(uri).is_err() {
                return Self::make_error_results_from_uris(uris);
            }
        }

        let mut reply = match self.send_direct(DataShareServiceCmd::UnsubscribeProxyData, &mut data)
        {
            Some(r) => r,
            None => return Self::make_error_results_from_uris(uris),
        };
        Self::read_proxy_results(&mut reply)
    }

    // ---- System App Flag ----

    pub fn set_system(is_system: bool) {
        IS_SYSTEM.store(is_system, Ordering::SeqCst);
    }

    pub fn is_system() -> bool {
        IS_SYSTEM.load(Ordering::SeqCst)
    }

    pub fn clean_system() {
        IS_SYSTEM.store(false, Ordering::SeqCst);
    }
}

// =====================================================================
// Buffer 模式序列化辅助函数
// 与 C++ MarshalProxyDataVecToBuffer / UnmarshalProxyDataVecFromBuffer 格式对齐。
// 线序：WriteInt32(blob_len) + WriteBuffer(blob)
// blob 内部使用 native-endian 二进制格式。
// =====================================================================

/// 向 buffer 写入 usize（平台字长，ARM32 = 4 字节）。
fn buf_write_usize(buf: &mut Vec<u8>, val: usize) {
    buf.extend_from_slice(&val.to_ne_bytes());
}

/// 向 buffer 写入 i32。
fn buf_write_i32(buf: &mut Vec<u8>, val: i32) {
    buf.extend_from_slice(&val.to_ne_bytes());
}

/// 向 buffer 写入 i64。
fn buf_write_i64(buf: &mut Vec<u8>, val: i64) {
    buf.extend_from_slice(&val.to_ne_bytes());
}

/// 向 buffer 写入 f64。
fn buf_write_f64(buf: &mut Vec<u8>, val: f64) {
    buf.extend_from_slice(&val.to_ne_bytes());
}

/// 向 buffer 写入 bool（1 字节）。
fn buf_write_bool(buf: &mut Vec<u8>, val: bool) {
    buf.push(if val { 1 } else { 0 });
}

/// 向 buffer 写入字符串（usize len + raw bytes）。
fn buf_write_string(buf: &mut Vec<u8>, s: &str) {
    buf_write_usize(buf, s.len());
    buf.extend_from_slice(s.as_bytes());
}

/// 向 buffer 写入字符串数组。
fn buf_write_string_vec(buf: &mut Vec<u8>, vec: &[String]) {
    buf_write_usize(buf, vec.len());
    for s in vec {
        buf_write_string(buf, s);
    }
}

/// 向 buffer 写入 DataProxyValue。
fn buf_write_proxy_value(buf: &mut Vec<u8>, val: &DataProxyValue) {
    match val {
        DataProxyValue::Int(v) => {
            buf_write_usize(buf, 0);
            buf_write_i64(buf, *v);
        }
        DataProxyValue::Double(v) => {
            buf_write_usize(buf, 1);
            buf_write_f64(buf, *v);
        }
        DataProxyValue::String(v) => {
            buf_write_usize(buf, 2);
            buf_write_string(buf, v);
        }
        DataProxyValue::Bool(v) => {
            buf_write_usize(buf, 3);
            buf_write_bool(buf, *v);
        }
    }
}

/// 向 buffer 写入单条 DataShareProxyData。
/// 字段顺序：uri, isValueUndefined, isAllowListUndefined, [value], [allowList]
fn buf_write_proxy_data(buf: &mut Vec<u8>, data: &DataShareProxyData) {
    buf_write_string(buf, &data.uri);
    buf_write_bool(buf, data.is_value_undefined);
    buf_write_bool(buf, data.is_allow_list_undefined);
    if !data.is_value_undefined {
        buf_write_proxy_value(buf, &data.value);
    }
    if !data.is_allow_list_undefined {
        buf_write_string_vec(buf, &data.allow_list);
    }
}

/// 将 Vec<DataShareProxyData> 序列化为 buffer 模式写入 parcel。
/// C++ 线序：WriteInt32(blob_len) + WriteRawData(blob)
/// 使用 ffi_ipc_raw_data::write_raw_data 正确处理 > 32KB 的 ashmem 路径。
fn write_proxy_data_vec_to_parcel(items: &[DataShareProxyData], parcel: &mut MsgParcel) -> bool {
    let mut blob = Vec::new();
    buf_write_usize(&mut blob, items.len());
    for item in items {
        buf_write_proxy_data(&mut blob, item);
    }
    let blob_len = blob.len() as i32;
    // 第一个 size：MarshalProxyDataVec 手动写的
    if parcel.write(&blob_len).is_err() {
        return false;
    }
    // 使用 WriteRawData 写入 blob（自动处理 > 32KB 的 ashmem 路径）
    ffi_ipc_raw_data::write_raw_data(parcel, &blob).is_ok()
}

/// 将 Vec<String>（URI 列表）序列化为 buffer 模式写入 parcel。
/// 线序：WriteInt32(blob_len) + WriteBuffer(blob)
fn write_string_vec_to_parcel(uris: &[String], parcel: &mut MsgParcel) -> bool {
    let mut blob = Vec::new();
    buf_write_usize(&mut blob, uris.len());
    for uri in uris {
        buf_write_string(&mut blob, uri);
    }
    if parcel.write(&(blob.len() as i32)).is_err() {
        return false;
    }
    parcel.write_buffer(&blob).is_ok()
}

// =====================================================================
// Buffer 模式反序列化辅助函数（用于读取服务端 reply）
// =====================================================================

use std::io::{Cursor, Read as IoRead};

/// 从 cursor 读取 usize（平台字长）。
fn buf_read_usize(cursor: &mut Cursor<&[u8]>) -> Option<usize> {
    let mut buf = [0u8; std::mem::size_of::<usize>()];
    cursor.read_exact(&mut buf).ok()?;
    Some(usize::from_ne_bytes(buf))
}

/// 从 cursor 读取 i32。
fn buf_read_i32(cursor: &mut Cursor<&[u8]>) -> Option<i32> {
    let mut buf = [0u8; 4];
    cursor.read_exact(&mut buf).ok()?;
    Some(i32::from_ne_bytes(buf))
}

/// 从 cursor 读取 i64。
fn buf_read_i64(cursor: &mut Cursor<&[u8]>) -> Option<i64> {
    let mut buf = [0u8; 8];
    cursor.read_exact(&mut buf).ok()?;
    Some(i64::from_ne_bytes(buf))
}

/// 从 cursor 读取 f64。
fn buf_read_f64(cursor: &mut Cursor<&[u8]>) -> Option<f64> {
    let mut buf = [0u8; 8];
    cursor.read_exact(&mut buf).ok()?;
    Some(f64::from_ne_bytes(buf))
}

/// 从 cursor 读取 bool（1 字节）。
fn buf_read_bool(cursor: &mut Cursor<&[u8]>) -> Option<bool> {
    let mut buf = [0u8; 1];
    cursor.read_exact(&mut buf).ok()?;
    Some(buf[0] != 0)
}

/// 从 cursor 读取字符串（usize len + raw bytes）。
fn buf_read_string(cursor: &mut Cursor<&[u8]>) -> Option<String> {
    let len = buf_read_usize(cursor)?;
    let mut buf = vec![0u8; len];
    cursor.read_exact(&mut buf).ok()?;
    String::from_utf8(buf).ok()
}

/// 从 cursor 读取字符串数组。
fn buf_read_string_vec(cursor: &mut Cursor<&[u8]>) -> Option<Vec<String>> {
    let count = buf_read_usize(cursor)?;
    let mut vec = Vec::with_capacity(count);
    for _ in 0..count {
        vec.push(buf_read_string(cursor)?);
    }
    Some(vec)
}

/// 从 cursor 读取 DataProxyValue。
fn buf_read_proxy_value(cursor: &mut Cursor<&[u8]>) -> Option<DataProxyValue> {
    let type_index = buf_read_usize(cursor)?;
    match type_index {
        0 => Some(DataProxyValue::Int(buf_read_i64(cursor)?)),
        1 => Some(DataProxyValue::Double(buf_read_f64(cursor)?)),
        2 => Some(DataProxyValue::String(buf_read_string(cursor)?)),
        3 => Some(DataProxyValue::Bool(buf_read_bool(cursor)?)),
        _ => None,
    }
}

/// 从 cursor 读取单条 DataProxyGetResult。
/// 字段顺序：uri, result(i32), value, allowList
fn buf_read_proxy_get_result(cursor: &mut Cursor<&[u8]>) -> Option<DataProxyGetResult> {
    let uri = buf_read_string(cursor)?;
    let result_code = buf_read_i32(cursor)?;
    let value = buf_read_proxy_value(cursor)?;
    let allow_list = buf_read_string_vec(cursor)?;
    Some(DataProxyGetResult {
        uri,
        result: DataProxyErrorCode::from_i32(result_code),
        value,
        allow_list,
    })
}

/// 从 parcel 读取 buffer 模式的 Vec<DataProxyGetResult>。
/// 线序：ReadInt32(blob_len) + ReadRawData(blob)
/// ReadRawData 内部：ReadInt32(blob_len) + raw bytes
fn read_proxy_get_result_vec_from_parcel(parcel: &mut MsgParcel) -> Vec<DataProxyGetResult> {
    // 第一个 int32：MarshalDataProxyGetResultVec 写的 blob_len
    let blob_len: i32 = match parcel.read() {
        Ok(v) => v,
        Err(_) => return Vec::new(),
    };
    if blob_len <= 0 {
        return Vec::new();
    }
    // 第二个 int32：WriteRawData 内部写的 blob_len
    let _blob_len2: i32 = match parcel.read() {
        Ok(v) => v,
        Err(_) => return Vec::new(),
    };
    // raw bytes
    let raw = match parcel.read_buffer(blob_len as usize) {
        Ok(v) => v,
        Err(_) => return Vec::new(),
    };
    let mut cursor = Cursor::new(raw.as_slice());
    let count = match buf_read_usize(&mut cursor) {
        Some(c) => c,
        None => return Vec::new(),
    };
    let mut results = Vec::with_capacity(count);
    for _ in 0..count {
        match buf_read_proxy_get_result(&mut cursor) {
            Some(r) => results.push(r),
            None => break,
        }
    }
    results
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_constants() {
        assert_eq!(INVALID_VALUE, -1);
        assert_eq!(DATA_SHARE_ERROR, -1);
        assert_eq!(DESCRIPTOR, "OHOS.DataShare.IDataShareService");
    }

    #[test]
    fn test_system_flag() {
        DataShareServiceProxy::set_system(true);
        assert!(DataShareServiceProxy::is_system());
        DataShareServiceProxy::clean_system();
        assert!(!DataShareServiceProxy::is_system());
    }
}
