/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ani.h>
#include <array>
#include <iostream>
#include "ani_utils.h"
#include "datashare_log.h"
#include "datashare_predicates_cleaner.h"

using namespace OHOS::DataShare;

static DataSharePredicates* unwrapp(ani_env *env, ani_object object)
{
    DataSharePredicates *holder = AniObjectUtils::Unwrap<DataSharePredicates>(env, object);
    if (holder == nullptr) {
        LOG_ERROR("holder is nullptr");
        return nullptr;
    }
    return holder;
}

static ani_long Create([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_class clazz)
{
    auto holder = new DataSharePredicates();
    return reinterpret_cast<ani_long>(holder);
}

static ani_object EqualTo([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_string field,
    ani_object value)
{
    auto stringContentOne = AniStringUtils::ToStd(env, field);

    auto dataSharePredicates = unwrapp(env, object);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    UnionAccessor unionAccessor(env, value);
    double fValue = 0.0;
    if (unionAccessor.TryConvert<double>(fValue)) {
        dataSharePredicates->EqualTo(stringContentOne, fValue);
    }

    std::string strValue = "";
    if (unionAccessor.TryConvert<std::string>(strValue)) {
        dataSharePredicates->EqualTo(stringContentOne, strValue);
    }

    bool boolValue = false;
    if (unionAccessor.TryConvert<bool>(boolValue)) {
        dataSharePredicates->EqualTo(stringContentOne, boolValue);
    }

    return object;
}

static ani_object NotEqualTo([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_string field,
    ani_object value)
{
    auto stringContentOne = AniStringUtils::ToStd(env, field);

    auto dataSharePredicates = unwrapp(env, object);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    UnionAccessor unionAccessor(env, value);
    double fValue = 0.0;
    if (unionAccessor.TryConvert<double>(fValue)) {
        dataSharePredicates->NotEqualTo(stringContentOne, fValue);
    }

    std::string strValue = "";
    if (unionAccessor.TryConvert<std::string>(strValue)) {
        dataSharePredicates->NotEqualTo(stringContentOne, strValue);
    }

    bool boolValue = false;
    if (unionAccessor.TryConvert<bool>(boolValue)) {
        dataSharePredicates->NotEqualTo(stringContentOne, boolValue);
    }
    return object;
}

static ani_object OrderByDesc([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_string field)
{
    auto stringContent = AniStringUtils::ToStd(env, field);
    auto dataSharePredicates = unwrapp(env, object);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    dataSharePredicates->OrderByDesc(stringContent);
    return object;
}

static ani_object OrderByAsc([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_string field)
{
    auto stringContent = AniStringUtils::ToStd(env, field);
    auto dataSharePredicates = unwrapp(env, object);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    dataSharePredicates->OrderByAsc(stringContent);
    return object;
}

static ani_object And([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object)
{
    auto dataSharePredicates = unwrapp(env, object);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    dataSharePredicates->And();
    return object;
}

static ani_object Limit([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_double total,
    ani_double offset)
{
    auto dataSharePredicates = unwrapp(env, object);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    dataSharePredicates->Limit(total, offset);
    return object;
}

static ani_object LessThan([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_string field,
    ani_object value)
{
    auto fieldStr = AniStringUtils::ToStd(env, field);

    auto dataSharePredicates = unwrapp(env, obj);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    UnionAccessor unionAccessor(env, value);
    double fValue = 0.0;
    if (unionAccessor.TryConvert<double>(fValue)) {
        dataSharePredicates->LessThan(fieldStr, fValue);
    }

    std::string strValue = "";
    if (unionAccessor.TryConvert<std::string>(strValue)) {
        dataSharePredicates->LessThan(fieldStr, strValue);
    }

    bool boolValue = false;
    if (unionAccessor.TryConvert<bool>(boolValue)) {
        dataSharePredicates->LessThan(fieldStr, boolValue);
    }
    return obj;
}

static ani_object Like([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_string field,
    ani_string value)
{
    auto fieldStr = AniStringUtils::ToStd(env, field);
    auto valueStr = AniStringUtils::ToStd(env, value);
    auto dataSharePredicates = unwrapp(env, obj);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    dataSharePredicates->Like(fieldStr, valueStr);
    return obj;
}

static ani_object EndWrap([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj)
{
    auto dataSharePredicates = unwrapp(env, obj);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    dataSharePredicates->EndWrap();
    return obj;
}

static ani_object GreaterThanOrEqualTo([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj,
    ani_string field, ani_object value)
{
    auto fieldStr = AniStringUtils::ToStd(env, field);

    auto dataSharePredicates = unwrapp(env, obj);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    UnionAccessor unionAccessor(env, value);
    double fValue = 0.0;
    if (unionAccessor.TryConvert<double>(fValue)) {
        std::cout << "Double value is " << fValue << std::endl;
        dataSharePredicates->GreaterThanOrEqualTo(fieldStr, fValue);
    }

    std::string strValue = "";
    if (unionAccessor.TryConvert<std::string>(strValue)) {
        std::cout << "String value is " << strValue << std::endl;
        dataSharePredicates->GreaterThanOrEqualTo(fieldStr, strValue);
    }

    bool boolValue = false;
    if (unionAccessor.TryConvert<bool>(boolValue)) {
        std::cout << "Boolean value is " << boolValue << std::endl;
        dataSharePredicates->GreaterThanOrEqualTo(fieldStr, boolValue);
    }
    return obj;
}

static ani_object Contains([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_string field,
    ani_string value)
{
    auto fieldStr = AniStringUtils::ToStd(env, field);
    auto valueStr = AniStringUtils::ToStd(env, value);

    auto dataSharePredicates = unwrapp(env, obj);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    dataSharePredicates->Contains(fieldStr, valueStr);
    return obj;
}

static ani_object Or([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj)
{
    auto dataSharePredicates = unwrapp(env, obj);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    dataSharePredicates->Or();
    return obj;
}

static ani_object BeginWrap([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj)
{
    auto dataSharePredicates = unwrapp(env, obj);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    dataSharePredicates->BeginWrap();
    return obj;
}

static ani_object GreaterThan([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_string field,
    ani_object value)
{
    auto fieldStr = AniStringUtils::ToStd(env, field);

    auto dataSharePredicates = unwrapp(env, obj);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    UnionAccessor unionAccessor(env, value);
    ani_double valueD = 0.0;
    if (unionAccessor.TryConvert<ani_double>(valueD)) {
        dataSharePredicates->GreaterThan(fieldStr, static_cast<double>(valueD));
    }

    ani_string strValue = nullptr;
    if (unionAccessor.IsInstanceOf("Lstd/core/String;")) {
        dataSharePredicates->GreaterThan(fieldStr, AniStringUtils::ToStd(env, strValue));
    }

    ani_boolean boolValue = 0;
    if (unionAccessor.IsInstanceOf("Lstd/core/Boolean;")) {
        if (ANI_OK != env->Object_CallMethodByName_Boolean(value, "unboxed", nullptr, &boolValue)) {
            dataSharePredicates->GreaterThan(fieldStr, static_cast<bool>(boolValue));
        }
    }

    return obj;
}

static ani_object GroupBy([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_object arrayObj)
{
    ani_double length;

    if (ANI_OK != env->Object_GetPropertyByName_Double(arrayObj, "length", &length)) {
        return obj;
    }

    std::vector<std::string> strings;
    for (int i = 0; i < int(length); i++) {
        ani_ref stringEntryRef;
        if (ANI_OK != env->Object_CallMethodByName_Ref(arrayObj, "$_get", "I:Lstd/core/Object;", &stringEntryRef,
            (ani_int)i)) {
            return obj;
        }
        strings.emplace_back(AniStringUtils::ToStd(env, static_cast<ani_string>(stringEntryRef)));
    }

    auto dataSharePredicates = unwrapp(env, obj);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    dataSharePredicates->GroupBy(strings);
    return obj;
}

static ani_object In([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_string fieldStr,
    ani_object arrayObj)
{
    UnionAccessor unionAccessor(env, arrayObj);
    auto dataSharePredicates = unwrapp(env, obj);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    auto stringContent = AniStringUtils::ToStd(env, static_cast<ani_string>(fieldStr));
    std::vector<double> arrayDoubleValues = {};
    if (unionAccessor.TryConvertArray(arrayDoubleValues) && arrayDoubleValues.size() > 0) {
        std::vector<std::string> values = convertVector(arrayDoubleValues);
        dataSharePredicates->In(stringContent, values);
    }
    std::vector<std::string> arrayStringValues = {};
    if (unionAccessor.TryConvertArray(arrayStringValues) && arrayStringValues.size() > 0) {
        dataSharePredicates->In(stringContent, arrayStringValues);
    }
    std::vector<bool> arrayBoolValues = {};
    if (unionAccessor.TryConvertArray(arrayBoolValues) && arrayBoolValues.size() > 0) {
        std::vector<std::string> values = convertVector(arrayBoolValues);
        dataSharePredicates->In(stringContent, values);
    }
    return obj;
}

static ani_object NotIn([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_string fieldStr,
    ani_object arrayObj)
{
    UnionAccessor unionAccessor(env, arrayObj);
    auto dataSharePredicates = unwrapp(env, obj);
    if (dataSharePredicates == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr");
        return nullptr;
    }
    auto stringContent = AniStringUtils::ToStd(env, static_cast<ani_string>(fieldStr));
    std::vector<double> arrayDoubleValues = {};
    if (unionAccessor.TryConvertArray<double>(arrayDoubleValues) && arrayDoubleValues.size() > 0) {
        std::vector<std::string> values = convertVector(arrayDoubleValues);
        dataSharePredicates->NotIn(stringContent, values);
    }
    std::vector<std::string> arrayStringValues = {};
    if (unionAccessor.TryConvertArray<std::string>(arrayStringValues) && arrayStringValues.size() > 0) {
        dataSharePredicates->NotIn(stringContent, arrayStringValues);
    }
    std::vector<bool> arrayBoolValues = {};
    if (unionAccessor.TryConvertArray<bool>(arrayBoolValues) && arrayBoolValues.size() > 0) {
        std::vector<std::string> values = convertVector(arrayBoolValues);
        dataSharePredicates->NotIn(stringContent, values);
    }
    return obj;
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    LOG_INFO("ANI_Constructor enter ");
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        std::cerr << "Unsupported ANI_VERSION_1" << std::endl;
        return ANI_ERROR;
    }

    ani_class cls;
    static const char *className = "@ohos.data.dataSharePredicates.dataSharePredicates.DataSharePredicates";
    if (env->FindClass(className, &cls) != ANI_OK) {
        LOG_ERROR("Class not found");
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function {"create", nullptr, reinterpret_cast<void *>(Create) },
        ani_native_function {"equalTo", nullptr, reinterpret_cast<void *>(EqualTo)},
        ani_native_function {"notEqualTo", nullptr, reinterpret_cast<void *>(NotEqualTo)},
        ani_native_function {"orderByDesc", nullptr, reinterpret_cast<void *>(OrderByDesc)},
        ani_native_function {"orderByAsc", nullptr, reinterpret_cast<void *>(OrderByAsc)},
        ani_native_function {"and", nullptr, reinterpret_cast<void *>(And)},
        ani_native_function {"limit", nullptr, reinterpret_cast<void *>(Limit)},
        ani_native_function {"lessThan", nullptr, reinterpret_cast<void *>(LessThan)},
        ani_native_function {"like", nullptr, reinterpret_cast<void *>(Like)},
        ani_native_function {"endWrap", nullptr, reinterpret_cast<void *>(EndWrap)},
        ani_native_function {"greaterThanOrEqualTo", nullptr, reinterpret_cast<void *>(GreaterThanOrEqualTo)},
        ani_native_function {"contains", nullptr, reinterpret_cast<void *>(Contains)},
        ani_native_function {"or", nullptr, reinterpret_cast<void *>(Or)},
        ani_native_function {"beginWrap", nullptr, reinterpret_cast<void *>(BeginWrap)},
        ani_native_function {"greaterThan", nullptr, reinterpret_cast<void *>(GreaterThan)},
        ani_native_function {"groupBy", nullptr, reinterpret_cast<void *>(GroupBy)},
        ani_native_function {"in", nullptr, reinterpret_cast<void *>(In)},
        ani_native_function {"notIn", nullptr, reinterpret_cast<void *>(NotIn)},
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        LOG_ERROR("Cannot bind native methods to %{public}s", className);
        return ANI_ERROR;
    };

    static const char *cleanerName = "@ohos.data.dataSharePredicates.dataSharePredicates.Cleaner";
    auto cleanerCls = AniTypeFinder(env).FindClass(cleanerName);
    DataSharePredicatesCleaner(env).Bind(cleanerCls.value());

    *result = ANI_VERSION_1;
    return ANI_OK;
}
