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

#![allow(missing_docs, clippy::not_unsafe_ptr_arg_deref, unused_imports)]

pub mod block_writer;
pub mod c_adapter;
#[cfg(feature = "ipc")]
pub mod ipc;
#[cfg(feature = "ipc")]
pub mod kvstore_proxy;
pub mod result_set;
pub mod result_set_bridge;
pub mod shared_block;

pub use block_writer::BlockWriter;
pub use result_set::{DataShareResultSet, DataType, ResultSet};
pub use result_set_bridge::{ResultSetBridge, Writer};
pub use shared_block::SharedBlock;
