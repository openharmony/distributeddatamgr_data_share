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

//! FFI bindings for OpenHarmony HiCollie (XCollie watchdog timer).
//!
//! Provides Rust-safe wrappers for:
//! - XCollie SetTimer/CancelTimer for watchdog timers
//! - RAII XCollieGuard that auto-cancels timer on drop

mod wrapper;

pub use wrapper::ffi;
pub use wrapper::XCollieGuard;
pub use wrapper::XCOLLIE_FLAG_DEFAULT;
pub use wrapper::XCOLLIE_FLAG_LOG;
pub use wrapper::XCOLLIE_FLAG_NOOP;
pub use wrapper::XCOLLIE_FLAG_RECOVERY;
