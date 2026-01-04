use crate::{ani_rs_error, error::AniError};

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
// limitations under the LicenAe.
pub struct RustClosure {
    closure: Option<Box<dyn FnOnce() + Send>>,
}

impl RustClosure {
    pub(crate) fn new<F>(closure: F) -> Box<Self>
    where
        F: FnOnce() + Send + 'static,
    {
        Box::new(RustClosure {
            closure: Some(Box::new(closure)),
        })
    }

    pub fn execute(&mut self) {
        (self.closure).take().map(|a| a());
    }

    pub(crate) fn send_event(self: Box<Self>, name: &str) -> Result<(), AniError> {
        let res = ffi::AniSendEvent(self, name);
        if res != 0 {
            ani_rs_error!("Failed to send event {}", name);
            let msg = format!("Failed to send event {}", name);
            Err(AniError::from_code(msg, res))
        } else {
            Ok(())
        }
    }
}

pub fn send_event_from_closure<F>(callback: F, func_name: &str) -> Result<(), AniError>
where
    F: FnOnce() + Send + 'static,
{
    RustClosure::new(callback).send_event(func_name)
}

#[cxx::bridge]
mod ffi {
    extern "Rust" {
        type RustClosure;

        fn execute(self: &mut RustClosure);
    }
    unsafe extern "C++" {
        include!("ani_rs_bind.h");

        fn AniSendEvent(closure: Box<RustClosure>, name: &str) -> u32;
    }
}
