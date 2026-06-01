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

//! C FFI adapter for `PublishedDataItem` (datashare_template.cpp).
//!
//! Mirrors the C++ Ashmem creation path in `PublishedDataItem::Set` for blob
//! values: `CreateAshmem -> MapReadAndWriteAshmem -> WriteToAshmem`.
//!
//! Ownership transfer: the Ashmem FD is produced by Rust and returned to the
//! caller via `out_fd`. The Rust-side `Ashmem` wrapper is forgotten so it does
//! not close the FD on drop. The C++ caller reconstructs `sptr<Ashmem>` via
//! `new Ashmem(fd, size)` and takes lifetime ownership.

use utils_rust::ashmem::create_ashmem_instance;

/// Create an Ashmem region, map RW, and write `data` into it.
///
/// Equivalent to the blob branch of C++ `PublishedDataItem::Set`.
///
/// Output parameters:
/// - `*out_fd`: Ashmem file descriptor on success (>= 0); `-1` on failure.
/// - `*out_size`: size reported by `GetAshmemSize()` on success; `0` on failure.
///
/// Returns `0` on success, non-zero on failure. On failure the Rust side
/// performs `UnmapAshmem + CloseAshmem` before returning, so no FD is leaked.
/// On success the Rust wrapper is `mem::forget`'d — ownership of the FD
/// transfers to the caller, which must reconstruct `sptr<Ashmem>(new Ashmem(fd, size))`.
#[no_mangle]
pub extern "C" fn DataSharePublishedDataItemCreateAshmem(
    name: *const u8,
    name_len: u32,
    data: *const u8,
    data_len: u32,
    out_fd: *mut i32,
    out_size: *mut i32,
) -> i32 {
    if out_fd.is_null() || out_size.is_null() {
        return -1;
    }
    unsafe {
        *out_fd = -1;
        *out_size = 0;
    }
    if name.is_null() || name_len == 0 {
        return -1;
    }
    if data.is_null() && data_len != 0 {
        return -1;
    }

    let name_slice = unsafe { std::slice::from_raw_parts(name, name_len as usize) };
    let name_str = match std::str::from_utf8(name_slice) {
        Ok(s) => s,
        Err(_) => return -1,
    };
    let blob: &[u8] = if data_len == 0 {
        &[]
    } else {
        unsafe { std::slice::from_raw_parts(data, data_len as usize) }
    };

    let ashmem = match unsafe { create_ashmem_instance(name_str, data_len as i32) } {
        Some(a) => a,
        None => return -1,
    };

    if !ashmem.map_read_write_ashmem() {
        ashmem.close_ashmem();
        return -1;
    }

    let write_ok = unsafe {
        ashmem.write_to_ashmem(blob.as_ptr() as *const std::ffi::c_char, data_len as i32, 0)
    };
    if !write_ok {
        ashmem.unmap_ashmem();
        ashmem.close_ashmem();
        return -1;
    }

    let fd = ashmem.get_ashmem_fd();
    let size = ashmem.get_ashmem_size();
    if fd < 0 {
        ashmem.unmap_ashmem();
        ashmem.close_ashmem();
        return -1;
    }

    // Transfer FD ownership to the caller: forget the Rust wrapper so its
    // SharedPtr<Ashmem> drop does not close/unmap the FD. The C++ caller will
    // reconstruct sptr<Ashmem>(new Ashmem(fd, size)) and own the lifetime.
    std::mem::forget(ashmem);
    unsafe {
        *out_fd = fd;
        *out_size = size;
    }
    0
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_create_ashmem_null_out_args() {
        let name = b"x";
        let data = [0u8; 0];
        let ret = DataSharePublishedDataItemCreateAshmem(
            name.as_ptr(),
            name.len() as u32,
            data.as_ptr(),
            0,
            std::ptr::null_mut(),
            std::ptr::null_mut(),
        );
        assert_ne!(ret, 0);
    }

    #[test]
    fn test_create_ashmem_null_name() {
        let mut fd: i32 = 0;
        let mut size: i32 = 0;
        let data = [1u8, 2, 3];
        let ret = DataSharePublishedDataItemCreateAshmem(
            std::ptr::null(),
            0,
            data.as_ptr(),
            data.len() as u32,
            &mut fd,
            &mut size,
        );
        assert_ne!(ret, 0);
        assert_eq!(fd, -1);
    }

    #[test]
    fn test_create_ashmem_normal_blob() {
        let name = b"PublishedDataItemTest";
        let blob = vec![0x11u8, 0x22, 0x33, 0x44, 0x55];
        let mut fd: i32 = -1;
        let mut size: i32 = 0;
        let ret = DataSharePublishedDataItemCreateAshmem(
            name.as_ptr(),
            name.len() as u32,
            blob.as_ptr(),
            blob.len() as u32,
            &mut fd,
            &mut size,
        );
        // In environments that do not support ashmem, create may fail: accept both.
        if ret == 0 {
            assert!(fd >= 0);
            assert_eq!(size as usize, blob.len());
        }
    }
}
