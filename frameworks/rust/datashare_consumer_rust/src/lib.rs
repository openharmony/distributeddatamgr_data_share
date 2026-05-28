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

#![allow(
    missing_docs,
    dead_code,
    unused_mut,
    unused_imports,
    clippy::not_unsafe_ptr_arg_deref,
    clippy::module_inception,
    clippy::missing_safety_doc,
    clippy::manual_strip,
    clippy::comparison_chain
)]

pub mod c_adapter;
pub mod connection;
pub mod controller;
pub mod ffi;
pub mod helper;
pub mod ipc;
pub mod proxy;
pub mod subscriber;
