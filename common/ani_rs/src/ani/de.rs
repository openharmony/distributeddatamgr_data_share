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

#![allow(unused)]

use std::ffi::{CStr, CString};

use serde::{
    de::{self, EnumAccess, MapAccess, SeqAccess, VariantAccess},
    Deserializer,
};

use crate::typed_array::TypedArray;
use crate::{
    env::AniExt,
    error::{AniError, Msg},
    iterator::AniIter,
    objects::{AniArray, AniEnum, AniEnumItem, AniObject, AniRef},
    signature, AniEnv,
};

use super::error_msg;

pub struct AniDe<'local> {
    env: AniEnv<'local>,
    obj: AniObject<'local>,

    typed_array: Option<TypedArray>,
    is_ani_ref: bool,
}

impl<'local> AniDe<'local> {
    pub fn new(env: &AniEnv<'local>, obj: AniObject<'local>) -> Self {
        Self {
            env: env.clone(),
            obj,
            typed_array: None,
            is_ani_ref: false,
        }
    }

    fn get_value<T: AniExt>(&mut self) -> Result<T, AniError> {
        self.env.unbox(&self.obj)
    }

    fn identifier(&mut self) -> Result<&'static str, AniError> {
        unimplemented!()
    }
}

struct StructDe<'local> {
    env: AniEnv<'local>,
    obj: AniObject<'local>,
    fields: &'static [&'static str],

    typed_array: Option<TypedArray>,
    is_ani_ref: bool,
}

impl<'local> StructDe<'local> {
    fn new(env: AniEnv<'local>, obj: AniObject<'local>, fields: &'static [&'static str]) -> Self {
        Self {
            env,
            obj,
            fields,
            typed_array: None,
            is_ani_ref: false,
        }
    }

    fn get_value<T: AniExt>(&mut self) -> Result<T, AniError> {
        let field_name = CStr::from_bytes_with_nul(self.fields[0].as_bytes())?;
        self.fields = self.fields[1..].as_ref();
        self.env.get_property::<T>(&self.obj, field_name)
    }

    fn identifier(&mut self) -> Result<&'static str, AniError> {
        Ok(self.fields[0])
    }
}

impl<'local> MapAccess<'local> for StructDe<'local> {
    type Error = AniError;

    fn next_key_seed<K>(&mut self, seed: K) -> Result<Option<K::Value>, Self::Error>
    where
        K: serde::de::DeserializeSeed<'local>,
    {
        if self.fields.is_empty() {
            Ok(None)
        } else {
            Ok(Some(seed.deserialize(self).unwrap()))
        }
    }

    fn next_value_seed<V>(&mut self, seed: V) -> Result<V::Value, Self::Error>
    where
        V: serde::de::DeserializeSeed<'local>,
    {
        seed.deserialize(self)
    }
}

struct MapDe<'local> {
    env: AniEnv<'local>,
    entries: AniIter<'local>,
}

impl<'local> MapDe<'local> {
    fn new(env: AniEnv<'local>, entries: AniIter<'local>) -> Self {
        Self { env, entries }
    }
}

impl<'local> MapAccess<'local> for MapDe<'local> {
    type Error = AniError;

    fn next_key_seed<K>(&mut self, seed: K) -> Result<Option<K::Value>, Self::Error>
    where
        K: serde::de::DeserializeSeed<'local>,
    {
        unimplemented!()
    }

    fn next_value_seed<V>(&mut self, seed: V) -> Result<V::Value, Self::Error>
    where
        V: serde::de::DeserializeSeed<'local>,
    {
        unimplemented!()
    }

    fn next_entry_seed<K, V>(
        &mut self,
        kseed: K,
        vseed: V,
    ) -> Result<Option<(K::Value, V::Value)>, Self::Error>
    where
        K: de::DeserializeSeed<'local>,
        V: de::DeserializeSeed<'local>,
    {
        match self.entries.next() {
            None => Ok(None),
            Some(Err(err)) => Err(err),
            Some(Ok((key, value))) => {
                let mut de = AniDe::new(&self.env, key.into());
                let key = kseed.deserialize(&mut de)?;
                let mut de = AniDe::new(&self.env, value.into());
                let value = vseed.deserialize(&mut de)?;
                Ok(Some((key, value)))
            }
        }
    }
}

struct ArrayDe<'local> {
    env: AniEnv<'local>,
    array: AniArray<'local>,
    len: usize,
    index: usize,

    typed_array: Option<TypedArray>,
    is_ani_ref: bool,
}

impl<'local> ArrayDe<'local> {
    fn new(env: AniEnv<'local>, array: AniArray<'local>, len: usize) -> Self {
        Self {
            env,
            array,
            len,
            index: 0,
            typed_array: None,
            is_ani_ref: false,
        }
    }

    fn get_value<T: AniExt>(&mut self) -> Result<T, AniError> {
        let index = self.index;
        self.index += 1;
        self.env.array_get(&self.array, index)
    }

    fn identifier(&mut self) -> Result<&'static str, AniError> {
        unimplemented!()
    }
}

impl<'local> SeqAccess<'local> for ArrayDe<'local> {
    type Error = AniError;

    fn next_element_seed<T>(&mut self, seed: T) -> Result<Option<T::Value>, Self::Error>
    where
        T: serde::de::DeserializeSeed<'local>,
    {
        if self.index >= self.len {
            return Ok(None);
        }
        seed.deserialize(self).map(|res| Some(res))
    }

    fn size_hint(&self) -> Option<usize> {
        Some(self.len - self.index)
    }
}

struct EnumDe<'local> {
    env: AniEnv<'local>,
    enum_item: AniObject<'local>,
    enum_name: Option<&'static CStr>,
    variants: &'static [&'static str],

    typed_array: Option<TypedArray>,
    is_ani_ref: bool,
}

impl<'local> EnumDe<'local> {
    fn new(
        env: AniEnv<'local>,
        enum_item: AniObject<'local>,
        enum_name: Option<&'static CStr>,
        variants: &'static [&'static str],
    ) -> Self {
        Self {
            env,
            enum_item,
            enum_name,
            variants,
            typed_array: None,
            is_ani_ref: false,
        }
    }

    fn get_value<T: AniExt>(&mut self) -> Result<T, AniError> {
        self.env.unbox(&self.enum_item)
    }

    fn identifier(&mut self) -> Result<&'static str, AniError> {
        if let Some(name) = self.enum_name {
            let class = self.env.find_class(name)?;
            if !self.env.instance_of(&self.enum_item, &class)? {
                let message = format!("enum item not match enum class {:?}", name);
                return Err(AniError::message(message));
            }

            let item = AniEnumItem::from(self.enum_item.clone());
            let index = self.env.get_enum_item_index(&item)?;
            return Ok(self.variants[index]);
        } else {
            for variant in self.variants {
                if *variant == "Null" {
                    if self.env.is_null(&self.enum_item)? {
                        return Ok(variant);
                    } else {
                        continue;
                    }
                }
                let variant_cstring = CString::new(*variant).unwrap();
                let class_name = match *variant {
                    "Boolean" => signature::BOOLEAN,
                    "I8" => signature::BYTE,
                    "I16" => signature::SHORT,
                    "I32" => signature::INT,
                    "I64" => signature::LONG,
                    "F32" => signature::FLOAT,
                    "F64" => signature::DOUBLE,
                    "S" => signature::STRING,
                    "Array" => signature::ARRAY,
                    "Record" => signature::RECORD,
                    "ArrayBuffer" => signature::ARRAY_BUFFER,
                    "Int8Array" => signature::INT8_ARRAY,
                    "Int16Array" => signature::INT16_ARRAY,
                    "Int32Array" => signature::INT32_ARRAY,
                    "Uint8Array" => signature::UINT8_ARRAY,
                    "Uint16Array" => signature::UINT16_ARRAY,
                    "Uint32Array" => signature::UINT32_ARRAY,
                    _ => variant_cstring.as_c_str(),
                };
                
                let class = self.env.find_class(&class_name)?;
                if self.env.instance_of(&self.enum_item, &class)? {
                    return Ok(variant);
                }
            }
            let message = format!("all variants {:?} not match", self.variants,);
            return Err(AniError::message(message));
        }
    }
}

impl<'local> VariantAccess<'local> for EnumDe<'local> {
    type Error = AniError;

    fn unit_variant(self) -> Result<(), Self::Error> {
        Ok(())
    }

    fn newtype_variant_seed<T>(mut self, seed: T) -> Result<T::Value, Self::Error>
    where
        T: de::DeserializeSeed<'local>,
    {
        seed.deserialize(&mut self)
    }

    fn struct_variant<V>(
        self,
        fields: &'static [&'static str],
        visitor: V,
    ) -> Result<V::Value, Self::Error>
    where
        V: de::Visitor<'local>,
    {
        todo!()
    }

    fn tuple_variant<V>(self, len: usize, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: de::Visitor<'local>,
    {
        todo!()
    }
}

impl<'local> EnumAccess<'local> for EnumDe<'local> {
    type Error = AniError;
    type Variant = Self;

    fn variant_seed<V>(mut self, seed: V) -> Result<(V::Value, Self), Self::Error>
    where
        V: serde::de::DeserializeSeed<'local>,
    {
        seed.deserialize(&mut self).map(|value| (value, self))
    }
}

macro_rules! impl_de {
    ($t:ty) => {
        impl<'local> Deserializer<'local> for $t {
            type Error = AniError;

            fn deserialize_any<V>(self, _visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                todo!()
            }

            fn deserialize_bool<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                visitor.visit_bool(self.get_value()?)
            }

            fn deserialize_i8<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                visitor.visit_i8(self.get_value()?)
            }

            fn deserialize_i16<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                visitor.visit_i16(self.get_value()?)
            }

            fn deserialize_i32<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                visitor.visit_i32(self.get_value()?)
            }

            fn deserialize_i64<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                if self.is_ani_ref {
                    let ani_ref: AniRef = self.get_value()?;
                    visitor.visit_i64(ani_ref.as_raw() as i64)
                } else {
                    visitor.visit_i64(self.get_value()?)
                }
            }

            fn deserialize_u8<V>(self, _visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                Err(error_msg::U8_UNSUPPORTED)
            }

            fn deserialize_u16<V>(self, _visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                Err(error_msg::U16_UNSUPPORTED)
            }

            fn deserialize_u32<V>(self, _visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                Err(error_msg::U32_UNSUPPORTED)
            }

            fn deserialize_u64<V>(self, _visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                Err(error_msg::U64_UNSUPPORTED)
            }

            fn deserialize_f32<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                visitor.visit_f32(self.get_value()?)
            }

            fn deserialize_f64<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                visitor.visit_f64(self.get_value()?)
            }

            fn deserialize_byte_buf<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                let value: AniRef<'local> = self.get_value()?;
                visitor.visit_bytes(self.env.array_buffer(&value.into())?)
            }

            fn deserialize_bytes<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                let value: AniRef<'local> = self.get_value()?;
                let obj = value.into();
                if let Some(typed_array) = self.typed_array.take() {
                    let class = typed_array.ani_class(&self.env)?;
                    let byte_length =
                        self.env.get_property::<i32>(&obj, signature::BYTE_LENGTH)? as usize;
                    let byte_offset =
                        self.env.get_property::<i32>(&obj, signature::BYTE_OFFSET)? as usize;
                    let buffer = self
                        .env
                        .get_property::<AniRef<'local>>(&obj, signature::BUFFER)?;
                    let data = self.env.array_buffer(&buffer)?;
                    visitor.visit_borrowed_bytes(&data[byte_offset..(byte_offset + byte_length)])
                } else {
                    visitor.visit_borrowed_bytes(self.env.array_buffer(&obj)?)
                }
            }

            fn deserialize_char<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                visitor.visit_char(self.get_value()?)
            }

            fn deserialize_enum<V>(
                self,
                name: &'static str,
                variants: &'static [&'static str],
                visitor: V,
            ) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                let ani_ref: AniRef<'local> = self.get_value()?;
                let name = CStr::from_bytes_with_nul(name.as_bytes()).ok();

                visitor.visit_enum(EnumDe::new(
                    self.env.clone(),
                    ani_ref.into(),
                    name,
                    variants,
                ))
            }

            fn deserialize_i128<V>(self, _visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                unimplemented!()
            }

            fn deserialize_u128<V>(self, _visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                unimplemented!()
            }

            fn deserialize_identifier<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                visitor.visit_str(self.identifier()?)
            }

            fn deserialize_ignored_any<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                todo!()
            }

            fn deserialize_map<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                let value: AniRef<'local> = self.get_value()?;
                let entries = self.env.record_entries(&value.into())?;
                visitor.visit_map(MapDe::new(self.env.clone(), entries))
            }

            fn deserialize_newtype_struct<V>(
                self,
                name: &'static str,
                visitor: V,
            ) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                self.typed_array = match name {
                    "@Int8Array" => Some(TypedArray::Int8),
                    "@Int16Array" => Some(TypedArray::Int16),
                    "@Int32Array" => Some(TypedArray::Int32),
                    "@Uint8Array" => Some(TypedArray::Uint8),
                    "@Uint16Array" => Some(TypedArray::Uint16),
                    "@Uint32Array" => Some(TypedArray::Uint32),
                    _ => None,
                };
                if name == "@AniRef" {
                    self.is_ani_ref = true;
                }
                visitor.visit_newtype_struct(self)
            }

            fn deserialize_option<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                let ani_ref: AniRef<'local> = self.get_value()?;
                if self.env.is_undefined(&ani_ref)? {
                    visitor.visit_none()
                } else {
                    let mut de = AniDe::new(&self.env, ani_ref.into());
                    visitor.visit_some(&mut de)
                }
            }

            fn deserialize_seq<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                let array: AniRef<'local> = self.get_value()?;
                let array = AniArray::from(array);
                let length = self.env.array_length(&array)?;
                visitor.visit_seq(ArrayDe::new(self.env.clone(), array, length))
            }

            fn deserialize_str<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                todo!()
            }

            fn deserialize_string<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                visitor.visit_string(self.get_value()?)
            }

            fn deserialize_struct<V>(
                self,
                name: &'static str,
                fields: &'static [&'static str],
                visitor: V,
            ) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                let value: AniRef<'local> = self.get_value()?;
                visitor.visit_map(StructDe::new(self.env.clone(), value.into(), fields))
            }

            fn deserialize_tuple<V>(self, len: usize, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                todo!()
            }

            fn deserialize_tuple_struct<V>(
                self,
                name: &'static str,
                len: usize,
                visitor: V,
            ) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                todo!()
            }

            fn deserialize_unit<V>(self, visitor: V) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                visitor.visit_unit()
            }

            fn deserialize_unit_struct<V>(
                self,
                name: &'static str,
                visitor: V,
            ) -> Result<V::Value, Self::Error>
            where
                V: serde::de::Visitor<'local>,
            {
                todo!()
            }
        }
    };
}

impl_de!(&mut AniDe<'local>);
impl_de!(&mut StructDe<'local>);
impl_de!(&mut ArrayDe<'local>);
impl_de!(&mut EnumDe<'local>);
