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

use serde::{Deserialize, Serialize};

macro_rules! box_impl {
    ($($name:ident $t:ty), *) => {
        $(
            #[repr(transparent)]
            pub struct $name {
                inner: $t,
            }

            impl Deref for $name {
                type Target = $t;

                fn deref(&self) -> &Self::Target {
                    &self.inner
                }
            }

            impl $name{
                pub fn new(value: $t) -> Self {
                    Self { inner: value }
                }

                pub fn unbox(self) -> $t {
                    self.inner
                }
            }

            impl Serialize for $name {
                fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
                where
                    S: serde::Serializer,
                {
                    serializer.serialize_newtype_struct("", &self.inner)
                }
            }

            impl<'de> Deserialize<'de> for $name {
                fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
                where
                    D: serde::Deserializer<'de>,
                {
                    let value = <$t>::deserialize(deserializer)?;
                    Ok($name::new(value))
                }
            }
        )*
    };
}

box_impl!(BoxI8 i8, BoxI16 i16, BoxI32 i32, BoxI64 i64, BoxF32 f32, BoxF64 f64);
