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

use ani_sys::{ani_namespace, ani_ref};

use super::AniRef;

#[repr(transparent)]
#[derive(Debug)]
pub struct AniNamespace<'local>(AniRef<'local>);

impl<'local> AsRef<AniNamespace<'local>> for AniNamespace<'local> {
    fn as_ref(&self) -> &AniNamespace<'local> {
        &self
    }
}

impl<'local> AsMut<AniNamespace<'local>> for AniNamespace<'local> {
    fn as_mut(&mut self) -> &mut AniNamespace<'local> {
        self
    }
}

impl<'local> Deref for AniNamespace<'local> {
    type Target = AniRef<'local>;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl<'local> From<AniNamespace<'local>> for AniRef<'local> {
    fn from(value: AniNamespace<'local>) -> Self {
        value.0
    }
}

impl<'local> From<AniRef<'local>> for AniNamespace<'local> {
    fn from(value: AniRef<'local>) -> Self {
        Self::from_raw(value.into_raw())
    }
}

impl<'local> AniNamespace<'local> {
    pub fn from_raw(ptr: ani_namespace) -> Self {
        Self(AniRef::from_raw(ptr as ani_ref))
    }

    pub fn as_raw(&self) -> ani_namespace {
        self.0.as_raw() as _
    }

    pub fn into_raw(self) -> ani_namespace {
        self.0.into_raw() as _
    }
}
