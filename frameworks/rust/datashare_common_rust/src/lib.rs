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

#![feature(let_chains)]
#![allow(
    missing_docs,
    clippy::not_unsafe_ptr_arg_deref,
    clippy::module_inception,
    unused_imports
)]

pub mod c_adapter;
pub mod call_reporter;
pub mod error;
pub mod ipc;
pub mod log;
pub mod observer;
pub mod operation;
pub mod predicates;
pub mod template;
pub mod types;
pub mod utils;
pub mod value_object;
pub mod values_bucket;

pub use error::DataShareError;
pub use observer::{ChangeInfo, ChangeType};
pub use operation::{Operation, OperationStatement};
pub use template::{CreateOptions, PredicateTemplateNode, Template, TemplateId};
pub use value_object::{DataShareValue, DataShareValueType};
pub use values_bucket::DataShareValuesBucket;

// Re-export commonly used predicates types
pub use predicates::{DataSharePredicates, OperationType};
