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
    ffi::{c_void, CStr},
    marker::PhantomData,
    ops::Deref,
    ptr::null_mut,
};

use ani_sys::{
    ani_array, ani_class, ani_enum, ani_enum_item, ani_env, ani_error, ani_method, ani_namespace,
    ani_object, ani_ref, ani_static_method, ani_string, ani_long,
};
use serde::{Deserialize, Serialize};

use crate::{
    ani::{AniDe, AniSer},
    error::AniError,
    iterator::AniIter,
    objects::{
        AniClass, AniEnum, AniEnumItem, AniFnObject, AniMethod, AniNamespace, AniNativeFunction,
        AniObject, AniRef, AniStaticMethod, AniString,
    },
    signature::{self, ENTRIES},
};

#[derive(Clone)]
#[repr(transparent)]
pub struct AniEnv<'local> {
    pub inner: *mut ani_env,
    lifetime: PhantomData<&'local ()>,
}

impl<'local> Deref for AniEnv<'local> {
    type Target = *mut ani_env;

    fn deref(&self) -> &Self::Target {
        &self.inner
    }
}

impl<'local> AniEnv<'local> {
    pub fn from_raw(ptr: *mut ani_env) -> Self {
        Self {
            inner: ptr,
            lifetime: PhantomData,
        }
    }

    pub fn serialize<T: Serialize>(&self, obj: &T) -> Result<AniRef<'local>, AniError> {
        let mut serializer = AniSer::new(self)?;
        obj.serialize(&mut serializer)?;
        Ok(serializer.finish())
    }

    pub fn deserialize<T: Deserialize<'local>>(
        &self,
        obj: AniObject<'local>,
    ) -> Result<T, AniError> {
        let mut deserializer = AniDe::new(self, obj);
        T::deserialize(&mut deserializer)
    }

    pub fn find_namespace(&self, name: &CStr) -> Result<AniNamespace<'local>, AniError> {
        unsafe {
            let mut namespace = null_mut() as ani_namespace;
            let res = (**self.inner).FindNamespace.unwrap()(
                self.inner,
                name.as_ptr(),
                &mut namespace as *mut _,
            );
            if res != 0 {
                let msg = format!("Failed to find namespace {}", name.to_string_lossy());
                Err(AniError::from_code(msg, res))
            } else {
                Ok(AniNamespace::from_raw(namespace))
            }
        }
    }

    pub fn find_class(&self, name: &CStr) -> Result<AniClass<'local>, AniError> {
        unsafe {
            let mut class = null_mut() as ani_class;
            let res =
                (**self.inner).FindClass.unwrap()(self.inner, name.as_ptr(), &mut class as *mut _);
            if res != 0 {
                let msg = format!("Failed to find class {}", name.to_string_lossy());
                Err(AniError::from_code(msg, res))
            } else {
                Ok(AniClass::from_raw(class))
            }
        }
    }

    pub fn find_method(
        &self,
        class: &AniClass,
        name: &CStr,
    ) -> Result<AniMethod<'local>, AniError> {
        unsafe {
            let mut method = null_mut() as ani_method;
            let res = (**self.inner).Class_FindMethod.unwrap()(
                self.inner,
                class.as_raw(),
                name.as_ptr(),
                null_mut(),
                &mut method as *mut _,
            );
            if res != 0 {
                let msg = format!("Failed to find method {}", name.to_string_lossy());
                Err(AniError::from_code(msg, res))
            } else {
                Ok(AniMethod::from_raw(method))
            }
        }
    }

    pub fn find_method_with_signature(
        &self,
        class: &AniClass,
        name: &CStr,
        signature: &CStr,
    ) -> Result<AniMethod<'local>, AniError> {
        unsafe {
            let mut method = null_mut() as ani_method;
            let res = (**self.inner).Class_FindMethod.unwrap()(
                self.inner,
                class.as_raw(),
                name.as_ptr(),
                signature.as_ptr(),
                &mut method as *mut _,
            );
            if res != 0 {
                let msg = format!("Failed to find method {}", name.to_string_lossy());
                Err(AniError::from_code(msg, res))
            } else {
                Ok(AniMethod::from_raw(method))
            }
        }
    }

    pub fn find_static_method(
        &self,
        class: &AniClass,
        name: &CStr,
    ) -> Result<AniStaticMethod<'local>, AniError> {
        unsafe {
            let mut method = null_mut() as ani_static_method;
            let res = (**self.inner).Class_FindStaticMethod.unwrap()(
                self.inner,
                class.as_raw(),
                name.as_ptr(),
                null_mut(),
                &mut method as *mut _,
            );
            if res != 0 {
                let msg = format!("Failed to find static method {}", name.to_string_lossy());
                Err(AniError::from_code(msg, res))
            } else {
                Ok(AniStaticMethod::from_raw(method))
            }
        }
    }

    pub fn find_static_method_with_signature(
        &self,
        class: &AniClass,
        name: &CStr,
        signature: &CStr,
    ) -> Result<AniStaticMethod<'local>, AniError> {
        unsafe {
            let mut method = null_mut() as ani_static_method;
            let res = (**self.inner).Class_FindStaticMethod.unwrap()(
                self.inner,
                class.as_raw(),
                name.as_ptr(),
                signature.as_ptr(),
                &mut method as *mut _,
            );
            if res != 0 {
                let msg = format!("Failed to find static method {}", name.to_string_lossy());
                Err(AniError::from_code(msg, res))
            } else {
                Ok(AniStaticMethod::from_raw(method))
            }
        }
    }

    pub fn find_enum(&self, name: &CStr) -> Result<AniEnum<'local>, AniError> {
        let mut ani_enum = null_mut() as ani_enum;
        let res = unsafe {
            (**self.inner).FindEnum.unwrap()(self.inner, name.as_ptr(), &mut ani_enum as *mut _)
        };
        if res != 0 {
            let msg = format!("Failed to find enum {}", name.to_string_lossy());
            Err(AniError::from_code(msg, res))
        } else {
            Ok(AniEnum::from_raw(ani_enum))
        }
    }

    pub fn bind_namespace_functions<const N: usize>(
        &self,
        namespace: AniNamespace,
        functions: &[(&'static CStr, *const c_void); N],
    ) -> Result<(), AniError> {
        let ani_functions: [AniNativeFunction<'static>; N] =
            std::array::from_fn(|i| AniNativeFunction::new(functions[i].0, functions[i].1));
        unsafe {
            let res = (**self.inner).Namespace_BindNativeFunctions.unwrap()(
                self.inner,
                namespace.into_raw(),
                ani_functions.as_ptr() as _,
                ani_functions.len() as _,
            );
            if res != 0 {
                let msg = format!(
                    "Failed to bind functions {}",
                    functions
                        .iter()
                        .map(|(name, _)| name.to_string_lossy())
                        .collect::<Vec<_>>()
                        .join(", ")
                );
                Err(AniError::from_code(msg, res))
            } else {
                Ok(())
            }
        }
    }

    pub fn bind_class_methods<const N: usize>(
        &self,
        class: AniClass,
        methods: &[(&'static CStr, *const c_void); N],
    ) -> Result<(), AniError> {
        let ani_methods: [AniNativeFunction<'static>; N] =
            std::array::from_fn(|i| AniNativeFunction::new(methods[i].0, methods[i].1));
        unsafe {
            let res = (**self.inner).Class_BindNativeMethods.unwrap()(
                self.inner,
                class.into_raw(),
                ani_methods.as_ptr() as _,
                ani_methods.len() as _,
            );
            if res != 0 {
                let msg = format!(
                    "Failed to bind methods {}",
                    methods
                        .iter()
                        .map(|(name, _)| name.to_string_lossy())
                        .collect::<Vec<_>>()
                        .join(", ")
                );
                Err(AniError::from_code(msg, res))
            } else {
                Ok(())
            }
        }
    }

    pub fn new_object<T: Input>(
        &self,
        class: &AniClass,
        input: T,
    ) -> Result<AniObject<'local>, AniError> {
        unsafe {
            let method =
                self.find_method(class, CStr::from_bytes_with_nul_unchecked(b"<ctor>\0"))?;
            let mut obj = null_mut() as ani_object;

            let res = T::object_new(input, &self, class, &method, &mut obj as _);

            if res != 0 {
                let msg = String::from("Failed to create a new object");
                Err(AniError::from_code(msg, res))
            } else {
                Ok(AniObject::from_raw(obj))
            }
        }
    }

    pub fn new_object_with_signature<T: Input>(
        &self,
        class: &AniClass,
        signature: &CStr,
        input: T,
    ) -> Result<AniObject<'local>, AniError> {
        unsafe {
            let method = self.find_method_with_signature(
                class,
                CStr::from_bytes_with_nul_unchecked(b"<ctor>\0"),
                signature,
            )?;

            let mut obj = null_mut() as ani_object;
            let res = T::object_new(input, &self, class, &method, &mut obj as _);
            if res != 0 {
                let msg = String::from("Failed to create a new object with signature");
                Err(AniError::from_code(msg, res))
            } else {
                Ok(AniObject::from_raw(obj))
            }
        }
    }

    pub fn new_enum_item(
        &self,
        ani_enum: &AniEnum<'local>,
        name: &CStr,
    ) -> Result<AniEnumItem<'local>, AniError> {
        unsafe {
            let mut enum_item = null_mut() as ani_enum_item;
            let res = (**self.inner).Enum_GetEnumItemByName.unwrap()(
                self.inner,
                ani_enum.as_raw(),
                name.as_ptr(),
                &mut enum_item as _,
            );
            if res != 0 {
                let msg = format!(
                    "Failed to create a new enum item {}",
                    name.to_string_lossy()
                );
                Err(AniError::from_code(msg, res))
            } else {
                Ok(AniEnumItem::from_raw(enum_item))
            }
        }
    }

    pub fn new_array<T: AniExt>(&self, size: usize) -> Result<AniRef<'local>, AniError> {
        T::new_array(self, size)
    }

    pub fn null_array(&self) -> Result<AniRef<'local>, AniError> {
        self.new_array::<bool>(0)
    }

    pub fn array_length(&self, array: &AniRef) -> Result<usize, AniError> {
        let mut length = 0usize;
        let res = unsafe {
            (**self.inner).Array_GetLength.unwrap()(self.inner, array.as_raw(), &mut length as _)
        };
        if res != 0 {
            let msg = String::from("Failed to get array length");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(length)
        }
    }

    pub fn array_set<T: AniExt>(
        &self,
        array: &AniRef,
        index: usize,
        value: T,
    ) -> Result<(), AniError> {
        T::array_set(self, array, index, value)
    }

    pub fn array_get<T: AniExt>(&self, array: &AniRef, index: usize) -> Result<T, AniError> {
        T::array_get(self, array, index)
    }

    pub fn get_enum_item_index(&self, item: &AniEnumItem) -> Result<usize, AniError> {
        let mut index = 0usize;
        let res = unsafe {
            (**self.inner).EnumItem_GetIndex.unwrap()(self.inner, item.as_raw(), &mut index)
        };
        if res != 0 {
            let msg = format!("Failed to get index of enum item");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(index)
        }
    }

    pub fn get_field<T: AniExt>(&self, obj: &AniObject, name: &CStr) -> Result<T, AniError> {
        T::get_field(self, obj, name)
    }

    pub fn get_property<T: AniExt>(&self, obj: &AniObject, name: &CStr) -> Result<T, AniError> {
        T::get_property(self, obj, name)
    }

    pub fn set_field<T: AniExt>(
        &self,
        obj: &AniObject,
        name: &CStr,
        value: T,
    ) -> Result<(), AniError> {
        T::set_field(self, obj, name, value)
    }

    pub fn set_property<T: AniExt>(
        &self,
        obj: &AniObject,
        name: &CStr,
        value: T,
    ) -> Result<(), AniError> {
        T::set_property(self, obj, name, value)
    }

    pub fn call_method<T: Input>(
        &self,
        obj: &AniObject,
        method: &AniMethod,
        value: T,
    ) -> Result<(), AniError> {
        let res = T::call_method(value, &self, obj, method);
        if res != 0 {
            let msg = String::from("Failed to call method");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(())
        }
    }

    pub fn call_method_long<T: Input>(
        &self,
        obj: &AniObject,
        method: &AniMethod,
        value: T,
    ) -> Result<i64, AniError> {
        let mut result = 0;
        let res = T::call_method_long(value, &self, obj, method, &mut result as _);
        if res != 0 {
            let msg = String::from("Failed to call method long");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(result)
        }
    }

    pub fn call_method_ref<T: Input>(
        &self,
        obj: &AniObject,
        method: &AniMethod,
        value: T,
    ) -> Result<AniRef<'local>, AniError> {
        let mut result = null_mut() as ani_ref;
        let res = T::call_method_ref(value, &self, obj, method, &mut result as _);
        if res != 0 {
            let msg = String::from("Failed to call method");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(AniRef::from_raw(result))
        }
    }

    pub fn call_static_method_ref<T: Input>(
        &self,
        class: &AniClass,
        method: &AniStaticMethod,
        value: T,
    ) -> Result<AniRef<'local>, AniError> {
        let mut result = null_mut() as ani_ref;
        let res = T::call_static_method_ref(value, &self, class, method, &mut result as _);
        if res != 0 {
            let msg = String::from("Failed to call static method");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(AniRef::from_raw(result))
        }
    }

    pub fn is_undefined(&self, ani_ref: &AniRef<'local>) -> Result<bool, AniError> {
        let mut is_undefined = 0u8;
        let res = unsafe {
            (**self.inner).Reference_IsUndefined.unwrap()(
                self.inner,
                ani_ref.as_raw(),
                &mut is_undefined as *mut _,
            )
        };
        if res != 0 {
            let msg = String::from("Failed to check if object is undefined");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(is_undefined == 1)
        }
    }

    pub fn is_null(&self, ani_ref: &AniRef<'local>) -> Result<bool, AniError> {
        let mut is_null = 0u8;
        let res = unsafe {
            (**self.inner).Reference_IsNull.unwrap()(
                self.inner,
                ani_ref.as_raw(),
                &mut is_null as *mut _,
            )
        };
        if res != 0 {
            let msg = String::from("Failed to check if object is undefined");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(is_null == 1)
        }
    }

    pub fn undefined(&self) -> Result<AniRef<'local>, AniError> {
        let mut undefined = null_mut() as ani_ref;
        let res = unsafe { (**self.inner).GetUndefined.unwrap()(self.inner, &mut undefined as _) };
        if res != 0 {
            let msg = String::from("Failed to get undefined");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(AniRef::from_raw(undefined))
        }
    }

    pub fn null(&self) -> Result<AniRef<'local>, AniError> {
        let mut undefined = null_mut() as ani_ref;
        let res = unsafe { (**self.inner).GetNull.unwrap()(self.inner, &mut undefined as _) };
        if res != 0 {
            let msg = String::from("Failed to get null");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(AniRef::from_raw(undefined))
        }
    }

    pub fn function_object_call(
        &self,
        function: &AniFnObject,
        args: &[AniRef],
    ) -> Result<AniRef<'local>, AniError> {
        let mut result = null_mut() as ani_ref;
        let res = unsafe {
            (**self.inner).FunctionalObject_Call.unwrap()(
                self.inner,
                function.as_raw(),
                args.len(),
                args.as_ptr() as _,
                &mut result as *mut _,
            )
        };
        if res != 0 {
            let msg = String::from("Failed to call function object");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(AniRef::from_raw(result))
        }
    }

    pub fn reference_equals(
        &self,
        a: &AniRef<'local>,
        b: &AniRef<'local>,
    ) -> Result<bool, AniError> {
        let mut is_equal = 0u8;
        let res = unsafe {
            (**self.inner).Reference_Equals.unwrap()(
                self.inner,
                a.as_raw(),
                b.as_raw(),
                &mut is_equal as *mut _,
            )
        };
        if res != 0 {
            let msg = String::from("Failed to compare references");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(is_equal == 1)
        }
    }

    pub fn reference_strict_equals(
        &self,
        a: &AniRef<'local>,
        b: &AniRef<'local>,
    ) -> Result<bool, AniError> {
        let mut is_equal = 0u8;
        let res = unsafe {
            (**self.inner).Reference_StrictEquals.unwrap()(
                self.inner,
                a.as_raw(),
                b.as_raw(),
                &mut is_equal as *mut _,
            )
        };
        if res != 0 {
            let msg = String::from("Failed to compare references");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(is_equal == 1)
        }
    }

    pub fn new_error(&self, message: &str) -> Result<AniRef<'local>, AniError> {
        unsafe {
            let class =
                self.find_class(CStr::from_bytes_with_nul_unchecked(b"escompat.Error\0"))?;

            let mut method = null_mut() as ani_method;

            let res = (**self.inner).Class_FindMethod.unwrap()(
                self.inner,
                class.as_raw(),
                CStr::from_bytes_with_nul_unchecked(b"<ctor>\0").as_ptr(),
                CStr::from_bytes_with_nul_unchecked(
                    b"C{std.core.String}C{escompat.ErrorOptions}:\0",
                )
                .as_ptr(),
                &mut method as *mut _,
            );

            if res != 0 {
                let msg = String::from("Failed to find error constructor");
                return Err(AniError::from_code(msg, res));
            }

            let message = self.convert_std_string(message)?;
            let undefined = self.undefined()?;
            let mut ani_error = null_mut() as ani_error;
            let res = (**self.inner).Object_New.unwrap()(
                self.inner,
                class.as_raw(),
                method,
                &mut ani_error as _,
                message.as_raw(),
                undefined.as_raw(),
            );
            if res != 0 {
                let msg = String::from("Failed to create error object");
                return Err(AniError::from_code(msg, res));
            }
            Ok(AniRef::from_raw(ani_error))
        }
    }

    pub fn throw_error(&self, message: &str) -> Result<(), AniError> {
        unsafe {
            let ani_error = self.new_error(message)?;
            let res = (**self.inner).ThrowError.unwrap()(self.inner, ani_error.as_raw());
            if res != 0 {
                let msg = String::from("Failed to throw error");
                return Err(AniError::from_code(msg, res));
            }
        }
        Ok(())
    }

    pub fn business_error(&self, code: i32, message: &str) -> Result<AniRef<'local>, AniError> {
        unsafe {
            let class = self.find_class(CStr::from_bytes_with_nul_unchecked(
                b"@ohos.base.BusinessError\0",
            ))?;
            let method = self.find_method_with_signature(
                &class,
                CStr::from_bytes_with_nul_unchecked(b"<ctor>\0"),
                CStr::from_bytes_with_nul_unchecked(b"iC{escompat.Error}:\0"),
            )?;
            let error = self.new_error(message)?;
            let mut business_error = null_mut() as ani_error;

            let res = (**self.inner).Object_New.unwrap()(
                self.inner,
                class.as_raw(),
                method.as_raw(),
                &mut business_error as _,
                code,
                error,
            );

            if res != 0 {
                let msg = String::from("Failed to create error object");
                return Err(AniError::from_code(msg, res));
            }
            Ok(AniRef::from_raw(business_error))
        }
    }

    pub fn throw_business_error(&self, code: i32, message: &str) -> Result<(), AniError> {
        unsafe {
            let business_error = self.business_error(code, message)?;
            let res = (**self.inner).ThrowError.unwrap()(self.inner, business_error.as_raw());
            if res != 0 {
                let msg = String::from("Failed to throw error");
                return Err(AniError::from_code(msg, res));
            }
        }
        Ok(())
    }

    pub fn instance_of(&self, obj: &AniObject, class: &AniClass) -> Result<bool, AniError> {
        let mut is_instance = 0u8;
        let res = unsafe {
            (**self.inner).Object_InstanceOf.unwrap()(
                self.inner,
                obj.as_raw(),
                class.as_raw(),
                &mut is_instance as *mut _,
            )
        };
        if res != 0 {
            let msg = String::from("Failed to check instance of");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(is_instance == 1)
        }
    }

    pub fn inbox<T: AniExt>(&self, value: T) -> Result<AniRef<'local>, AniError> {
        T::inbox(self, value)
    }

    pub fn unbox<T: AniExt>(&self, ani_object: &AniObject) -> Result<T, AniError> {
        T::unbox(self, &ani_object)
    }

    pub fn convert_ani_string(&self, ani_string: &AniString<'local>) -> Result<String, AniError> {
        unsafe {
            let mut size = 0usize;
            let res = (**self.inner).String_GetUTF8Size.unwrap()(
                self.inner,
                ani_string.as_raw(),
                &mut size as *mut _,
            );

            if res != 0 {
                let msg = String::from("Failed to get string size");
                return Err(AniError::from_code(msg, res));
            }

            let mut buffer = vec![0u8; size + 1];
            let mut buffer_write = 0usize;
            let res = (**self.inner).String_GetUTF8.unwrap()(
                self.inner,
                ani_string.as_raw(),
                buffer.as_mut_ptr() as *mut _,
                size + 1,
                &mut buffer_write as *mut _,
            );
            if res != 0 {
                let msg = String::from("Failed to get string");
                Err(AniError::from_code(msg, res))
            } else {
                buffer.pop();
                Ok(String::from_utf8(buffer).unwrap())
            }
        }
    }

    pub fn convert_std_string(&self, value: &str) -> Result<AniString<'local>, AniError> {
        let mut ani_s = null_mut() as ani_string;
        let res = unsafe {
            (**self.inner).String_NewUTF8.unwrap()(
                self.inner,
                value.as_bytes().as_ptr() as _,
                value.len(),
                &mut ani_s as *mut _,
            )
        };
        if res != 0 {
            let msg = format!("Failed to create string {}", value);
            Err(AniError::from_code(msg, res))
        } else {
            Ok(AniString::from_raw(ani_s))
        }
    }

    pub fn new_record(&self) -> Result<AniObject<'local>, AniError> {
        let class = self.find_class(signature::RECORD)?;
        self.new_object(&class, ())
    }

    pub fn get_record<T: AniExt>(
        &self,
        record: &AniObject<'local>,
        key: AniRef<'local>,
    ) -> Result<T, AniError> {
        let class = self.find_class(signature::RECORD)?;
        let method = self.find_method(&class, signature::GET)?;
        let ani_ref = self.call_method_ref(record, &method, (key.as_raw(),))?;
        self.unbox::<T>(&ani_ref.into())
    }

    pub fn set_record(
        &self,
        record: &AniObject<'local>,
        key: &AniRef<'local>,
        value: &AniRef<'local>,
    ) -> Result<(), AniError> {
        let class = self.find_class(signature::RECORD)?;
        let method = self.find_method(&class, signature::SET)?;
        self.call_method(record, &method, (key.as_raw(), value.as_raw()))
    }

    pub fn record_entries(&self, record: &AniObject<'local>) -> Result<AniIter<'local>, AniError> {
        let class = self.find_class(signature::MAP)?;
        let method = self.find_method(&class, ENTRIES)?;
        let iter = self.call_method_ref(record, &method, ())?;
        Ok(AniIter::new(self, iter.into()))
    }

    pub fn strict_eq(&self, a: &AniRef<'local>, b: &AniRef<'local>) -> Result<bool, AniError> {
        let mut is_eq = 0u8;
        let res = unsafe {
            (**self.inner).Reference_StrictEquals.unwrap()(
                self.inner,
                a.as_raw(),
                b.as_raw(),
                &mut is_eq as *mut _,
            )
        };
        if res != 0 {
            let msg = String::from("Failed to compare references");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(is_eq == 1)
        }
    }

    pub fn array_buffer(&self, array: &AniRef<'local>) -> Result<&'local [u8], AniError> {
        let mut ptr = null_mut() as *mut c_void;
        let mut size = 0usize;
        let res = unsafe {
            (**self.inner).ArrayBuffer_GetInfo.unwrap()(
                self.inner,
                array.as_raw(),
                &mut ptr as _,
                &mut size as *mut _,
            )
        };

        if res != 0 {
            let msg = String::from("Failed to get array buffer");
            Err(AniError::from_code(msg, res))
        } else {
            unsafe { Ok(std::slice::from_raw_parts(ptr as *const u8, size)) }
        }
    }

    pub fn create_array_buffer(
        &self,
        length: usize,
    ) -> Result<(AniRef<'local>, *mut u8), AniError> {
        let mut ani_ref = null_mut() as ani_ref;
        let mut data = null_mut() as *mut u8;
        let res = unsafe {
            (**self.inner).CreateArrayBuffer.unwrap()(
                self.inner,
                length,
                &mut data as *mut _ as _,
                &mut ani_ref as *mut _,
            )
        };

        if res != 0 {
            let msg = String::from("Failed to create array buffer");
            Err(AniError::from_code(msg, res))
        } else {
            Ok((AniRef::from_raw(ani_ref), data))
        }
    }

    pub fn get_tuple_value_ref(
        &self,
        tuple: &AniObject<'local>,
        index: usize,
    ) -> Result<AniRef<'local>, AniError> {
        let mut ret = null_mut() as ani_ref;
        let res = unsafe {
            (**self.inner).TupleValue_GetItem_Ref.unwrap()(
                self.inner,
                tuple.as_raw(),
                index,
                &mut ret as *mut _,
            )
        };
        if res != 0 {
            let msg = String::from("Failed to get tuple value");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(AniRef::from_raw(ret))
        }
    }

    pub fn create_global_ref(&self, local: AniRef<'local>) -> Result<AniRef<'static>, AniError> {
        let mut ani_ref = null_mut() as ani_ref;
        let res = unsafe {
            (**self.inner).GlobalReference_Create.unwrap()(
                self.inner,
                local.as_raw(),
                &mut ani_ref as *mut _,
            )
        };
        if res != 0 {
            let msg = String::from("Failed to create global ref");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(AniRef::from_raw(ani_ref))
        }
    }

    pub fn delete_global_ref(&self, global: AniRef<'static>) -> Result<(), AniError> {
        let res =
            unsafe { (**self.inner).GlobalReference_Delete.unwrap()(self.inner, global.as_raw()) };
        if res != 0 {
            let msg = String::from("Failed to delete global ref");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(())
        }
    }

    pub fn exist_unhandled_error(&self) -> Result<bool, AniError> {
        let mut has_unhandled_error = 0u8;

        let res = unsafe {
            (**self.inner).ExistUnhandledError.unwrap()(
                self.inner,
                &mut has_unhandled_error as *mut _,
            )
        };
        if res != 0 {
            let msg = String::from("Failed to invoke ExistUnhandledError");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(has_unhandled_error == 1)
        }
    }

    pub fn describe_error(&self) -> Result<(), AniError> {
        let res = unsafe {
            (**self.inner).DescribeError.unwrap()(self.inner)
        };
        if res != 0 {
            let msg = String::from("Failed to invoke DescribeError");
            Err(AniError::from_code(msg, res))
        } else {
            Ok(())
        }
    }
}

pub trait Input {
    fn call_method(self, env: &AniEnv, obj: &AniObject, method: &AniMethod) -> u32;

    fn call_method_long(
        self,
        env: &AniEnv,
        obj: &AniObject,
        method: &AniMethod,
        result: *mut ani_long,
    ) -> u32;

    fn call_method_ref(
        self,
        env: &AniEnv,
        obj: &AniObject,
        method: &AniMethod,
        result: *mut ani_ref,
    ) -> u32;

    fn object_new(
        self,
        env: &AniEnv,
        class: &AniClass,
        method: &AniMethod,
        result: *mut ani_object,
    ) -> u32;

    fn call_static_method_ref(
        self,
        env: &AniEnv,
        class: &AniClass,
        method: &AniStaticMethod,
        result: *mut ani_ref,
    ) -> u32;
}

macro_rules! single_tuple_impl {
    ($(($field:tt $ftype:ident)),* ) => {
        impl <$($ftype,) *> Input for ($($ftype,)*) {
            fn call_method(self, env: &AniEnv, obj: &AniObject, method: &AniMethod)
        -> u32{
                unsafe {
                    (**env.inner).Object_CallMethod_Void.unwrap()(
                        env.inner,
                        obj.as_raw(),
                        method.as_raw(),
                        $(self.$field,)*
                    )
                }
            }

            fn call_method_long(self, env: &AniEnv, obj: &AniObject, method: &AniMethod, result: *mut ani_long)
        -> u32{
                unsafe {
                    (**env.inner).Object_CallMethod_Long.unwrap()(
                        env.inner,
                        obj.as_raw(),
                        method.as_raw(),
                        result,
                        $(self.$field,)*
                    )
                }
            }

            fn call_method_ref(self, env: &AniEnv, obj: &AniObject, method: &AniMethod,result: *mut ani_ref,)
        -> u32{
                unsafe {
                    (**env.inner).Object_CallMethod_Ref.unwrap()(
                        env.inner,
                        obj.as_raw(),
                        method.as_raw(),
                        result,
                        $(self.$field,)*
                    )
                }
            }

            fn object_new(
                self,
                env: &AniEnv,
                class: &AniClass,
                method: &AniMethod,
                result: *mut ani_object,
            ) -> u32{
                unsafe {
                    (**env.inner).Object_New.unwrap()(
                        env.inner,
                        class.as_raw(),
                        method.as_raw(),
                        result,
                        $(self.$field,)*
                    )
                }
            }

            fn call_static_method_ref(
                self,
                env: &AniEnv,
                class: &AniClass,
                method: &AniStaticMethod,
                result: *mut ani_ref,
            ) -> u32 {
                unsafe {
                    (**env.inner).Class_CallStaticMethod_Ref.unwrap()(
                        env.inner,
                        class.as_raw(),
                        method.as_raw(),
                        result,
                        $(self.$field,)*
                    )
                }
            }
        }
    };
}

single_tuple_impl!();
single_tuple_impl!((0 A));
single_tuple_impl!((0 A), (1 B));
single_tuple_impl!((0 A), (1 B), (2 C));
single_tuple_impl!((0 A), (1 B), (2 C), (3 D));
single_tuple_impl!((0 A), (1 B), (2 C), (3 D), (4 E));

pub trait AniExt: Sized {
    fn get_field<'local>(
        env: &AniEnv<'local>,
        obj: &AniObject<'local>,
        name: &CStr,
    ) -> Result<Self, AniError>;

    fn get_property<'local>(
        env: &AniEnv<'local>,
        obj: &AniObject<'local>,
        name: &CStr,
    ) -> Result<Self, AniError>;

    fn set_field<'local>(
        env: &AniEnv<'local>,
        obj: &AniObject<'local>,
        name: &CStr,
        value: Self,
    ) -> Result<(), AniError>;

    fn set_property<'local>(
        env: &AniEnv<'local>,
        obj: &AniObject<'local>,
        name: &CStr,
        value: Self,
    ) -> Result<(), AniError>;

    fn new_array<'local>(env: &AniEnv<'local>, size: usize) -> Result<AniRef<'local>, AniError>;

    fn array_set<'local>(
        env: &AniEnv<'local>,
        array: &AniRef<'local>,
        index: usize,
        value: Self,
    ) -> Result<(), AniError>;

    fn array_get<'local>(
        env: &AniEnv<'local>,
        array: &AniRef<'local>,
        index: usize,
    ) -> Result<Self, AniError>;

    fn unbox<'local>(env: &AniEnv<'local>, ani_ref: &AniRef<'local>) -> Result<Self, AniError>;

    fn inbox<'local>(env: &AniEnv<'local>, value: Self) -> Result<AniRef<'local>, AniError>;
}
