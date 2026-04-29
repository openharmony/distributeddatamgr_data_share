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

//! FFI bindings for GRD (Gauss Relational Database) document store.
//!
//! Provides Rust-safe wrappers for the GRD document API used by
//! data_share's kv_delegate:
//! - GrdDb: database open/close, collection management, document CRUD
//! - GrdResultSet: cursor-based query result iteration

mod wrapper;

pub use wrapper::ffi;
pub use wrapper::GrdDb;
pub use wrapper::GrdResultSet;
pub use wrapper::GRD_DB_CLOSE;
pub use wrapper::GRD_DB_FLUSH_ASYNC;
pub use wrapper::GRD_DB_FLUSH_SYNC;
pub use wrapper::GRD_DB_OPEN_CHECK;
pub use wrapper::GRD_DB_OPEN_CHECK_FOR_ABNORMAL;
pub use wrapper::GRD_DB_OPEN_CREATE;
pub use wrapper::GRD_DB_OPEN_ONLY;
pub use wrapper::GRD_DOC_ID_DISPLAY;
pub use wrapper::GRD_DOC_APPEND;
pub use wrapper::GRD_DOC_REPLACE;
pub use wrapper::GRD_OK;
