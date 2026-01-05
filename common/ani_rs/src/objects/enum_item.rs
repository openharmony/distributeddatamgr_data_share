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

use ani_sys::{ani_enum_item, ani_object};

use super::{AniObject, AniRef};

#[repr(transparent)]
#[derive(Debug)]
pub struct AniEnumItem<'local>(AniObject<'local>);

impl<'local> AsRef<AniEnumItem<'local>> for AniEnumItem<'local> {
    fn as_ref(&self) -> &AniEnumItem<'local> {
        &self
    }
}

impl<'local> AsMut<AniEnumItem<'local>> for AniEnumItem<'local> {
    fn as_mut(&mut self) -> &mut AniEnumItem<'local> {
        self
    }
}

impl<'local> Deref for AniEnumItem<'local> {
    type Target = AniObject<'local>;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl<'local> From<AniEnumItem<'local>> for AniObject<'local> {
    fn from(value: AniEnumItem<'local>) -> Self {
        value.0
    }
}

impl<'local> From<AniEnumItem<'local>> for AniRef<'local> {
    fn from(value: AniEnumItem<'local>) -> Self {
        value.0.into()
    }
}

impl<'local> From<AniObject<'local>> for AniEnumItem<'local> {
    fn from(value: AniObject<'local>) -> Self {
        Self::from_raw(value.into_raw())
    }
}

impl<'local> AniEnumItem<'local> {
    pub fn from_raw(ptr: ani_enum_item) -> Self {
        Self(AniObject::from_raw(ptr as ani_object))
    }

    pub fn as_raw(&self) -> ani_enum_item {
        self.0.as_raw() as _
    }

    pub fn into_raw(self) -> ani_enum_item {
        self.0.into_raw() as _
    }
}
