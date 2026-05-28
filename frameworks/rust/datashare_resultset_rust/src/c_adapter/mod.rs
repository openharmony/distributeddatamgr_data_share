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

//! C FFI shim for `SharedBlock`, `BlockWriter`, and `DataShareResultSet`.

#![allow(clippy::missing_safety_doc)]
#![allow(clippy::not_unsafe_ptr_arg_deref)]
#![allow(non_snake_case)]

use std::ffi::{c_char, c_void, CStr};
use std::ptr;

use crate::block_writer::BlockWriter;
use crate::result_set::{DataShareResultSet, ResultSet};
use crate::shared_block::{SharedBlock, SHARED_BLOCK_BAD_VALUE, SHARED_BLOCK_OK};

type OpaqueSharedBlock = SharedBlock;
type OpaqueBlockWriter = BlockWriter;
type OpaqueResultSet = DataShareResultSet;

// ---------------------------------------------------------------------------
// Helper: read a `name`/`name_len` pair into a borrowed &str.
// ---------------------------------------------------------------------------
unsafe fn name_slice<'a>(name: *const c_char, name_len: u32) -> Option<&'a str> {
    if name.is_null() {
        return None;
    }
    let bytes = std::slice::from_raw_parts(name as *const u8, name_len as usize);
    std::str::from_utf8(bytes).ok()
}

// ===========================================================================
// SharedBlock FFI
// ===========================================================================

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockCreate(
    name: *const c_char,
    name_len: u32,
    size: u32,
    out: *mut *mut OpaqueSharedBlock,
) -> i32 {
    if out.is_null() {
        return SHARED_BLOCK_BAD_VALUE;
    }
    *out = ptr::null_mut();
    let name_str = match name_slice(name, name_len) {
        Some(s) => s,
        None => return SHARED_BLOCK_BAD_VALUE,
    };
    match SharedBlock::create(name_str, size as usize) {
        Ok(boxed) => {
            *out = Box::into_raw(boxed);
            SHARED_BLOCK_OK
        }
        Err(code) => code,
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockNewFromFd(
    name: *const c_char,
    name_len: u32,
    fd: i32,
    ash_size: i32,
    read_only: bool,
    out: *mut *mut OpaqueSharedBlock,
) -> i32 {
    if out.is_null() {
        return SHARED_BLOCK_BAD_VALUE;
    }
    *out = ptr::null_mut();
    let name_str = match name_slice(name, name_len) {
        Some(s) => s.to_string(),
        None => return SHARED_BLOCK_BAD_VALUE,
    };
    match SharedBlock::create_from_fd(name_str, fd, ash_size as usize, read_only) {
        Ok(boxed) => {
            *out = Box::into_raw(boxed);
            SHARED_BLOCK_OK
        }
        Err(code) => code,
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockFree(h: *mut OpaqueSharedBlock) {
    if !h.is_null() {
        drop(Box::from_raw(h));
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockInit(h: *mut OpaqueSharedBlock) -> bool {
    if h.is_null() {
        return false;
    }
    (*h).init()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockClear(h: *mut OpaqueSharedBlock) -> i32 {
    if h.is_null() {
        return SHARED_BLOCK_BAD_VALUE;
    }
    (*h).clear()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockSetColumnNum(
    h: *mut OpaqueSharedBlock,
    cols: u32,
) -> i32 {
    if h.is_null() {
        return SHARED_BLOCK_BAD_VALUE;
    }
    (*h).set_column_num(cols)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockAllocRow(h: *mut OpaqueSharedBlock) -> i32 {
    if h.is_null() {
        return SHARED_BLOCK_BAD_VALUE;
    }
    (*h).alloc_row()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockFreeLastRow(h: *mut OpaqueSharedBlock) -> i32 {
    if h.is_null() {
        return SHARED_BLOCK_BAD_VALUE;
    }
    (*h).free_last_row()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockPutBlob(
    h: *mut OpaqueSharedBlock,
    row: u32,
    col: u32,
    value: *const c_void,
    size: u32,
) -> i32 {
    if h.is_null() || (value.is_null() && size != 0) {
        return SHARED_BLOCK_BAD_VALUE;
    }
    let slice = std::slice::from_raw_parts(value as *const u8, size as usize);
    (*h).put_blob(row, col, slice)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockPutString(
    h: *mut OpaqueSharedBlock,
    row: u32,
    col: u32,
    value: *const c_char,
    size_with_null: u32,
) -> i32 {
    if h.is_null() || (value.is_null() && size_with_null != 0) {
        return SHARED_BLOCK_BAD_VALUE;
    }
    (*h).put_string(row, col, value as *const u8, size_with_null as usize)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockPutLong(
    h: *mut OpaqueSharedBlock,
    row: u32,
    col: u32,
    v: i64,
) -> i32 {
    if h.is_null() {
        return SHARED_BLOCK_BAD_VALUE;
    }
    (*h).put_long(row, col, v)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockPutDouble(
    h: *mut OpaqueSharedBlock,
    row: u32,
    col: u32,
    v: f64,
) -> i32 {
    if h.is_null() {
        return SHARED_BLOCK_BAD_VALUE;
    }
    (*h).put_double(row, col, v)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockPutNull(
    h: *mut OpaqueSharedBlock,
    row: u32,
    col: u32,
) -> i32 {
    if h.is_null() {
        return SHARED_BLOCK_BAD_VALUE;
    }
    (*h).put_null(row, col)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockGetCellUnit(
    h: *mut OpaqueSharedBlock,
    row: u32,
    col: u32,
) -> *const c_void {
    if h.is_null() {
        return ptr::null();
    }
    (*h).get_cell_unit(row, col) as *const c_void
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockGetCellUnitValueString(
    h: *mut OpaqueSharedBlock,
    cell: *const c_void,
    out_size: *mut u32,
) -> *const c_char {
    if h.is_null() || cell.is_null() || out_size.is_null() {
        return ptr::null();
    }
    let mut sz: u32 = 0;
    let p = (*h).get_cell_unit_value_string(cell as *const _, &mut sz);
    *out_size = sz;
    p as *const c_char
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockGetCellUnitValueBlob(
    h: *mut OpaqueSharedBlock,
    cell: *const c_void,
    out_size: *mut u32,
) -> *const c_void {
    if h.is_null() || cell.is_null() || out_size.is_null() {
        return ptr::null();
    }
    let mut sz: u32 = 0;
    let p = (*h).get_cell_unit_value_blob(cell as *const _, &mut sz);
    *out_size = sz;
    p as *const c_void
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockName(
    h: *mut OpaqueSharedBlock,
    out_buf: *mut *mut u8,
    out_len: *mut u32,
) -> i32 {
    if h.is_null() || out_buf.is_null() || out_len.is_null() {
        return SHARED_BLOCK_BAD_VALUE;
    }
    let bytes = (*h).name().as_bytes().to_vec();
    let len = bytes.len() as u32;
    let boxed = bytes.into_boxed_slice();
    *out_buf = Box::into_raw(boxed) as *mut u8;
    *out_len = len;
    SHARED_BLOCK_OK
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockSize(h: *mut OpaqueSharedBlock) -> usize {
    if h.is_null() {
        return 0;
    }
    (*h).size()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockGetRowNum(h: *mut OpaqueSharedBlock) -> u32 {
    if h.is_null() {
        return 0;
    }
    (*h).row_num()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockGetColumnNum(h: *mut OpaqueSharedBlock) -> u32 {
    if h.is_null() {
        return 0;
    }
    (*h).column_num()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockGetUsedBytes(h: *mut OpaqueSharedBlock) -> usize {
    if h.is_null() {
        return 0;
    }
    (*h).used_bytes()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockGetFd(h: *mut OpaqueSharedBlock) -> i32 {
    if h.is_null() {
        return -1;
    }
    (*h).fd()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockGetHeader(h: *mut OpaqueSharedBlock) -> *const c_void {
    if h.is_null() {
        return ptr::null();
    }
    (*h).header_ptr() as *const c_void
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockSetRawData(
    h: *mut OpaqueSharedBlock,
    data: *const c_void,
    size: u32,
) -> usize {
    if h.is_null() {
        return SHARED_BLOCK_BAD_VALUE as usize;
    }
    (*h).set_raw_data(data as *const u8, size as usize)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockGetStartPos(h: *mut OpaqueSharedBlock) -> u32 {
    if h.is_null() {
        return 0;
    }
    (*h).start_pos()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockGetLastPos(h: *mut OpaqueSharedBlock) -> u32 {
    if h.is_null() {
        return 0;
    }
    (*h).last_pos()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockGetBlockPos(h: *mut OpaqueSharedBlock) -> u32 {
    if h.is_null() {
        return 0;
    }
    (*h).block_pos()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockSetStartPos(h: *mut OpaqueSharedBlock, v: u32) {
    if h.is_null() {
        return;
    }
    (*h).set_start_pos(v);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockSetLastPos(h: *mut OpaqueSharedBlock, v: u32) {
    if h.is_null() {
        return;
    }
    (*h).set_last_pos(v);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockSetBlockPos(h: *mut OpaqueSharedBlock, v: u32) {
    if h.is_null() {
        return;
    }
    (*h).set_block_pos(v);
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockWriteMessageParcel(
    h: *mut OpaqueSharedBlock,
    parcel_ptr: *mut c_void,
) -> i32 {
    if h.is_null() || parcel_ptr.is_null() {
        return SHARED_BLOCK_BAD_VALUE;
    }
    (*h).write_message_parcel(parcel_ptr)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareSharedBlockReadMessageParcel(
    parcel_ptr: *mut c_void,
    out: *mut *mut OpaqueSharedBlock,
) -> i32 {
    if out.is_null() || parcel_ptr.is_null() {
        return SHARED_BLOCK_BAD_VALUE;
    }
    *out = ptr::null_mut();
    match SharedBlock::read_message_parcel(parcel_ptr) {
        Ok(boxed) => {
            *out = Box::into_raw(boxed);
            SHARED_BLOCK_OK
        }
        Err(code) => code,
    }
}

// ===========================================================================
// BlockWriter FFI
// ===========================================================================

#[no_mangle]
pub unsafe extern "C" fn DataShareBlockWriterNew() -> *mut OpaqueBlockWriter {
    Box::into_raw(Box::new(BlockWriter::new()))
}

#[no_mangle]
pub unsafe extern "C" fn DataShareBlockWriterNewWithBlock(
    name: *const c_char,
    name_len: u32,
    size: u32,
) -> *mut OpaqueBlockWriter {
    let name_str = match name_slice(name, name_len) {
        Some(s) => s,
        None => return ptr::null_mut(),
    };
    Box::into_raw(Box::new(BlockWriter::new_with_block(
        name_str,
        size as usize,
    )))
}

#[no_mangle]
pub unsafe extern "C" fn DataShareBlockWriterFree(h: *mut OpaqueBlockWriter) {
    if !h.is_null() {
        drop(Box::from_raw(h));
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareBlockWriterAllocRow(h: *mut OpaqueBlockWriter) -> i32 {
    if h.is_null() {
        return -1;
    }
    (*h).alloc_row()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareBlockWriterFreeLastRow(h: *mut OpaqueBlockWriter) -> i32 {
    if h.is_null() {
        return -1;
    }
    (*h).free_last_row()
}

#[no_mangle]
pub unsafe extern "C" fn DataShareBlockWriterWriteNull(h: *mut OpaqueBlockWriter, col: u32) -> i32 {
    if h.is_null() {
        return -1;
    }
    (*h).write_null(col)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareBlockWriterWriteLong(
    h: *mut OpaqueBlockWriter,
    col: u32,
    v: i64,
) -> i32 {
    if h.is_null() {
        return -1;
    }
    (*h).write_long(col, v)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareBlockWriterWriteDouble(
    h: *mut OpaqueBlockWriter,
    col: u32,
    v: f64,
) -> i32 {
    if h.is_null() {
        return -1;
    }
    (*h).write_double(col, v)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareBlockWriterWriteBlob(
    h: *mut OpaqueBlockWriter,
    col: u32,
    v: *const u8,
    size: u32,
) -> i32 {
    if h.is_null() || (v.is_null() && size != 0) {
        return -1;
    }
    let slice = std::slice::from_raw_parts(v, size as usize);
    (*h).write_blob(col, slice)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareBlockWriterWriteString(
    h: *mut OpaqueBlockWriter,
    col: u32,
    v: *const c_char,
    size_with_null: u32,
) -> i32 {
    if h.is_null() || v.is_null() {
        return -1;
    }
    (*h).write_string(col, v as *const u8, size_with_null as usize)
}

#[no_mangle]
pub unsafe extern "C" fn DataShareBlockWriterGetBlock(
    h: *mut OpaqueBlockWriter,
) -> *mut OpaqueSharedBlock {
    if h.is_null() {
        return ptr::null_mut();
    }
    match (*h).block_mut() {
        Some(b) => b as *mut SharedBlock,
        None => ptr::null_mut(),
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareBlockWriterGetCurrentRowIndex(
    h: *mut OpaqueBlockWriter,
    out: *mut u32,
) -> bool {
    if h.is_null() || out.is_null() {
        return false;
    }
    match (*h).current_row_index() {
        Some(v) => {
            *out = v;
            true
        }
        None => false,
    }
}

// ===========================================================================
// DataShareResultSet FFI — preserved from the previous generation, minimally
// updated for the new SharedBlock API.
// ===========================================================================

#[no_mangle]
pub extern "C" fn DataShareResultSetNew() -> *mut OpaqueResultSet {
    Box::into_raw(Box::new(DataShareResultSet::new()))
}

#[no_mangle]
pub unsafe extern "C" fn DataShareResultSetFree(rs: *mut OpaqueResultSet) {
    if !rs.is_null() {
        drop(Box::from_raw(rs));
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareResultSetSetBlock(
    rs: *mut OpaqueResultSet,
    block: *mut OpaqueSharedBlock,
) -> i32 {
    if rs.is_null() || block.is_null() {
        return -1;
    }
    // Transfer ownership: consume the Box and move the SharedBlock into the
    // result set.
    let boxed = Box::from_raw(block);
    (*rs).set_block(*boxed);
    0
}

#[no_mangle]
pub unsafe extern "C" fn DataShareResultSetSetColumnNames(
    rs: *mut OpaqueResultSet,
    names: *const *const c_char,
    count: u32,
) -> i32 {
    if rs.is_null() || names.is_null() {
        return -1;
    }
    let mut column_names = Vec::new();
    for i in 0..count {
        let name_ptr = *names.add(i as usize);
        if name_ptr.is_null() {
            continue;
        }
        let name = CStr::from_ptr(name_ptr).to_string_lossy().into_owned();
        column_names.push(name);
    }
    (*rs).set_column_names(column_names);
    0
}

#[no_mangle]
pub unsafe extern "C" fn DataShareResultSetGoToRow(rs: *mut OpaqueResultSet, position: i32) -> i32 {
    if rs.is_null() {
        return -1;
    }
    match (*rs).go_to_row(position) {
        Ok(true) => 1,
        Ok(false) => 0,
        Err(e) => e as i32,
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareResultSetGetLong(
    rs: *mut OpaqueResultSet,
    column: i32,
    out_value: *mut i64,
) -> i32 {
    if rs.is_null() || out_value.is_null() {
        return -1;
    }
    match (*rs).get_long(column) {
        Ok(value) => {
            *out_value = value;
            0
        }
        Err(e) => e as i32,
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareResultSetGetDouble(
    rs: *mut OpaqueResultSet,
    column: i32,
    out_value: *mut f64,
) -> i32 {
    if rs.is_null() || out_value.is_null() {
        return -1;
    }
    match (*rs).get_double(column) {
        Ok(value) => {
            *out_value = value;
            0
        }
        Err(e) => e as i32,
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareResultSetGetString(
    rs: *mut OpaqueResultSet,
    column: i32,
    out_data: *mut *mut u8,
    out_len: *mut usize,
) -> i32 {
    if rs.is_null() || out_data.is_null() || out_len.is_null() {
        return -1;
    }
    match (*rs).get_string(column) {
        Ok(value) => {
            let bytes = value.into_bytes();
            let len = bytes.len();
            let boxed = bytes.into_boxed_slice();
            *out_data = Box::into_raw(boxed) as *mut u8;
            *out_len = len;
            0
        }
        Err(e) => e as i32,
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareResultSetIsColumnNull(
    rs: *mut OpaqueResultSet,
    column: i32,
) -> i32 {
    if rs.is_null() {
        return -1;
    }
    match (*rs).is_column_null(column) {
        Ok(true) => 1,
        Ok(false) => 0,
        Err(e) => e as i32,
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareResultSetClose(rs: *mut OpaqueResultSet) -> i32 {
    if rs.is_null() {
        return -1;
    }
    match (*rs).close() {
        Ok(_) => 0,
        Err(e) => e as i32,
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareResultSetGetRowCount(
    rs: *mut OpaqueResultSet,
    out_count: *mut i32,
) -> i32 {
    if rs.is_null() || out_count.is_null() {
        return -1;
    }
    match (*rs).get_row_count() {
        Ok(count) => {
            *out_count = count;
            0
        }
        Err(e) => e as i32,
    }
}

#[no_mangle]
pub unsafe extern "C" fn DataShareResultSetGetColumnCount(
    rs: *mut OpaqueResultSet,
    out_count: *mut i32,
) -> i32 {
    if rs.is_null() || out_count.is_null() {
        return -1;
    }
    match (*rs).get_column_count() {
        Ok(count) => {
            *out_count = count;
            0
        }
        Err(e) => e as i32,
    }
}
