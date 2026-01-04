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

use crate::{
    ani_rs_error,
    business_error::BusinessError,
    error::AniError,
    global::GlobalRef,
    objects::{AniObject, AniRef},
    wrapper::RustClosure,
    AniEnv, AniVm,
};
use ani_sys::{ani_fn_object, ani_object};
use serde::{Deserialize, Serialize};
use std::{
    ops::Deref,
    sync::{Arc, Once},
};
use ylong_runtime::builder::RuntimeBuilder;

static INIT_RUNTIME: Once = Once::new();
const MAX_BLOCKING_POOL_SIZE: u8 = 1;

#[repr(transparent)]
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct AniFnObject<'local>(AniObject<'local>);

#[repr(transparent)]
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct AniAsyncCallback<'local>(AniObject<'local>);

#[repr(transparent)]
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct AniErrorCallback<'local>(AniObject<'local>);

impl<'local> AsRef<AniFnObject<'local>> for AniFnObject<'local> {
    fn as_ref(&self) -> &AniFnObject<'local> {
        &self
    }
}

impl<'local> AsRef<AniObject<'local>> for AniAsyncCallback<'local> {
    fn as_ref(&self) -> &AniObject<'local> {
        &self.0
    }
}

impl<'local> AsRef<AniObject<'local>> for AniErrorCallback<'local> {
    fn as_ref(&self) -> &AniObject<'local> {
        &self.0
    }
}

impl<'local> Deref for AniFnObject<'local> {
    type Target = AniObject<'local>;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl<'local> Deref for AniAsyncCallback<'local> {
    type Target = AniObject<'local>;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl<'local> Deref for AniErrorCallback<'local> {
    type Target = AniObject<'local>;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl<'local> From<AniFnObject<'local>> for AniObject<'local> {
    fn from(value: AniFnObject<'local>) -> Self {
        value.0
    }
}

impl<'local> From<AniAsyncCallback<'local>> for AniObject<'local> {
    fn from(value: AniAsyncCallback<'local>) -> Self {
        value.0
    }
}

impl<'local> From<AniErrorCallback<'local>> for AniObject<'local> {
    fn from(value: AniErrorCallback<'local>) -> Self {
        value.0
    }
}

impl<'local> From<AniFnObject<'local>> for AniRef<'local> {
    fn from(value: AniFnObject<'local>) -> Self {
        value.0.into()
    }
}

impl<'local> From<AniAsyncCallback<'local>> for AniRef<'local> {
    fn from(value: AniAsyncCallback<'local>) -> Self {
        value.0.into()
    }
}

impl<'local> From<AniErrorCallback<'local>> for AniRef<'local> {
    fn from(value: AniErrorCallback<'local>) -> Self {
        value.0.into()
    }
}

impl<'local> From<AniRef<'local>> for AniFnObject<'local> {
    fn from(value: AniRef<'local>) -> Self {
        Self::from_raw(value.as_raw() as ani_fn_object)
    }
}

impl<'local> From<AniRef<'local>> for AniAsyncCallback<'local> {
    fn from(value: AniRef<'local>) -> Self {
        Self::from_raw(value.as_raw() as ani_object)
    }
}

impl<'local> From<AniRef<'local>> for AniErrorCallback<'local> {
    fn from(value: AniRef<'local>) -> Self {
        Self::from_raw(value.as_raw() as ani_object)
    }
}

impl<'local> From<AniObject<'local>> for AniFnObject<'local> {
    fn from(value: AniObject<'local>) -> Self {
        Self::from_raw(value.into_raw())
    }
}

impl<'local> From<AniObject<'local>> for AniAsyncCallback<'local> {
    fn from(value: AniObject<'local>) -> Self {
        Self::from_raw(value.into_raw())
    }
}

impl<'local> From<AniObject<'local>> for AniErrorCallback<'local> {
    fn from(value: AniObject<'local>) -> Self {
        Self::from_raw(value.into_raw())
    }
}

impl<'de> Deserialize<'de> for AniFnObject<'_> {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: serde::Deserializer<'de>,
    {
        let obj = AniObject::deserialize(deserializer)?;
        Ok(AniFnObject::from(obj))
    }
}

impl<'de> Deserialize<'de> for AniAsyncCallback<'_> {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: serde::Deserializer<'de>,
    {
        let obj = AniObject::deserialize(deserializer)?;
        Ok(AniAsyncCallback::from(obj))
    }
}

impl<'de> Deserialize<'de> for AniErrorCallback<'_> {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: serde::Deserializer<'de>,
    {
        let obj = AniObject::deserialize(deserializer)?;
        Ok(AniErrorCallback::from(obj))
    }
}

impl<'local> AniFnObject<'local> {
    pub fn from_raw(ptr: ani_fn_object) -> Self {
        Self(AniObject::from_raw(ptr as ani_object))
    }

    pub fn as_raw(&self) -> ani_fn_object {
        self.0.as_raw() as _
    }

    pub fn into_raw(self) -> ani_fn_object {
        self.0.into_raw() as _
    }

    pub fn into_global(self, env: &AniEnv) -> Result<GlobalRef<AniFnObject<'static>>, AniError> {
        let global = env.create_global_ref(self.into())?;
        let fn_object = AniFnObject::from_raw(global.as_raw() as ani_fn_object);
        Ok(GlobalRef(fn_object))
    }
}

impl<'local> AniAsyncCallback<'local> {
    pub fn from_raw(ptr: ani_object) -> Self {
        Self(AniObject::from_raw(ptr))
    }

    pub fn as_raw(&self) -> ani_object {
        self.0.as_raw()
    }

    pub fn into_raw(self) -> ani_object {
        self.0.into_raw()
    }

    pub fn into_global(
        self,
        env: &AniEnv,
    ) -> Result<GlobalRef<AniAsyncCallback<'static>>, AniError> {
        let global = env.create_global_ref(self.into())?;
        let async_callback = AniAsyncCallback::from_raw(global.as_raw());
        Ok(GlobalRef(async_callback))
    }
}

impl<'local> AniErrorCallback<'local> {
    pub fn from_raw(ptr: ani_object) -> Self {
        Self(AniObject::from_raw(ptr))
    }

    pub fn as_raw(&self) -> ani_object {
        self.0.as_raw()
    }

    pub fn into_raw(self) -> ani_object {
        self.0.into_raw()
    }

    pub fn into_global(
        self,
        env: &AniEnv,
    ) -> Result<GlobalRef<AniErrorCallback<'static>>, AniError> {
        let global = env.create_global_ref(self.into())?;
        let error_callback = AniErrorCallback::from_raw(global.as_raw());
        Ok(GlobalRef(error_callback))
    }
}

impl<'local> AniFnObject<'local> {
    pub fn execute_local<T>(&self, env: &AniEnv<'local>, input: T) -> Result<AniRef, AniError>
    where
        T: InputVec,
    {
        let input = input.input(&env);
        env.function_object_call(&self, &input)
    }

    pub fn execute_current<T>(&self, input: T) -> Result<AniRef, AniError>
    where
        T: InputVec,
    {
        if let Ok(env) = AniVm::get_instance().get_env() {
            self.execute_local(&env, input)
        } else {
            let env = AniVm::get_instance().attach_current_thread()?;
            let res = self.execute_local(&env, input);
            AniVm::get_instance().detach_current_thread()?;
            res
        }
    }

    pub fn into_global_callback<T: InputVec + Send + 'static>(
        self,
        env: &AniEnv<'local>,
    ) -> Result<GlobalRefCallback<T>, AniError> {
        let global_ref = self.into_global(env)?;
        Ok(GlobalRefCallback {
            inner: Arc::new(global_ref),
            phantom: std::marker::PhantomData::<T>,
        })
    }
}

impl<'local> AniAsyncCallback<'local> {
    pub fn execute_local<T>(
        &self,
        env: &AniEnv<'local>,
        business_error: Option<BusinessError>,
        input: T,
    ) -> Result<AniRef, AniError>
    where
        T: InputVec,
    {
        let business_error = if let Some(err) = business_error {
            env.business_error(err.code(), err.message())?
        } else {
            env.business_error(0, "Ok")?
        };
        let mut v = vec![business_error];
        v.append(&mut input.input(env));
        let f = AniFnObject::from(self.0.clone());
        env.function_object_call(&f, &v)
    }

    pub fn execute_current<T>(
        &self,
        business_error: Option<BusinessError>,
        input: T,
    ) -> Result<AniRef, AniError>
    where
        T: InputVec,
    {
        if let Ok(env) = AniVm::get_instance().get_env() {
            self.execute_local(&env, business_error, input)
        } else {
            let env = AniVm::get_instance().attach_current_thread()?;
            let res = self.execute_local(&env, business_error, input);
            AniVm::get_instance().detach_current_thread()?;
            res
        }
    }

    pub fn into_global_callback<T: InputVec + Send + 'static>(
        self,
        env: &AniEnv<'local>,
    ) -> Result<GlobalRefAsyncCallback<T>, AniError> {
        let global_ref = self.into_global(env)?;
        Ok(GlobalRefAsyncCallback {
            inner: Arc::new(global_ref),
            phantom: std::marker::PhantomData::<T>,
        })
    }
}

impl<'local> AniErrorCallback<'local> {
    pub fn execute_local(
        &self,
        env: &AniEnv<'local>,
        business_error: BusinessError,
    ) -> Result<AniRef, AniError> {
        let business_error_ref =
            env.business_error(business_error.code(), business_error.message())?;
        let v = vec![business_error_ref];
        let f = AniFnObject::from(self.0.clone());
        env.function_object_call(&f, &v)
    }

    pub fn execute_current(&self, business_error: BusinessError) -> Result<AniRef, AniError> {
        if let Ok(env) = AniVm::get_instance().get_env() {
            self.execute_local(&env, business_error)
        } else {
            let env = AniVm::get_instance().attach_current_thread()?;
            let res = self.execute_local(&env, business_error);
            AniVm::get_instance().detach_current_thread()?;
            res
        }
    }

    pub fn into_global_callback(
        self,
        env: &AniEnv<'local>,
    ) -> Result<GlobalRefErrorCallback, AniError> {
        let global_ref = self.into_global(env)?;
        Ok(GlobalRefErrorCallback {
            inner: Arc::new(global_ref),
        })
    }
}

impl GlobalRef<AniFnObject<'static>> {
    pub fn execute_global<T>(self: &Arc<Self>, input: T)
    where
        Self: 'static,
        T: InputVec + Send + 'static,
    {
        let me = self.clone();
        let _ = RustClosure::new(move || {
            if let Ok(env) = AniVm::get_instance().get_env() {
                let _ = me.0.execute_local(&env, input).map_err(|err| {
                    ani_rs_error!("Failed to execute Arkts Callback, error = {}", err);
                    env.exist_unhandled_error().map(|is_unhandled| {
                        if is_unhandled {
                            let _ = env.describe_error();
                        }
                    })
                });
            } else {
                ani_rs_error!("Failed to get_env in thread");
            }
        })
        .send_event("callback execute global");
    }

    pub fn execute_global_spawn_thread<T>(self: &Arc<Self>, input: T)
    where
        Self: 'static,
        T: InputVec + Send + 'static,
    {
        let me = self.clone();
        INIT_RUNTIME.call_once(|| {
            RuntimeBuilder::new_multi_thread()
                .max_blocking_pool_size(MAX_BLOCKING_POOL_SIZE)
                .worker_num(1)
                .build_global()
                .unwrap_or_else(|_| {
                    ani_rs_error!("Failed to init runtime");
                });
        });
        ylong_runtime::spawn_blocking(move || {
            if let Ok(env) = AniVm::get_instance().get_env() {
                me.0.execute_local(&env, input).unwrap();
            } else {
                match AniVm::get_instance().attach_current_thread() {
                    Ok(env) => {
                        me.0.execute_local(&env, input).unwrap();
                        AniVm::get_instance().detach_current_thread().unwrap();
                    }
                    Err(err) => {
                        ani_rs_error!("Failed to attach_current_thread in thread, err = {:?}", err);
                    }
                }
            }
        });
    }
}

impl GlobalRef<AniAsyncCallback<'static>> {
    pub fn execute_global<T>(self: &Arc<Self>, business_error: Option<BusinessError>, input: T)
    where
        Self: 'static,
        T: InputVec + Send + 'static,
    {
        let me = self.clone();
        let _ = RustClosure::new(move || {
            if let Ok(env) = AniVm::get_instance().get_env() {
                let _ =
                    me.0.execute_local(&env, business_error, input)
                        .map_err(|err| {
                            ani_rs_error!("Failed to execute arkts AsyncCallback, error = {}", err);
                            env.exist_unhandled_error().map(|is_unhandled| {
                                if is_unhandled {
                                    let _ = env.describe_error();
                                }
                            })
                        });
            } else {
                ani_rs_error!("Failed to get_env in thread");
            }
        })
        .send_event("async callback execute global");
    }

    pub fn execute_global_spawn_thread<T>(
        self: &Arc<Self>,
        business_error: Option<BusinessError>,
        input: T,
    ) where
        Self: 'static,
        T: InputVec + Send + 'static,
    {
        let me = self.clone();
        INIT_RUNTIME.call_once(|| {
            RuntimeBuilder::new_multi_thread()
                .max_blocking_pool_size(MAX_BLOCKING_POOL_SIZE)
                .worker_num(1)
                .build_global()
                .unwrap_or_else(|_| {
                    ani_rs_error!("Failed to init runtime");
                });
        });
        ylong_runtime::spawn_blocking(move || {
            if let Ok(env) = AniVm::get_instance().get_env() {
                me.0.execute_local(&env, business_error, input).unwrap();
            } else {
                match AniVm::get_instance().attach_current_thread() {
                    Ok(env) => {
                        me.0.execute_local(&env, business_error, input).unwrap();
                        AniVm::get_instance().detach_current_thread().unwrap();
                    }
                    Err(err) => {
                        ani_rs_error!("Failed to attach_current_thread in thread, err = {:?}", err);
                    }
                }
            }
        });
    }
}

impl GlobalRef<AniErrorCallback<'static>> {
    pub fn execute_global(self: &Arc<Self>, business_error: BusinessError)
    where
        Self: 'static,
    {
        let me = self.clone();
        let _ = RustClosure::new(move || {
            if let Ok(env) = AniVm::get_instance().get_env() {
                let _ = me.0.execute_local(&env, business_error).map_err(|err| {
                    ani_rs_error!("Failed to execute Arkts ErrorCallback, error = {}", err);
                    env.exist_unhandled_error().map(|is_unhandled| {
                        if is_unhandled {
                            let _ = env.describe_error();
                        }
                    })
                });
            } else {
                ani_rs_error!("Failed to get_env in thread");
            }
        })
        .send_event("Error callback execute global");
    }

    pub fn execute_global_spawn_thread(self: &Arc<Self>, business_error: BusinessError)
    where
        Self: 'static,
    {
        let me = self.clone();
        INIT_RUNTIME.call_once(|| {
            RuntimeBuilder::new_multi_thread()
                .max_blocking_pool_size(MAX_BLOCKING_POOL_SIZE)
                .worker_num(1)
                .build_global()
                .unwrap_or_else(|_| {
                    ani_rs_error!("Failed to init runtime");
                });
        });
        ylong_runtime::spawn_blocking(move || {
            if let Ok(env) = AniVm::get_instance().get_env() {
                me.0.execute_local(&env, business_error).unwrap();
            } else {
                match AniVm::get_instance().attach_current_thread() {
                    Ok(env) => {
                        me.0.execute_local(&env, business_error).unwrap();
                        AniVm::get_instance().detach_current_thread().unwrap();
                    }
                    Err(err) => {
                        ani_rs_error!("Failed to attach_current_thread in thread, err = {:?}", err);
                    }
                }
            }
        });
    }
}

#[derive(Clone)]
pub struct GlobalRefCallback<T: InputVec + Send + 'static> {
    inner: Arc<GlobalRef<AniFnObject<'static>>>,
    phantom: std::marker::PhantomData<T>,
}

#[derive(Clone)]
pub struct GlobalRefAsyncCallback<T: InputVec + Send + 'static> {
    inner: Arc<GlobalRef<AniAsyncCallback<'static>>>,
    phantom: std::marker::PhantomData<T>,
}

#[derive(Clone)]
pub struct GlobalRefErrorCallback {
    inner: Arc<GlobalRef<AniErrorCallback<'static>>>,
}

impl<T: InputVec + Send + 'static> PartialEq for GlobalRefCallback<T> {
    fn eq(&self, other: &Self) -> bool {
        self.inner == other.inner
    }
}

impl<T: InputVec + Send + 'static> PartialEq for GlobalRefAsyncCallback<T> {
    fn eq(&self, other: &Self) -> bool {
        self.inner == other.inner
    }
}

impl PartialEq for GlobalRefErrorCallback {
    fn eq(&self, other: &Self) -> bool {
        self.inner == other.inner
    }
}

impl<T: InputVec + Send + 'static> Eq for GlobalRefAsyncCallback<T> {}

impl<T: InputVec + Send + 'static> GlobalRefAsyncCallback<T> {
    pub fn execute(&self, business_error: Option<BusinessError>, input: T) {
        self.inner.execute_global(business_error, input);
    }

    pub fn execute_spawn_thread(&self, business_error: Option<BusinessError>, input: T) {
        self.inner
            .execute_global_spawn_thread(business_error, input);
    }
}

impl<T: InputVec + Send + 'static> Eq for GlobalRefCallback<T> {}

impl<T: InputVec + Send + 'static> GlobalRefCallback<T> {
    pub fn execute(&self, input: T) {
        self.inner.execute_global(input);
    }

    pub fn execute_spawn_thread(&self, input: T) {
        self.inner.execute_global_spawn_thread(input);
    }
}

impl Eq for GlobalRefErrorCallback {}

impl GlobalRefErrorCallback {
    pub fn execute(&self, business_error: BusinessError) {
        self.inner.execute_global(business_error);
    }

    pub fn execute_spawn_thread(&self, business_error: BusinessError) {
        self.inner.execute_global_spawn_thread(business_error);
    }
}

pub trait InputVec {
    fn input<'local>(&self, env: &AniEnv<'local>) -> Vec<AniRef<'local>>;
}

impl InputVec for () {
    fn input<'local>(&self, _env: &AniEnv<'local>) -> Vec<AniRef<'local>> {
        vec![]
    }
}

macro_rules! single_tuple_impl {
    ( $flen:tt $(($field:tt $ftype:ident)),*) => {
        impl<$($ftype),*> InputVec for ($($ftype,)*)
        where $($ftype: Serialize), *
        {
            fn input<'local>(&self, env: &AniEnv<'local>) -> Vec<AniRef<'local>> {
                vec![
                    $(env.serialize(&self.$field).unwrap(),)*
                ]
            }
        }

    };
}

single_tuple_impl!(1 (0 A));
single_tuple_impl!(2 (0 A), (1 B));
single_tuple_impl!(3 (0 A), (1 B), (2 C));
single_tuple_impl!(4 (0 A), (1 B), (2 C), (3 D));
single_tuple_impl!(5 (0 A), (1 B), (2 C), (3 D), (4 E));
single_tuple_impl!(6 (0 A), (1 B), (2 C), (3 D), (4 E), (5 F));
