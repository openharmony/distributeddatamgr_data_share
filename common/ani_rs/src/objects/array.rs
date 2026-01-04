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

use std::ops::Deref;

use ani_sys::{ani_array, ani_object};

use super::{AniObject, AniRef};

#[repr(transparent)]
#[derive(Debug)]
pub struct AniArray<'local>(AniObject<'local>);

impl<'local> AsRef<AniArray<'local>> for AniArray<'local> {
    fn as_ref(&self) -> &AniArray<'local> {
        &self
    }
}

impl<'local> AsMut<AniArray<'local>> for AniArray<'local> {
    fn as_mut(&mut self) -> &mut AniArray<'local> {
        self
    }
}

impl<'local> Deref for AniArray<'local> {
    type Target = AniObject<'local>;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl<'local> From<AniArray<'local>> for AniObject<'local> {
    fn from(value: AniArray<'local>) -> Self {
        value.0
    }
}

impl<'local> From<AniArray<'local>> for AniRef<'local> {
    fn from(value: AniArray<'local>) -> Self {
        value.0.into()
    }
}

impl<'local> From<AniRef<'local>> for AniArray<'local> {
    fn from(value: AniRef<'local>) -> Self {
        let object = AniObject::from(value);
        Self::from(object)
    }
}

impl<'local> From<AniObject<'local>> for AniArray<'local> {
    fn from(value: AniObject<'local>) -> Self {
        Self::from_raw(value.into_raw())
    }
}

impl<'local> AniArray<'local> {
    pub fn from_raw(ptr: ani_array) -> Self {
        Self(AniObject::from_raw(ptr as ani_object))
    }

    pub fn as_raw(&self) -> ani_array {
        self.0.as_raw() as _
    }

    pub fn into_raw(self) -> ani_array {
        self.0.into_raw() as _
    }
}
