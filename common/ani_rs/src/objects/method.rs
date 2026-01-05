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

use std::marker::PhantomData;

use ani_sys::{ani_method, ani_static_method};

#[repr(transparent)]
pub struct AniMethod<'local> {
    pub inner: ani_method,
    lifetime: PhantomData<&'local ()>,
}

impl AniMethod<'_> {
    pub fn from_raw(ptr: ani_method) -> Self {
        Self {
            inner: ptr,
            lifetime: PhantomData,
        }
    }

    pub fn as_raw(&self) -> ani_method {
        self.inner
    }

    pub fn into_raw(self) -> ani_method {
        self.inner
    }
}

#[repr(transparent)]
pub struct AniStaticMethod<'local> {
    pub inner: ani_static_method,
    lifetime: PhantomData<&'local ()>,
}

impl AniStaticMethod<'_> {
    pub fn from_raw(ptr: ani_static_method) -> Self {
        Self {
            inner: ptr,
            lifetime: PhantomData,
        }
    }

    pub fn as_raw(&self) -> ani_static_method {
        self.inner
    }

    pub fn into_raw(self) -> ani_static_method {
        self.inner
    }
}