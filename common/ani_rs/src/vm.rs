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

use std::{ptr::null_mut, sync::OnceLock};

use ani_sys::{ani_env, ani_vm, ANI_VERSION_1};

use crate::{error::AniError, AniEnv};

#[repr(transparent)]
pub struct AniVm {
    inner: *mut ani_vm,
}

unsafe impl Send for AniVm {}
unsafe impl Sync for AniVm {}

static VM: OnceLock<AniVm> = OnceLock::new();

impl AniVm {
    pub fn init(vm: AniVm) {
        let _ = VM.set(vm);
    }

    pub fn get_instance() -> &'static AniVm {
        VM.get().unwrap()
    }

    pub fn get_env<'local>(&self) -> Result<AniEnv<'local>, AniError> {
        unsafe {
            let env = null_mut() as *mut ani_env;
            let res = (**self.inner).GetEnv.unwrap()(
                self.inner,
                ANI_VERSION_1,
                &env as *const *mut ani_env as *mut *mut ani_env,
            );
            if res != 0 {
                Err(AniError::from_code(
                    "Failed to get environment".to_string(),
                    res,
                ))
            } else {
                Ok(AniEnv::from_raw(env))
            }
        }
    }

    pub fn attach_current_thread<'local>(&self) -> Result<AniEnv<'local>, AniError> {
        unsafe {
            let env = null_mut() as *mut ani_env;
            let res = (**self.inner).AttachCurrentThread.unwrap()(
                self.inner,
                null_mut(),
                ANI_VERSION_1,
                &env as *const *mut ani_env as *mut *mut ani_env,
            );
            if res != 0 {
                Err(AniError::from_code(
                    "Failed to attach current thread".to_string(),
                    res,
                ))
            } else {
                Ok(AniEnv::from_raw(env))
            }
        }
    }

    pub fn detach_current_thread(&self) -> Result<(), AniError> {
        unsafe {
            let res = (**self.inner).DetachCurrentThread.unwrap()(self.inner);
            if res != 0 {
                Err(AniError::from_code(
                    "Failed to detach current thread".to_string(),
                    res,
                ))
            } else {
                Ok(())
            }
        }
    }
}
