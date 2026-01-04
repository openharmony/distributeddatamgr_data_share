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

use std::{
    ffi::CStr, fmt::Debug, ops::{Deref, DerefMut}
};

use serde::{Deserialize, Serialize};

use crate::{error::AniError, objects::AniClass, signature, AniEnv};

#[derive(Debug)]
pub enum TypedArray {
    Int8,
    Int16,
    Int32,
    Uint8,
    Uint16,
    Uint32,
}

impl TypedArray {
    pub fn ani_class<'local>(&self, env: &AniEnv<'local>) -> Result<AniClass<'local>, AniError> {
        let class_name: &'static CStr = match self {
            TypedArray::Int8 => signature::INT8_ARRAY,
            TypedArray::Int16 => signature::INT16_ARRAY,
            TypedArray::Int32 => signature::INT32_ARRAY,
            TypedArray::Uint8 => signature::UINT8_ARRAY,
            TypedArray::Uint16 => signature::UINT16_ARRAY,
            TypedArray::Uint32 => signature::UINT32_ARRAY,
        };
        env.find_class(class_name)
    }

    pub fn get_byte_size(&self) -> usize {
        match self {
            TypedArray::Int8 => 1,
            TypedArray::Int16 => 2,
            TypedArray::Int32 => 4,
            TypedArray::Uint8 => 1,
            TypedArray::Uint16 => 2,
            TypedArray::Uint32 => 4,
        }
    }
}

macro_rules! impl_typed_array {
    ($name: ident, $helper_name: ident, $rust_type: ident, $array_type: expr, $serde_name: literal) => {
        #[derive(Serialize, Deserialize)]
        #[serde(rename = $serde_name)]
        struct $helper_name<'local>(&'local [u8]);

        pub struct $name {
            inner: ArrayBuffer,
        }

        impl $name {
            pub fn new_with_vec(data: Vec<$rust_type>) -> Self {
                let raw_buffer = Vec::into_raw_parts(data);
                Self {
                    inner: ArrayBuffer::new(
                        raw_buffer.0 as *mut u8,
                        raw_buffer.1 * $array_type.get_byte_size(),
                        Some(raw_buffer.2)
                    ),
                }
            }

            pub fn len(&self) -> usize {
                self.inner.len() / $array_type.get_byte_size()
            }

            pub fn to_vec(&self) -> Vec<$rust_type> {
                Vec::from(self.as_ref())
            }
        }

        impl AsRef<[$rust_type]> for $name {
            fn as_ref(&self) -> &[$rust_type] {
                if self.inner.data_ptr.is_null() {
                    return &[];
                }
                unsafe {
                    std::slice::from_raw_parts(self.inner.data_ptr as *mut $rust_type, self.len())
                }
            }
        }

        impl AsMut<[$rust_type]> for $name {
            fn as_mut(&mut self) -> &mut [$rust_type] {
                if self.inner.data_ptr.is_null() {
                    return &mut [];
                }
                unsafe {
                    std::slice::from_raw_parts_mut(
                        self.inner.data_ptr as *mut $rust_type,
                        self.len(),
                    )
                }
            }
        }

        impl Deref for $name {
            type Target = [$rust_type];

            fn deref(&self) -> &[$rust_type] {
                self.as_ref()
            }
        }

        impl DerefMut for $name {
            fn deref_mut(&mut self) -> &mut Self::Target {
                self.as_mut()
            }
        }

        impl Serialize for $name {
            fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
            where
                S: serde::Serializer,
            {
                let data = self.inner.as_ref();
                let helper = $helper_name(data);
                helper.serialize(serializer)
            }
        }

        impl<'de> Deserialize<'de> for $name {
            fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
            where
                D: serde::Deserializer<'de>,
            {
                let value = $helper_name::deserialize(deserializer)?;
                Ok($name {
                        inner: ArrayBuffer::new_with_external_slice(value.0),
                    }
                )
            }
        }

        impl Drop for $name {
            fn drop(&mut self) {
                if let Some(cap) = self.inner.cap.take() {
                    let _ = unsafe {
                        Vec::from_raw_parts(self.inner.data_ptr, self.len(), cap)
                    };

                }
            }
        }

        impl Debug for $name {
            fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
                write!(f, "{:?}", self.as_ref())
            }
        }

        impl Clone for $name {
            fn clone(&self) -> Self {
                Self::new_with_vec(self.to_vec())
            }
        }

        unsafe impl Send for $name {}
        unsafe impl Sync for $name {}
    };
}

impl_typed_array!(
    Int8Array,
    Int8ArrayHelper,
    i8,
    TypedArray::Int8,
    "@Int8Array"
);
impl_typed_array!(
    Int16Array,
    Int16ArrayHelper,
    i16,
    TypedArray::Int16,
    "@Int16Array"
);
impl_typed_array!(
    Int32Array,
    Int32ArrayHelper,
    i32,
    TypedArray::Int32,
    "@Int32Array"
);
impl_typed_array!(
    Uint8Array,
    Uint8ArrayHelper,
    u8,
    TypedArray::Uint8,
    "@Uint8Array"
);
impl_typed_array!(
    Uint16Array,
    Uint16ArrayHelper,
    u16,
    TypedArray::Uint16,
    "@Uint16Array"
);
impl_typed_array!(
    Uint32Array,
    Uint32ArrayHelper,
    u32,
    TypedArray::Uint32,
    "@Uint32Array"
);

#[derive(Serialize, Deserialize)]
struct ArrayBufferHelper<'local>(&'local [u8]);

pub struct ArrayBuffer {
    data_ptr: *mut u8,
    length: usize,
    cap: Option<usize>, //Some when created from rust, only used to free Vec.
}

impl ArrayBuffer {
    fn new(data_ptr: *mut u8, length: usize, cap: Option<usize>) -> Self {
        Self { data_ptr, length, cap }
    }

    fn new_with_external_slice(data: &[u8]) -> Self {
        Self::new(data.as_ptr() as *mut u8, data.len(), None)
    }

    pub fn new_with_vec(data: Vec<u8>) -> Self {
        let raw_buffer = Vec::into_raw_parts(data);
        Self::new(raw_buffer.0, raw_buffer.1, Some(raw_buffer.2))
    }

    pub fn len(&self) -> usize {
        self.length
    }

    pub fn to_vec(&self) -> Vec<u8> {
        Vec::from(self.as_ref())
    }
}

impl AsRef<[u8]> for ArrayBuffer {
    fn as_ref(&self) -> &[u8] {
        if self.data_ptr.is_null() {
            return &[];
        }
        unsafe { std::slice::from_raw_parts(self.data_ptr, self.length) }
    }
}

impl AsMut<[u8]> for ArrayBuffer {
    fn as_mut(&mut self) -> &mut [u8] {
        if self.data_ptr.is_null() {
            return &mut [];
        }
        unsafe { std::slice::from_raw_parts_mut(self.data_ptr, self.length) }
    }
}

impl Deref for ArrayBuffer {
    type Target = [u8];

    fn deref(&self) -> &[u8] {
        self.as_ref()
    }
}

impl DerefMut for ArrayBuffer {
    fn deref_mut(&mut self) -> &mut Self::Target {
        self.as_mut()
    }
}

impl Serialize for ArrayBuffer {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        let data = self.as_ref();
        let helper = ArrayBufferHelper(data);
        helper.serialize(serializer)
    }
}

impl<'de> Deserialize<'de> for ArrayBuffer {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: serde::Deserializer<'de>,
    {
        let value = ArrayBufferHelper::deserialize(deserializer)?;
        Ok(ArrayBuffer::new_with_external_slice(value.0))
    }
}

impl Drop for ArrayBuffer {
    fn drop(&mut self) {
        if let Some(cap) = self.cap.take() {
            let _ = unsafe {
                Vec::from_raw_parts(self.data_ptr, self.len(), cap)
            };
        }
    }
}

impl Debug for ArrayBuffer {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", self.as_ref())
    }
}

impl Clone for ArrayBuffer {
    fn clone(&self) -> Self {
        Self::new_with_vec(self.to_vec())
    }
}

unsafe impl Send for ArrayBuffer {}
unsafe impl Sync for ArrayBuffer {}