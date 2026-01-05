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

use ani_sys::{ani_enum, ani_type};

use super::{AniClass, AniType};

#[repr(transparent)]
#[derive(Debug)]
pub struct AniEnum<'local>(AniType<'local>);

impl<'local> AsRef<AniEnum<'local>> for AniEnum<'local> {
    fn as_ref(&self) -> &AniEnum<'local> {
        &self
    }
}

impl<'local> AsMut<AniEnum<'local>> for AniEnum<'local> {
    fn as_mut(&mut self) -> &mut AniEnum<'local> {
        self
    }
}

impl<'local> Deref for AniEnum<'local> {
    type Target = AniType<'local>;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl<'local> From<AniEnum<'local>> for AniType<'local> {
    fn from(value: AniEnum<'local>) -> Self {
        value.0
    }
}

impl<'local> From<AniEnum<'local>> for AniClass<'local> {
    fn from(value: AniEnum<'local>) -> Self {
        value.0.into()
    }
}

impl<'local> From<AniType<'local>> for AniEnum<'local> {
    fn from(value: AniType<'local>) -> Self {
        Self::from_raw(value.into_raw())
    }
}

impl<'local> AniEnum<'local> {
    pub fn from_raw(ptr: ani_enum) -> Self {
        Self(AniType::from_raw(ptr as ani_type))
    }

    pub fn as_raw(&self) -> ani_enum {
        self.0.as_raw() as _
    }

    pub fn into_raw(self) -> ani_enum {
        self.0.into_raw() as _
    }
}
