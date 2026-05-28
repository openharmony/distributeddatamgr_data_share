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

//! MsgParcel serialization implementations for DataShare types.
//!
//! Implements `ipc::parcel::Serialize` and `Deserialize` traits for DataShare types
//! to enable IPC marshalling via `ipc_rust` MsgParcel.

use ipc::parcel::{Deserialize, MsgParcel, Serialize};
use ipc::IpcResult;

use crate::observer::ChangeInfo;
use crate::operation::{BackReference, OperationStatement};
use crate::predicates::operations::{OperationItem, OperationType, SingleValue};
use crate::predicates::DataSharePredicates;
use crate::template::{PredicateTemplateNode, Template, TemplateId};
use crate::types::{
    AshmemNode, BatchUpdateResult, Data, DataProxyChangeInfo, DataProxyConfig, DataProxyErrorCode,
    DataProxyGetResult, DataProxyMaxValueLength, DataProxyResult, DataProxyType, DataProxyValue,
    DataShareProxyData, ExecResult, ExecResultSet, OperationResult, PublishedDataItem,
    PublishedDataValue, RdbChangeNode, UpdateOperation, UpdateOperations,
};
use crate::value_object::{DataShareValue, DataShareValueType};
use crate::values_bucket::DataShareValuesBucket;
use utils_rust::ashmem::Ashmem as UtilsAshmem;

// =====================================================================
// String serialization (already provided by ipc_rust)
// =====================================================================

// String implements Serialize/Deserialize in ipc_rust

// =====================================================================
// DataShareValuesBucket serialization
// =====================================================================

impl Serialize for DataShareValuesBucket {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        // Write as map: size + (key, value) pairs
        let size = self.size() as i32;
        parcel.write(&size)?;

        for (key, value) in self.get_all().iter() {
            parcel.write(key)?;
            parcel.write(value)?;
        }
        Ok(())
    }
}

impl Deserialize for DataShareValuesBucket {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let mut bucket = DataShareValuesBucket::new();
        let size: i32 = parcel.read()?;

        for _ in 0..size {
            let key: String = parcel.read()?;
            let value: DataShareValue = parcel.read()?;
            bucket.put(&key, value);
        }
        Ok(bucket)
    }
}

// =====================================================================
// DataShareValue serialization
// =====================================================================

impl Serialize for DataShareValue {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        match self {
            DataShareValue::Null => {
                parcel.write(&(DataShareValueType::Null as i32))?;
            }
            DataShareValue::Bool(v) => {
                parcel.write(&(DataShareValueType::Bool as i32))?;
                parcel.write(v)?;
            }
            DataShareValue::Int(v) => {
                parcel.write(&(DataShareValueType::Int as i32))?;
                parcel.write(v)?;
            }
            DataShareValue::Double(v) => {
                parcel.write(&(DataShareValueType::Double as i32))?;
                parcel.write(v)?;
            }
            DataShareValue::String(v) => {
                parcel.write(&(DataShareValueType::String as i32))?;
                parcel.write(v)?;
            }
            DataShareValue::Blob(v) => {
                parcel.write(&(DataShareValueType::Blob as i32))?;
                parcel.write(&(v.len() as i32))?;
                parcel.write_buffer(v)?;
            }
        }
        Ok(())
    }
}

impl Deserialize for DataShareValue {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let type_code: i32 = parcel.read()?;
        let value_type =
            DataShareValueType::from_i32(type_code).map_err(|_| ipc::IpcStatusCode::Failed)?;

        match value_type {
            DataShareValueType::Null => Ok(DataShareValue::Null),
            DataShareValueType::Bool => {
                let v: bool = parcel.read()?;
                Ok(DataShareValue::Bool(v))
            }
            DataShareValueType::Int => {
                let v: i64 = parcel.read()?;
                Ok(DataShareValue::Int(v))
            }
            DataShareValueType::Double => {
                let v: f64 = parcel.read()?;
                Ok(DataShareValue::Double(v))
            }
            DataShareValueType::String => {
                let v: String = parcel.read()?;
                Ok(DataShareValue::String(v))
            }
            DataShareValueType::Blob => {
                let len: i32 = parcel.read()?;
                let v = parcel.read_buffer(len as usize)?;
                Ok(DataShareValue::Blob(v))
            }
        }
    }
}

// =====================================================================
// DataSharePredicates serialization — standard IPC format
// =====================================================================
//
// Matches C++ Marshalling(const Predicates &, MessageParcel &) which uses
// ITypesUtil::Marshal(parcel, operations, whereClause, whereArgs, order, mode).
// Each OperationItem is marshalled as:
//   int32 operation + vector<SingleValue::Type> singleParams + vector<MutliValue::Type> multiParams
// Variant format: uint32 index + value data.

fn write_single_value_variant(parcel: &mut MsgParcel, v: &SingleValue) -> IpcResult<()> {
    match v {
        SingleValue::Null => {
            parcel.write(&0u32)?;
        }
        SingleValue::Int(i) => {
            parcel.write(&1u32)?;
            parcel.write(i)?;
        }
        SingleValue::Double(d) => {
            parcel.write(&2u32)?;
            parcel.write(d)?;
        }
        SingleValue::String(s) => {
            parcel.write(&3u32)?;
            parcel.write(s)?;
        }
        SingleValue::Bool(b) => {
            parcel.write(&4u32)?;
            parcel.write(b)?;
        }
        SingleValue::Long(l) => {
            parcel.write(&5u32)?;
            parcel.write(l)?;
        }
        SingleValue::Blob(_) => {
            parcel.write(&0u32)?;
        }
    }
    Ok(())
}

fn read_single_value_variant(parcel: &mut MsgParcel) -> IpcResult<SingleValue> {
    let index: u32 = parcel.read()?;
    match index {
        0 => Ok(SingleValue::Null),
        1 => Ok(SingleValue::Int(parcel.read()?)),
        2 => Ok(SingleValue::Double(parcel.read()?)),
        3 => Ok(SingleValue::String(parcel.read()?)),
        4 => Ok(SingleValue::Bool(parcel.read()?)),
        5 => Ok(SingleValue::Long(parcel.read()?)),
        _ => Err(ipc::IpcStatusCode::Failed),
    }
}

fn write_multi_value_variant(parcel: &mut MsgParcel, values: &[SingleValue]) -> IpcResult<()> {
    if values.is_empty() {
        parcel.write(&0u32)?;
        return Ok(());
    }
    match &values[0] {
        SingleValue::Int(_) => {
            parcel.write(&1u32)?;
            let v: Vec<i32> = values
                .iter()
                .filter_map(|x| {
                    if let SingleValue::Int(i) = x {
                        Some(*i)
                    } else {
                        None
                    }
                })
                .collect();
            parcel.write(&(v.len() as i32))?;
            for i in &v {
                parcel.write(i)?;
            }
        }
        SingleValue::Long(_) => {
            parcel.write(&2u32)?;
            let v: Vec<i64> = values
                .iter()
                .filter_map(|x| {
                    if let SingleValue::Long(l) = x {
                        Some(*l)
                    } else {
                        None
                    }
                })
                .collect();
            parcel.write(&(v.len() as i32))?;
            for l in &v {
                parcel.write(l)?;
            }
        }
        SingleValue::Double(_) => {
            parcel.write(&3u32)?;
            let v: Vec<f64> = values
                .iter()
                .filter_map(|x| {
                    if let SingleValue::Double(d) = x {
                        Some(*d)
                    } else {
                        None
                    }
                })
                .collect();
            parcel.write(&(v.len() as i32))?;
            for d in &v {
                parcel.write(d)?;
            }
        }
        SingleValue::String(_) => {
            parcel.write(&4u32)?;
            let v: Vec<&str> = values
                .iter()
                .filter_map(|x| {
                    if let SingleValue::String(s) = x {
                        Some(s.as_str())
                    } else {
                        None
                    }
                })
                .collect();
            parcel.write(&(v.len() as i32))?;
            for s in &v {
                parcel.write(&s.to_string())?;
            }
        }
        _ => {
            parcel.write(&0u32)?;
        }
    }
    Ok(())
}

fn read_multi_value_variant(parcel: &mut MsgParcel) -> IpcResult<Vec<SingleValue>> {
    let index: u32 = parcel.read()?;
    match index {
        0 => Ok(Vec::new()),
        1 => {
            let count: i32 = parcel.read()?;
            (0..count)
                .map(|_| Ok(SingleValue::Int(parcel.read()?)))
                .collect()
        }
        2 => {
            let count: i32 = parcel.read()?;
            (0..count)
                .map(|_| Ok(SingleValue::Long(parcel.read()?)))
                .collect()
        }
        3 => {
            let count: i32 = parcel.read()?;
            (0..count)
                .map(|_| Ok(SingleValue::Double(parcel.read()?)))
                .collect()
        }
        4 => {
            let count: i32 = parcel.read()?;
            (0..count)
                .map(|_| Ok(SingleValue::String(parcel.read()?)))
                .collect()
        }
        _ => Err(ipc::IpcStatusCode::Failed),
    }
}

impl Serialize for DataSharePredicates {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        let ops = self.get_operation_list();
        parcel.write(&(ops.len() as i32))?;
        for item in ops {
            parcel.write(&(item.operation as i32))?;

            let mut sp_count: i32 = 0;
            if !item.field.is_empty() {
                sp_count += 1;
            }
            if item.value.is_some() {
                sp_count += 1;
            }
            sp_count += item.extra_single_params.len() as i32;
            parcel.write(&sp_count)?;
            if !item.field.is_empty() {
                write_single_value_variant(parcel, &SingleValue::String(item.field.clone()))?;
            }
            if let Some(ref v) = item.value {
                write_single_value_variant(parcel, v)?;
            }
            for v in &item.extra_single_params {
                write_single_value_variant(parcel, v)?;
            }

            let mp_count: i32 = if item.multi_values.is_some() { 1 } else { 0 };
            parcel.write(&mp_count)?;
            if let Some(ref multi) = item.multi_values {
                write_multi_value_variant(parcel, multi)?;
            }
        }

        parcel.write(&self.get_where_clause().to_string())?;

        let args = self.get_where_args();
        parcel.write(&(args.len() as i32))?;
        for arg in args {
            parcel.write(arg)?;
        }

        parcel.write(&self.get_order().to_string())?;
        parcel.write(&self.get_setting_mode())?;
        Ok(())
    }
}

impl Deserialize for DataSharePredicates {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let ops_count: i32 = parcel.read()?;
        let mut operations = Vec::with_capacity(ops_count.max(0) as usize);
        for _ in 0..ops_count {
            let op: i32 = parcel.read()?;
            let operation = OperationType::from_i32(op).ok_or(ipc::IpcStatusCode::Failed)?;

            let sp_count: i32 = parcel.read()?;
            let mut single_params = Vec::with_capacity(sp_count.max(0) as usize);
            for _ in 0..sp_count {
                single_params.push(read_single_value_variant(parcel)?);
            }

            let mp_count: i32 = parcel.read()?;
            let multi_values = if mp_count > 0 {
                let vals = read_multi_value_variant(parcel)?;
                for _ in 1..mp_count {
                    let _ = read_multi_value_variant(parcel)?;
                }
                if vals.is_empty() {
                    None
                } else {
                    Some(vals)
                }
            } else {
                None
            };

            let (field, value, extra_single_params) = match single_params.len() {
                0 => (String::new(), None, Vec::new()),
                1 => {
                    let param = single_params.into_iter().next().unwrap();
                    if matches!(operation, OperationType::Limit | OperationType::Offset) {
                        (String::new(), Some(param), Vec::new())
                    } else if let SingleValue::String(s) = param {
                        (s, None, Vec::new())
                    } else {
                        (String::new(), Some(param), Vec::new())
                    }
                }
                _ => {
                    let mut iter = single_params.into_iter();
                    let first = iter.next().unwrap();
                    let second = iter.next();
                    let extra: Vec<SingleValue> = iter.collect();
                    if let SingleValue::String(s) = first {
                        (s, second, extra)
                    } else {
                        let mut e = Vec::new();
                        if let Some(v) = second {
                            e.push(v);
                        }
                        e.extend(extra);
                        (String::new(), Some(first), e)
                    }
                }
            };

            operations.push(OperationItem {
                operation,
                field,
                value,
                multi_values,
                extra_single_params,
            });
        }

        let where_clause: String = parcel.read()?;

        let args_count: i32 = parcel.read()?;
        let mut where_args = Vec::with_capacity(args_count.max(0) as usize);
        for _ in 0..args_count {
            where_args.push(parcel.read::<String>()?);
        }

        let order: String = parcel.read()?;
        let mode: i16 = parcel.read()?;

        let mut predicates = DataSharePredicates::from_operations(operations);
        predicates.set_where_clause(&where_clause);
        predicates.set_where_args(where_args);
        predicates.set_order(&order);
        predicates.set_setting_mode(mode);
        Ok(predicates)
    }
}

// =====================================================================
// UpdateOperation serialization
// =====================================================================

impl Serialize for UpdateOperation {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&self.values_bucket)?;
        parcel.write(&self.predicates)?;
        Ok(())
    }
}

impl Deserialize for UpdateOperation {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let values_bucket = parcel.read()?;
        let predicates = parcel.read()?;
        Ok(UpdateOperation {
            values_bucket,
            predicates,
        })
    }
}

// =====================================================================
// UpdateOperations (BTreeMap) serialization
// =====================================================================

impl Serialize for UpdateOperations {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&(self.len() as i32))?;
        for (uri, ops) in self.iter() {
            parcel.write(uri)?;
            parcel.write(&(ops.len() as i32))?;
            for op in ops {
                parcel.write(op)?;
            }
        }
        Ok(())
    }
}

impl Deserialize for UpdateOperations {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let mut result = UpdateOperations::new();
        let map_size: i32 = parcel.read()?;

        for _ in 0..map_size {
            let uri: String = parcel.read()?;
            let ops_count: i32 = parcel.read()?;
            let mut ops = Vec::new();
            for _ in 0..ops_count {
                ops.push(parcel.read()?);
            }
            result.insert(uri, ops);
        }
        Ok(result)
    }
}

// =====================================================================
// BatchUpdateResult serialization
// =====================================================================

impl Serialize for BatchUpdateResult {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&self.uri)?;
        parcel.write(&(self.codes.len() as i32))?;
        for code in &self.codes {
            parcel.write(code)?;
        }
        Ok(())
    }
}

impl Deserialize for BatchUpdateResult {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let uri = parcel.read()?;
        let codes_count: i32 = parcel.read()?;
        let mut codes = Vec::new();
        for _ in 0..codes_count {
            codes.push(parcel.read()?);
        }
        Ok(BatchUpdateResult { uri, codes })
    }
}

// =====================================================================
// ExecResult serialization
// =====================================================================

impl Serialize for ExecResult {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&(self.operation_type as i32))?;
        parcel.write(&self.code)?;
        parcel.write(&self.message)?;
        Ok(())
    }
}

impl Deserialize for ExecResult {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let op_type: i32 = parcel.read()?;
        let code: i32 = parcel.read()?;
        let message: String = parcel.read()?;
        Ok(ExecResult {
            operation_type: crate::operation::Operation::from_i32(op_type)
                .ok_or(ipc::IpcStatusCode::Failed)?,
            code,
            message,
        })
    }
}

// =====================================================================
// ExecResultSet serialization
// =====================================================================

impl Serialize for ExecResultSet {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&(self.error_code as i32))?;
        parcel.write(&(self.results.len() as i32))?;
        for result in &self.results {
            parcel.write(result)?;
        }
        Ok(())
    }
}

impl Deserialize for ExecResultSet {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let error_code_val: i32 = parcel.read()?;
        let error_code = crate::types::ExecErrorCode::from_i32(error_code_val);
        let results_count: i32 = parcel.read()?;
        let mut results = Vec::new();
        for _ in 0..results_count {
            results.push(parcel.read()?);
        }
        Ok(ExecResultSet {
            error_code,
            results,
        })
    }
}

// =====================================================================
// ChangeInfo serialization
// =====================================================================

impl Serialize for ChangeInfo {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&(self.change_type as u32))?;
        parcel.write(&(self.uris.len() as i32))?;
        for uri in &self.uris {
            parcel.write(uri)?;
        }
        parcel.write(&(self.data.len() as i32))?;
        if !self.data.is_empty() {
            parcel.write_buffer(&self.data)?;
        }
        parcel.write(&(self.value_buckets.len() as i32))?;
        for bucket in &self.value_buckets {
            parcel.write(&(bucket.len() as i32))?;
            for (k, v) in bucket {
                parcel.write(k)?;
                parcel.write(v)?;
            }
        }
        Ok(())
    }
}

impl Deserialize for ChangeInfo {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let change_type_val: u32 = parcel.read()?;
        let change_type = crate::observer::ChangeType::from_u32(change_type_val);

        let uris_count: i32 = parcel.read()?;
        let mut uris = Vec::new();
        for _ in 0..uris_count {
            uris.push(parcel.read()?);
        }

        let data_len: i32 = parcel.read()?;
        let data = if data_len > 0 {
            parcel.read_buffer(data_len as usize)?
        } else {
            Vec::new()
        };

        let buckets_count: i32 = parcel.read()?;
        let mut value_buckets = Vec::new();
        for _ in 0..buckets_count {
            let bucket_size: i32 = parcel.read()?;
            let mut bucket = std::collections::BTreeMap::new();
            for _ in 0..bucket_size {
                let k: String = parcel.read()?;
                let v: DataShareValue = parcel.read()?;
                bucket.insert(k, v);
            }
            value_buckets.push(bucket);
        }

        Ok(ChangeInfo {
            change_type,
            uris,
            data,
            value_buckets,
        })
    }
}

// =====================================================================
// PublishedDataItem serialization
// =====================================================================

impl Serialize for PublishedDataValue {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        match self {
            PublishedDataValue::Ashmem(node) => {
                parcel.write(&0u32)?; // variant index 0 = AshmemNode
                                      // Write Ashmem via parcel.write_ashmem (takes by value; clone SharedPtr).
                let ashmem = match node.clone_ashmem() {
                    Some(a) => a,
                    None => return Err(ipc::IpcStatusCode::Failed),
                };
                parcel.write_ashmem(ashmem)?;
            }
            PublishedDataValue::String(s) => {
                parcel.write(&1u32)?; // variant index 1 = String
                parcel.write(s)?;
            }
        }
        Ok(())
    }
}

impl Deserialize for PublishedDataValue {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let variant: u32 = parcel.read()?;
        if variant == 0 {
            let ashmem = parcel.read_ashmem()?;
            Ok(PublishedDataValue::Ashmem(AshmemNode {
                ashmem: Some(ashmem),
                is_managed: true,
            }))
        } else {
            let s: String = parcel.read()?;
            Ok(PublishedDataValue::String(s))
        }
    }
}

impl Serialize for PublishedDataItem {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&self.key)?;
        parcel.write(&self.subscriber_id)?;
        parcel.write(&self.value)?;
        Ok(())
    }
}

impl Deserialize for PublishedDataItem {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let key: String = parcel.read()?;
        let subscriber_id: i64 = parcel.read()?;
        let value: PublishedDataValue = parcel.read()?;
        Ok(PublishedDataItem {
            key,
            subscriber_id,
            value,
        })
    }
}

// =====================================================================
// OperationStatement serialization
// =====================================================================

impl Serialize for OperationStatement {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&(self.operation_type as i32))?;
        parcel.write(&self.uri)?;
        parcel.write(&self.values_bucket)?;
        parcel.write(&self.predicates)?;
        Ok(())
    }
}

impl Deserialize for OperationStatement {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let op_type: i32 = parcel.read()?;
        let uri: String = parcel.read()?;
        let values_bucket = parcel.read()?;
        let predicates = parcel.read()?;
        Ok(OperationStatement {
            operation_type: crate::operation::Operation::from_i32(op_type)
                .unwrap_or(crate::operation::Operation::Insert),
            uri,
            predicates,
            values_bucket,
            back_reference: BackReference::default(),
        })
    }
}

// =====================================================================
// OperationResult serialization
// =====================================================================

impl Serialize for OperationResult {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&self.key)?;
        parcel.write(&self.err_code)?;
        Ok(())
    }
}

impl Deserialize for OperationResult {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let key: String = parcel.read()?;
        let err_code: i32 = parcel.read()?;
        Ok(OperationResult { key, err_code })
    }
}

// =====================================================================
// DataProxyValue serialization
// =====================================================================

impl Serialize for DataProxyValue {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&(self.type_index() as i32))?;
        match self {
            DataProxyValue::Int(v) => parcel.write(v)?,
            DataProxyValue::Double(v) => parcel.write(v)?,
            DataProxyValue::String(v) => parcel.write(v)?,
            DataProxyValue::Bool(v) => parcel.write(v)?,
        }
        Ok(())
    }
}

impl Deserialize for DataProxyValue {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let type_index: i32 = parcel.read()?;
        match type_index {
            0 => Ok(DataProxyValue::Int(parcel.read()?)),
            1 => Ok(DataProxyValue::Double(parcel.read()?)),
            2 => Ok(DataProxyValue::String(parcel.read()?)),
            3 => Ok(DataProxyValue::Bool(parcel.read()?)),
            _ => Err(ipc::IpcStatusCode::Failed),
        }
    }
}

// =====================================================================
// DataProxyConfig serialization
// =====================================================================

impl Serialize for DataProxyConfig {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&(self.proxy_type as i32))?;
        parcel.write(&(self.max_value_length as i32))?;
        Ok(())
    }
}

impl Deserialize for DataProxyConfig {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let _proxy_type: i32 = parcel.read()?;
        let max_value_length: i32 = parcel.read()?;
        Ok(DataProxyConfig {
            proxy_type: DataProxyType::SharedConfig,
            max_value_length: DataProxyMaxValueLength::from_i32(max_value_length)
                .unwrap_or(DataProxyMaxValueLength::MaxLength4K),
        })
    }
}

// =====================================================================
// DataShareProxyData serialization
// =====================================================================

impl Serialize for DataShareProxyData {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        // C++ wire order: uri, value, isValueUndefined, allowList, isAllowListUndefined
        parcel.write(&self.uri)?;
        parcel.write(&self.value)?;
        parcel.write(&self.is_value_undefined)?;
        parcel.write(&(self.allow_list.len() as i32))?;
        for item in &self.allow_list {
            parcel.write(item)?;
        }
        parcel.write(&self.is_allow_list_undefined)?;
        Ok(())
    }
}

impl Deserialize for DataShareProxyData {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let uri: String = parcel.read()?;
        let value: DataProxyValue = parcel.read()?;
        let is_value_undefined: bool = parcel.read()?;
        let count: i32 = parcel.read()?;
        let mut allow_list = Vec::new();
        for _ in 0..count {
            allow_list.push(parcel.read()?);
        }
        let is_allow_list_undefined: bool = parcel.read()?;
        Ok(DataShareProxyData {
            uri,
            value,
            allow_list,
            is_value_undefined,
            is_allow_list_undefined,
        })
    }
}

// =====================================================================
// DataProxyResult serialization
// =====================================================================

impl Serialize for DataProxyResult {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        // C++ wire order: errorCode first, then uri
        parcel.write(&(self.result as i32))?;
        parcel.write(&self.uri)?;
        Ok(())
    }
}

impl Deserialize for DataProxyResult {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let result_code: i32 = parcel.read()?;
        let uri: String = parcel.read()?;
        Ok(DataProxyResult {
            uri,
            result: DataProxyErrorCode::from_i32(result_code),
        })
    }
}

// =====================================================================
// DataProxyGetResult serialization
// =====================================================================

impl Serialize for DataProxyGetResult {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        // C++ wire order: errorCode, uri, value, allowList
        parcel.write(&(self.result as i32))?;
        parcel.write(&self.uri)?;
        parcel.write(&self.value)?;
        parcel.write(&(self.allow_list.len() as i32))?;
        for item in &self.allow_list {
            parcel.write(item)?;
        }
        Ok(())
    }
}

impl Deserialize for DataProxyGetResult {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let result_code: i32 = parcel.read()?;
        let uri: String = parcel.read()?;
        let value: DataProxyValue = parcel.read()?;
        let count: i32 = parcel.read()?;
        let mut allow_list = Vec::new();
        for _ in 0..count {
            allow_list.push(parcel.read()?);
        }
        Ok(DataProxyGetResult {
            uri,
            result: DataProxyErrorCode::from_i32(result_code),
            value,
            allow_list,
        })
    }
}

// =====================================================================
// DataProxyChangeInfo serialization
// =====================================================================

impl Serialize for DataProxyChangeInfo {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        // C++ wire order: int32_t changeType, string uri, DataProxyValue value
        parcel.write(&(self.change_type as i32))?;
        parcel.write(&self.uri)?;
        parcel.write(&self.value)?;
        Ok(())
    }
}

impl Deserialize for DataProxyChangeInfo {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let change_type_val: i32 = parcel.read()?;
        let uri: String = parcel.read()?;
        let value: DataProxyValue = parcel.read()?;
        Ok(DataProxyChangeInfo {
            change_type: crate::observer::ChangeType::from_u32(change_type_val as u32),
            uri,
            value,
        })
    }
}

// =====================================================================
// TemplateId serialization
// =====================================================================

impl Serialize for TemplateId {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&self.subscriber_id)?;
        parcel.write(&self.bundle_name)?;
        Ok(())
    }
}

impl Deserialize for TemplateId {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let subscriber_id: i64 = parcel.read()?;
        let bundle_name: String = parcel.read()?;
        Ok(TemplateId::new(subscriber_id, bundle_name))
    }
}

// =====================================================================
// RdbChangeNode serialization
// =====================================================================
//
// C++ wire format (see datashare_itypes_utils.cpp):
//   Marshal(parcel, uri, templateId, data, isSharedMemory)
//   if isSharedMemory { WriteAshmem(memory) }
//   Marshal(parcel, size)

impl Serialize for RdbChangeNode {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&self.uri)?;
        parcel.write(&self.template_id)?;
        parcel.write(&(self.data.len() as i32))?;
        for s in &self.data {
            parcel.write(s)?;
        }
        parcel.write(&self.is_shared_memory)?;
        if self.is_shared_memory {
            // Clone the SharedPtr since write_ashmem takes Ashmem by value.
            let ashmem = match &self.memory {
                Some(a) => {
                    let shared = unsafe { a.c_ashmem() }.clone();
                    UtilsAshmem::new(shared)
                }
                None => return Err(ipc::IpcStatusCode::Failed),
            };
            parcel.write_ashmem(ashmem)?;
        }
        parcel.write(&self.size)?;
        Ok(())
    }
}

impl Deserialize for RdbChangeNode {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let uri: String = parcel.read()?;
        let template_id: TemplateId = parcel.read()?;
        let data_count: i32 = parcel.read()?;
        let mut data = Vec::with_capacity(data_count.max(0) as usize);
        for _ in 0..data_count {
            data.push(parcel.read()?);
        }
        let is_shared_memory: bool = parcel.read()?;
        let memory = if is_shared_memory {
            Some(parcel.read_ashmem()?)
        } else {
            None
        };
        let size: i32 = parcel.read()?;
        Ok(RdbChangeNode {
            uri,
            template_id,
            data,
            is_shared_memory,
            memory,
            size,
        })
    }
}

// =====================================================================
// Data serialization (PublishedData container)
// =====================================================================
//
// C++ wire format (datashare_itypes_utils.cpp):
//   Marshal(parcel, data.datas_, data.version_)
//   = int32 count + PublishedDataItem[count] + int32 version

impl Serialize for Data {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&(self.datas.len() as i32))?;
        for item in &self.datas {
            parcel.write(item)?;
        }
        parcel.write(&self.version)?;
        Ok(())
    }
}

impl Deserialize for Data {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let count: i32 = parcel.read()?;
        let mut datas = Vec::with_capacity(count.max(0) as usize);
        for _ in 0..count {
            datas.push(parcel.read::<PublishedDataItem>()?);
        }
        let version: i32 = parcel.read()?;
        Ok(Data { datas, version })
    }
}

// =====================================================================
// Template serialization
// =====================================================================
//
// C++ wire format (data_share_service_proxy.cpp):
//   Marshal(parcel, tpl.update_, tpl.predicates_, tpl.scheduler_)
//   = String update + (int32 count + PredicateTemplateNode[count]) + String scheduler
//   PredicateTemplateNode = String key + String selectSql

impl Serialize for Template {
    fn serialize(&self, parcel: &mut MsgParcel) -> IpcResult<()> {
        parcel.write(&self.update)?;
        parcel.write(&(self.predicates.len() as i32))?;
        for node in &self.predicates {
            parcel.write(&node.key)?;
            parcel.write(&node.select_sql)?;
        }
        parcel.write(&self.scheduler)?;
        Ok(())
    }
}

impl Deserialize for Template {
    fn deserialize(parcel: &mut MsgParcel) -> IpcResult<Self> {
        let update: String = parcel.read()?;
        let count: i32 = parcel.read()?;
        let mut predicates = Vec::with_capacity(count.max(0) as usize);
        for _ in 0..count {
            let key: String = parcel.read()?;
            let select_sql: String = parcel.read()?;
            predicates.push(PredicateTemplateNode::new(key, select_sql));
        }
        let scheduler: String = parcel.read()?;
        Ok(Template {
            update,
            predicates,
            scheduler,
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::template::TemplateId;

    #[test]
    fn test_value_serialization() {
        let val = DataShareValue::String("test".to_string());
        let mut parcel = MsgParcel::new();
        assert!(val.serialize(&mut parcel).is_ok());
        parcel.set_read_position(0);
        let deserialized: DataShareValue = parcel.read().unwrap();
        assert_eq!(deserialized, val);
    }

    #[test]
    fn test_values_bucket_serialization() {
        let mut bucket = DataShareValuesBucket::new();
        bucket.put("key1", DataShareValue::Int(42));
        bucket.put("key2", DataShareValue::String("value".to_string()));

        let mut parcel = MsgParcel::new();
        assert!(bucket.serialize(&mut parcel).is_ok());
        parcel.set_read_position(0);
        let deserialized: DataShareValuesBucket = parcel.read().unwrap();
        assert_eq!(deserialized.len(), 2);
    }

    #[test]
    fn test_published_data_item_string_roundtrip() {
        let item = PublishedDataItem {
            key: "test_key".to_string(),
            subscriber_id: 42,
            value: PublishedDataValue::String("hello".to_string()),
        };
        let mut parcel = MsgParcel::new();
        assert!(item.serialize(&mut parcel).is_ok());
        parcel.set_read_position(0);
        let deserialized: PublishedDataItem = parcel.read().unwrap();
        assert_eq!(deserialized.key, "test_key");
        assert_eq!(deserialized.subscriber_id, 42);
        match deserialized.value {
            PublishedDataValue::String(s) => assert_eq!(s, "hello"),
            _ => panic!("expected String variant"),
        }
    }

    #[test]
    fn test_published_data_item_ashmem_roundtrip() {
        use crate::types::PublishedDataType;
        let blob = vec![1u8, 2, 3, 4];
        let item = PublishedDataItem::new(
            "blob_key".to_string(),
            99,
            PublishedDataType::Blob(blob.clone()),
        );
        let mut parcel = MsgParcel::new();
        assert!(item.serialize(&mut parcel).is_ok());
        parcel.set_read_position(0);
        let deserialized: PublishedDataItem = parcel.read().unwrap();
        assert_eq!(deserialized.key, "blob_key");
        match deserialized.get_data() {
            PublishedDataType::Blob(data) => assert_eq!(data, blob),
            _ => panic!("expected Blob variant"),
        }
    }

    #[test]
    fn test_template_id_roundtrip() {
        let tid = TemplateId::new(42, "com.example.app".to_string());
        let mut parcel = MsgParcel::new();
        assert!(tid.serialize(&mut parcel).is_ok());
        parcel.set_read_position(0);
        let got: TemplateId = parcel.read().unwrap();
        assert_eq!(got.subscriber_id, 42);
        assert_eq!(got.bundle_name, "com.example.app");
    }

    #[test]
    fn test_rdb_change_node_roundtrip_no_shm() {
        let node = RdbChangeNode {
            uri: "datashare:///test".to_string(),
            template_id: TemplateId::new(1, "b".to_string()),
            data: vec!["row1".to_string(), "row2".to_string()],
            is_shared_memory: false,
            memory: None,
            size: 2,
        };
        let mut parcel = MsgParcel::new();
        assert!(node.serialize(&mut parcel).is_ok());
        parcel.set_read_position(0);
        let got: RdbChangeNode = parcel.read().unwrap();
        assert_eq!(got.uri, "datashare:///test");
        assert_eq!(got.data.len(), 2);
        assert!(!got.is_shared_memory);
        assert!(got.memory.is_none());
        assert_eq!(got.size, 2);
    }

    #[test]
    fn test_rdb_change_node_roundtrip_with_shm() {
        use utils_rust::ashmem::create_ashmem_instance;
        let ashmem = unsafe { create_ashmem_instance("RdbChangeNodeTest", 64) };
        let ashmem = match ashmem {
            Some(a) => a,
            None => return, // environment may not support ashmem
        };
        let node = RdbChangeNode {
            uri: "datashare:///shm".to_string(),
            template_id: TemplateId::new(7, "bundle".to_string()),
            data: vec!["r".to_string()],
            is_shared_memory: true,
            memory: Some(ashmem),
            size: 1,
        };
        let mut parcel = MsgParcel::new();
        assert!(node.serialize(&mut parcel).is_ok());
        parcel.set_read_position(0);
        let got: RdbChangeNode = parcel.read().unwrap();
        assert!(got.is_shared_memory);
        assert!(got.memory.is_some());
    }

    #[test]
    fn test_operation_statement_roundtrip() {
        use crate::operation::{Operation, OperationStatement};
        let stmt = OperationStatement::new(Operation::Update, "datashare:///test".to_string());
        let mut parcel = MsgParcel::new();
        assert!(stmt.serialize(&mut parcel).is_ok());
        parcel.set_read_position(0);
        let deserialized: OperationStatement = parcel.read().unwrap();
        assert_eq!(deserialized.operation_type, Operation::Update);
        assert_eq!(deserialized.uri, "datashare:///test");
    }
}
