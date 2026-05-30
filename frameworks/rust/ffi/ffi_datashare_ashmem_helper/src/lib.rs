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

//! Reverse FFI helper: construct a `utils_rust::ashmem::Ashmem` from a raw
//! fd + size. Used by the DataShare Rust migration to take over ownership
//! of an ashmem FD delivered across IPC or over the C FFI boundary.

mod wrapper;

use utils_rust::ashmem::Ashmem;

/// Build a Rust `Ashmem` that owns an existing ashmem fd of the given size.
///
/// Returns `None` when allocation on the C++ side fails.
///
/// # Safety
/// `fd` must be a valid ashmem file descriptor of at least `size` bytes.
/// Ownership of the fd transfers into the returned `Ashmem` (the underlying
/// `OHOS::Ashmem` destructor will close it when the shared_ptr drops).
pub fn ashmem_from_fd(fd: i32, size: i32) -> Option<Ashmem> {
    let shared = wrapper::ffi::AshmemFromFd(fd, size);
    if shared.is_null() {
        None
    } else {
        Some(Ashmem::new(shared))
    }
}
