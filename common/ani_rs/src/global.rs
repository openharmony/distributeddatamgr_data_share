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
use serde::Serialize;
use crate::{ani_rs_error, objects::AniRef, AniVm};

impl<T: Into<AniRef<'static>> + Clone> Drop for GlobalRef<T> {
    fn drop(&mut self) {
        if let Ok(env) = AniVm::get_instance().get_env() {
            let _ = env.delete_global_ref(self.0.clone().into());
        } else {
            if let Ok(env) = AniVm::get_instance().attach_current_thread() {
                let _ = env.delete_global_ref(self.0.clone().into());
                let _ = AniVm::get_instance().detach_current_thread();
            } else {
                ani_rs_error!("Failed to attach_current_thread in drop");
            }
        }
    }
}

#[repr(transparent)]
pub struct GlobalRef<T: Into<AniRef<'static>> + Clone>(pub T);

impl<T: PartialEq + Into<AniRef<'static>> + Clone> PartialEq for GlobalRef<T> {
    fn eq(&self, other: &Self) -> bool {
        self.0 == other.0
    }
}

impl<T: Into<AniRef<'static>> + Clone> Deref for GlobalRef<T> {
    type Target = T;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl<T: Into<AniRef<'static>> + Clone> Serialize for GlobalRef<T> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        let ani_ref: AniRef = self.0.clone().into();
        ani_ref.serialize(serializer)
    }
}