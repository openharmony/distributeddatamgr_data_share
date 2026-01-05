// Copyright (C) 2025 Huawei Device Co., Ltd.
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

use std::ffi::CStr;

use crate::error::AniError;

pub const I128_UNSUPPORTED: AniError = AniError::literal("i128 unsupported in ani");
pub const U8_UNSUPPORTED: AniError = AniError::literal("u8 unsupported in ani");
pub const U16_UNSUPPORTED: AniError = AniError::literal("u16 unsupported in ani");
pub const U32_UNSUPPORTED: AniError = AniError::literal("u32 unsupported in ani");
pub const U64_UNSUPPORTED: AniError = AniError::literal("u64 unsupported in ani");
pub const U128_UNSUPPORTED: AniError = AniError::literal("u128 unsupported in ani");

pub const ARRAY_WITHOUT_LENGTH_UNSUPPORTED: AniError =
    AniError::literal("Array without length unsupported in ani");
