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
    objects::{AniClass, AniObject, AniRef},
    AniEnv,
};

use crate::{get_native_ptr, wrapper};

#[derive(serde::Serialize, serde::Deserialize, Debug)]
pub enum ValueType {
    S(String),
    F64(f64),
    Boolean(bool),
}

pub fn create<'local>(_env: AniEnv<'local>, _clazz: AniClass<'local>) -> i64 {
    wrapper::ffi::DataSharePredicatesNew()
}

pub fn native_equal_to<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
    value: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();
    let value_rust: ValueType = env.deserialize(value).unwrap();

    wrapper::ffi::DataSharePredicatesEqualTo(predicates_ptr, filed_rust, &value_rust);

    ani_this
}

pub fn native_not_equal_to<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
    value: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();
    let value_rust: ValueType = env.deserialize(value).unwrap();

    wrapper::ffi::DataSharePredicatesNotEqualTo(predicates_ptr, filed_rust, &value_rust);

    ani_this
}

pub fn native_begin_wrap<'local>(env: AniEnv<'local>, ani_this: AniRef<'local>) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesBeginWrap(predicates_ptr);

    ani_this
}

pub fn native_end_wrap<'local>(env: AniEnv<'local>, ani_this: AniRef<'local>) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesEndWrap(predicates_ptr);

    ani_this
}

pub fn native_or<'local>(env: AniEnv<'local>, ani_this: AniRef<'local>) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesOr(predicates_ptr);

    ani_this
}

pub fn native_and<'local>(env: AniEnv<'local>, ani_this: AniRef<'local>) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesAnd(predicates_ptr);

    ani_this
}

pub fn native_contains<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
    value: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();
    let value_rust: String = env.deserialize(value).unwrap();
    wrapper::ffi::DataSharePredicatesContains(predicates_ptr, filed_rust, value_rust);
    ani_this
}

pub fn native_is_null<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();
    wrapper::ffi::DataSharePredicatesIsNull(predicates_ptr, filed_rust);
    ani_this
}

pub fn native_is_not_null<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();
    wrapper::ffi::DataSharePredicatesIsNotNull(predicates_ptr, filed_rust);
    ani_this
}

pub fn native_like<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
    value: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();
    let value_rust: String = env.deserialize(value).unwrap();
    wrapper::ffi::DataSharePredicatesLike(predicates_ptr, filed_rust, value_rust);
    ani_this
}

pub fn native_between<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
    low: AniObject<'local>,
    high: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();
    let low_rust: ValueType = env.deserialize(low).unwrap();
    let high_rust: ValueType = env.deserialize(high).unwrap();

    wrapper::ffi::DataSharePredicatesBetween(predicates_ptr, filed_rust, &low_rust, &high_rust);
    ani_this
}

pub fn native_greater_than<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
    value: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();
    let value_rust: ValueType = env.deserialize(value).unwrap();

    wrapper::ffi::DataSharePredicatesGreaterThan(predicates_ptr, filed_rust, &value_rust);
    ani_this
}

pub fn native_greater_than_or_equal_to<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
    value: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();
    let value_rust: ValueType = env.deserialize(value).unwrap();

    wrapper::ffi::DataSharePredicatesGreaterThanOrEqualTo(predicates_ptr, filed_rust, &value_rust);
    ani_this
}

pub fn native_less_than_or_equal_to<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
    value: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();
    let value_rust: ValueType = env.deserialize(value).unwrap();

    wrapper::ffi::DataSharePredicatesLessThanOrEqualTo(predicates_ptr, filed_rust, &value_rust);
    ani_this
}

pub fn native_less_than<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
    value: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();
    let value_rust: ValueType = env.deserialize(value).unwrap();

    wrapper::ffi::DataSharePredicatesLessThan(predicates_ptr, filed_rust, &value_rust);
    ani_this
}

pub fn native_order_by_asc<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();

    wrapper::ffi::DataSharePredicatesOrderByAsc(predicates_ptr, filed_rust);
    ani_this
}

pub fn native_order_by_desc<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();

    wrapper::ffi::DataSharePredicatesOrderByDesc(predicates_ptr, filed_rust);
    ani_this
}

pub fn native_limit<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    total: f64,
    offset: f64,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesLimit(predicates_ptr, total, offset);
    ani_this
}

pub fn native_group_by<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    fields: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let fields_rust: Vec<String> = env.deserialize(fields).unwrap();

    wrapper::ffi::DataSharePredicatesGroupBy(predicates_ptr, fields_rust);
    ani_this
}

pub fn native_in<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
    value: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();
    let value_rust: Vec<ValueType> = env.deserialize(value).unwrap();

    wrapper::ffi::DataSharePredicatesIn(predicates_ptr, filed_rust, value_rust);

    ani_this
}

pub fn native_not_in<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: AniObject<'local>,
    value: AniObject<'local>,
) -> AniRef<'local> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    let filed_rust: String = env.deserialize(field).unwrap();
    let value_rust: Vec<ValueType> = env.deserialize(value).unwrap();

    wrapper::ffi::DataSharePredicatesNotIn(predicates_ptr, filed_rust, value_rust);

    ani_this
}

pub fn native_clean<'local>(
    env: AniEnv<'local>,
    ani_this: AniRef<'local>,
) {
    let predicates_ptr = get_native_ptr(&env, &ani_this.into());
    wrapper::ffi::DataSharePredicatesClean(predicates_ptr);
}