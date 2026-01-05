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

#![feature(vec_into_raw_parts)]

pub use ani_rs_macros::ani;
pub use ani_rs_macros::native;

pub mod box_type;

mod env;
pub use env::AniEnv;

mod vm;
pub use vm::AniVm;

mod ani;
pub use ani::AniDe;
pub use ani::AniSer;

pub mod business_error;
pub mod error;
pub mod objects;
pub mod signature;
pub mod typed_array;
pub mod global;

mod iterator;
mod macros;
mod primitive;
mod wrapper;
pub use wrapper::send_event_from_closure;
mod log;

const LOG_LABEL: hilog_rust::HiLogLabel = hilog_rust::HiLogLabel {
    log_type: hilog_rust::LogType::LogCore,
    domain: 0xD0015B0,
    tag: "AniRs",
};
