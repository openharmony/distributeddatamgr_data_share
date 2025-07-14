// Copyright (c) 2023 Huawei Device Co., Ltd.
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

use ani_rs::{
    objects::{AniObject, AniRef},
    AniEnv,
};

use crate::{get_native_ptr, wrapper};

pub fn go_to_first_row<'local>(env: AniEnv<'local>, ani_this: AniRef<'local>) -> bool {
    let result_set_ptr = get_native_ptr(&env, &ani_this.into());
    wrapper::ffi::GoToFirstRow(result_set_ptr)
}

pub fn go_to_last_row<'local>(env: AniEnv<'local>, ani_this: AniRef<'local>) -> bool {
    let result_set_ptr = get_native_ptr(&env, &ani_this.into());
    wrapper::ffi::GoToLastRow(result_set_ptr)
}

pub fn go_to_next_row<'local>(env: AniEnv<'local>, ani_this: AniRef<'local>) -> bool {
    let result_set_ptr = get_native_ptr(&env, &ani_this.into());
    wrapper::ffi::GoToNextRow(result_set_ptr)
}

pub fn get_string<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    column_index: i32,
) -> AniRef<'local> {
    let result_set_ptr = get_native_ptr(&env, &ani_this.into());

    let s = wrapper::ffi::GetString(result_set_ptr, column_index);
    env.serialize(&s).unwrap()
}

pub fn get_long<'local>(env: AniEnv<'local>, ani_this: AniRef<'local>, column_index: i32) -> i64 {
    let result_set_ptr = get_native_ptr(&env, &ani_this.into());
    wrapper::ffi::GetLong(result_set_ptr, column_index)
}

pub fn get_column_index<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    column_name: AniObject<'local>,
) -> i32 {
    let result_set_ptr = get_native_ptr(&env, &ani_this.into());
    let s: String = env.deserialize(column_name).unwrap();
    wrapper::ffi::GetColumnIndex(result_set_ptr, s)
}

pub fn close<'local>(env: AniEnv<'local>, ani_this: AniRef<'local>) {
    let result_set_ptr = get_native_ptr(&env, &ani_this.into());
    wrapper::ffi::Close(result_set_ptr);
}
