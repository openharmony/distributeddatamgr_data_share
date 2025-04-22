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

#ifndef ANI_UTILS_H
#define ANI_UTILS_H

#include <ani.h>
#include <string>
#include <iostream>
#include <sstream>
#include "ani_util_class.h"
#include "ani_util_native_ptr.h"

namespace OHOS {
namespace DataShare {
class AniObjectUtils {
public:
    static ani_object Create(ani_env *env, const char* nsName, const char* clsName, ...);
    static ani_object Create(ani_env *env, const char* clsName, ...);
    static ani_object Create(ani_env *env, ani_class cls, ...);
    static ani_object From(ani_env *env, bool value);
    template<typename T>
    static ani_status Wrap(ani_env *env, ani_object object, T* nativePtr, const char* propName = "nativePtr")
    {
        if (nativePtr == nullptr) {
            return ANI_ERROR;
        }
        return env->Object_SetFieldByName_Long(object, propName, reinterpret_cast<ani_long>(nativePtr));
    }

    template<typename T>
    static T* Unwrap(ani_env *env, ani_object object, const char* propName = "nativePtr")
    {
        ani_long nativePtr;
        if (ANI_OK != env->Object_GetFieldByName_Long(object, propName, &nativePtr)) {
            return nullptr;
        }

        return reinterpret_cast<T*>(nativePtr);
    }
};

class AniStringUtils {
public:
    static std::string ToStd(ani_env *env, ani_string ani_str);
    static ani_string ToAni(ani_env* env, const std::string& str);
};

class UnionAccessor {
public:
    UnionAccessor(ani_env *env, ani_object &obj);
    bool IsInstanceOf(const std::string& cls_name);
    bool IsInstanceOf(const std::string& cls_name, ani_object obj);
    template<typename T> bool IsInstanceOfType();
    template<typename T> bool TryConvert(T &value);
    template<typename T> bool TryConvertArray(std::vector<T> &value);

private:
    ani_env *env_;
    ani_object obj_;
};
class OptionalAccessor {
public:
    OptionalAccessor(ani_env *env, ani_object &obj);
    bool IsUndefined();
    bool IsNull();
    template<typename T> std::optional<T> Convert();

private:
    ani_env *env_;
    ani_object obj_;
};

template<typename T>
struct Converter {
    static std::string convert(const T& value)
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
};

template<>
struct Converter<bool> {
    static std::string convert(const bool& value)
    {
        return value ? "true" : "false";
    }
};


template<>
struct Converter<double> {
    static std::string convert(const double& value)
    {
        std::ostringstream buf;
        buf << value;
        std::string str = buf.str();
        return str;
    }
};

template<typename T>
std::vector<std::string> convertVector(const std::vector<T>& input)
{
    std::vector<std::string> result;
    result.reserve(input.size());

    for (const auto& elem : input) {
        result.push_back(Converter<T>::convert(elem));
    }
    return result;
}

template<typename T> using convertCallback = bool(*)(ani_env*, ani_ref&, ani_ref&, std::shared_ptr<T>);

template<typename F, typename T>
bool forEachMapEntry(ani_env *env, ani_object map_object, F &&callback, std::shared_ptr<T> records)
{
    ani_ref iter;
    if (ANI_OK != env->Object_CallMethodByName_Ref(map_object, "$_iterator", nullptr, &iter)) {
        std::cout << "Failed to get keys iterator" << std::endl;
        return false;
    }

    ani_ref next;
    ani_boolean done;
    while (ANI_OK == env->Object_CallMethodByName_Ref(static_cast<ani_object>(iter), "next", nullptr, &next)) {
        if (ANI_OK != env->Object_GetFieldByName_Boolean(static_cast<ani_object>(next), "done", &done)) {
            std::cout << "Failed to check iterator done" << std::endl;
            return false;
        }

        if (done) {
            std::cout << "[forEachMapEntry] done break" << std::endl;
            return true;
        }

        ani_ref key_value;
        if (ANI_OK != env->Object_GetFieldByName_Ref(static_cast<ani_object>(next), "value", &key_value)) {
            std::cout << "Failed to get key value" << std::endl;
            return false;
        }

        ani_ref ani_key;
        if (ANI_OK != env->TupleValue_GetItem_Ref(static_cast<ani_tuple_value>(key_value), 0, &ani_key)) {
            std::cout << "Failed to get key value" << std::endl;
            return false;
        }

        ani_ref ani_val;
        if (ANI_OK != env->TupleValue_GetItem_Ref(static_cast<ani_tuple_value>(key_value), 1, &ani_val)) {
            std::cout << "Failed to get key value" << std::endl;
            return false;
        }

        if (!callback(env, ani_key, ani_val, records)) {
            return false;
        }
    }

    std::cout << "Failed to get next key" << std::endl;
    return false;
}

ani_object DoubleToObject(ani_env *env, double value);
ani_object BoolToObject(ani_env *env, bool value);
ani_object StringToObject(ani_env *env, std::string value);
ani_object Uint8ArrayToObject(ani_env *env, const std::vector<uint8_t> &values);
}
}
#endif // ANI_UTILS_H
