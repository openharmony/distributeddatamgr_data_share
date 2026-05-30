// Copyright (c) 2026 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

use crate::block_writer::BlockWriter;
use crate::result_set_bridge::ResultSetBridge;
use crate::shared_block::SharedBlock;
use datashare_common::error::{DataShareError, Result};
use std::collections::HashMap;
use std::panic::catch_unwind;

const INIT_POS: i32 = -1;

/// Cell data type enumeration, matching C++ SharedBlock cell types.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DataType {
    TypeNull = 0,
    TypeInteger = 1,
    TypeFloat = 2,
    TypeString = 3,
    TypeBlob = 4,
}

impl From<i32> for DataType {
    fn from(val: i32) -> Self {
        match val {
            0 => DataType::TypeNull,
            1 => DataType::TypeInteger,
            2 => DataType::TypeFloat,
            3 => DataType::TypeString,
            4 => DataType::TypeBlob,
            _ => DataType::TypeNull,
        }
    }
}

impl From<DataType> for i32 {
    fn from(val: DataType) -> Self {
        match val {
            DataType::TypeNull => 0,
            DataType::TypeInteger => 1,
            DataType::TypeFloat => 2,
            DataType::TypeString => 3,
            DataType::TypeBlob => 4,
        }
    }
}

/// Abstract result set interface.
///
/// Equivalent to C++ `DataShareAbsResultSet` + `ResultSet` base classes.
/// Combines cursor navigation (from AbsResultSet) and data access methods.
pub trait ResultSet {
    fn get_row_count(&self) -> Result<i32>;
    fn get_all_column_names(&self) -> Result<Vec<String>>;
    fn get_column_count(&self) -> Result<i32>;
    fn get_column_index(&self, name: &str) -> Result<i32>;
    fn get_column_name(&self, index: i32) -> Result<String>;
    fn get_column_type(&self, index: i32) -> Result<DataType>;
    fn go_to_row(&mut self, position: i32) -> Result<bool>;
    fn go_to(&mut self, offset: i32) -> Result<bool>;
    fn go_to_first_row(&mut self) -> Result<bool>;
    fn go_to_last_row(&mut self) -> Result<bool>;
    fn go_to_next_row(&mut self) -> Result<bool>;
    fn go_to_previous_row(&mut self) -> Result<bool>;
    fn get_row_index(&self) -> Result<i32>;
    fn is_started(&self) -> Result<bool>;
    fn is_ended(&self) -> Result<bool>;
    fn is_at_first_row(&self) -> Result<bool>;
    fn is_at_last_row(&mut self) -> Result<bool>;
    fn get_blob(&self, column: i32) -> Result<Vec<u8>>;
    fn get_string(&self, column: i32) -> Result<String>;
    fn get_int(&self, column: i32) -> Result<i32>;
    fn get_long(&self, column: i32) -> Result<i64>;
    fn get_double(&self, column: i32) -> Result<f64>;
    fn is_column_null(&self, column: i32) -> Result<bool>;
    fn is_closed(&self) -> bool;
    fn close(&mut self) -> Result<()>;
}

/// DataShare result set implementation.
///
/// Equivalent to C++ `DataShareResultSet`. Manages a SharedBlock for data storage,
/// optional ResultSetBridge for paging, and cursor-based navigation.
///
/// Type coercion behavior matches C++:
/// - `get_string`: INTEGER→to_string, FLOAT→to_string, BLOB→hex, NULL→""
/// - `get_long`: FLOAT→truncate, STRING→parse, NULL→0
/// - `get_double`: INTEGER→cast, STRING→parse, NULL→0.0
/// - `get_int`: delegates to get_long, truncate to i32
pub struct DataShareResultSet {
    // SharedBlock owned by this result set
    block: Option<SharedBlock>,
    // Column names
    column_names: Vec<String>,
    // Current cursor position
    row_pos: i32,
    // Cached row count
    count: i32,
    // Whether the result set is closed
    is_closed: bool,
    // Column name -> index cache (case-insensitive keys)
    index_cache: HashMap<String, i32>,

    // --- Bridge/paging fields ---
    // Optional bridge for data paging (populated rows via on_go callback)
    bridge: Option<Box<dyn ResultSetBridge>>,
    // Start position of cached rows in the block
    start_row_pos: i32,
    // End position of cached rows in the block
    end_row_pos: i32,

    // --- Lifecycle callback ---
    // 可选的关闭回调，在 DataShareResultSet 被 drop 时触发，用于清理外部资源
    on_close: Option<Box<dyn FnOnce() + Send>>,
}

impl DataShareResultSet {
    pub fn new() -> Self {
        Self {
            block: None,
            column_names: Vec::new(),
            row_pos: INIT_POS,
            count: 0,
            is_closed: false,
            index_cache: HashMap::new(),
            bridge: None,
            start_row_pos: -1,
            end_row_pos: -1,
            on_close: None,
        }
    }

    /// Create a result set backed by a bridge.
    /// Equivalent to C++ `DataShareResultSet(shared_ptr<ResultSetBridge>& bridge)`.
    pub fn with_bridge(mut self, bridge: Box<dyn ResultSetBridge>) -> Self {
        // Get column names and row count from bridge
        if let Ok(names) = bridge.get_all_column_names() {
            self.set_column_names(names);
        }
        if let Ok(count) = bridge.get_row_count() {
            self.count = count;
        }
        self.bridge = Some(bridge);
        self
    }

    pub fn with_block(mut self, block: SharedBlock) -> Self {
        let row_count = block.get_row_count() as i32;
        self.block = Some(block);
        self.count = row_count;
        self.start_row_pos = 0;
        self.end_row_pos = row_count - 1;
        self
    }

    pub fn set_column_names(&mut self, names: Vec<String>) {
        self.column_names = names.clone();
        self.index_cache.clear();
        for (i, name) in names.into_iter().enumerate() {
            self.index_cache.insert(name.to_lowercase(), i as i32);
        }
    }

    pub fn set_block(&mut self, block: SharedBlock) {
        let row_count = block.get_row_count() as i32;
        self.block = Some(block);
        self.count = row_count;
        self.start_row_pos = 0;
        self.end_row_pos = row_count - 1;
    }

    pub fn get_block(&self) -> Option<&SharedBlock> {
        self.block.as_ref()
    }

    /// 取出 SharedBlock 的所有权，取出后内部 block 变为 None。
    /// 用于序列化场景：在将 ResultSet 交给 IPC stub 之前先取出 block 进行 marshalling。
    pub fn take_block_owned(&mut self) -> Option<SharedBlock> {
        self.block.take()
    }

    pub fn has_block(&self) -> bool {
        self.block.is_some()
    }

    pub fn get_bridge(&self) -> Option<&dyn ResultSetBridge> {
        self.bridge.as_deref()
    }

    /// Attach a SharedBlock for shared-memory IPC without modifying cursor state.
    /// The block may be empty; data will be filled lazily via on_go.
    pub fn attach_shared_block(&mut self, block: SharedBlock) {
        self.block = Some(block);
    }

    /// Get column names slice.
    pub fn get_column_names(&self) -> &[String] {
        &self.column_names
    }

    /// 注册一个关闭回调，在 DataShareResultSet 被 drop 时触发。
    /// 用于外部调用方在 resultSet 生命周期结束时执行清理逻辑（如递减计数器）。
    pub fn set_on_close(&mut self, f: impl FnOnce() + Send + 'static) {
        self.on_close = Some(Box::new(f));
    }

    fn check_state(&self, column_index: i32) -> Result<()> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        // Match C++ CheckState order: check column index first, then row position
        let col_count = if !self.column_names.is_empty() {
            self.column_names.len() as i32
        } else if let Some(ref block) = self.block {
            block.get_column_count() as i32
        } else {
            0
        };
        if col_count > 0 && (column_index < 0 || column_index >= col_count) {
            return Err(DataShareError::InvalidColumnIndex);
        }
        if self.row_pos == INIT_POS {
            return Err(DataShareError::InvalidRowIndex);
        }
        Ok(())
    }

    /// Called when cursor moves to a position outside the cached range.
    /// Clears the shared block and re-populates via bridge (same ashmem for shared memory).
    ///
    /// Returns the end row position on success, or -1 on failure.
    /// This is the IPC-facing OnGo method (equivalent to C++ `DataShareResultSet::OnGo`).
    pub fn on_go(&mut self, start_row_index: i32, target_row_index: i32) -> Result<i32> {
        if self.bridge.is_none() {
            return Ok(-1);
        }

        // Take existing block (same ashmem shared with client), or create new if none exists
        let block = self
            .block
            .take()
            .map_or_else(
                || {
                    SharedBlock::create("result_set_block", 2 * 1024 * 1024)
                        .map(|b| *b)
                        .map_err(|_| DataShareError::OutOfMemory)
                },
                Ok,
            )?;

        // Wrap in BlockWriter, clear, set columns, fill via bridge
        let mut writer = BlockWriter::from_block(block);
        let fill_result = (|| -> Result<()> {
            writer.clear()?;
            writer.set_column_num(self.column_names.len() as u32)?;
            let bridge = self.bridge.as_mut().unwrap();
            bridge.on_go(start_row_index, target_row_index, &mut writer)
        })();

        // Always put block back — preserves shared memory link even on error
        if let Some(filled_block) = writer.take_block() {
            let row_count = filled_block.get_row_count() as i32;
            self.block = Some(filled_block);
            if fill_result.is_ok() && row_count > 0 {
                self.start_row_pos = start_row_index;
                self.end_row_pos = start_row_index + row_count - 1;
            } else {
                // Block was cleared but no rows written (or error) — invalidate cache
                self.start_row_pos = -1;
                self.end_row_pos = -1;
            }
        }

        fill_result?;
        Ok(self.end_row_pos)
    }

    /// Get the row index within the shared block for the current cursor position.
    fn block_row_index(&self) -> i32 {
        if self.start_row_pos >= 0 {
            self.row_pos - self.start_row_pos
        } else {
            self.row_pos
        }
    }
}

impl Default for DataShareResultSet {
    fn default() -> Self {
        Self::new()
    }
}

impl Drop for DataShareResultSet {
    fn drop(&mut self) {
        if let Some(f) = self.on_close.take() {
            // 使用 catch_unwind 防止回调 panic 传播到 drop 调用方
            let _ = catch_unwind(std::panic::AssertUnwindSafe(f));
        }
    }
}

impl ResultSet for DataShareResultSet {
    fn get_row_count(&self) -> Result<i32> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        Ok(self.count)
    }

    fn get_all_column_names(&self) -> Result<Vec<String>> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        Ok(self.column_names.clone())
    }

    fn get_column_count(&self) -> Result<i32> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        // Prefer column_names.len(), fall back to block's column count
        let count = if !self.column_names.is_empty() {
            self.column_names.len() as i32
        } else if let Some(ref block) = self.block {
            block.get_column_count() as i32
        } else {
            0
        };
        Ok(count)
    }

    fn get_column_index(&self, name: &str) -> Result<i32> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        let lower = name.to_lowercase();
        if let Some(index) = self.index_cache.get(&lower) {
            Ok(*index)
        } else {
            Err(DataShareError::InvalidColumnName)
        }
    }

    fn get_column_name(&self, index: i32) -> Result<String> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        if index >= 0 && index < self.column_names.len() as i32 {
            Ok(self.column_names[index as usize].clone())
        } else {
            Err(DataShareError::InvalidColumnIndex)
        }
    }

    fn get_column_type(&self, index: i32) -> Result<DataType> {
        self.check_state(index)?;

        if let Some(b) = self.block.as_ref() {
            let block_row = self.block_row_index() as u32;
            let (cell_type, _cell) = b.get_cell_value(block_row, index as u32)?;
            Ok(DataType::from(cell_type))
        } else {
            Ok(DataType::TypeNull)
        }
    }

    fn go_to_row(&mut self, position: i32) -> Result<bool> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        if position < 0 || position >= self.count {
            return Ok(false);
        }

        // Check if we need to page (position outside cached range)
        if self.bridge.is_some() && (position < self.start_row_pos || position > self.end_row_pos) {
            self.on_go(position, self.count - 1)?;
        }

        self.row_pos = position;
        Ok(true)
    }

    fn go_to(&mut self, offset: i32) -> Result<bool> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        let new_pos = self.row_pos + offset;
        self.go_to_row(new_pos)
    }

    fn go_to_first_row(&mut self) -> Result<bool> {
        self.go_to_row(0)
    }

    fn go_to_last_row(&mut self) -> Result<bool> {
        if self.count <= 0 {
            return Ok(false);
        }
        self.go_to_row(self.count - 1)
    }

    fn go_to_next_row(&mut self) -> Result<bool> {
        self.go_to(1)
    }

    fn go_to_previous_row(&mut self) -> Result<bool> {
        self.go_to(-1)
    }

    fn get_row_index(&self) -> Result<i32> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        Ok(self.row_pos)
    }

    fn is_started(&self) -> Result<bool> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        Ok(self.row_pos == INIT_POS)
    }

    fn is_ended(&self) -> Result<bool> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        Ok(self.count > 0 && self.row_pos >= self.count)
    }

    fn is_at_first_row(&self) -> Result<bool> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        Ok(self.row_pos == 0)
    }

    fn is_at_last_row(&mut self) -> Result<bool> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        if self.count <= 0 {
            return Ok(false);
        }
        Ok(self.row_pos == self.count - 1)
    }

    /// Get blob value with type coercion.
    /// C++ behavior: STRING and BLOB cells return their data; other types return error.
    fn get_blob(&self, column: i32) -> Result<Vec<u8>> {
        self.check_state(column)?;

        let b = self.block.as_ref().ok_or(DataShareError::InvalidOperation)?;
        let block_row = self.block_row_index() as u32;
        let (cell_type, cell) = b.get_cell_value(block_row, column as u32)?;

        match cell_type {
            3 | 4 => {
                // STRING or BLOB
                b.get_string_data(cell.string_or_blob_offset, cell.string_or_blob_size)
            }
            _ => Err(DataShareError::InvalidOperation),
        }
    }

    /// Get string value with type coercion.
    /// C++ behavior:
    /// - STRING → return string
    /// - INTEGER → to_string
    /// - FLOAT → to_string
    /// - BLOB → empty string (C++ returns empty)
    /// - NULL → empty string
    fn get_string(&self, column: i32) -> Result<String> {
        self.check_state(column)?;

        let b = self.block.as_ref().ok_or(DataShareError::InvalidOperation)?;
        let block_row = self.block_row_index() as u32;
        let (cell_type, cell) = b.get_cell_value(block_row, column as u32)?;

        match cell_type {
            0 => Ok(String::new()),                 // NULL
            1 => Ok(cell.long_value.to_string()),   // INTEGER -> to_string
            2 => Ok(cell.double_value.to_string()), // FLOAT -> to_string
            3 => {
                // STRING — zero-copy read, then allocate once for the owned String
                b.get_cell_unit_str(&cell).map(|s| s.to_owned())
            }
            4 => {
                // BLOB -> return as string if valid UTF-8, else empty
                let data =
                    b.get_string_data(cell.string_or_blob_offset, cell.string_or_blob_size)?;
                String::from_utf8(data).map_err(|_| DataShareError::InvalidOperation)
            }
            _ => Ok(String::new()),
        }
    }

    /// Get int value. Delegates to get_long and truncates.
    fn get_int(&self, column: i32) -> Result<i32> {
        let val = self.get_long(column)?;
        Ok(val as i32)
    }

    /// Get long value with type coercion.
    /// C++ behavior:
    /// - INTEGER → return value
    /// - FLOAT → truncate to i64
    /// - STRING → parse as i64 (0 if parse fails)
    /// - NULL → 0
    fn get_long(&self, column: i32) -> Result<i64> {
        self.check_state(column)?;

        let b = self.block.as_ref().ok_or(DataShareError::InvalidOperation)?;
        let block_row = self.block_row_index() as u32;
        let (cell_type, cell) = b.get_cell_value(block_row, column as u32)?;

        match cell_type {
            0 => Ok(0),                        // NULL
            1 => Ok(cell.long_value),          // INTEGER
            2 => Ok(cell.double_value as i64), // FLOAT -> truncate
            3 => {
                // STRING -> parse (zero-copy, no allocation)
                let s = b.get_cell_unit_str(&cell).unwrap_or("");
                Ok(s.parse::<i64>().unwrap_or(0))
            }
            _ => Ok(0),
        }
    }

    /// Get double value with type coercion.
    /// C++ behavior:
    /// - FLOAT → return value
    /// - INTEGER → cast to f64
    /// - STRING → parse as f64 (0.0 if parse fails)
    /// - NULL → 0.0
    fn get_double(&self, column: i32) -> Result<f64> {
        self.check_state(column)?;

        let b = self.block.as_ref().ok_or(DataShareError::InvalidOperation)?;
        let block_row = self.block_row_index() as u32;
        let (cell_type, cell) = b.get_cell_value(block_row, column as u32)?;

        match cell_type {
            0 => Ok(0.0),                    // NULL
            1 => Ok(cell.long_value as f64), // INTEGER -> cast
            2 => Ok(cell.double_value),      // FLOAT
            3 => {
                // STRING -> parse (zero-copy, no allocation)
                let s = b.get_cell_unit_str(&cell).unwrap_or("");
                Ok(s.parse::<f64>().unwrap_or(0.0))
            }
            _ => Ok(0.0),
        }
    }

    fn is_column_null(&self, column: i32) -> Result<bool> {
        self.check_state(column)?;

        let b = self.block.as_ref().ok_or(DataShareError::InvalidOperation)?;
        let block_row = self.block_row_index() as u32;
        let (cell_type, _) = b.get_cell_value(block_row, column as u32)?;
        Ok(cell_type == 0)
    }

    fn is_closed(&self) -> bool {
        self.is_closed
    }

    fn close(&mut self) -> Result<()> {
        if self.is_closed {
            return Err(DataShareError::InvalidOperation);
        }
        self.is_closed = true;
        self.block = None;
        self.bridge = None;
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn create_test_block() -> SharedBlock {
        let mut block = SharedBlock::create("test", 4096).unwrap();
        block.set_column_num(3).unwrap();
        block.alloc_row().unwrap();
        block.put_long(0, 0, 100).unwrap();
        block.put_double(0, 1, 3.14).unwrap();
        block.put_string(0, 2, "hello").unwrap();
        block
    }

    #[test]
    fn test_result_set_new() {
        let rs = DataShareResultSet::new();
        assert!(!rs.is_closed());
        assert_eq!(rs.get_row_count().unwrap(), 0);
    }

    #[test]
    fn test_result_set_with_block() {
        let block = create_test_block();
        let mut rs = DataShareResultSet::new();
        rs.set_block(block);

        assert_eq!(rs.get_row_count().unwrap(), 1);
        assert_eq!(rs.get_column_count().unwrap(), 3);
    }

    #[test]
    fn test_result_set_column_names() {
        let block = create_test_block();
        let mut rs = DataShareResultSet::new();
        rs.set_block(block);
        rs.set_column_names(vec![
            "id".to_string(),
            "value".to_string(),
            "name".to_string(),
        ]);

        assert_eq!(rs.get_column_name(0).unwrap(), "id");
        assert_eq!(rs.get_column_index("value").unwrap(), 1);
    }

    #[test]
    fn test_result_set_case_insensitive_column_index() {
        let block = create_test_block();
        let mut rs = DataShareResultSet::new();
        rs.set_block(block);
        rs.set_column_names(vec![
            "ID".to_string(),
            "Value".to_string(),
            "Name".to_string(),
        ]);

        // Case-insensitive lookup
        assert_eq!(rs.get_column_index("id").unwrap(), 0);
        assert_eq!(rs.get_column_index("ID").unwrap(), 0);
        assert_eq!(rs.get_column_index("value").unwrap(), 1);
        assert_eq!(rs.get_column_index("VALUE").unwrap(), 1);
    }

    #[test]
    fn test_result_set_go_to() {
        let block = create_test_block();
        let mut rs = DataShareResultSet::new();
        rs.set_block(block);

        assert!(rs.is_started().unwrap());

        rs.go_to_first_row().unwrap();
        assert_eq!(rs.get_row_index().unwrap(), 0);
        assert!(rs.is_at_first_row().unwrap());

        let has_next = rs.go_to_next_row().unwrap();
        assert!(!has_next); // Only 1 row, can't go next
    }

    #[test]
    fn test_result_set_get_long() {
        let block = create_test_block();
        let mut rs = DataShareResultSet::new();
        rs.set_block(block);

        rs.go_to_row(0).unwrap();
        assert_eq!(rs.get_long(0).unwrap(), 100);
    }

    #[test]
    fn test_result_set_get_double() {
        let block = create_test_block();
        let mut rs = DataShareResultSet::new();
        rs.set_block(block);

        rs.go_to_row(0).unwrap();
        assert!((rs.get_double(1).unwrap() - 3.14).abs() < 0.001);
    }

    #[test]
    fn test_result_set_get_string() {
        let block = create_test_block();
        let mut rs = DataShareResultSet::new();
        rs.set_block(block);

        rs.go_to_row(0).unwrap();
        assert_eq!(rs.get_string(2).unwrap(), "hello");
    }

    #[test]
    fn test_result_set_type_coercion_long_to_string() {
        let block = create_test_block();
        let mut rs = DataShareResultSet::new();
        rs.set_block(block);

        rs.go_to_row(0).unwrap();
        // Column 0 is INTEGER(100), get_string should coerce to "100"
        assert_eq!(rs.get_string(0).unwrap(), "100");
    }

    #[test]
    fn test_result_set_type_coercion_double_to_string() {
        let block = create_test_block();
        let mut rs = DataShareResultSet::new();
        rs.set_block(block);

        rs.go_to_row(0).unwrap();
        // Column 1 is FLOAT(3.14), get_string should coerce to "3.14"
        let s = rs.get_string(1).unwrap();
        assert!(s.starts_with("3.14"));
    }

    #[test]
    fn test_result_set_type_coercion_double_to_long() {
        let block = create_test_block();
        let mut rs = DataShareResultSet::new();
        rs.set_block(block);

        rs.go_to_row(0).unwrap();
        // Column 1 is FLOAT(3.14), get_long should truncate to 3
        assert_eq!(rs.get_long(1).unwrap(), 3);
    }

    #[test]
    fn test_result_set_type_coercion_long_to_double() {
        let block = create_test_block();
        let mut rs = DataShareResultSet::new();
        rs.set_block(block);

        rs.go_to_row(0).unwrap();
        // Column 0 is INTEGER(100), get_double should cast to 100.0
        assert!((rs.get_double(0).unwrap() - 100.0).abs() < 0.001);
    }

    #[test]
    fn test_result_set_null_coercion() {
        let mut block = SharedBlock::create("test", 4096).unwrap();
        block.set_column_num(2).unwrap();
        block.alloc_row().unwrap();
        block.put_null(0, 0).unwrap();
        block.put_long(0, 1, 42).unwrap();

        let mut rs = DataShareResultSet::new();
        rs.set_block(block);

        rs.go_to_row(0).unwrap();
        assert!(rs.is_column_null(0).unwrap());
        assert!(!rs.is_column_null(1).unwrap());
        // NULL coercion: long returns 0, double returns 0.0, string returns ""
        assert_eq!(rs.get_long(0).unwrap(), 0);
        assert!((rs.get_double(0).unwrap() - 0.0).abs() < 0.001);
        assert_eq!(rs.get_string(0).unwrap(), "");
    }

    #[test]
    fn test_result_set_close() {
        let block = create_test_block();
        let mut rs = DataShareResultSet::new();
        rs.set_block(block);

        rs.close().unwrap();
        assert!(rs.is_closed());
    }

    #[test]
    fn test_result_set_closed_error() {
        let mut rs = DataShareResultSet::new();
        rs.close().unwrap();

        assert!(rs.get_row_count().is_err());
    }

    #[test]
    fn test_result_set_multiple_rows() {
        let mut block = SharedBlock::create("test", 8192).unwrap();
        block.set_column_num(2).unwrap();

        block.alloc_row().unwrap();
        block.put_long(0, 0, 1).unwrap();
        block.put_string(0, 1, "Alice").unwrap();

        block.alloc_row().unwrap();
        block.put_long(1, 0, 2).unwrap();
        block.put_string(1, 1, "Bob").unwrap();

        block.alloc_row().unwrap();
        block.put_long(2, 0, 3).unwrap();
        block.put_string(2, 1, "Charlie").unwrap();

        let mut rs = DataShareResultSet::new();
        rs.set_block(block);

        assert_eq!(rs.get_row_count().unwrap(), 3);

        // Navigate forward
        assert!(rs.go_to_first_row().unwrap());
        assert_eq!(rs.get_long(0).unwrap(), 1);
        assert_eq!(rs.get_string(1).unwrap(), "Alice");

        assert!(rs.go_to_next_row().unwrap());
        assert_eq!(rs.get_long(0).unwrap(), 2);
        assert_eq!(rs.get_string(1).unwrap(), "Bob");

        assert!(rs.go_to_next_row().unwrap());
        assert_eq!(rs.get_long(0).unwrap(), 3);
        assert_eq!(rs.get_string(1).unwrap(), "Charlie");
        assert!(rs.is_at_last_row().unwrap());

        // Navigate backward
        assert!(rs.go_to_previous_row().unwrap());
        assert_eq!(rs.get_string(1).unwrap(), "Bob");

        // Go to specific row
        assert!(rs.go_to_row(0).unwrap());
        assert_eq!(rs.get_string(1).unwrap(), "Alice");
    }

    #[test]
    fn test_result_set_string_parse_coercion() {
        let mut block = SharedBlock::create("test", 4096).unwrap();
        block.set_column_num(2).unwrap();
        block.alloc_row().unwrap();
        block.put_string(0, 0, "42").unwrap();
        block.put_string(0, 1, "3.14").unwrap();

        let mut rs = DataShareResultSet::new();
        rs.set_block(block);
        rs.go_to_row(0).unwrap();

        // STRING "42" -> get_long should parse as 42
        assert_eq!(rs.get_long(0).unwrap(), 42);
        // STRING "3.14" -> get_double should parse as 3.14
        assert!((rs.get_double(1).unwrap() - 3.14).abs() < 0.001);
    }

    #[test]
    fn test_result_set_check_state_before_move() {
        let block = create_test_block();
        let mut rs = DataShareResultSet::new();
        rs.set_block(block);

        // Before moving (row_pos == INIT_POS), data access should fail
        assert!(rs.get_long(0).is_err());
    }
}
