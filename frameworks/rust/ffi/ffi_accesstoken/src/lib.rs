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

//! FFI bindings for OpenHarmony AccessToken (Permission Token Management).
//!
//! Provides Rust-safe wrappers for AccessTokenKit APIs:
//! - Get calling token information (token ID, process name, etc.)
//! - Verify permissions for the calling process
//! - Token ID to process/bundle name resolution

mod wrapper;

pub use wrapper::ffi;
pub use wrapper::AccessTokenKit;
pub use wrapper::ffi::CallingInfo;
pub use wrapper::ffi::FfiHapTokenInfo;
pub use wrapper::ffi::FfiNativeTokenInfo;
