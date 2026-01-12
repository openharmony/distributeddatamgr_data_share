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

const fn cstr(s: &[u8]) -> &CStr {
    unsafe { CStr::from_bytes_with_nul_unchecked(s) }
}

pub const BOOLEAN: &CStr = cstr(b"std.core.Boolean\0");
pub const BYTE: &CStr = cstr(b"std.core.Byte\0");
pub const SHORT: &CStr = cstr(b"std.core.Short\0");
pub const INT: &CStr = cstr(b"std.core.Int\0");
pub const LONG: &CStr = cstr(b"std.core.Long\0");
pub const FLOAT: &CStr = cstr(b"std.core.Float\0");
pub const DOUBLE: &CStr = cstr(b"std.core.Double\0");
pub const STRING: &CStr = cstr(b"std.core.String\0");
pub const CHAR: &CStr = cstr(b"std.core.Char\0");

pub const MAP: &CStr = cstr(b"std.core.ReadonlyMap\0");
pub const ARRAY: &CStr = cstr(b"std.core.Array\0");
pub const ARRAY_BUFFER: &CStr = cstr(b"std.core.ArrayBuffer\0");
pub const RECORD: &CStr = cstr(b"std.core.Record\0");
pub const ITERATOR: &CStr = cstr(b"std.core.Iterator\0");

pub const INT8_ARRAY: &CStr = cstr(b"escompat.Int8Array\0");
pub const INT16_ARRAY: &CStr = cstr(b"escompat.Int16Array\0");
pub const INT32_ARRAY: &CStr = cstr(b"escompat.Int32Array\0");

pub const UINT8_ARRAY: &CStr = cstr(b"escompat.Uint8Array\0");
pub const UINT16_ARRAY: &CStr = cstr(b"escompat.Uint16Array\0");
pub const UINT32_ARRAY: &CStr = cstr(b"escompat.Uint32Array\0");

pub const SET: &CStr = cstr(b"$_set\0");
pub const GET: &CStr = cstr(b"$_get\0");
pub const CTOR: &CStr = cstr(b"<ctor>\0");

pub const ENTRIES: &CStr = cstr(b"entries\0");
pub const NEXT: &CStr = cstr(b"next\0");
pub const VALUE: &CStr = cstr(b"value\0");
pub const ANI_UNIONT: &CStr = cstr(b"ani_union\0");
pub const TOBOOLEAN: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"toBoolean\0") };
pub const TOBYTE: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"toByte\0") };
pub const TOCHAR: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"toChar\0") };
pub const TOSHORT: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"toShort\0") };
pub const TOINT: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"toInt\0") };
pub const TOLONG: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"toLong\0") };
pub const TOFLOAT: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"toFloat\0") };
pub const TODOUBLE: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"toDouble\0") };


pub const TYPED_ARRAY_CTOR: &CStr =
    cstr(b"C{std.core.ArrayBuffer}d:\0");
pub const BYTE_LENGTH: &CStr = cstr(b"byteLength\0");
pub const BYTE_OFFSET: &CStr = cstr(b"byteOffset\0");
pub const BUFFER: &CStr = cstr(b"buffer\0");
