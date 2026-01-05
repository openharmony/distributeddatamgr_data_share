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

use core::panic;
use std::{
    any::type_name,
    array,
    ffi::{CStr, CString},
};

use serde::{de::MapAccess, ser::SerializeMap};

use crate::{
    env::AniExt,
    error::AniError,
    objects::{AniArray, AniClass, AniObject, AniRef},
    signature::{self, ARRAY, RECORD},
    typed_array::TypedArray,
    AniEnv,
};

use super::error_msg;

pub struct NoSer;

pub trait AniSerExt {
    fn recur(&mut self, value: AniRef) -> Result<(), AniError>;
}

pub struct AniSer<'local> {
    env: AniEnv<'local>,
    res: AniRef<'local>,
    typed_array: Option<TypedArray>,
    is_ani_ref: bool,
}

impl<'local> AniSer<'local> {
    pub fn new(env: &AniEnv<'local>) -> Result<Self, AniError> {
        let res = env.undefined()?;
        Ok(AniSer {
            env: env.clone(),
            res,
            typed_array: None,
            is_ani_ref: false,
        })
    }

    pub fn set_value<T: AniExt>(&mut self, value: T) -> Result<(), AniError> {
        self.res = self.env.inbox(value)?;
        Ok(())
    }

    pub fn finish(self) -> AniRef<'local> {
        self.res
    }

    pub fn set_typed_array(&mut self, typed_array: TypedArray) {
        self.typed_array = Some(typed_array);
    }

    pub fn typed_array(&self) -> Option<&TypedArray> {
        self.typed_array.as_ref()
    }
}

impl AniSerExt for AniSer<'_> {
    fn recur(&mut self, value: AniRef) -> Result<(), AniError> {
        self.set_value(value)
    }
}

pub struct StructSer<'recur, 'local> {
    env: AniEnv<'local>,
    obj: AniObject<'local>,
    field: &'static CStr,
    recur: &'recur mut dyn AniSerExt,
}

impl<'recur, 'local> StructSer<'recur, 'local> {
    pub fn new(
        env: AniEnv<'local>,
        obj: AniObject<'local>,
        field: &'static CStr,
        recur: &'recur mut dyn AniSerExt,
    ) -> Self {
        StructSer {
            env,
            obj,
            field,
            recur,
        }
    }

    pub fn set_value<T: AniExt>(&mut self, value: T) -> Result<(), AniError> {
        self.env.set_property(&self.obj, self.field, value)?;
        Ok(())
    }
}

impl AniSerExt for StructSer<'_, '_> {
    fn recur(&mut self, value: AniRef) -> Result<(), AniError> {
        self.set_value(value)
    }
}

pub struct ArraySer<'recur, 'local> {
    env: AniEnv<'local>,
    array: Option<AniRef<'local>>,
    ref_class: Option<AniClass<'local>>,
    data: Option<*mut u8>,
    len: usize,
    index: usize,
    typed_array: Option<TypedArray>,
    recur: &'recur mut dyn AniSerExt,
}

impl<'recur, 'local> ArraySer<'recur, 'local> {
    pub fn new(
        env: AniEnv<'local>,
        len: usize,
        typed_array: Option<TypedArray>,
        recur: &'recur mut dyn AniSerExt,
    ) -> Self {
        ArraySer {
            env: env.clone(),
            array: None,
            ref_class: None,
            data: None,
            len,
            typed_array,
            index: 0,
            recur,
        }
    }

    pub fn set_value<T: AniExt>(&mut self, value: T) -> Result<(), AniError> {
        let array = match &mut self.array {
            Some(array) => array,
            None => {
                let array = T::new_array(&self.env, self.len)?;
                self.array = Some(array);
                self.array.as_mut().unwrap()
            }
        };

        self.env.array_set(array, self.index, value)?;
        self.index += 1;
        Ok(())
    }
}

impl<'recur, 'local> AniSerExt for ArraySer<'recur, 'local> {
    fn recur(&mut self, value: AniRef) -> Result<(), AniError> {
        self.set_value(value)
    }
}

pub struct MapSer<'recur, 'local> {
    env: AniEnv<'local>,
    key: Option<AniRef<'local>>,
    obj: Option<AniObject<'local>>,
    recur: &'recur mut dyn AniSerExt,
}

impl<'recur, 'local> MapSer<'recur, 'local> {
    fn new(env: AniEnv<'local>, recur: &'recur mut dyn AniSerExt) -> Self {
        MapSer {
            env,
            key: None,
            obj: None,
            recur,
        }
    }
}

impl<'recur, 'local> SerializeMap for MapSer<'recur, 'local> {
    type Ok = ();
    type Error = AniError;

    fn serialize_key<T>(&mut self, key: &T) -> Result<(), Self::Error>
    where
        T: ?Sized + serde::Serialize,
    {
        let record = match &self.obj {
            Some(record) => record,
            None => {
                let record = self.env.new_record()?;
                self.obj = Some(record);
                self.obj.as_ref().unwrap()
            }
        };

        let mut se = AniSer::new(&self.env)?;
        key.serialize(&mut se)?;
        let key = se.finish();
        self.key = Some(key);
        Ok(())
    }

    fn serialize_value<T>(&mut self, value: &T) -> Result<(), Self::Error>
    where
        T: ?Sized + serde::Serialize,
    {
        let record = match &self.obj {
            Some(record) => record,
            None => {
                let record = self.env.new_record()?;
                self.obj = Some(record);
                self.obj.as_ref().unwrap()
            }
        };
        let mut se = AniSer::new(&self.env)?;
        value.serialize(&mut se)?;
        let value = se.finish();

        if let Some(key) = &self.key {
            self.env.set_record(record, &key, &value)?;
        }
        Ok(())
    }

    fn serialize_entry<K, V>(&mut self, key: &K, value: &V) -> Result<(), Self::Error>
    where
        K: ?Sized + serde::Serialize,
        V: ?Sized + serde::Serialize,
    {
        let record = match &self.obj {
            Some(record) => record,
            None => {
                let record = self.env.new_record()?;
                self.obj = Some(record);
                self.obj.as_ref().unwrap()
            }
        };
        let mut se = AniSer::new(&self.env)?;
        key.serialize(&mut se)?;
        let key = se.finish();

        let mut se = AniSer::new(&self.env)?;
        value.serialize(&mut se)?;
        let value = se.finish();
        self.env.set_record(record, &key, &value)?;
        Ok(())
    }

    fn end(self) -> Result<Self::Ok, Self::Error> {
        let record = match self.obj {
            Some(record) => record,
            None => self.env.new_record()?,
        };
        let ani_ref: AniRef<'_> = record.into();
        self.recur.recur(ani_ref)
    }
}

macro_rules! common_impl {
    () => {
        type Ok = ();
        type Error = AniError;

        type SerializeMap = MapSer<'a, 'local>;
        type SerializeTuple = NoSer;
        type SerializeTupleStruct = NoSer;
        type SerializeTupleVariant = NoSer;
        type SerializeStruct = StructSer<'a, 'local>;
        type SerializeStructVariant = NoSer;
        type SerializeSeq = ArraySer<'a, 'local>;

        fn serialize_bool(self, v: bool) -> Result<Self::Ok, Self::Error> {
            self.set_value(v)
        }

        fn serialize_i8(self, v: i8) -> Result<Self::Ok, Self::Error> {
            self.set_value(v)
        }

        fn serialize_i16(self, v: i16) -> Result<Self::Ok, Self::Error> {
            self.set_value(v)
        }

        fn serialize_i32(self, v: i32) -> Result<Self::Ok, Self::Error> {
            self.set_value(v)
        }

        fn serialize_i128(self, _v: i128) -> Result<Self::Ok, Self::Error> {
            Err(error_msg::I128_UNSUPPORTED)
        }

        fn serialize_u16(self, v: u16) -> Result<Self::Ok, Self::Error> {
            Err(error_msg::U16_UNSUPPORTED)
        }

        fn serialize_u32(self, _v: u32) -> Result<Self::Ok, Self::Error> {
            Err(error_msg::U32_UNSUPPORTED)
        }

        fn serialize_u64(self, _v: u64) -> Result<Self::Ok, Self::Error> {
            Err(error_msg::U64_UNSUPPORTED)
        }

        fn serialize_u128(self, _v: u128) -> Result<Self::Ok, Self::Error> {
            Err(error_msg::U128_UNSUPPORTED)
        }

        fn serialize_f32(self, v: f32) -> Result<Self::Ok, Self::Error> {
            self.set_value(v)
        }

        fn serialize_f64(self, v: f64) -> Result<Self::Ok, Self::Error> {
            self.set_value(v)
        }

        fn serialize_char(self, v: char) -> Result<Self::Ok, Self::Error> {
            self.set_value(v)
        }

        fn serialize_str(self, v: &str) -> Result<Self::Ok, Self::Error> {
            self.set_value(v.to_string())
        }

        fn serialize_bytes(self, v: &[u8]) -> Result<Self::Ok, Self::Error> {
            unimplemented!()
        }

        fn serialize_none(mut self) -> Result<Self::Ok, Self::Error> {
            self.set_value(self.env.undefined()?)
        }

        fn serialize_some<T: ?Sized>(mut self, value: &T) -> Result<Self::Ok, Self::Error>
        where
            T: serde::Serialize,
        {
            let mut se = AniSer::new(&self.env)?;
            value.serialize(&mut se)?;
            let value = se.finish();
            self.set_value(value)
        }

        fn serialize_unit(mut self) -> Result<Self::Ok, Self::Error> {
            self.set_value(self.env.null()?)
        }

        fn serialize_unit_struct(self, name: &'static str) -> Result<Self::Ok, Self::Error> {
            Ok(())
        }

        fn serialize_newtype_struct<T: ?Sized>(
            self,
            name: &'static str,
            value: &T,
        ) -> Result<Self::Ok, Self::Error>
        where
            T: serde::Serialize,
        {
            let typed_array = match name {
                "@Int8Array" => Some(TypedArray::Int8),
                "@Int16Array" => Some(TypedArray::Int16),
                "@Int32Array" => Some(TypedArray::Int32),
                "@Uint8Array" => Some(TypedArray::Uint8),
                "@Uint16Array" => Some(TypedArray::Uint16),
                "@Uint32Array" => Some(TypedArray::Uint32),
                _ => None,
            };
            let mut ser = AniSer::new(&self.env)?;
            if let Some(typed_array) = typed_array {
                ser.set_typed_array(typed_array);
            }
            if name == "@AniRef" {
                ser.is_ani_ref = true;
            }
            value.serialize(&mut ser);
            let value = ser.finish();
            self.set_value(value)
        }

        fn serialize_tuple(self, len: usize) -> Result<Self::SerializeTuple, Self::Error> {
            todo!()
        }

        fn serialize_tuple_struct(
            self,
            name: &'static str,
            len: usize,
        ) -> Result<Self::SerializeTupleStruct, Self::Error> {
            todo!()
        }

        fn serialize_tuple_variant(
            self,
            name: &'static str,
            variant_index: u32,
            variant: &'static str,
            len: usize,
        ) -> Result<Self::SerializeTupleVariant, Self::Error> {
            todo!()
        }

        fn serialize_struct_variant(
            self,
            name: &'static str,
            variant_index: u32,
            variant: &'static str,
            len: usize,
        ) -> Result<Self::SerializeStructVariant, Self::Error> {
            todo!()
        }

        fn serialize_newtype_variant<T: ?Sized>(
            self,
            _name: &'static str,
            _variant_index: u32,
            _variant: &'static str,
            value: &T,
        ) -> Result<Self::Ok, Self::Error>
        where
            T: serde::Serialize,
        {
            let mut se = AniSer::new(&self.env)?;
            value.serialize(&mut se)?;
            let ani_ref = se.finish();
            self.set_value(ani_ref)
        }
    };
}

impl<'a, 'local> serde::ser::Serializer for &'a mut AniSer<'local> {
    common_impl!();

    fn serialize_i64(self, v: i64) -> Result<Self::Ok, Self::Error> {
        if self.is_ani_ref {
            let ani_ref = AniRef::from_raw(v as _);
            self.set_value(ani_ref)
        } else {
            self.set_value(v)
        }
    }

    fn serialize_u8(self, _v: u8) -> Result<Self::Ok, Self::Error> {
        Err(error_msg::U8_UNSUPPORTED)
    }

    fn serialize_unit_variant(
        self,
        name: &'static str,
        variant_index: u32,
        variant: &'static str,
    ) -> Result<Self::Ok, Self::Error> {
        let class_name = CStr::from_bytes_with_nul(name.as_bytes())?;
        let enum_type = self.env.find_enum(&class_name)?;
        let enum_name = CStr::from_bytes_with_nul(variant.as_bytes())?;
        let enum_item = self.env.new_enum_item(&enum_type, enum_name)?;
        let ani_ref = AniRef::from(enum_item);
        self.set_value(ani_ref)
    }

    fn serialize_seq(mut self, len: Option<usize>) -> Result<Self::SerializeSeq, Self::Error> {
        match len {
            Some(len) => {
                let env = self.env.clone();
                Ok(ArraySer::new(env, len, self.typed_array.take(), self))
            }
            None => Err(error_msg::ARRAY_WITHOUT_LENGTH_UNSUPPORTED),
        }
    }

    fn serialize_map(self, len: Option<usize>) -> Result<Self::SerializeMap, Self::Error> {
        Ok(MapSer::new(self.env.clone(), self))
    }

    fn serialize_struct(
        mut self,
        name: &'static str,
        _len: usize,
    ) -> Result<Self::SerializeStruct, Self::Error> {
        let class_name = CStr::from_bytes_with_nul(name.as_bytes())?;
        let class = self.env.find_class(&class_name)?;

        let obj = self.env.new_object(&class, ())?;
        Ok(StructSer::new(self.env.clone(), obj, class_name, self))
    }
}

impl<'a, 'recur, 'local> serde::ser::Serializer for &'a mut StructSer<'recur, 'local> {
    common_impl!();

    fn serialize_i64(self, v: i64) -> Result<Self::Ok, Self::Error> {
        self.set_value(v)
    }

    fn serialize_u8(self, _v: u8) -> Result<Self::Ok, Self::Error> {
        Err(error_msg::U8_UNSUPPORTED)
    }

    fn serialize_unit_variant(
        self,
        name: &'static str,
        variant_index: u32,
        variant: &'static str,
    ) -> Result<Self::Ok, Self::Error> {
        let class_name = CStr::from_bytes_with_nul(name.as_bytes())?;
        let enum_type = self.env.find_enum(&class_name)?;
        let enum_name = CStr::from_bytes_with_nul(variant.as_bytes())?;
        let enum_item = self.env.new_enum_item(&enum_type, enum_name)?;
        let ani_ref = AniRef::from(enum_item);
        self.set_value(ani_ref)
    }

    fn serialize_map(self, len: Option<usize>) -> Result<Self::SerializeMap, Self::Error> {
        Ok(MapSer::new(self.env.clone(), self))
    }

    fn serialize_seq(self, len: Option<usize>) -> Result<Self::SerializeSeq, Self::Error> {
        match len {
            Some(len) => {
                let env = self.env.clone();
                Ok(ArraySer::new(env, len, None, self))
            }
            None => Err(error_msg::ARRAY_WITHOUT_LENGTH_UNSUPPORTED),
        }
    }

    fn serialize_struct(
        mut self,
        name: &'static str,
        _len: usize,
    ) -> Result<Self::SerializeStruct, Self::Error> {
        let class_name = CStr::from_bytes_with_nul(name.as_bytes())?;
        let class = self.env.find_class(&class_name)?;

        let obj = self.env.new_object(&class, ())?;
        Ok(StructSer::new(self.env.clone(), obj, class_name, self))
    }
}

impl<'a, 'recur, 'local> serde::ser::Serializer for &'a mut ArraySer<'recur, 'local> {
    common_impl!();

    fn serialize_i64(self, v: i64) -> Result<Self::Ok, Self::Error> {
        self.set_value(v)
    }

    fn serialize_u8(self, v: u8) -> Result<Self::Ok, Self::Error> {
        let data = match self.data.as_mut() {
            Some(mut data) => {
                unsafe { *data = data.add(1) }
                *data
            }
            None => {
                let data = self.env.create_array_buffer(self.len)?;
                self.array = Some(data.0);
                self.data = Some(data.1);
                self.data.unwrap()
            }
        };
        unsafe {
            *data = v;
        }
        Ok(())
    }

    fn serialize_unit_variant(
        self,
        name: &'static str,
        variant_index: u32,
        variant: &'static str,
    ) -> Result<Self::Ok, Self::Error> {
        let class_name = CStr::from_bytes_with_nul(name.as_bytes())?;
        let enum_type = self.env.find_enum(&class_name)?;
        let enum_name = CStr::from_bytes_with_nul(variant.as_bytes())?;
        let enum_item = self.env.new_enum_item(&enum_type, enum_name)?;

        self.ref_class = Some(enum_type.into());
        let ani_ref = AniRef::from(enum_item);
        self.set_value(ani_ref)
    }

    fn serialize_seq(self, len: Option<usize>) -> Result<Self::SerializeSeq, Self::Error> {
        unimplemented!()
    }

    fn serialize_map(self, len: Option<usize>) -> Result<Self::SerializeMap, Self::Error> {
        self.ref_class = Some(self.env.find_class(RECORD)?);
        Ok(MapSer::new(self.env.clone(), self))
    }

    fn serialize_struct(
        mut self,
        name: &'static str,
        _len: usize,
    ) -> Result<Self::SerializeStruct, Self::Error> {
        let class_name = CStr::from_bytes_with_nul(name.as_bytes())?;
        let class = self.env.find_class(&class_name)?;
        let obj = self.env.new_object(&class, ())?;
        self.ref_class = Some(class);

        Ok(StructSer::new(self.env.clone(), obj, class_name, self))
    }
}

impl<'recur, 'local> serde::ser::SerializeStruct for StructSer<'recur, 'local> {
    type Ok = ();
    type Error = AniError;

    fn serialize_field<T: ?Sized>(
        &mut self,
        key: &'static str,
        value: &T,
    ) -> Result<Self::Ok, Self::Error>
    where
        T: serde::Serialize,
    {
        self.field = CStr::from_bytes_with_nul(key.as_bytes())?;
        value.serialize(self)?;
        Ok(())
    }

    fn end(self) -> Result<Self::Ok, Self::Error> {
        self.recur.recur(self.obj.into())
    }
}

impl serde::ser::SerializeSeq for ArraySer<'_, '_> {
    type Ok = ();
    type Error = AniError;

    fn serialize_element<T: ?Sized>(&mut self, value: &T) -> Result<Self::Ok, Self::Error>
    where
        T: serde::Serialize,
    {
        value.serialize(self)
    }

    fn end(mut self) -> Result<Self::Ok, Self::Error> {
        let array = match self.array {
            Some(array) => array,
            None => self.env.new_array::<bool>(0).unwrap(),
        };
        if let Some(typed_array) = self.typed_array {
            let class = typed_array.ani_class(&self.env)?;
            let undefined = self.env.undefined()?;
            let obj = self.env.new_object_with_signature(
                &class,
                signature::TYPED_ARRAY_CTOR,
                (array.as_raw(), 0),
            )?;
            self.recur.recur(obj.into())
        } else {
            self.recur.recur(array.into())
        }
    }
}

impl serde::ser::SerializeTuple for NoSer {
    type Ok = ();
    type Error = AniError;

    fn serialize_element<T: ?Sized>(&mut self, value: &T) -> Result<Self::Ok, Self::Error>
    where
        T: serde::Serialize,
    {
        Ok(())
    }

    fn end(self) -> Result<Self::Ok, Self::Error> {
        Ok(())
    }
}

impl serde::ser::SerializeTupleStruct for NoSer {
    type Ok = ();
    type Error = AniError;

    fn serialize_field<T: ?Sized>(&mut self, value: &T) -> Result<Self::Ok, Self::Error>
    where
        T: serde::Serialize,
    {
        Ok(())
    }

    fn end(self) -> Result<Self::Ok, Self::Error> {
        Ok(())
    }
}

impl<'e> serde::ser::SerializeTupleVariant for NoSer {
    type Ok = ();
    type Error = AniError;

    fn serialize_field<T: ?Sized>(&mut self, value: &T) -> Result<Self::Ok, Self::Error>
    where
        T: serde::Serialize,
    {
        Ok(())
    }

    fn end(self) -> Result<Self::Ok, Self::Error> {
        Ok(())
    }
}

impl serde::ser::SerializeStructVariant for NoSer {
    type Ok = ();
    type Error = AniError;

    fn serialize_field<T: ?Sized>(
        &mut self,
        key: &'static str,
        value: &T,
    ) -> Result<Self::Ok, Self::Error>
    where
        T: serde::Serialize,
    {
        Ok(())
    }

    fn end(self) -> Result<Self::Ok, Self::Error> {
        Ok(())
    }
}

impl serde::ser::SerializeMap for NoSer {
    type Ok = ();
    type Error = AniError;
    fn serialize_key<T: ?Sized>(&mut self, _key: &T) -> Result<Self::Ok, Self::Error>
    where
        T: serde::Serialize,
    {
        Ok(())
    }
    fn serialize_value<T: ?Sized>(&mut self, _value: &T) -> Result<Self::Ok, Self::Error>
    where
        T: serde::Serialize,
    {
        Ok(())
    }

    fn serialize_entry<K, V>(&mut self, key: &K, value: &V) -> Result<(), Self::Error>
    where
        K: ?Sized + serde::Serialize,
        V: ?Sized + serde::Serialize,
    {
        Ok(())
    }

    fn end(self) -> Result<Self::Ok, Self::Error> {
        Ok(())
    }
}
