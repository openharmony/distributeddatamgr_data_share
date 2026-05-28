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

//! C FFI adapter layer for DataShare
//! This module provides C-compatible interfaces for Rust implementations

pub mod call_reporter_ffi;
pub mod error_ffi;
pub mod itypes_utils_ffi;
pub mod predicates_ffi;
pub mod predicates_verify_ffi;
pub mod serialization_ffi;
pub mod string_utils_ffi;
pub mod template_ffi;
pub mod uri_utils_ffi;
pub mod value_object_ffi;
pub mod values_bucket_ffi;
