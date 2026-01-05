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

use std::{marker::PhantomData, ops::Deref, ptr::null_mut};

use ani_sys::ani_ref;
use serde::{Deserialize, Serialize};
use crate::{AniVm, AniEnv, global::GlobalRef, error::AniError};

#[repr(transparent)]
#[derive(Debug, Clone)]
pub struct AniRef<'local> {
    pub inner: ani_ref,
    lifetime: PhantomData<&'local ()>,
}

unsafe impl Send for AniRef<'static> {}
unsafe impl Sync for AniRef<'static> {}

impl<'local> AsRef<AniRef<'local>> for AniRef<'local> {
    fn as_ref(&self) -> &AniRef<'local> {
        self
    }
}

impl<'local> AsMut<AniRef<'local>> for AniRef<'local> {
    fn as_mut(&mut self) -> &mut AniRef<'local> {
        self
    }
}

impl Deref for AniRef<'_> {
    type Target = ani_ref;

    fn deref(&self) -> &Self::Target {
        &self.inner
    }
}

impl AniRef<'_> {
    pub fn from_raw(ptr: ani_ref) -> Self {
        Self {
            inner: ptr,
            lifetime: PhantomData,
        }
    }

    pub fn as_raw(&self) -> ani_ref {
        self.inner
    }

    pub fn into_raw(self) -> ani_ref {
        self.inner
    }

    pub fn null() -> Self {
        Self::from_raw(null_mut() as _)
    }

    pub fn into_global(self, env: &AniEnv) -> Result<GlobalRef<AniRef<'static>>, AniError> {
        let ani_ref = env.create_global_ref(self)?;
        Ok(GlobalRef(ani_ref))
    }
}

impl PartialEq for AniRef<'_> {
    fn eq(&self, other: &Self) -> bool {
        let Ok(env) = AniVm::get_instance().get_env() else {
            return false;
        };
        env.reference_strict_equals(self, other).unwrap_or(false)
    }
}

impl Eq for AniRef<'_> {}

#[derive(Serialize, Deserialize)]
#[derive(Debug)]
#[serde(rename = "@AniRef")]
struct AniRefHelper(i64);

impl<'de> Deserialize<'de> for AniRef<'_> {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: serde::Deserializer<'de> {
        let ani_ref_raw = AniRefHelper::deserialize(deserializer)?;
        Ok(AniRef::from_raw(ani_ref_raw.0 as ani_ref))
    }
}

impl Serialize for AniRef<'_> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer {
        let ani_ref_raw = self.as_raw() as i64;
        let helper = AniRefHelper(ani_ref_raw);
        helper.serialize(serializer)
    }
}