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

//! Binary-compatible `SharedBlock` backed by an `OHOS::Ashmem` region.
//!
//! The on-wire layout must match C++
//! `foundation/distributeddatamgr/data_share/frameworks/native/common/include/shared_block.h`
//! exactly. All structures are `#[repr(C)]` / `#[repr(C, packed)]` to guarantee
//! identical byte layout with the C++ side.

#![allow(clippy::missing_safety_doc)]
#![allow(clippy::not_unsafe_ptr_arg_deref)]

use std::ffi::c_void;

use datashare_common::error::{DataShareError, Result as DsResult};
use utils_rust::ashmem::{create_ashmem_instance, Ashmem};

// ---------------------------------------------------------------------------
// Error / type constants (must match C++ exactly)
// ---------------------------------------------------------------------------

pub const SHARED_BLOCK_OK: i32 = 0;
pub const SHARED_BLOCK_BAD_VALUE: i32 = 1;
pub const SHARED_BLOCK_NO_MEMORY: i32 = 2;
pub const SHARED_BLOCK_INVALID_OPERATION: i32 = 3;
pub const SHARED_BLOCK_ASHMEM_ERROR: i32 = 4;
pub const SHARED_BLOCK_SET_PORT_ERROR: i32 = 5;

pub const CELL_UNIT_TYPE_NULL: i32 = 0;
pub const CELL_UNIT_TYPE_INTEGER: i32 = 1;
pub const CELL_UNIT_TYPE_FLOAT: i32 = 2;
pub const CELL_UNIT_TYPE_STRING: i32 = 3;
pub const CELL_UNIT_TYPE_BLOB: i32 = 4;

pub const COL_MAX_NUM: u32 = 32767;
pub const ROW_OFFSETS_NUM: usize = 100;

// ---------------------------------------------------------------------------
// Binary-compatible structs
// ---------------------------------------------------------------------------

#[repr(C)]
#[derive(Clone, Copy)]
pub struct SharedBlockHeader {
    pub unused_offset: u32,
    pub first_row_group_offset: u32,
    pub row_nums: u32,
    pub column_nums: u32,
    pub start_pos: u32,
    pub last_pos: u32,
    pub block_pos: u32,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct RowGroupHeader {
    pub row_offsets: [u32; ROW_OFFSETS_NUM],
    pub next_group_offset: u32,
}

#[repr(C, packed)]
#[derive(Clone, Copy)]
pub struct StringOrBlob {
    pub offset: u32,
    pub size: u32,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub union CellUnitValue {
    pub double_value: f64,
    pub long_value: i64,
    pub string_or_blob: StringOrBlob,
}

#[repr(C, packed)]
pub struct CellUnit {
    pub cell_type: i32,
    pub value: CellUnitValue,
}

impl Clone for CellUnit {
    fn clone(&self) -> Self {
        // SAFETY: `CellUnit` is POD from the C side (trivially copyable).
        unsafe { std::ptr::read(self as *const _) }
    }
}

// Flattened view used by the existing `DataShareResultSet` API surface.
// Kept as a compatibility shim so `result_set.rs` continues to compile.
#[derive(Debug, Clone, Copy, Default)]
pub struct CellValueLegacy {
    pub cell_type: i32,
    pub long_value: i64,
    pub double_value: f64,
    pub string_or_blob_offset: u32,
    pub string_or_blob_size: u32,
}

// ---------------------------------------------------------------------------
// SharedBlock
// ---------------------------------------------------------------------------

pub struct SharedBlock {
    name: String,
    ashmem: Ashmem,
    size: usize,
    read_only: bool,
    base: *mut u8, // mmap base pointer obtained via read_from_ashmem
}

// SAFETY: The underlying Ashmem is internally reference-counted via SharedPtr
// and the mmap region is stable for the lifetime of the object. Access is not
// inherently thread-safe, but the C++ side relies on external synchronisation,
// and so must the Rust side.
unsafe impl Send for SharedBlock {}

impl SharedBlock {
    // -----------------------------------------------------------------------
    // Construction
    // -----------------------------------------------------------------------

    /// Raw constructor matching the C++ ctor. Does NOT call `init`.
    pub fn new(name: String, ashmem: Ashmem, size: usize, read_only: bool) -> Self {
        Self {
            name,
            ashmem,
            size,
            read_only,
            base: std::ptr::null_mut(),
        }
    }

    /// Create a freshly-allocated SharedBlock backed by a new ashmem region.
    pub fn create(name: &str, size: usize) -> std::result::Result<Box<Self>, i32> {
        let ashmem_name = format!("SharedBlock:{}", name);
        let ashmem = match unsafe { create_ashmem_instance(&ashmem_name, size as i32) } {
            Some(a) => a,
            None => return Err(SHARED_BLOCK_ASHMEM_ERROR),
        };
        if !ashmem.map_read_write_ashmem() {
            ashmem.close_ashmem();
            return Err(SHARED_BLOCK_SET_PORT_ERROR);
        }

        let mut block = Box::new(Self::new(name.to_string(), ashmem, size, false));
        if !block.init() {
            block.ashmem.unmap_ashmem();
            block.ashmem.close_ashmem();
            return Err(SHARED_BLOCK_ASHMEM_ERROR);
        }

        // Initialize header to a known-empty state (matches C++ CreateSharedBlock
        // behaviour — the backing ashmem is zeroed by the kernel, so after Init
        // we install default header values for the first row group too).
        unsafe {
            let header = block.header_ptr_mut();
            (*header).unused_offset = std::mem::size_of::<SharedBlockHeader>() as u32
                + std::mem::size_of::<RowGroupHeader>() as u32;
            (*header).first_row_group_offset = std::mem::size_of::<SharedBlockHeader>() as u32;
            (*header).row_nums = 0;
            (*header).column_nums = 0;
            (*header).start_pos = 0;
            (*header).last_pos = 0;
            (*header).block_pos = 0;

            // First row group lives right after the header.
            if let Some(first) = block.offset_to_ptr(
                (*header).first_row_group_offset,
                std::mem::size_of::<RowGroupHeader>() as u32,
            ) {
                let group = first as *mut RowGroupHeader;
                std::ptr::write_bytes(group, 0, 1);
                (*group).next_group_offset = 0;
            }
        }

        Ok(block)
    }

    /// Take ownership of an existing ashmem fd (typically delivered across IPC)
    /// and wrap it as a `SharedBlock`.
    pub fn create_from_fd(
        name: String,
        fd: i32,
        size: usize,
        read_only: bool,
    ) -> std::result::Result<Box<Self>, i32> {
        let ashmem = match ffi_datashare_ashmem_helper::ashmem_from_fd(fd, size as i32) {
            Some(a) => a,
            None => return Err(SHARED_BLOCK_ASHMEM_ERROR),
        };

        let mapped = if read_only {
            ashmem.map_read_only_ashmem()
        } else {
            ashmem.map_read_write_ashmem()
        };
        if !mapped {
            ashmem.close_ashmem();
            return Err(SHARED_BLOCK_SET_PORT_ERROR);
        }

        let mut block = Box::new(Self::new(name, ashmem, size, read_only));
        if !block.init() {
            block.ashmem.unmap_ashmem();
            block.ashmem.close_ashmem();
            return Err(SHARED_BLOCK_ASHMEM_ERROR);
        }
        Ok(block)
    }

    /// Install the mmap base pointer (reads the first `header_size` bytes of
    /// the ashmem region, but the kernel returns the whole mapping).
    pub fn init(&mut self) -> bool {
        let header_size = std::mem::size_of::<SharedBlockHeader>() as i32;
        let ptr = unsafe { self.ashmem.read_from_ashmem(header_size, 0) } as *mut u8;
        if ptr.is_null() {
            return false;
        }
        self.base = ptr;
        true
    }

    // -----------------------------------------------------------------------
    // Pointer helpers
    // -----------------------------------------------------------------------

    #[inline]
    pub fn header_ptr(&self) -> *const SharedBlockHeader {
        self.base as *const SharedBlockHeader
    }

    #[inline]
    fn header_ptr_mut(&self) -> *mut SharedBlockHeader {
        self.base as *mut SharedBlockHeader
    }

    /// Returns a pointer inside the mmap region, or `None` on OOB.
    /// Mirrors C++ `SharedBlock::OffsetToPtr` exactly.
    #[inline]
    fn offset_to_ptr(&self, offset: u32, buffer_size: u32) -> Option<*mut u8> {
        if self.base.is_null() {
            return None;
        }
        if (offset as usize) >= self.size {
            return None;
        }
        if offset > u32::MAX - buffer_size {
            return None;
        }
        if (offset as usize) + (buffer_size as usize) >= self.size {
            return None;
        }
        // SAFETY: all bounds verified above.
        unsafe { Some(self.base.add(offset as usize)) }
    }

    // -----------------------------------------------------------------------
    // Mutators
    // -----------------------------------------------------------------------

    pub fn clear(&mut self) -> i32 {
        if self.read_only {
            return SHARED_BLOCK_INVALID_OPERATION;
        }
        let header = self.header_ptr_mut();
        unsafe {
            (*header).unused_offset = std::mem::size_of::<SharedBlockHeader>() as u32
                + std::mem::size_of::<RowGroupHeader>() as u32;
            (*header).first_row_group_offset = std::mem::size_of::<SharedBlockHeader>() as u32;
            (*header).row_nums = 0;
            (*header).column_nums = 0;
            (*header).start_pos = 0;
            (*header).last_pos = 0;
            (*header).block_pos = 0;
            let first_off = (*header).first_row_group_offset;
            let group_size = std::mem::size_of::<RowGroupHeader>() as u32;
            let group_ptr = match self.offset_to_ptr(first_off, group_size) {
                Some(p) => p as *mut RowGroupHeader,
                None => return SHARED_BLOCK_BAD_VALUE,
            };
            std::ptr::addr_of_mut!((*group_ptr).next_group_offset).write_unaligned(0);
        }
        SHARED_BLOCK_OK
    }

    pub fn set_column_num(&mut self, n: u32) -> i32 {
        if self.read_only {
            return SHARED_BLOCK_INVALID_OPERATION;
        }
        unsafe {
            let h = self.header_ptr_mut();
            let cur = (*h).column_nums;
            let rows = (*h).row_nums;
            if (cur > 0 || rows > 0) && cur != n {
                return SHARED_BLOCK_INVALID_OPERATION;
            }
            if n > COL_MAX_NUM {
                return SHARED_BLOCK_INVALID_OPERATION;
            }
            (*h).column_nums = n;
        }
        SHARED_BLOCK_OK
    }

    pub fn alloc_row(&mut self) -> i32 {
        if self.read_only {
            return SHARED_BLOCK_INVALID_OPERATION;
        }
        let row_off_ptr = self.alloc_row_offset();
        if row_off_ptr.is_null() {
            return SHARED_BLOCK_NO_MEMORY;
        }

        let column_nums = unsafe { (*self.header_ptr()).column_nums };
        let field_dir_size = (column_nums as usize) * std::mem::size_of::<CellUnit>();
        let field_dir_offset = self.alloc(field_dir_size, true);
        if field_dir_offset == 0 {
            // Back out the new row accounting.
            unsafe {
                let h = self.header_ptr_mut();
                if (*h).row_nums > 0 {
                    (*h).row_nums -= 1;
                }
            }
            return SHARED_BLOCK_NO_MEMORY;
        }

        let field_dir_ptr =
            match self.offset_to_ptr(field_dir_offset, std::mem::size_of::<CellUnit>() as u32) {
                Some(p) => p,
                None => return SHARED_BLOCK_BAD_VALUE,
            };
        // SAFETY: `alloc` returned `field_dir_size` contiguous writable bytes.
        unsafe {
            std::ptr::write_bytes(field_dir_ptr, 0, field_dir_size);
            *row_off_ptr = field_dir_offset;
        }
        SHARED_BLOCK_OK
    }

    pub fn free_last_row(&mut self) -> i32 {
        if self.read_only {
            return SHARED_BLOCK_INVALID_OPERATION;
        }
        unsafe {
            let h = self.header_ptr_mut();
            if (*h).row_nums > 0 {
                (*h).row_nums -= 1;
            }
        }
        SHARED_BLOCK_OK
    }

    pub fn put_blob(&mut self, row: u32, col: u32, data: &[u8]) -> i32 {
        self.put_blob_or_string(row, col, data.as_ptr(), data.len(), CELL_UNIT_TYPE_BLOB)
    }

    /// `size_inc_null` includes the trailing NUL byte, matching C++.
    pub fn put_string(
        &mut self,
        row: u32,
        col: u32,
        value: *const u8,
        size_inc_null: usize,
    ) -> i32 {
        self.put_blob_or_string(row, col, value, size_inc_null, CELL_UNIT_TYPE_STRING)
    }

    pub fn put_long(&mut self, row: u32, col: u32, v: i64) -> i32 {
        if self.read_only {
            return SHARED_BLOCK_INVALID_OPERATION;
        }
        let cell = self.get_cell_unit(row, col) as *mut CellUnit;
        if cell.is_null() {
            return SHARED_BLOCK_BAD_VALUE;
        }
        // SAFETY: `cell` is a packed struct inside the mmap region.
        unsafe {
            let cell_val = CellUnit {
                cell_type: CELL_UNIT_TYPE_INTEGER,
                value: CellUnitValue { long_value: v },
            };
            std::ptr::write_unaligned(cell, cell_val);
        }
        SHARED_BLOCK_OK
    }

    pub fn put_double(&mut self, row: u32, col: u32, v: f64) -> i32 {
        if self.read_only {
            return SHARED_BLOCK_INVALID_OPERATION;
        }
        let cell = self.get_cell_unit(row, col) as *mut CellUnit;
        if cell.is_null() {
            return SHARED_BLOCK_BAD_VALUE;
        }
        unsafe {
            let cell_val = CellUnit {
                cell_type: CELL_UNIT_TYPE_FLOAT,
                value: CellUnitValue { double_value: v },
            };
            std::ptr::write_unaligned(cell, cell_val);
        }
        SHARED_BLOCK_OK
    }

    pub fn put_null(&mut self, row: u32, col: u32) -> i32 {
        if self.read_only {
            return SHARED_BLOCK_INVALID_OPERATION;
        }
        let cell = self.get_cell_unit(row, col) as *mut CellUnit;
        if cell.is_null() {
            return SHARED_BLOCK_BAD_VALUE;
        }
        unsafe {
            let cell_val = CellUnit {
                cell_type: CELL_UNIT_TYPE_NULL,
                value: CellUnitValue {
                    string_or_blob: StringOrBlob { offset: 0, size: 0 },
                },
            };
            std::ptr::write_unaligned(cell, cell_val);
        }
        SHARED_BLOCK_OK
    }

    // -----------------------------------------------------------------------
    // Lookups
    // -----------------------------------------------------------------------

    /// Returns a pointer to the cell in the mmap region, or null on OOB.
    pub fn get_cell_unit(&self, row: u32, col: u32) -> *const CellUnit {
        let (rows, cols) = unsafe {
            (
                (*self.header_ptr()).row_nums,
                (*self.header_ptr()).column_nums,
            )
        };
        if row >= rows || col >= cols {
            return std::ptr::null();
        }
        let row_off_ptr = self.get_row_offset(row);
        if row_off_ptr.is_null() {
            return std::ptr::null();
        }
        let row_off = unsafe { *row_off_ptr };
        let cell_size = std::mem::size_of::<CellUnit>() as u32;
        let base = match self.offset_to_ptr(row_off, cell_size) {
            Some(p) => p,
            None => return std::ptr::null(),
        };
        // SAFETY: bounds already validated by `offset_to_ptr` for at least one
        // cell; callers only read `col < column_nums` cells.
        unsafe { base.add((col as usize) * std::mem::size_of::<CellUnit>()) as *const CellUnit }
    }

    /// Unsafe: `cell` must be a valid `CellUnit` of type STRING inside the
    /// mmap region. Returns a pointer to the string bytes (including NUL).
    pub unsafe fn get_cell_unit_value_string(
        &self,
        cell: *const CellUnit,
        out_size: &mut u32,
    ) -> *const u8 {
        let sob = std::ptr::read_unaligned(std::ptr::addr_of!((*cell).value.string_or_blob));
        *out_size = sob.size;
        match self.offset_to_ptr(sob.offset, sob.size) {
            Some(p) => p as *const u8,
            None => std::ptr::null(),
        }
    }

    pub unsafe fn get_cell_unit_value_blob(
        &self,
        cell: *const CellUnit,
        out_size: &mut u32,
    ) -> *const u8 {
        self.get_cell_unit_value_string(cell, out_size)
    }

    // -----------------------------------------------------------------------
    // Simple getters/setters
    // -----------------------------------------------------------------------

    pub fn name(&self) -> &str {
        &self.name
    }
    pub fn size(&self) -> usize {
        self.size
    }
    pub fn fd(&self) -> i32 {
        self.ashmem.get_ashmem_fd()
    }
    pub fn ashmem(&self) -> &Ashmem {
        &self.ashmem
    }
    pub fn is_read_only(&self) -> bool {
        self.read_only
    }
    pub fn row_num(&self) -> u32 {
        if self.base.is_null() {
            return 0;
        }
        unsafe { (*self.header_ptr()).row_nums }
    }
    pub fn column_num(&self) -> u32 {
        if self.base.is_null() {
            return 0;
        }
        unsafe { (*self.header_ptr()).column_nums }
    }
    pub fn used_bytes(&self) -> usize {
        if self.base.is_null() {
            return 0;
        }
        unsafe { (*self.header_ptr()).unused_offset as usize }
    }
    pub fn start_pos(&self) -> u32 {
        unsafe { (*self.header_ptr()).start_pos }
    }
    pub fn last_pos(&self) -> u32 {
        unsafe { (*self.header_ptr()).last_pos }
    }
    pub fn block_pos(&self) -> u32 {
        unsafe { (*self.header_ptr()).block_pos }
    }
    pub fn set_start_pos(&mut self, v: u32) {
        unsafe { (*self.header_ptr_mut()).start_pos = v }
    }
    pub fn set_last_pos(&mut self, v: u32) {
        unsafe { (*self.header_ptr_mut()).last_pos = v }
    }
    pub fn set_block_pos(&mut self, v: u32) {
        unsafe { (*self.header_ptr_mut()).block_pos = v }
    }

    /// Copy `size` bytes from `data` into the mmap region starting at offset 0.
    /// Returns SHARED_BLOCK_OK on success, else an error code (as `usize` to
    /// match C++ return type).
    pub fn set_raw_data(&mut self, data: *const u8, size: usize) -> usize {
        if size == 0 {
            return SHARED_BLOCK_INVALID_OPERATION as usize;
        }
        if size > self.size {
            return SHARED_BLOCK_NO_MEMORY as usize;
        }
        if self.base.is_null() || data.is_null() {
            return SHARED_BLOCK_NO_MEMORY as usize;
        }
        // SAFETY: caller provides `size` valid bytes at `data`; we validated
        // the destination fits.
        unsafe {
            std::ptr::copy_nonoverlapping(data, self.base, size);
        }
        SHARED_BLOCK_OK as usize
    }

    // -----------------------------------------------------------------------
    // MessageParcel serialization
    // -----------------------------------------------------------------------

    /// Write `name` (as interface token) + ashmem handle into the parcel.
    /// `parcel_ptr` must be a `*mut OHOS::MessageParcel`.
    pub fn write_message_parcel(&self, parcel_ptr: *mut c_void) -> i32 {
        use ipc::parcel::MsgParcel;
        let mut parcel = MsgParcel::from_ptr(parcel_ptr as *mut _);
        if parcel.write_string16(&self.name).is_err() {
            std::mem::forget(parcel);
            return SHARED_BLOCK_BAD_VALUE;
        }
        let shared_clone = unsafe { self.ashmem.c_ashmem() }.clone();
        let ashmem_to_write = utils_rust::ashmem::Ashmem::new(shared_clone);
        let res = match parcel.write_ashmem(ashmem_to_write) {
            Ok(()) => SHARED_BLOCK_OK,
            Err(_) => SHARED_BLOCK_BAD_VALUE,
        };
        // Do not drop `parcel` (which would free the underlying C++ parcel
        // owned by the caller) — we only borrowed it.
        std::mem::forget(parcel);
        res
    }

    /// Read a SharedBlock from a parcel. Counterpart of `write_message_parcel`.
    pub fn read_message_parcel(parcel_ptr: *mut c_void) -> std::result::Result<Box<Self>, i32> {
        use ipc::parcel::MsgParcel;
        let mut parcel = MsgParcel::from_ptr(parcel_ptr as *mut _);
        let name = match parcel.read_string16() {
            Ok(n) => n,
            Err(_) => {
                std::mem::forget(parcel);
                return Err(SHARED_BLOCK_BAD_VALUE);
            }
        };
        let ashmem = match parcel.read_ashmem() {
            Ok(a) => a,
            Err(_) => {
                std::mem::forget(parcel);
                return Err(SHARED_BLOCK_BAD_VALUE);
            }
        };
        std::mem::forget(parcel);

        if !ashmem.map_read_write_ashmem() {
            ashmem.close_ashmem();
            return Err(SHARED_BLOCK_SET_PORT_ERROR);
        }
        let size = ashmem.get_ashmem_size() as usize;
        let mut block = Box::new(Self::new(name, ashmem, size, true));
        if !block.init() {
            block.ashmem.unmap_ashmem();
            block.ashmem.close_ashmem();
            return Err(SHARED_BLOCK_ASHMEM_ERROR);
        }
        Ok(block)
    }

    // -----------------------------------------------------------------------
    // Private helpers
    // -----------------------------------------------------------------------

    /// Reserve `size` bytes in the mmap region, bumping `header.unused_offset`.
    /// Returns the starting offset, or 0 on OOM (matches C++).
    fn alloc(&mut self, size: usize, aligned: bool) -> u32 {
        unsafe {
            let h = self.header_ptr_mut();
            let offset_digit: u32 = 3;
            let padding = if aligned {
                ((!(*h).unused_offset).wrapping_add(1)) & offset_digit
            } else {
                0
            };
            let offset = (*h).unused_offset + padding;
            if (offset as usize) + size >= self.size {
                return 0;
            }
            (*h).unused_offset = offset + size as u32;
            offset
        }
    }

    fn get_row_offset(&self, row: u32) -> *mut u32 {
        let mut row_pos = row;
        let group_size = std::mem::size_of::<RowGroupHeader>() as u32;
        let first_off = unsafe { (*self.header_ptr()).first_row_group_offset };
        let mut group_ptr = match self.offset_to_ptr(first_off, group_size) {
            Some(p) => p as *mut RowGroupHeader,
            None => return std::ptr::null_mut(),
        };
        while (row_pos as usize) >= ROW_OFFSETS_NUM {
            let next = unsafe { (*group_ptr).next_group_offset };
            group_ptr = match self.offset_to_ptr(next, group_size) {
                Some(p) => p as *mut RowGroupHeader,
                None => return std::ptr::null_mut(),
            };
            row_pos -= ROW_OFFSETS_NUM as u32;
        }
        unsafe { &mut (*group_ptr).row_offsets[row_pos as usize] as *mut u32 }
    }

    fn alloc_row_offset(&mut self) -> *mut u32 {
        let mut row_pos = unsafe { (*self.header_ptr()).row_nums };
        let group_size = std::mem::size_of::<RowGroupHeader>() as u32;
        let first_off = unsafe { (*self.header_ptr()).first_row_group_offset };
        let mut group_ptr = match self.offset_to_ptr(first_off, group_size) {
            Some(p) => p as *mut RowGroupHeader,
            None => return std::ptr::null_mut(),
        };
        while row_pos > ROW_OFFSETS_NUM as u32 {
            let next = unsafe { (*group_ptr).next_group_offset };
            group_ptr = match self.offset_to_ptr(next, group_size) {
                Some(p) => p as *mut RowGroupHeader,
                None => return std::ptr::null_mut(),
            };
            row_pos -= ROW_OFFSETS_NUM as u32;
        }
        if row_pos == ROW_OFFSETS_NUM as u32 {
            let next_existing = unsafe { (*group_ptr).next_group_offset };
            if next_existing == 0 {
                let new_off = self.alloc(std::mem::size_of::<RowGroupHeader>(), true);
                if new_off == 0 {
                    return std::ptr::null_mut();
                }
                unsafe {
                    (*group_ptr).next_group_offset = new_off;
                }
            }
            let next_off = unsafe { (*group_ptr).next_group_offset };
            group_ptr = match self.offset_to_ptr(next_off, group_size) {
                Some(p) => p as *mut RowGroupHeader,
                None => return std::ptr::null_mut(),
            };
            unsafe {
                (*group_ptr).next_group_offset = 0;
            }
            row_pos = 0;
        }
        unsafe {
            (*self.header_ptr_mut()).row_nums += 1;
            &mut (*group_ptr).row_offsets[row_pos as usize] as *mut u32
        }
    }

    fn put_blob_or_string(
        &mut self,
        row: u32,
        col: u32,
        data: *const u8,
        size: usize,
        kind: i32,
    ) -> i32 {
        if self.read_only {
            return SHARED_BLOCK_INVALID_OPERATION;
        }
        let cell = self.get_cell_unit(row, col) as *mut CellUnit;
        if cell.is_null() {
            return SHARED_BLOCK_BAD_VALUE;
        }
        let offset = self.alloc(size, false);
        if offset == 0 {
            return SHARED_BLOCK_NO_MEMORY;
        }
        let dst = match self.offset_to_ptr(offset, size as u32) {
            Some(p) => p,
            None => return SHARED_BLOCK_NO_MEMORY,
        };
        if size != 0 && !data.is_null() {
            // SAFETY: caller guarantees `data` has at least `size` bytes.
            unsafe {
                std::ptr::copy_nonoverlapping(data, dst, size);
            }
        }
        // SAFETY: `cell` is a packed CellUnit in the mmap region.
        unsafe {
            let cell_val = CellUnit {
                cell_type: kind,
                value: CellUnitValue {
                    string_or_blob: StringOrBlob {
                        offset,
                        size: size as u32,
                    },
                },
            };
            std::ptr::write_unaligned(cell, cell_val);
        }
        SHARED_BLOCK_OK
    }

    // -----------------------------------------------------------------------
    // Legacy compatibility shims (used by result_set.rs / block_writer.rs).
    // These map between the new i32 error codes and `DataShareError`, and
    // expose a flattened `CellValueLegacy` view.
    // -----------------------------------------------------------------------

    pub fn get_row_count(&self) -> u32 {
        self.row_num()
    }
    pub fn get_column_count(&self) -> u32 {
        self.column_num()
    }
    pub fn get_used_bytes(&self) -> usize {
        self.used_bytes()
    }
    pub fn get_fd(&self) -> i32 {
        self.fd()
    }

    pub fn get_cell_value(&self, row: u32, col: u32) -> DsResult<(i32, CellValueLegacy)> {
        let ptr = self.get_cell_unit(row, col);
        if ptr.is_null() {
            return Err(DataShareError::InvalidColumnIndex);
        }
        // SAFETY: valid CellUnit pointer inside mmap region.
        let cell = unsafe { std::ptr::read_unaligned(ptr) };
        let ty = cell.cell_type;
        let mut legacy = CellValueLegacy {
            cell_type: ty,
            ..Default::default()
        };
        unsafe {
            match ty {
                CELL_UNIT_TYPE_INTEGER => legacy.long_value = cell.value.long_value,
                CELL_UNIT_TYPE_FLOAT => legacy.double_value = cell.value.double_value,
                CELL_UNIT_TYPE_STRING | CELL_UNIT_TYPE_BLOB => {
                    let sob = cell.value.string_or_blob;
                    legacy.string_or_blob_offset = sob.offset;
                    legacy.string_or_blob_size = sob.size;
                }
                _ => {}
            }
        }
        Ok((ty, legacy))
    }

    pub fn get_string_data(&self, offset: u32, size: u32) -> DsResult<Vec<u8>> {
        let p = self
            .offset_to_ptr(offset, size)
            .ok_or(DataShareError::InvalidColumnIndex)?;
        // SAFETY: `offset_to_ptr` validated `size` bytes are inside the mmap.
        let slice = unsafe { std::slice::from_raw_parts(p, size as usize) };
        Ok(slice.to_vec())
    }

    pub fn get_cell_unit_string(&self, cell: &CellValueLegacy) -> DsResult<String> {
        self.get_cell_unit_str(cell).map(|s| s.to_owned())
    }

    /// Zero-copy string access: returns a `&str` slice directly from the mmap region.
    pub fn get_cell_unit_str(&self, cell: &CellValueLegacy) -> DsResult<&str> {
        if cell.cell_type != CELL_UNIT_TYPE_STRING {
            return Err(DataShareError::InvalidOperation);
        }
        let p = self
            .offset_to_ptr(cell.string_or_blob_offset, cell.string_or_blob_size)
            .ok_or(DataShareError::InvalidColumnIndex)?;
        // SAFETY: `offset_to_ptr` validated `size` bytes are inside the mmap.
        let slice = unsafe { std::slice::from_raw_parts(p, cell.string_or_blob_size as usize) };
        let end = slice.iter().position(|&b| b == 0).unwrap_or(slice.len());
        std::str::from_utf8(&slice[..end]).map_err(|_| DataShareError::InvalidOperation)
    }

    // Legacy, string-convenience wrappers (existing callers pass `&str`).
    pub fn put_string_str(&mut self, row: u32, col: u32, value: &str) -> DsResult<()> {
        let mut buf = value.as_bytes().to_vec();
        buf.push(0);
        let code = self.put_string(row, col, buf.as_ptr(), buf.len());
        result_from_code(code)
    }
}

impl Drop for SharedBlock {
    fn drop(&mut self) {
        self.ashmem.unmap_ashmem();
        self.ashmem.close_ashmem();
    }
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

pub(crate) fn result_from_code(code: i32) -> DsResult<()> {
    match code {
        SHARED_BLOCK_OK => Ok(()),
        SHARED_BLOCK_NO_MEMORY => Err(DataShareError::OutOfMemory),
        SHARED_BLOCK_INVALID_OPERATION => Err(DataShareError::InvalidOperation),
        SHARED_BLOCK_BAD_VALUE => Err(DataShareError::InvalidParameter),
        _ => Err(DataShareError::GeneralError),
    }
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn struct_sizes_match_cpp() {
        assert_eq!(std::mem::size_of::<SharedBlockHeader>(), 28);
        assert_eq!(std::mem::size_of::<RowGroupHeader>(), 404);
        assert_eq!(std::mem::size_of::<CellUnit>(), 12);
    }

    #[test]
    fn shared_block_is_send() {
        fn assert_send<T: Send>() {}
        assert_send::<SharedBlock>();
    }

    #[test]
    fn end_to_end_put_get() {
        let mut block = match SharedBlock::create("x", 8192) {
            Ok(b) => b,
            Err(_) => return, // environment may not support ashmem in unit tests
        };
        assert_eq!(block.set_column_num(2), SHARED_BLOCK_OK);
        assert_eq!(block.alloc_row(), SHARED_BLOCK_OK);
        assert_eq!(block.put_long(0, 0, 42), SHARED_BLOCK_OK);
        let s = b"hello\0";
        assert_eq!(block.put_string(0, 1, s.as_ptr(), s.len()), SHARED_BLOCK_OK);

        let cell_ptr = block.get_cell_unit(0, 0);
        assert!(!cell_ptr.is_null());
        let cell = unsafe { std::ptr::read_unaligned(cell_ptr) };
        let ty = cell.cell_type;
        assert_eq!(ty, CELL_UNIT_TYPE_INTEGER);
        let v = unsafe { cell.value.long_value };
        assert_eq!(v, 42);
    }
}
