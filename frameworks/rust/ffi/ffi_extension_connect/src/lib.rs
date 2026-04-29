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

//! FFI bindings for DataShare extension connection management.
//!
//! Provides low-level Rust-safe wrappers for the C++ extension connection
//! primitives: SAMGR proxy management, `AbilityConnectionStub` callbacks,
//! and `ExecutorPool`-based delayed disconnection.
//!
//! ## C++ FFI functions
//!
//! | Rust wrapper              | C++ function              | Purpose                          |
//! |---------------------------|---------------------------|----------------------------------|
//! | `connect_extension`       | `connect_extension`       | Create callback + SAMGR connect  |
//! | `disconnect_extension`    | `disconnect_extension`    | Remove callback + SAMGR disconnect |
//! | `has_active_connection`   | `has_active_connection`   | Check callback map               |
//! | `schedule_disconnect`     | `schedule_disconnect`     | Executor-scheduled disconnect    |
//! | `init_executor`           | `init_executor`           | Set ExecutorPool from OnBind     |
//! | `build_corruption_want_params` | `build_corruption_want_params` | Allocate WantParams for corruption notify |
//! | `destroy_want_params`     | `destroy_want_params`     | Free WantParams allocated above  |

mod wrapper;

pub use wrapper::{
    build_corruption_want_params, connect_extension, destroy_want_params, disconnect_extension,
    has_active_connection, init_executor, schedule_disconnect,
};
