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

//! Unified FFI crate — aggregates all datashare C FFI symbols into one cdylib (.so).
//!
//! Each sub-crate defines its own `c_adapter` module with `#[no_mangle] extern "C"` functions.
//! Re-exporting them here ensures the linker includes all FFI symbols in the final `.so`.
//!
//! Built as cdylib so the Rust stdlib and Rust dylib deps (ipc_rust, samgr_rust)
//! are resolved automatically at link time.

#![allow(unused_imports)]

pub use datashare_common::c_adapter as common_ffi;
pub use datashare_consumer::c_adapter as consumer_ffi;
pub use datashare_permission::c_adapter as permission_ffi;
pub use datashare_provider::module_loader as provider_loader;
pub use datashare_resultset::c_adapter as resultset_ffi;
