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

//! Additional IPC data types for datashare serialization.
//!
//! These types correspond to C++ structs in `datashare_template.h`,
//! `datashare_operation_statement.h`, `dataproxy_handle_common.h`,
//! and `datashare_common.h`.

use crate::observer::ChangeType;
use crate::operation::Operation;
use crate::predicates::DataSharePredicates;
use crate::template::TemplateId;
use crate::values_bucket::DataShareValuesBucket;

use utils_rust::ashmem::{create_ashmem_instance, Ashmem as UtilsAshmem, PROT_READ};

// --- Operation statement types ---

/// Update operation: a values bucket + predicates pair.
/// Equivalent to C++ `UpdateOperation`.
#[derive(Debug, Clone)]
pub struct UpdateOperation {
    pub values_bucket: DataShareValuesBucket,
    pub predicates: DataSharePredicates,
}

/// Map of URI -> list of UpdateOperation.
/// Equivalent to C++ `UpdateOperations`.
#[derive(Debug, Clone, Default)]
pub struct UpdateOperations(pub std::collections::BTreeMap<String, Vec<UpdateOperation>>);

impl UpdateOperations {
    pub fn new() -> Self {
        Self(std::collections::BTreeMap::new())
    }

    pub fn insert(
        &mut self,
        key: String,
        value: Vec<UpdateOperation>,
    ) -> Option<Vec<UpdateOperation>> {
        self.0.insert(key, value)
    }

    pub fn get(&self, key: &str) -> Option<&Vec<UpdateOperation>> {
        self.0.get(key)
    }

    pub fn iter(&self) -> std::collections::btree_map::Iter<String, Vec<UpdateOperation>> {
        self.0.iter()
    }

    pub fn len(&self) -> usize {
        self.0.len()
    }

    pub fn is_empty(&self) -> bool {
        self.0.is_empty()
    }
}

/// Result of a batch update operation.
/// Equivalent to C++ `BatchUpdateResult`.
#[derive(Debug, Clone)]
pub struct BatchUpdateResult {
    pub uri: String,
    pub codes: Vec<i32>,
}

/// Error codes for exec operations.
/// Equivalent to C++ `ExecErrorCode`.
#[repr(i32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ExecErrorCode {
    Success = 0,
    Failed = 1,
    PartialSuccess = 2,
}

impl ExecErrorCode {
    pub fn from_i32(val: i32) -> Self {
        match val {
            0 => ExecErrorCode::Success,
            1 => ExecErrorCode::Failed,
            2 => ExecErrorCode::PartialSuccess,
            _ => ExecErrorCode::Failed,
        }
    }
}

/// Result of a single exec operation.
/// Equivalent to C++ `ExecResult`.
#[derive(Debug, Clone)]
pub struct ExecResult {
    pub operation_type: Operation,
    pub code: i32,
    pub message: String,
}

/// Result set for exec operations.
/// Equivalent to C++ `ExecResultSet`.
#[derive(Debug, Clone)]
pub struct ExecResultSet {
    pub error_code: ExecErrorCode,
    pub results: Vec<ExecResult>,
}

/// Option for registering observer.
/// Equivalent to C++ `RegisterOption`.
#[derive(Debug, Clone, Copy)]
pub struct RegisterOption {
    pub is_reconnect: bool,
}

// --- Template & subscription types ---

/// Ashmem node for cross-process shared memory.
///
/// Equivalent to C++ `AshmemNode { sptr<Ashmem> ashmem; bool isManaged; }`.
///
/// The `ashmem` field wraps `utils_rust::ashmem::Ashmem`, which in turn wraps
/// `cxx::SharedPtr<ffi::Ashmem>` (atomic reference counted, shared with C++).
#[derive(Default)]
pub struct AshmemNode {
    /// Underlying Ashmem handle (None if invalid/empty).
    pub ashmem: Option<UtilsAshmem>,
    /// Whether this node owns/manages the ashmem lifecycle (unmap+close on drop).
    pub is_managed: bool,
}

// SharedPtr<ffi::Ashmem> uses atomic reference counting and C++ uses it across threads.
unsafe impl Send for AshmemNode {}

impl std::fmt::Debug for AshmemNode {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("AshmemNode")
            .field("fd", &self.fd())
            .field("size", &self.size())
            .field("is_managed", &self.is_managed)
            .finish()
    }
}

impl AshmemNode {
    /// Ashmem FD, or -1 if none.
    pub fn fd(&self) -> i32 {
        self.ashmem
            .as_ref()
            .map(|a| a.get_ashmem_fd())
            .unwrap_or(-1)
    }

    /// Ashmem size, or 0 if none.
    pub fn size(&self) -> i32 {
        self.ashmem
            .as_ref()
            .map(|a| a.get_ashmem_size())
            .unwrap_or(0)
    }

    /// Clone the underlying SharedPtr to share ownership with another node.
    pub fn clone_ashmem(&self) -> Option<UtilsAshmem> {
        self.ashmem.as_ref().map(|a| {
            let shared = unsafe { a.c_ashmem() }.clone();
            UtilsAshmem::new(shared)
        })
    }
}

/// Published data item value: either an Ashmem-backed blob or a String.
/// Equivalent to C++ `PublishedDataItem::value_` which is
/// `variant<AshmemNode, string>` (index 0 = AshmemNode, index 1 = string).
#[derive(Debug)]
pub enum PublishedDataValue {
    Ashmem(AshmemNode),
    String(String),
}

impl Default for PublishedDataValue {
    fn default() -> Self {
        PublishedDataValue::String(String::new())
    }
}

/// Published data item data type: either blob or string.
/// Equivalent to C++ `PublishedDataItem::DataType` (variant<vector<uint8_t>, string>).
#[derive(Debug, Clone)]
pub enum PublishedDataType {
    Blob(Vec<u8>),
    String(String),
}

/// Published data item.
///
/// Equivalent to C++ `PublishedDataItem` in `datashare_template.h/cpp`.
///
/// When created with blob data, the C++ implementation creates an Ashmem region
/// and writes the blob into it for efficient cross-process transfer.
/// In this Rust implementation, blob data is stored in a local `Vec<u8>` buffer
/// within `AshmemNode` until Ashmem C-FFI is integrated.
#[derive(Debug)]
pub struct PublishedDataItem {
    pub key: String,
    pub subscriber_id: i64,
    pub value: PublishedDataValue,
}

impl PublishedDataItem {
    /// Create a new PublishedDataItem.
    ///
    /// Equivalent to C++ `PublishedDataItem(string key, int64_t subscriberId, DataType value)`.
    /// For blob data, creates an AshmemNode to hold the data.
    /// For string data, stores the string directly.
    pub fn new(key: String, subscriber_id: i64, data: PublishedDataType) -> Self {
        let mut item = Self {
            key,
            subscriber_id,
            value: PublishedDataValue::String(String::new()),
        };
        item.set(data);
        item
    }

    /// Set the value, replacing any previous value.
    ///
    /// Equivalent to C++ `PublishedDataItem::Set(DataType& value)`.
    /// Calls `clear()` first, then:
    /// - For blob: creates an Ashmem region, maps it read-write, writes blob.
    /// - For string: stores string directly.
    pub fn set(&mut self, data: PublishedDataType) {
        self.clear();
        match data {
            PublishedDataType::Blob(blob) => {
                let node = create_managed_ashmem_from_blob(&blob).unwrap_or_default();
                self.value = PublishedDataValue::Ashmem(node);
            }
            PublishedDataType::String(s) => {
                self.value = PublishedDataValue::String(s);
            }
        }
    }

    /// Get the data stored in this item.
    ///
    /// Equivalent to C++ `PublishedDataItem::GetData() const`.
    /// For Ashmem values, maps read-only and reads out the blob.
    pub fn get_data(&self) -> PublishedDataType {
        match &self.value {
            PublishedDataValue::Ashmem(node) => {
                let blob = match &node.ashmem {
                    Some(ashmem) => read_ashmem_blob(ashmem).unwrap_or_default(),
                    None => Vec::new(),
                };
                PublishedDataType::Blob(blob)
            }
            PublishedDataValue::String(s) => PublishedDataType::String(s.clone()),
        }
    }

    /// Check if this item holds Ashmem-backed blob data.
    /// Equivalent to C++ `value_.index() == 0`.
    pub fn is_ashmem(&self) -> bool {
        matches!(self.value, PublishedDataValue::Ashmem(_))
    }

    /// Check if this item holds a string value.
    /// Equivalent to C++ `value_.index() == 1`.
    pub fn is_string(&self) -> bool {
        matches!(self.value, PublishedDataValue::String(_))
    }

    /// Set an AshmemNode directly (for IPC deserialization).
    ///
    /// Equivalent to C++ `PublishedDataItem::SetAshmem(sptr<Ashmem>, bool)`.
    pub fn set_ashmem_node(&mut self, node: AshmemNode) {
        self.value = PublishedDataValue::Ashmem(node);
    }

    /// Clear the value, releasing any managed Ashmem resources.
    ///
    /// Equivalent to C++ `PublishedDataItem::Clear()`.
    /// If the current value is a managed AshmemNode, unmap + close the ashmem.
    /// Then the value is reset to empty string.
    pub fn clear(&mut self) {
        if let PublishedDataValue::Ashmem(node) = &self.value {
            if node.is_managed {
                if let Some(ashmem) = &node.ashmem {
                    ashmem.unmap_ashmem();
                    ashmem.close_ashmem();
                }
            }
        }
        self.value = PublishedDataValue::String(String::new());
    }

    /// Move out the AshmemNode, transferring ownership.
    ///
    /// Equivalent to C++ `PublishedDataItem::MoveOutAshmem()`.
    /// Returns the AshmemNode if the value is a managed Ashmem.
    /// After this call, the source node's `is_managed` flag is set to false
    /// and its `ashmem` is taken.
    pub fn move_out_ashmem(&mut self) -> Option<AshmemNode> {
        if let PublishedDataValue::Ashmem(node) = &mut self.value {
            if !node.is_managed {
                return None;
            }
            node.is_managed = false;
            let ashmem = node.ashmem.take();
            Some(AshmemNode {
                ashmem,
                is_managed: true,
            })
        } else {
            None
        }
    }
}

/// Helper: create a managed AshmemNode containing the given blob.
/// Mirrors C++ `PublishedDataItem::Set` for blob values.
fn create_managed_ashmem_from_blob(blob: &[u8]) -> Option<AshmemNode> {
    let size = blob.len();
    let ashmem = unsafe { create_ashmem_instance("PublishedDataItem", size as i32)? };
    if !ashmem.map_read_write_ashmem() {
        ashmem.close_ashmem();
        return None;
    }
    let ok =
        unsafe { ashmem.write_to_ashmem(blob.as_ptr() as *const std::ffi::c_char, size as i32, 0) };
    if !ok {
        ashmem.unmap_ashmem();
        ashmem.close_ashmem();
        return None;
    }
    Some(AshmemNode {
        ashmem: Some(ashmem),
        is_managed: true,
    })
}

/// Helper: read bytes out of a read-mapped ashmem.
/// If the ashmem is not mapped readable, attempts to map read-only first.
fn read_ashmem_blob(ashmem: &UtilsAshmem) -> Option<Vec<u8>> {
    let size = ashmem.get_ashmem_size();
    if size <= 0 {
        return Some(Vec::new());
    }
    if (ashmem.get_protection() & PROT_READ) == 0 {
        ashmem.map_read_only_ashmem();
    }
    let ptr = unsafe { ashmem.read_from_ashmem(size, 0) };
    if ptr.is_null() {
        return None;
    }
    let slice = unsafe { std::slice::from_raw_parts(ptr as *const u8, size as usize) };
    Some(slice.to_vec())
}

/// Published data container.
/// Equivalent to C++ `Data`.
#[derive(Debug, Default)]
pub struct Data {
    pub datas: Vec<PublishedDataItem>,
    pub version: i32,
}

/// Change node for published data callbacks.
/// Equivalent to C++ `PublishedDataChangeNode`.
#[derive(Debug, Default)]
pub struct PublishedDataChangeNode {
    pub owner_bundle_name: String,
    pub datas: Vec<PublishedDataItem>,
}

/// Change node for RDB data callbacks.
/// Equivalent to C++ `RdbChangeNode`.
pub struct RdbChangeNode {
    pub uri: String,
    pub template_id: TemplateId,
    pub data: Vec<String>,
    pub is_shared_memory: bool,
    /// Ashmem handle when `is_shared_memory` is true; None otherwise.
    pub memory: Option<UtilsAshmem>,
    pub size: i32,
}

// SharedPtr<ffi::Ashmem> uses atomic reference counting and C++ uses it across threads.
unsafe impl Send for RdbChangeNode {}
unsafe impl Sync for RdbChangeNode {}

impl std::fmt::Debug for RdbChangeNode {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("RdbChangeNode")
            .field("uri", &self.uri)
            .field("template_id", &self.template_id)
            .field("data", &self.data)
            .field("is_shared_memory", &self.is_shared_memory)
            .field(
                "memory_fd",
                &self
                    .memory
                    .as_ref()
                    .map(|a| a.get_ashmem_fd())
                    .unwrap_or(-1),
            )
            .field("size", &self.size)
            .finish()
    }
}

impl Default for RdbChangeNode {
    fn default() -> Self {
        Self {
            uri: String::new(),
            template_id: TemplateId::new(0, String::new()),
            data: Vec::new(),
            is_shared_memory: false,
            memory: None,
            size: 0,
        }
    }
}

/// Operation result.
/// Equivalent to C++ `OperationResult`.
#[derive(Debug, Clone)]
pub struct OperationResult {
    pub key: String,
    pub err_code: i32,
}

impl OperationResult {
    pub fn new(key: String, err_code: i32) -> Self {
        Self { key, err_code }
    }
}

// --- Data proxy types ---

/// Data proxy error codes.
/// Equivalent to C++ `DataProxyErrorCode`.
#[repr(i32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DataProxyErrorCode {
    Success = 0,
    UriNotExist = 1,
    NoPermission = 2,
    OverLimit = 3,
    InnerError = 4,
}

impl DataProxyErrorCode {
    pub fn from_i32(val: i32) -> Self {
        match val {
            0 => DataProxyErrorCode::Success,
            1 => DataProxyErrorCode::UriNotExist,
            2 => DataProxyErrorCode::NoPermission,
            3 => DataProxyErrorCode::OverLimit,
            _ => DataProxyErrorCode::InnerError,
        }
    }
}

/// Data proxy value type enum.
/// Equivalent to C++ `DataProxyValueType`.
#[repr(i32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DataProxyValueType {
    Int = 0,
    Double = 1,
    String = 2,
    Bool = 3,
}

/// Data proxy value: variant of int64, double, string, bool.
/// Equivalent to C++ `DataProxyValue`.
#[derive(Debug, Clone, PartialEq)]
pub enum DataProxyValue {
    Int(i64),
    Double(f64),
    String(String),
    Bool(bool),
}

impl Default for DataProxyValue {
    fn default() -> Self {
        DataProxyValue::String(String::new())
    }
}

impl DataProxyValue {
    pub fn value_type(&self) -> DataProxyValueType {
        match self {
            DataProxyValue::Int(_) => DataProxyValueType::Int,
            DataProxyValue::Double(_) => DataProxyValueType::Double,
            DataProxyValue::String(_) => DataProxyValueType::String,
            DataProxyValue::Bool(_) => DataProxyValueType::Bool,
        }
    }

    pub fn type_index(&self) -> u32 {
        match self {
            DataProxyValue::Int(_) => 0,
            DataProxyValue::Double(_) => 1,
            DataProxyValue::String(_) => 2,
            DataProxyValue::Bool(_) => 3,
        }
    }
}

/// Data proxy type.
/// Equivalent to C++ `DataProxyType`.
#[repr(i32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DataProxyType {
    SharedConfig = 0,
}

/// 数据代理值最大长度限制。
/// 对应 C++ `DataProxyMaxValueLength`。
#[repr(i32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DataProxyMaxValueLength {
    MaxLength4K = 4096,
    MaxLength100K = 102400,
}

impl DataProxyMaxValueLength {
    pub fn from_i32(val: i32) -> Option<Self> {
        match val {
            4096 => Some(DataProxyMaxValueLength::MaxLength4K),
            102400 => Some(DataProxyMaxValueLength::MaxLength100K),
            _ => None,
        }
    }
}

/// Data proxy configuration.
/// Equivalent to C++ `DataProxyConfig`.
#[derive(Debug, Clone)]
pub struct DataProxyConfig {
    pub proxy_type: DataProxyType,
    pub max_value_length: DataProxyMaxValueLength,
}

impl Default for DataProxyConfig {
    fn default() -> Self {
        Self {
            proxy_type: DataProxyType::SharedConfig,
            max_value_length: DataProxyMaxValueLength::MaxLength4K,
        }
    }
}

/// Data share proxy data.
/// Equivalent to C++ `DataShareProxyData`.
#[derive(Debug, Clone, Default)]
pub struct DataShareProxyData {
    pub uri: String,
    pub value: DataProxyValue,
    pub allow_list: Vec<String>,
    pub is_value_undefined: bool,
    pub is_allow_list_undefined: bool,
}

impl DataShareProxyData {
    pub fn new(uri: String, value: DataProxyValue, allow_list: Vec<String>) -> Self {
        Self {
            uri,
            value,
            allow_list,
            is_value_undefined: false,
            is_allow_list_undefined: false,
        }
    }
}

/// Data proxy result.
/// Equivalent to C++ `DataProxyResult`.
#[derive(Debug, Clone)]
pub struct DataProxyResult {
    pub uri: String,
    pub result: DataProxyErrorCode,
}

impl DataProxyResult {
    pub fn new(uri: String, result: DataProxyErrorCode) -> Self {
        Self { uri, result }
    }
}

/// Data proxy get result.
/// Equivalent to C++ `DataProxyGetResult`.
#[derive(Debug, Clone)]
pub struct DataProxyGetResult {
    pub uri: String,
    pub result: DataProxyErrorCode,
    pub value: DataProxyValue,
    pub allow_list: Vec<String>,
}

impl Default for DataProxyGetResult {
    fn default() -> Self {
        Self {
            uri: String::new(),
            result: DataProxyErrorCode::Success,
            value: DataProxyValue::default(),
            allow_list: Vec::new(),
        }
    }
}

/// Data proxy change info.
/// Equivalent to C++ `DataProxyChangeInfo`.
#[derive(Debug, Clone)]
pub struct DataProxyChangeInfo {
    pub change_type: ChangeType,
    pub uri: String,
    pub value: DataProxyValue,
}

impl Default for DataProxyChangeInfo {
    fn default() -> Self {
        Self {
            change_type: ChangeType::Invalid,
            uri: String::new(),
            value: DataProxyValue::default(),
        }
    }
}

// --- Common types ---

/// DataShare option.
/// Equivalent to C++ `DataShareOption`.
#[derive(Debug, Clone, Copy, Default)]
pub struct DataShareOption {
    pub timeout: u32,
}

/// URI info with extension URI and option.
/// Equivalent to C++ `UriInfo`.
#[derive(Debug, Clone, Default)]
pub struct UriInfo {
    pub uri: String,
    pub ext_uri: String,
    pub option: DataShareOption,
}

// --- VBucket conversion ---
// In C++, `Value` = `variant<monostate, int64_t, double, string, bool, vector<uint8_t>>`
// and `VBucket` = `map<string, Value>`.
// In Rust, `DataShareValue` serves as the universal value type, and
// `BTreeMap<String, DataShareValue>` is equivalent to C++ `VBucket`.
// These conversion utilities match C++ `ValueProxy::Convert` in
// `datashare_valuebucket_convert.cpp`.

/// Convert a `DataShareValuesBucket` to a VBucket (`BTreeMap<String, DataShareValue>`).
///
/// Equivalent to C++ `ValueProxy::Convert(vector<DataShareValuesBucket>&&) -> VBuckets`
/// for a single bucket.
pub fn values_bucket_to_vbucket(
    bucket: &DataShareValuesBucket,
) -> std::collections::BTreeMap<String, crate::value_object::DataShareValue> {
    bucket.get_all().clone()
}

/// Convert a VBucket (`BTreeMap<String, DataShareValue>`) to `DataShareValuesBucket`.
///
/// Equivalent to C++ `ValueProxy::Convert(VBuckets&&) -> vector<DataShareValuesBucket>`
/// for a single bucket.
pub fn vbucket_to_values_bucket(
    vbucket: &std::collections::BTreeMap<String, crate::value_object::DataShareValue>,
) -> DataShareValuesBucket {
    let mut bucket = DataShareValuesBucket::new();
    for (key, value) in vbucket {
        bucket.put(key, value.clone());
    }
    bucket
}

/// Convert a Vec of `DataShareValuesBucket` to VBuckets.
///
/// Equivalent to C++ `ValueProxy::Convert(vector<DataShareValuesBucket>&&) -> VBuckets`.
pub fn values_buckets_to_vbuckets(
    buckets: &[DataShareValuesBucket],
) -> Vec<std::collections::BTreeMap<String, crate::value_object::DataShareValue>> {
    buckets.iter().map(values_bucket_to_vbucket).collect()
}

/// Convert VBuckets to Vec of `DataShareValuesBucket`.
///
/// Equivalent to C++ `ValueProxy::Convert(VBuckets&&) -> vector<DataShareValuesBucket>`.
pub fn vbuckets_to_values_buckets(
    vbuckets: &[std::collections::BTreeMap<String, crate::value_object::DataShareValue>],
) -> Vec<DataShareValuesBucket> {
    vbuckets.iter().map(vbucket_to_values_bucket).collect()
}

// --- Constants ---

pub const URI_MAX_SIZE: i32 = 256;
pub const VALUE_MAX_SIZE: i32 = 4096;
pub const APPIDENTIFIER_MAX_SIZE: i32 = 128;
pub const URI_MAX_COUNT: i32 = 32;
pub const PROXY_DATA_MAX_COUNT: i32 = 32;
pub const ALLOW_LIST_MAX_COUNT: i32 = 256;
pub const ALLOW_ALL: &str = "all";
pub const DATA_PROXY_SCHEMA: &str = "datashareproxy://";

/// Maximum IPC transfer size (128MB).
pub const MAX_IPC_SIZE: usize = 128 * 1024 * 1024;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_operation_result() {
        let result = OperationResult::new("key1".to_string(), 0);
        assert_eq!(result.key, "key1");
        assert_eq!(result.err_code, 0);
    }

    #[test]
    fn test_batch_update_result() {
        let result = BatchUpdateResult {
            uri: "content://com.example/test".to_string(),
            codes: vec![0, 0, 1],
        };
        assert_eq!(result.codes.len(), 3);
    }

    #[test]
    fn test_data_proxy_value() {
        let v = DataProxyValue::Int(42);
        assert_eq!(v.value_type(), DataProxyValueType::Int);
        assert_eq!(v.type_index(), 0);

        let v = DataProxyValue::String("hello".to_string());
        assert_eq!(v.value_type(), DataProxyValueType::String);
        assert_eq!(v.type_index(), 2);
    }

    #[test]
    fn test_data_proxy_error_code() {
        assert_eq!(DataProxyErrorCode::from_i32(0), DataProxyErrorCode::Success);
        assert_eq!(
            DataProxyErrorCode::from_i32(3),
            DataProxyErrorCode::OverLimit
        );
        assert_eq!(
            DataProxyErrorCode::from_i32(99),
            DataProxyErrorCode::InnerError
        );
    }

    #[test]
    fn test_exec_error_code() {
        assert_eq!(ExecErrorCode::from_i32(0), ExecErrorCode::Success);
        assert_eq!(ExecErrorCode::from_i32(2), ExecErrorCode::PartialSuccess);
    }

    #[test]
    fn test_data_share_proxy_data() {
        let data = DataShareProxyData::new(
            "datashareproxy://com.example/test".to_string(),
            DataProxyValue::Int(100),
            vec!["com.allowed.app".to_string()],
        );
        assert_eq!(data.uri, "datashareproxy://com.example/test");
        assert!(!data.is_value_undefined);
        assert_eq!(data.allow_list.len(), 1);
    }

    #[test]
    fn test_rdb_change_node() {
        let node = RdbChangeNode::default();
        assert!(node.uri.is_empty());
        assert!(!node.is_shared_memory);
        assert!(node.memory.is_none());
    }

    #[test]
    fn test_published_data_item() {
        let item = PublishedDataItem::new(
            "key".to_string(),
            123,
            PublishedDataType::String("data".to_string()),
        );
        assert_eq!(item.key, "key");
        assert_eq!(item.subscriber_id, 123);
        assert!(item.is_string());
        assert!(!item.is_ashmem());

        // Verify get_data returns the string
        match item.get_data() {
            PublishedDataType::String(s) => assert_eq!(s, "data"),
            _ => panic!("expected string"),
        }
    }

    #[test]
    fn test_published_data_item_blob() {
        let blob_data = vec![1u8, 2, 3, 4, 5];
        let item = PublishedDataItem::new(
            "blob_key".to_string(),
            456,
            PublishedDataType::Blob(blob_data.clone()),
        );
        assert!(item.is_ashmem());
        assert!(!item.is_string());

        // Verify get_data returns the blob
        match item.get_data() {
            PublishedDataType::Blob(data) => assert_eq!(data, blob_data),
            _ => panic!("expected blob"),
        }
    }

    #[test]
    fn test_published_data_item_set_and_clear() {
        let mut item = PublishedDataItem::new(
            "key".to_string(),
            1,
            PublishedDataType::String("initial".to_string()),
        );
        assert!(item.is_string());

        // Set to blob
        item.set(PublishedDataType::Blob(vec![10, 20, 30]));
        assert!(item.is_ashmem());

        // Clear resets to empty string
        item.clear();
        assert!(item.is_string());
        match item.get_data() {
            PublishedDataType::String(s) => assert!(s.is_empty()),
            _ => panic!("expected string after clear"),
        }
    }

    #[test]
    fn test_published_data_item_move_out_ashmem() {
        let mut item =
            PublishedDataItem::new("key".to_string(), 1, PublishedDataType::Blob(vec![42, 43]));

        let node = item.move_out_ashmem();
        assert!(node.is_some());
        let node = node.unwrap();
        assert!(node.is_managed);
        assert!(node.ashmem.is_some());

        // After move, the original is no longer managed
        if let PublishedDataValue::Ashmem(n) = &item.value {
            assert!(!n.is_managed);
        }
    }

    #[test]
    fn test_send_bounds() {
        fn assert_send<T: Send>() {}
        assert_send::<AshmemNode>();
        assert_send::<RdbChangeNode>();
    }

    #[test]
    fn test_published_data_item_move_out_string() {
        let mut item = PublishedDataItem::new(
            "key".to_string(),
            1,
            PublishedDataType::String("hello".to_string()),
        );
        // move_out_ashmem on string value returns None
        assert!(item.move_out_ashmem().is_none());
    }

    #[test]
    fn test_vbucket_conversion() {
        use crate::value_object::DataShareValue;

        let mut bucket = DataShareValuesBucket::new();
        bucket.put("name", DataShareValue::String("Alice".to_string()));
        bucket.put("age", DataShareValue::Int(30));

        // Convert to VBucket
        let vbucket = values_bucket_to_vbucket(&bucket);
        assert_eq!(vbucket.len(), 2);
        assert_eq!(
            vbucket.get("name"),
            Some(&DataShareValue::String("Alice".to_string()))
        );
        assert_eq!(vbucket.get("age"), Some(&DataShareValue::Int(30)));

        // Convert back
        let result = vbucket_to_values_bucket(&vbucket);
        assert_eq!(result.get("name"), bucket.get("name"));
        assert_eq!(result.get("age"), bucket.get("age"));
    }

    #[test]
    fn test_vbuckets_conversion() {
        use crate::value_object::DataShareValue;

        let mut b1 = DataShareValuesBucket::new();
        b1.put("x", DataShareValue::Int(1));
        let mut b2 = DataShareValuesBucket::new();
        b2.put("y", DataShareValue::Double(2.0));

        let buckets = vec![b1, b2];
        let vbuckets = values_buckets_to_vbuckets(&buckets);
        assert_eq!(vbuckets.len(), 2);

        let result = vbuckets_to_values_buckets(&vbuckets);
        assert_eq!(result.len(), 2);
        assert_eq!(result[0].get("x"), Some(&DataShareValue::Int(1)));
        assert_eq!(result[1].get("y"), Some(&DataShareValue::Double(2.0)));
    }

    #[test]
    fn test_uri_info() {
        let info = UriInfo {
            uri: "content://com.example/data".to_string(),
            ext_uri: "".to_string(),
            option: DataShareOption { timeout: 5000 },
        };
        assert_eq!(info.option.timeout, 5000);
    }

    #[test]
    fn test_register_option() {
        let opt = RegisterOption { is_reconnect: true };
        assert!(opt.is_reconnect);
    }
}
