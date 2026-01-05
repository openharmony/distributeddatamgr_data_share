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

use ani_sys::{ani_object, ani_string};

use super::{AniObject, AniRef};

#[repr(transparent)]
#[derive(Debug)]
pub struct AniString<'local>(AniObject<'local>);

impl<'local> AsRef<AniString<'local>> for AniString<'local> {
    fn as_ref(&self) -> &AniString<'local> {
        &self
    }
}

impl<'local> AsMut<AniString<'local>> for AniString<'local> {
    fn as_mut(&mut self) -> &mut AniString<'local> {
        self
    }
}

impl<'local> Deref for AniString<'local> {
    type Target = AniObject<'local>;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl<'local> From<AniString<'local>> for AniObject<'local> {
    fn from(value: AniString<'local>) -> Self {
        value.0
    }
}

impl<'local> From<AniObject<'local>> for AniString<'local> {
    fn from(value: AniObject<'local>) -> Self {
        Self::from_raw(value.into_raw())
    }
}

impl<'local> From<AniString<'local>> for AniRef<'local> {
    fn from(value: AniString<'local>) -> Self {
        value.0.into()
    }
}

impl<'local> From<AniRef<'local>> for AniString<'local> {
    fn from(value: AniRef<'local>) -> Self {
        Self::from_raw(value.into_raw())
    }
}

impl<'local> AniString<'local> {
    pub fn from_raw(ptr: ani_string) -> Self {
        Self(AniObject::from_raw(ptr as ani_object))
    }

    pub fn as_raw(&self) -> ani_string {
        self.0.as_raw() as _
    }

    pub fn into_raw(self) -> ani_string {
        self.0.into_raw() as _
    }
}
