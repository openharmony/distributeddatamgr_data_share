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

//! FFI bindings for `MessageParcel::ReadRawData` and `WriteRawData`.
//!
//! The IPC Rust crate does not expose these two methods, but they are
//! needed by DataShare (and may be needed by future consumers) to marshal
//! binary blobs in the same wire format that C++ clients use via
//! `ITypesUtil::Marshal*Vec` + `WriteRawData`.
//!
//! Wire format (matches `MessageParcel::WriteRawData` in
//! `foundation/communication/ipc/ipc/native/src/core/framework/source/message_parcel.cpp`):
//! a 4-byte int32 length prefix followed by raw bytes inline when the
//! payload is ≤ 32 KiB (`MIN_RAWDATA_SIZE`), or an ashmem file descriptor
//! when the payload exceeds that threshold. This is strictly different
//! from the `ReadUInt8Vector`/`WriteUInt8Vector` wire format used by
//! `MsgParcel::read::<Vec<u8>>()`.

#![warn(missing_docs)]

mod wrapper;

pub use wrapper::{read_raw_data, write_raw_data};
