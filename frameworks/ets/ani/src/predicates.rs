// Copyright (c) 2025 Huawei Device Co., Ltd.
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
    business_error::BusinessError,
};

use crate::{get_native_ptr, wrapper};

#[derive(serde::Serialize, serde::Deserialize, Debug, Clone)]
pub enum ValueType {
    S(String),
    F64(f64),
    Boolean(bool),
    I64(i64),
}

pub fn create<'local>(_env: AniEnv<'local>, _clazz: AniClass<'local>) -> i64 {
    wrapper::ffi::DataSharePredicatesNew()
}

#[ani_rs::native]
pub fn native_equal_to<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: ValueType,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesEqualTo(predicates_ptr, field, &value);

    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_not_equal_to<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: ValueType,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesNotEqualTo(predicates_ptr, field, &value);

    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_begin_wrap<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesBeginWrap(predicates_ptr);

    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_end_wrap<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesEndWrap(predicates_ptr);

    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_or<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesOr(predicates_ptr);

    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_and<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesAnd(predicates_ptr);

    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_contains<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: String,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    wrapper::ffi::DataSharePredicatesContains(predicates_ptr, field, value);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_begins_with<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: String,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    wrapper::ffi::DataSharePredicatesBeginsWith(predicates_ptr, field, value);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_ends_with<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: String,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    wrapper::ffi::DataSharePredicatesEndsWith(predicates_ptr, field, value);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_is_null<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    wrapper::ffi::DataSharePredicatesIsNull(predicates_ptr, field);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_is_not_null<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    wrapper::ffi::DataSharePredicatesIsNotNull(predicates_ptr, field);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_like<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: String,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    wrapper::ffi::DataSharePredicatesLike(predicates_ptr, field, value);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_unlike<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: String,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    wrapper::ffi::DataSharePredicatesUnlike(predicates_ptr, field, value);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_glob<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: String,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    wrapper::ffi::DataSharePredicatesGlob(predicates_ptr, field, value);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_between<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    low: ValueType,
    high: ValueType,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesBetween(predicates_ptr, field, &low, &high);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_not_between<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    low: ValueType,
    high: ValueType,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    wrapper::ffi::DataSharePredicatesNotBetween(predicates_ptr, field, &low, &high);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_greater_than<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: ValueType,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesGreaterThan(predicates_ptr, field, &value);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_greater_than_or_equal_to<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: ValueType,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesGreaterThanOrEqualTo(predicates_ptr, field, &value);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_less_than_or_equal_to<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: ValueType,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesLessThanOrEqualTo(predicates_ptr, field, &value);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_less_than<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: ValueType,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesLessThan(predicates_ptr, field, &value);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_order_by_asc<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesOrderByAsc(predicates_ptr, field);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_order_by_desc<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesOrderByDesc(predicates_ptr, field);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_distinct<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());
    wrapper::ffi::DataSharePredicatesDistinct(predicates_ptr);
    Ok(ani_this)
}


#[ani_rs::native]
pub fn native_limit<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    total: i32,
    offset: i32,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesLimit(predicates_ptr, total, offset);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_group_by<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    fields: Vec<String>,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesGroupBy(predicates_ptr, fields);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_indexed_by<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesIndexedBy(predicates_ptr, field);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_in<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: Vec<ValueType>,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesIn(predicates_ptr, field, value);

    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_not_in<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    field: String,
    value: Vec<ValueType>,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesNotIn(predicates_ptr, field, value);

    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_prefix_key<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    prefix: String,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesPrefixKey(predicates_ptr, prefix);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_in_keys<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
    keys: Vec<String>,
) -> Result<AniRef<'local>, BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.clone().into());

    wrapper::ffi::DataSharePredicatesInKeys(predicates_ptr, keys);
    Ok(ani_this)
}

#[ani_rs::native]
pub fn native_clean<'local>(
    env: &AniEnv<'local>,
    ani_this: AniRef<'local>,
) -> Result<(), BusinessError> {
    let predicates_ptr = get_native_ptr(&env, &ani_this.into());
    wrapper::ffi::DataSharePredicatesClean(predicates_ptr);
    Ok(())
}
