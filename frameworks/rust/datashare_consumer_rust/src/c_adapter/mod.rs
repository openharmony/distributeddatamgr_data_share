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

//! C FFI adapter layer for DataShare consumer
//! This module provides C-compatible interfaces for Rust implementations
//!
//! Design: Opaque handle pattern — C++ holds a `*mut DataShareHelperImpl`
//! and calls `extern "C"` functions to operate on it.

pub mod batch_ffi;
pub mod connection_ffi;
pub mod controller_ext_special_ffi;
pub mod controller_persistent_ffi;
pub mod controller_provider_ffi;
pub mod controller_published_ffi;
pub mod controller_service_ffi;
pub mod crud_ffi;
pub mod dataproxy_handle_ffi;
pub mod ext_ops_ffi;
pub mod file_ops_ffi;
pub mod helper_ffi;
pub mod manager_impl_ffi;
pub mod observer_ffi;
pub mod observer_stub_ffi;
pub mod parcel_ffi;
pub mod publish_ffi;
pub mod sa_connection_ffi;
pub mod service_proxy_ffi;
pub mod template_ffi;
pub mod types_ffi;
