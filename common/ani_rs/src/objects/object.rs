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

use ani_sys::{ani_object, ani_ref};
use serde::{Deserialize, Serialize};

use crate::{AniEnv, global::GlobalRef, error::AniError};

use super::AniRef;

#[repr(transparent)]
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct AniObject<'local>(AniRef<'local>);

impl<'local> AsRef<AniObject<'local>> for AniObject<'local> {
    fn as_ref(&self) -> &AniObject<'local> {
        &self
    }
}

impl<'local> AsMut<AniObject<'local>> for AniObject<'local> {
    fn as_mut(&mut self) -> &mut AniObject<'local> {
        self
    }
}

impl<'local> Deref for AniObject<'local> {
    type Target = AniRef<'local>;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl<'local> From<AniObject<'local>> for AniRef<'local> {
    fn from(value: AniObject<'local>) -> Self {
        value.0
    }
}

impl<'local> From<AniRef<'local>> for AniObject<'local> {
    fn from(value: AniRef<'local>) -> Self {
        Self::from_raw(value.into_raw())
    }
}

impl<'local> AniObject<'local> {
    pub fn from_raw(ptr: ani_object) -> Self {
        Self(AniRef::from_raw(ptr as ani_ref))
    }

    pub fn as_raw(&self) -> ani_object {
        self.0.as_raw() as _
    }

    pub fn into_raw(self) -> ani_object {
        self.0.into_raw() as _
    }

    pub fn into_global(self, env: &AniEnv) -> Result<GlobalRef<AniObject<'static>>, AniError> {
        let ani_ref = env.create_global_ref(self.into())?;
        let ani_object = AniObject::from(ani_ref);
        Ok(GlobalRef(ani_object))
    }
}

impl<'de> Deserialize<'de> for AniObject<'_> {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: serde::Deserializer<'de> {
        let ani_ref = AniRef::deserialize(deserializer)?;
        Ok(Self::from(ani_ref))
    }
}

impl Serialize for AniObject<'_> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer {
        let ani_ref = AniRef::from(self.clone());
        ani_ref.serialize(serializer)
    }
}