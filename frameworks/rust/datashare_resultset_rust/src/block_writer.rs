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

//! `BlockWriter` — a thin adapter over `SharedBlock` that tracks the "current
//! row" for column-by-column population. Matches C++ `DataShareBlockWriterImpl`.

#![allow(clippy::missing_safety_doc)]
#![allow(clippy::not_unsafe_ptr_arg_deref)]

use crate::shared_block::{result_from_code, SharedBlock, SHARED_BLOCK_NO_MEMORY, SHARED_BLOCK_OK};
use datashare_common::error::{DataShareError, Result as DsResult};

// Match C++ `datashare_errno.h` surface used by ConvertErrorCode.
pub const E_OK: i32 = 0;
pub const E_ERROR: i32 = -1;

/// Mirror of C++ `DataShareBlockWriterImpl::ConvertErrorCode`.
/// The C++ implementation simply returns `E_OK` on `SHARED_BLOCK_OK` and
/// `E_ERROR` for every other error code.
pub fn convert_error_code(code: i32) -> i32 {
    match code {
        SHARED_BLOCK_OK => E_OK,
        _ => E_ERROR,
    }
}

pub struct BlockWriter {
    block: Option<Box<SharedBlock>>,
}

impl BlockWriter {
    pub fn new() -> Self {
        Self { block: None }
    }

    pub fn new_with_block(name: &str, size: usize) -> Self {
        let block = SharedBlock::create(name, size).ok();
        Self { block }
    }

    /// Wrap an existing SharedBlock for reuse in shared-memory scenarios.
    pub fn from_block(block: SharedBlock) -> Self {
        Self {
            block: Some(Box::new(block)),
        }
    }

    /// Clear the underlying block (reset header, keep same ashmem).
    pub fn clear(&mut self) -> DsResult<()> {
        let b = self.block.as_mut().ok_or(DataShareError::InvalidOperation)?;
        result_from_code(b.clear())
    }

    pub fn alloc_row(&mut self) -> i32 {
        match self.block.as_mut() {
            Some(b) => convert_error_code(b.alloc_row()),
            None => E_ERROR,
        }
    }

    pub fn free_last_row(&mut self) -> i32 {
        match self.block.as_mut() {
            Some(b) => convert_error_code(b.free_last_row()),
            None => E_ERROR,
        }
    }

    pub fn write_null(&mut self, col: u32) -> i32 {
        let row = match self.current_row_index() {
            Some(r) => r,
            None => return E_ERROR,
        };
        match self.block.as_mut() {
            Some(b) => convert_error_code(b.put_null(row, col)),
            None => E_ERROR,
        }
    }

    pub fn write_long(&mut self, col: u32, v: i64) -> i32 {
        let row = match self.current_row_index() {
            Some(r) => r,
            None => return E_ERROR,
        };
        match self.block.as_mut() {
            Some(b) => convert_error_code(b.put_long(row, col, v)),
            None => E_ERROR,
        }
    }

    pub fn write_double(&mut self, col: u32, v: f64) -> i32 {
        let row = match self.current_row_index() {
            Some(r) => r,
            None => return E_ERROR,
        };
        match self.block.as_mut() {
            Some(b) => convert_error_code(b.put_double(row, col, v)),
            None => E_ERROR,
        }
    }

    pub fn write_blob(&mut self, col: u32, data: &[u8]) -> i32 {
        let row = match self.current_row_index() {
            Some(r) => r,
            None => return E_ERROR,
        };
        match self.block.as_mut() {
            Some(b) => convert_error_code(b.put_blob(row, col, data)),
            None => E_ERROR,
        }
    }

    pub fn write_string(&mut self, col: u32, data: *const u8, size_inc_null: usize) -> i32 {
        let row = match self.current_row_index() {
            Some(r) => r,
            None => return E_ERROR,
        };
        match self.block.as_mut() {
            Some(b) => convert_error_code(b.put_string(row, col, data, size_inc_null)),
            None => E_ERROR,
        }
    }

    pub fn block(&self) -> Option<&SharedBlock> {
        self.block.as_deref()
    }

    pub fn block_mut(&mut self) -> Option<&mut SharedBlock> {
        self.block.as_deref_mut()
    }

    pub fn current_row_index(&self) -> Option<u32> {
        let b = self.block.as_deref()?;
        let n = b.row_num();
        if n > 0 {
            Some(n - 1)
        } else {
            None
        }
    }

    // ------------------------------------------------------------------
    // Legacy compatibility shims (used by result_set.rs &
    // result_set_bridge.rs).
    // ------------------------------------------------------------------

    pub fn create(name: &str, size: usize) -> DsResult<Self> {
        let block = SharedBlock::create(name, size).map_err(|_| DataShareError::OutOfMemory)?;
        Ok(Self { block: Some(block) })
    }

    pub fn set_column_num(&mut self, n: u32) -> DsResult<()> {
        let b = self
            .block
            .as_mut()
            .ok_or(DataShareError::InvalidOperation)?;
        result_from_code(b.set_column_num(n))
    }

    pub fn alloc_row_legacy(&mut self) -> DsResult<()> {
        let b = self
            .block
            .as_mut()
            .ok_or(DataShareError::InvalidOperation)?;
        result_from_code(b.alloc_row())
    }

    pub fn write_string_str(&mut self, col: u32, value: &str) -> DsResult<()> {
        let mut buf = value.as_bytes().to_vec();
        buf.push(0);
        let b = self
            .block
            .as_mut()
            .ok_or(DataShareError::InvalidOperation)?;
        let row = b.row_num();
        if row == 0 {
            return Err(DataShareError::InvalidOperation);
        }
        result_from_code(b.put_string(row - 1, col, buf.as_ptr(), buf.len()))
    }

    pub fn write_blob_slice(&mut self, col: u32, data: &[u8]) -> DsResult<()> {
        let b = self
            .block
            .as_mut()
            .ok_or(DataShareError::InvalidOperation)?;
        let row = b.row_num();
        if row == 0 {
            return Err(DataShareError::InvalidOperation);
        }
        result_from_code(b.put_blob(row - 1, col, data))
    }

    pub fn write_null_legacy(&mut self, col: u32) -> DsResult<()> {
        let b = self
            .block
            .as_mut()
            .ok_or(DataShareError::InvalidOperation)?;
        let row = b.row_num();
        if row == 0 {
            return Err(DataShareError::InvalidOperation);
        }
        result_from_code(b.put_null(row - 1, col))
    }

    pub fn write_long_legacy(&mut self, col: u32, v: i64) -> DsResult<()> {
        let b = self
            .block
            .as_mut()
            .ok_or(DataShareError::InvalidOperation)?;
        let row = b.row_num();
        if row == 0 {
            return Err(DataShareError::InvalidOperation);
        }
        result_from_code(b.put_long(row - 1, col, v))
    }

    pub fn write_double_legacy(&mut self, col: u32, v: f64) -> DsResult<()> {
        let b = self
            .block
            .as_mut()
            .ok_or(DataShareError::InvalidOperation)?;
        let row = b.row_num();
        if row == 0 {
            return Err(DataShareError::InvalidOperation);
        }
        result_from_code(b.put_double(row - 1, col, v))
    }

    /// Borrow a reference to the underlying block (alias for `block()`).
    pub fn get_block(&self) -> Option<&SharedBlock> {
        self.block.as_deref()
    }

    /// Transfer ownership of the block out of the writer. Returns `None` if
    /// already taken.
    pub fn take_block(&mut self) -> Option<SharedBlock> {
        self.block.take().map(|b| *b)
    }
}

impl Default for BlockWriter {
    fn default() -> Self {
        Self::new()
    }
}

// ---------------------------------------------------------------------------
// `Writer` trait glue (re-exposed from result_set_bridge) — adapt the legacy
// `-> Result<()>` surface to the new `i32`-returning methods.
// ---------------------------------------------------------------------------

impl crate::result_set_bridge::Writer for BlockWriter {
    fn alloc_row(&mut self) -> DsResult<()> {
        self.alloc_row_legacy()
    }
    fn write_null(&mut self, column: u32) -> DsResult<()> {
        self.write_null_legacy(column)
    }
    fn write_long(&mut self, column: u32, value: i64) -> DsResult<()> {
        self.write_long_legacy(column, value)
    }
    fn write_double(&mut self, column: u32, value: f64) -> DsResult<()> {
        self.write_double_legacy(column, value)
    }
    fn write_blob(&mut self, column: u32, value: &[u8]) -> DsResult<()> {
        self.write_blob_slice(column, value)
    }
    fn write_string(&mut self, column: u32, value: &str) -> DsResult<()> {
        self.write_string_str(column, value)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn convert_error_code_matches_cpp() {
        assert_eq!(convert_error_code(SHARED_BLOCK_OK), E_OK);
        assert_eq!(convert_error_code(SHARED_BLOCK_NO_MEMORY), E_ERROR);
        assert_eq!(convert_error_code(99), E_ERROR);
    }

    #[test]
    fn empty_writer() {
        let mut w = BlockWriter::new();
        assert_eq!(w.alloc_row(), E_ERROR);
        assert!(w.current_row_index().is_none());
    }
}
