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

//! CXX bridge: reconstruct an OHOS::Ashmem from a raw FD + size.
//!
//! `utils_rust::ashmem` only exposes `CreateAshmemStd(name, size)`, which
//! creates a brand-new anonymous region. We need the other C++ constructor
//! `Ashmem::Ashmem(int fd, int32_t size)` so the Rust side can wrap an
//! ashmem that was either created from an existing FD (e.g. passed over
//! IPC) or produced by a `sptr<Ashmem>` already owned by C++.

#[cxx::bridge(namespace = "OHOS::DataShare::FfiAshmem")]
pub mod ffi {
    unsafe extern "C++" {
        include!("ffi_ashmem_bridge.h");

        #[namespace = "OHOS"]
        type Ashmem = utils_rust::ashmem::ffi::Ashmem;

        /// Build a managed `shared_ptr<Ashmem>` from an existing fd / size.
        /// Callers must ensure `fd` is a valid ashmem fd. Returns a null
        /// SharedPtr if allocation fails.
        fn AshmemFromFd(fd: i32, size: i32) -> SharedPtr<Ashmem>;
    }
}
