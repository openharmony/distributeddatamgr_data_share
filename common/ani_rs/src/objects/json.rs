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

use super::{AniRef, AniString};
use crate::{error::AniError, global::GlobalRef, AniEnv};
use ani_sys::ani_ref;
use serde::{Deserialize, Serialize};
use std::ffi::CStr;

#[repr(transparent)]
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct JsonValue<'local>(AniRef<'local>);

impl<'local> From<AniRef<'local>> for JsonValue<'local> {
    fn from(value: AniRef<'local>) -> Self {
        Self(value)
    }
}

impl<'local> From<JsonValue<'local>> for AniRef<'local> {
    fn from(value: JsonValue<'local>) -> Self {
        value.0
    }
}

impl JsonValue<'_> {
    const TOOL_CLASS_NAME: &'static CStr = unsafe {
        CStr::from_bytes_with_nul_unchecked(b"@ohos.app.ability.Want.RecordSerializeTool\0")
    };

    pub fn from_raw(ptr: ani_ref) -> Self {
        Self(AniRef::from_raw(ptr))
    }

    pub fn as_raw(&self) -> ani_ref {
        self.0.as_raw()
    }

    pub fn into_raw(self) -> ani_ref {
        self.0.into_raw()
    }

    pub fn stringify(&self, env: &AniEnv) -> Result<String, AniError> {
        const JSON_TOOL_CLASS_NAME: &'static CStr = unsafe {
            CStr::from_bytes_with_nul_unchecked(b"std.core.JSON\0")
        };
        let cls = env.find_class(JSON_TOOL_CLASS_NAME)?;
        let stringify_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"stringify\0") };
        let stringify_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"C{std.core.Object}:C{std.core.String}\0") };
        let method = env.find_static_method_with_signature(&cls, stringify_name, stringify_signature)?;

        let param = self.0.as_raw();
        let res = env.call_static_method_ref(&cls, &method, (param,))?;

        let ani_string = AniString::from(res);
        env.convert_ani_string(&ani_string)
    }

    pub fn parse<'local>(
        env: &AniEnv<'local>,
        param_string: &str,
    ) -> Result<JsonValue<'local>, AniError> {
        let cls = env.find_class(Self::TOOL_CLASS_NAME)?;
        let parse_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"parseNoThrow\0") };
        let method = env.find_static_method(&cls, parse_name)?;

        let ani_string = env.convert_std_string(param_string)?;
        let param = ani_string.as_raw();
        let res = env.call_static_method_ref(&cls, &method, (param,))?;
        Ok(JsonValue(res))
    }

    pub fn into_global(self, env: &AniEnv) -> Result<GlobalRef<JsonValue<'static>>, AniError> {
        let ani_ref = env.create_global_ref(self.into())?;
        let ani_json = JsonValue::from(ani_ref);
        Ok(GlobalRef(ani_json))
    }
}

impl<'de> Deserialize<'de> for JsonValue<'_> {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: serde::Deserializer<'de>,
    {
        let ani_ref = AniRef::deserialize(deserializer)?;
        Ok(Self::from(ani_ref))
    }
}

impl Serialize for JsonValue<'_> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        let ani_ref = AniRef::from(self.clone());
        ani_ref.serialize(serializer)
    }
}
