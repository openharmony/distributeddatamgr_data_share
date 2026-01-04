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

use std::{ffi::CStr, marker::PhantomData, os::raw::c_void};

use ani_sys::ani_native_function;

#[repr(transparent)]
pub struct AniNativeFunction<'local> {
    pub inner: ani_native_function,
    lifetime: PhantomData<&'local ()>,
}

impl<'local> AniNativeFunction<'local> {
    pub fn from_raw(ptr: ani_native_function) -> Self {
        Self {
            inner: ptr,
            lifetime: PhantomData,
        }
    }

    pub fn new(name: &'local CStr, f: *const c_void) -> Self {
        Self {
            inner: ani_native_function {
                name: name.as_ptr() as _,
                signature: std::ptr::null_mut(),
                pointer: f,
            },
            lifetime: PhantomData,
        }
    }

    pub fn new_with_signature(
        name: &'local CStr,
        signature: &'local CStr,
        f: *const c_void,
    ) -> Self {
        Self {
            inner: ani_native_function {
                name: name.as_ptr() as _,
                signature: signature.as_ptr() as _,
                pointer: f,
            },
            lifetime: PhantomData,
        }
    }
}
