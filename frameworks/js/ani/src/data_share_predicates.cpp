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
#include "datashare_predicates.h"

using namespace OHOS::DataShare;

static DataSharePredicates* unwrapp(ani_env *env, ani_object object)
{
    return NativeObjectWrapper<DataSharePredicates>::Unwrap(env, object);
}

static ani_object Create([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_class clazz)
{
    return NativeObjectWrapper<DataSharePredicates>::Create(env, clazz);
}

static ani_object EqualTo([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_string field,
    ani_object value)
{
    std::cout << "EqualTo enter string" << std::endl;

    auto stringContentOne = ANIUtils_ANIStringToStdString(env, static_cast<ani_string>(field));

    auto dataSharePredicates = unwrapp(env, object);
    UnionAccessor unionAccessor(env, value);
    double aniValue = 0;
    if (unionAccessor.TryConvert(aniValue)) {
        std::cout << "Double value is " << static_cast<bool>(aniValue) << std::endl;
        dataSharePredicates->EqualTo(stringContentOne, static_cast<bool>(aniValue));
    }

    ani_string strValue = nullptr;
    if (unionAccessor.TryConvert<ani_string>(strValue)) {
        std::cout << "String value is " << ANIUtils_ANIStringToStdString(env, strValue) << std::endl;
        dataSharePredicates->EqualTo(stringContentOne, ANIUtils_ANIStringToStdString(env, strValue));
    }

    bool boolValue = 0;
    if (unionAccessor.TryConvert(boolValue)) {
        std::cout << "Boolean value is " << boolValue << std::endl;
        dataSharePredicates->EqualTo(stringContentOne, static_cast<bool>(boolValue));
    }

    return object;
}

static ani_object NotEqualTo([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_string field,
    ani_object value)
{
    std::cout << "NotEqualTo enter string" << std::endl;
    auto stringContentOne = ANIUtils_ANIStringToStdString(env, static_cast<ani_string>(field));

    auto dataSharePredicates = unwrapp(env, object);
    UnionAccessor unionAccessor(env, value);
    double aniValue = 0;
    if (unionAccessor.TryConvert(aniValue)) {
        std::cout << "Double value is " << static_cast<bool>(aniValue) << std::endl;
        dataSharePredicates->NotEqualTo(stringContentOne, static_cast<bool>(aniValue));
    }

    ani_string strValue = nullptr;
    if (unionAccessor.TryConvert<ani_string>(strValue)) {
        std::cout << "String value is " << ANIUtils_ANIStringToStdString(env, strValue) << std::endl;
        dataSharePredicates->NotEqualTo(stringContentOne, ANIUtils_ANIStringToStdString(env, strValue));
    }

    bool boolValue = 0;
    if (unionAccessor.TryConvert(boolValue)) {
        std::cout << "Boolean value is " << boolValue << std::endl;
        dataSharePredicates->NotEqualTo(stringContentOne, static_cast<bool>(boolValue));
    }
    return object;
}

static ani_object OrderByDesc([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_string field)
{
    std::cout << "OrderByDesc enter string" << std::endl;
    auto stringContent = ANIUtils_ANIStringToStdString(env, field);
    auto dataSharePredicates = unwrapp(env, object);
    dataSharePredicates->OrderByDesc(stringContent);
    return object;
}

static ani_object OrderByAsc([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_string field)
{
    std::cout << "OrderByAsc enter string" << std::endl;
    auto stringContent = ANIUtils_ANIStringToStdString(env, field);
    auto dataSharePredicates = unwrapp(env, object);
    dataSharePredicates->OrderByAsc(stringContent);
    return object;
}

static ani_object And([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object)
{
    std::cout << "And enter string" << std::endl;
    auto dataSharePredicates = unwrapp(env, object);
    dataSharePredicates->And();
    return object;
}

static ani_object Limit([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_double total,
    ani_double offset)
{
    std::cout << "enter Limit" << total << std::endl;
    auto dataSharePredicates = unwrapp(env, object);
    dataSharePredicates->Limit(total, offset);
    return object;
}

static ani_object LessThan([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_string field,
    ani_object value)
{
    std::cout << "enter LessThan func" << std::endl;
    auto fieldStr = ANIUtils_ANIStringToStdString(env, field);

    std::cout << "LessThan Get field: "<< fieldStr << std::endl;
    auto dataSharePredicates = unwrapp(env, obj);
    UnionAccessor unionAccessor(env, value);
    double aniValue = 0;
    if (unionAccessor.TryConvert(aniValue)) {
        std::cout << "Double value is " << static_cast<bool>(aniValue) << std::endl;
        dataSharePredicates->LessThan(fieldStr, static_cast<bool>(aniValue));
    }

    ani_string strValue = nullptr;
    if (unionAccessor.TryConvert<ani_string>(strValue)) {
        std::cout << "String value is " << ANIUtils_ANIStringToStdString(env, strValue) << std::endl;
        dataSharePredicates->LessThan(fieldStr, ANIUtils_ANIStringToStdString(env, strValue));
    }

    bool boolValue = 0;
    if (unionAccessor.TryConvert(boolValue)) {
        std::cout << "Boolean value is " << boolValue << std::endl;
        dataSharePredicates->LessThan(fieldStr, static_cast<bool>(boolValue));
    }
    return obj;
}

static ani_object Like([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_string field,
    ani_string value)
{
    std::cout << "enter Like func" << std::endl;
    auto fieldStr = ANIUtils_ANIStringToStdString(env, field);
    std::cout << "Like Get field: "<< fieldStr << std::endl;
    auto valueStr = ANIUtils_ANIStringToStdString(env, value);
    std::cout << "Like Get value: "<< valueStr << std::endl;
    auto dataSharePredicates = unwrapp(env, obj);
    dataSharePredicates->Like(fieldStr, valueStr);
    return obj;
}

static ani_object EndWrap([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj)
{
    std::cout << "enter EndWrap func" << std::endl;
    auto dataSharePredicates = unwrapp(env, obj);
    dataSharePredicates->EndWrap();
    return obj;
}

static ani_object GreaterThanOrEqualTo([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj,
    ani_string field, ani_object value)
{
    std::cout << "enter GreaterThanOrEqualTo func" << std::endl;
    auto fieldStr = ANIUtils_ANIStringToStdString(env, field);
    std::cout << "LessThan Get field: "<< fieldStr << std::endl;

    auto dataSharePredicates = unwrapp(env, obj);
    UnionAccessor unionAccessor(env, value);
    double aniValue = 0;
    if (unionAccessor.TryConvert(aniValue)) {
        std::cout << "Double value is " << static_cast<bool>(aniValue) << std::endl;
        dataSharePredicates->GreaterThanOrEqualTo(fieldStr, static_cast<bool>(aniValue));
    }

    ani_string strValue = nullptr;
    if (unionAccessor.TryConvert<ani_string>(strValue)) {
        std::cout << "String value is " << ANIUtils_ANIStringToStdString(env, strValue) << std::endl;
        dataSharePredicates->GreaterThanOrEqualTo(fieldStr, ANIUtils_ANIStringToStdString(env, strValue));
    }

    bool boolValue = 0;
    if (unionAccessor.TryConvert(boolValue)) {
        std::cout << "Boolean value is " << boolValue << std::endl;
        dataSharePredicates->GreaterThanOrEqualTo(fieldStr, static_cast<bool>(boolValue));
    }
    return obj;
}

static ani_object Contains([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_string field,
    ani_string value)
{
    std::cout << "enter Contains func" << std::endl;
    auto fieldStr = ANIUtils_ANIStringToStdString(env, field);
    std::cout << "Contains Get field: "<< fieldStr << std::endl;
    auto valueStr = ANIUtils_ANIStringToStdString(env, value);
    std::cout << "Contains Get value: "<< valueStr << std::endl;
    auto dataSharePredicates = unwrapp(env, obj);
    dataSharePredicates->Contains(fieldStr, valueStr);
    return obj;
}

static ani_object Or([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj)
{
    std::cout << "enter Or func" << std::endl;
    auto dataSharePredicates = unwrapp(env, obj);
    dataSharePredicates->Or();
    return obj;
}

static ani_object BeginWrap([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj)
{
    std::cout << "enter BeginWrap func" << std::endl;
    auto dataSharePredicates = unwrapp(env, obj);
    dataSharePredicates->BeginWrap();
    return obj;
}

static ani_object GreaterThan([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_string field,
    ani_object value)
{
    std::cout << "enter GreaterThan func" << std::endl;
    auto fieldStr = ANIUtils_ANIStringToStdString(env, field);
    std::cout << "LessThan Get field: "<< fieldStr << std::endl;
    auto dataSharePredicates = unwrapp(env, obj);
    UnionAccessor unionAccessor(env, value);
    ani_double valueD = 0.0;
    if (unionAccessor.TryConvert<ani_double>(valueD)) {
        std::cout << "Double value is " << valueD << std::endl;
        dataSharePredicates->GreaterThan(fieldStr, static_cast<double>(valueD));
    }

    ani_string strValue = nullptr;
    if (unionAccessor.IsInstanceOf("Lstd/core/String;")) {
        std::cout << "ani_string value is " << ANIUtils_ANIStringToStdString(env, strValue) << std::endl;
        dataSharePredicates->GreaterThan(fieldStr, ANIUtils_ANIStringToStdString(env, strValue));
    }

    ani_boolean boolValue = 0;
    if (unionAccessor.IsInstanceOf("Lstd/core/Boolean;")) {
        if (ANI_OK != env->Object_CallMethodByName_Boolean(value, "unboxed", nullptr, &boolValue)) {
            std::cerr << "Object_CallMethodByName_Double unbox Failed" << std::endl;
            dataSharePredicates->GreaterThan(fieldStr, static_cast<bool>(boolValue));
        }
    }

    return obj;
}

static ani_object GroupBy([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_object arrayObj)
{
    ani_double length;

    if (ANI_OK != env->Object_GetPropertyByName_Double(arrayObj, "length", &length)) {
        std::cerr << "Object_GetPropertyByName_Double length Failed" << std::endl;
        return obj;
    }

    std::vector<std::string> strings;
    for (int i = 0; i < int(length); i++) {
        ani_ref stringEntryRef;
        if (ANI_OK != env->Object_CallMethodByName_Ref(arrayObj, "$_get", "I:Lstd/core/Object;", &stringEntryRef,
            (ani_int)i)) {
            std::cerr << "Object_GetPropertyByName_Double length Failed" << std::endl;
            return obj;
        }
        strings.emplace_back(ANIUtils_ANIStringToStdString(env, static_cast<ani_string>(stringEntryRef)));
    }
    for (const auto &s : strings) {
        std::cout << "Array String Content:" << s.c_str() << std::endl;
    }
    auto dataSharePredicates = unwrapp(env, obj);
    dataSharePredicates->GroupBy(strings);
    return obj;
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    std::cout << "ANI_Constructor enter " << std::endl;
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        std::cerr << "Unsupported ANI_VERSION_1" << std::endl;
        return ANI_ERROR;
    }
    ani_namespace ns;
    if (env->FindNamespace("Ldata_share_predicates/dataSharePredicates;", &ns) != ANI_OK) {
        std::cerr << "Namespace not found" << std::endl;
        return ANI_ERROR;
    };

    ani_class cls;
    static const char *className = "LDataSharePredicates;";
    if (env->Namespace_FindClass(ns, className, &cls) != ANI_OK) {
        std::cerr << "Class not found" << std::endl;
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function {"create", ":Ldata_share_predicates/dataSharePredicates/DataSharePredicates;",
            reinterpret_cast<void *>(Create) },
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
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        std::cerr << "Cannot bind native methods to '" << className << "'" << std::endl;
        return ANI_ERROR;
    };

    *result = ANI_VERSION_1;
    return ANI_OK;
}