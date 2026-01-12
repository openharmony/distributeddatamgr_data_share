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

#define LOG_TAG "ani_utils"

#include <cstdarg>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>
#include <securec.h>

#include "datashare_log.h"
#include "ani_utils.h"


namespace OHOS::DataShare {
ani_object AniObjectUtils::Create(ani_env *env, const char* nsName, const char* clsName, ...)
{
    ani_object nullobj{};
    if (env == nullptr) {
        return nullobj;
    }
    ani_namespace ns;
    if (ANI_OK != env->FindNamespace(nsName, &ns)) {
        LOG_ERROR("[ANI] Not found namespace %{public}s", nsName);
        return nullobj;
    }

    ani_class cls;
    const std::string fullClsName = std::string(nsName).append(".").append(clsName);
    if (ANI_OK != env->FindClass(fullClsName.c_str(), &cls)) {
        LOG_ERROR("[ANI] Not found class %{public}s", clsName);
        return nullobj;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        LOG_ERROR("[ANI] Not found <ctor> for class %{public}s", clsName);
        return nullobj;
    }

    ani_object obj;
    va_list args;
    va_start(args, clsName);
    ani_status status = env->Object_New_V(cls, ctor, &obj, args);
    va_end(args);
    if (ANI_OK != status) {
        LOG_ERROR("[ANI] Failed to Object_New for class %{public}s", clsName);
        return nullobj;
    }

    return obj;
}


ani_object AniObjectUtils::Create(ani_env *env, const char* clsName, ...)
{
    ani_object nullobj{};
    if (env == nullptr) {
        return nullobj;
    }
    ani_class cls;
    if (ANI_OK != env->FindClass(clsName, &cls)) {
        LOG_ERROR("[ANI] Not found class %{public}s", clsName);
        return nullobj;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        LOG_ERROR("[ANI] Not found <ctor> for class %{public}s", clsName);
        return nullobj;
    }

    ani_object obj;
    va_list args;
    va_start(args, clsName);
    ani_status status = env->Object_New_V(cls, ctor, &obj, args);
    va_end(args);
    if (ANI_OK != status) {
        LOG_ERROR("[ANI] Failed to Object_New for class %{public}s", clsName);
        return nullobj;
    }

    return obj;
}

ani_object AniObjectUtils::Create(ani_env *env, ani_class cls, ...)
{
    ani_object nullobj{};
    if (env == nullptr) {
        return nullobj;
    }
    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        LOG_ERROR("[ANI] Not found <ctor> for class");
        return nullobj;
    }

    ani_object obj;
    va_list args;
    va_start(args, cls);
    ani_status status = env->Object_New_V(cls, ctor, &obj, args);
    va_end(args);
    if (ANI_OK != status) {
        LOG_ERROR("[ANI] Failed to Object_New for class");
        return nullobj;
    }

    return obj;
}

ani_object AniObjectUtils::From(ani_env *env, bool value)
{
    return Create(env, "std.core.Boolean", static_cast<ani_boolean>(value));
}

std::string AniStringUtils::ToStd(ani_env *env, ani_string ani_str)
{
    ani_size strSize;
    env->String_GetUTF8Size(ani_str, &strSize);

    std::vector<char> buffer(strSize + 1); // +1 for null terminator
    char* utf8_buffer = buffer.data();

    //String_GetUTF8 Supportted by https://gitee.com/openharmony/arkcompiler_runtime_core/pulls/3416
    ani_size bytes_written = 0;
    env->String_GetUTF8(ani_str, utf8_buffer, strSize + 1, &bytes_written);

    utf8_buffer[bytes_written] = '\0';
    std::string content = std::string(utf8_buffer);

    return content;
}

ani_string AniStringUtils::ToAni(ani_env* env, const std::string& str)
{
    ani_string aniStr = nullptr;
    if (ANI_OK != env->String_NewUTF8(str.data(), str.size(), &aniStr)) {
        LOG_ERROR("[ANI] Unsupported ANI_VERSION_1");
        return nullptr;
    }

    return aniStr;
}

UnionAccessor::UnionAccessor(ani_env *env, ani_object &obj) : env_(env), obj_(obj)
{
}

bool UnionAccessor::IsInstanceOf(const std::string& clsName)
{
    ani_class cls;
    env_->FindClass(clsName.c_str(), &cls);

    ani_boolean ret;
    env_->Object_InstanceOf(obj_, cls, &ret);

    return ret;
}

bool UnionAccessor::IsInstanceOf(const std::string& clsName, ani_object obj)
{
    ani_class cls;
    env_->FindClass(clsName.c_str(), &cls);

    ani_boolean ret;
    env_->Object_InstanceOf(obj, cls, &ret);
    return ret;
}

template<>
bool UnionAccessor::IsInstanceOfType<bool>()
{
    return IsInstanceOf("std.core.Boolean");
}

template<>
bool UnionAccessor::IsInstanceOfType<int>()
{
    return IsInstanceOf("std.core.Int");
}

template<>
bool UnionAccessor::IsInstanceOfType<double>()
{
    return IsInstanceOf("std.core.Double");
}

template<>
bool UnionAccessor::IsInstanceOfType<std::string>()
{
    return IsInstanceOf("std.core.String");
}

template<>
bool UnionAccessor::TryConvert<bool>(bool &value)
{
    if (!IsInstanceOfType<bool>()) {
        return false;
    }

    ani_boolean aniValue;
    auto ret = env_->Object_CallMethodByName_Boolean(obj_, "toBoolean", nullptr, &aniValue);
    if (ret != ANI_OK) {
        return false;
    }

    value = static_cast<bool>(aniValue);

    return true;
}

template<>
bool UnionAccessor::TryConvert<int>(int &value)
{
    if (!IsInstanceOfType<int>()) {
        return false;
    }

    ani_int aniValue;
    auto ret = env_->Object_CallMethodByName_Int(obj_, "toInt", nullptr, &aniValue);
    if (ret != ANI_OK) {
        return false;
    }

    value = static_cast<int>(aniValue);

    return true;
}

template<>
bool UnionAccessor::TryConvert<double>(double &value)
{
    if (!IsInstanceOfType<double>()) {
        return false;
    }

    ani_double aniValue;
    auto ret = env_->Object_CallMethodByName_Double(obj_, "toDouble", nullptr, &aniValue);
    if (ret != ANI_OK) {
        return false;
    }

    value = static_cast<double>(aniValue);

    return true;
}

template<>
bool UnionAccessor::TryConvert<std::string>(std::string &value)
{
    if (!IsInstanceOfType<std::string>()) {
        return false;
    }

    value = AniStringUtils::ToStd(env_, static_cast<ani_string>(obj_));

    return true;
}

template<>
bool UnionAccessor::TryConvertArray<bool>(std::vector<bool> &value)
{
    ani_double length;
    if (ANI_OK != env_->Object_GetPropertyByName_Double(obj_, "length", &length)) {
        LOG_ERROR("Object_GetPropertyByName_Double length failed");
        return false;
    }

    for (int i = 0; i < int(length); i++) {
        ani_ref ref;
        if (ANI_OK != env_->Object_CallMethodByName_Ref(obj_, "$_get", "i:Y", &ref, (ani_int)i)) {
            LOG_ERROR("Object_GetPropertyByName_Ref failed");
            return false;
        }

        if (!IsInstanceOf("std.core.Boolean", static_cast<ani_object>(ref))) {
            LOG_ERROR("Not found 'std.core.Boolean'");
            return false;
        }

        ani_boolean val;
        if (ANI_OK != env_->Object_CallMethodByName_Boolean(static_cast<ani_object>(ref), "toBoolean", nullptr, &val)) {
            LOG_ERROR("Object_CallMethodByName_Double unbox failed");
            return false;
        }

        value.push_back(static_cast<bool>(val));
    }

    return true;
}

template<>
bool UnionAccessor::TryConvertArray<int>(std::vector<int> &value)
{
    ani_double length;
    if (ANI_OK != env_->Object_GetPropertyByName_Double(obj_, "length", &length)) {
        LOG_ERROR("Object_GetPropertyByName_Double length failed");
        return false;
    }

    for (int i = 0; i < int(length); i++) {
        ani_ref ref;
        if (ANI_OK != env_->Object_CallMethodByName_Ref(obj_, "$_get", "i:Y", &ref, (ani_int)i)) {
            LOG_ERROR("Object_GetPropertyByName_Ref failed");
            return false;
        }

        if (!IsInstanceOf("std.core.Int", static_cast<ani_object>(ref))) {
            LOG_ERROR("Not found 'std.core.Double'");
            return false;
        }

        ani_int intValue;
        if (ANI_OK != env_->Object_CallMethodByName_Int(static_cast<ani_object>(ref), "toInt", nullptr, &intValue)) {
            LOG_ERROR("Object_CallMethodByName_Double unbox failed");
            return false;
        }

        value.push_back(static_cast<int>(intValue));
    }

    return true;
}

template<>
bool UnionAccessor::TryConvertArray<double>(std::vector<double> &value)
{
    ani_double length;
    if (ANI_OK != env_->Object_GetPropertyByName_Double(obj_, "length", &length)) {
        LOG_ERROR("Object_GetPropertyByName_Double length failed");
        return false;
    }

    for (int i = 0; i < int(length); i++) {
        ani_ref ref;
        if (ANI_OK != env_->Object_CallMethodByName_Ref(obj_, "$_get", "i:Y", &ref, (ani_int)i)) {
            LOG_ERROR("Object_GetPropertyByName_Ref failed");
            return false;
        }

        if (!IsInstanceOf("std.core.Double", static_cast<ani_object>(ref))) {
            LOG_ERROR("Not found 'std.core.Double'");
            return false;
        }

        ani_double val;
        if (ANI_OK != env_->Object_CallMethodByName_Double(static_cast<ani_object>(ref), "toDouble", nullptr, &val)) {
            LOG_ERROR("Object_CallMethodByName_Double unbox failed");
            return false;
        }

        value.push_back(static_cast<double>(val));
    }

    return true;
}

template<>
bool UnionAccessor::TryConvertArray<uint8_t>(std::vector<uint8_t> &value)
{
    LOG_INFO("TryConvertArray std::vector<uint8_t>");
    ani_ref buffer;
    if (ANI_OK != env_->Object_GetFieldByName_Ref(obj_, "buffer", &buffer)) {
        LOG_INFO("Object_GetFieldByName_Ref failed");
        return false;
    }

    void* data;
    size_t length;
    if (ANI_OK != env_->ArrayBuffer_GetInfo(static_cast<ani_arraybuffer>(buffer), &data, &length)) {
        LOG_ERROR("ArrayBuffer_GetInfo failed");
        return false;
    }

    LOG_INFO("Length of buffer is %{public}zu", length);
    for (size_t i = 0; i < length; i++) {
        value.push_back(static_cast<uint8_t*>(data)[i]);
    }

    return true;
}

template<>
bool UnionAccessor::TryConvertArray<std::string>(std::vector<std::string> &value)
{
    ani_double length;
    if (ANI_OK != env_->Object_GetPropertyByName_Double(obj_, "length", &length)) {
        LOG_ERROR("Object_GetPropertyByName_Double length failed");
        return false;
    }

    for (int i = 0; i < int(length); i++) {
        ani_ref ref;
        if (ANI_OK != env_->Object_CallMethodByName_Ref(obj_, "$_get", "i:Y", &ref, (ani_int)i)) {
            LOG_ERROR("Object_GetPropertyByName_Double length failed");
            return false;
        }

        if (!IsInstanceOf("std.core.String", static_cast<ani_object>(ref))) {
            LOG_ERROR("Not found 'std.core.String'");
            return false;
        }

        value.push_back(AniStringUtils::ToStd(env_, static_cast<ani_string>(ref)));
    }

    return true;
}

OptionalAccessor::OptionalAccessor(ani_env *env, ani_object &obj) : env_(env), obj_(obj)
{
}

bool OptionalAccessor::IsUndefined()
{
    ani_boolean isUndefined;
    env_->Reference_IsUndefined(obj_, &isUndefined);

    return isUndefined;
}

bool OptionalAccessor::IsNull()
{
    ani_boolean isNull;
    env_->Reference_IsNull(obj_, &isNull);

    return isNull;
}

template<>
std::optional<bool> OptionalAccessor::Convert<bool>()
{
    if (IsUndefined()) {
        return std::nullopt;
    }

    ani_boolean aniValue;
    auto ret = env_->Object_CallMethodByName_Boolean(obj_, "toBoolean", nullptr, &aniValue);
    if (ret != ANI_OK) {
        return std::nullopt;
    }

    auto value = static_cast<bool>(aniValue);

    return value;
}

template<>
std::optional<double> OptionalAccessor::Convert<double>()
{
    if (IsUndefined()) {
        return std::nullopt;
    }

    ani_double aniValue;
    auto ret = env_->Object_CallMethodByName_Double(obj_, "doubleValue", nullptr, &aniValue);
    if (ret != ANI_OK) {
        return std::nullopt;
    }

    auto value = static_cast<double>(aniValue);

    return value;
}

template<>
std::optional<std::string> OptionalAccessor::Convert<std::string>()
{
    if (IsUndefined()) {
        return std::nullopt;
    }

    ani_size strSize;
    env_->String_GetUTF8Size(static_cast<ani_string>(obj_), &strSize);

    std::vector<char> buffer(strSize + 1);
    char* utf8_buffer = buffer.data();

    ani_size bytes_written = 0;
    env_->String_GetUTF8(static_cast<ani_string>(obj_), utf8_buffer, strSize + 1, &bytes_written);

    utf8_buffer[bytes_written] = '\0';
    std::string content = std::string(utf8_buffer);

    return content;
}

ani_object DoubleToObject(ani_env *env, double value)
{
    ani_object aniObject = nullptr;
    ani_double doubleValue = static_cast<ani_double>(value);
    const char *className = "std.core.Double";
    ani_class aniClass;
    if (ANI_OK != env->FindClass(className, &aniClass)) {
        LOG_ERROR("Not found '%{public}s'.", className);
        return aniObject;
    }
    ani_method personInfoCtor;
    if (ANI_OK != env->Class_FindMethod(aniClass, "<ctor>", "d:", &personInfoCtor)) {
        LOG_ERROR("Class_GetMethod Failed '%{public}s <ctor>.'", className);
        return aniObject;
    }

    if (ANI_OK != env->Object_New(aniClass, personInfoCtor, &aniObject, doubleValue)) {
        LOG_ERROR("Object_New Failed '%{public}s. <ctor>", className);
        return aniObject;
    }
    return aniObject;
}

ani_object BoolToObject(ani_env *env, bool value)
{
    ani_object aniObject = nullptr;
    ani_boolean boolValue = static_cast<bool>(value);
    const char *className = "std.core.Boolean";
    ani_class aniClass;
    if (ANI_OK != env->FindClass(className, &aniClass)) {
        LOG_ERROR("Not found '%{public}s.'", className);
        return aniObject;
    }

    ani_method personInfoCtor;
    if (ANI_OK != env->Class_FindMethod(aniClass, "<ctor>", "z:", &personInfoCtor)) {
        LOG_ERROR("Class_GetMethod Failed '%{public}s' <ctor>.", className);
        return aniObject;
    }

    if (ANI_OK != env->Object_New(aniClass, personInfoCtor, &aniObject, boolValue)) {
        LOG_ERROR("Object_New Failed '%{public}s' <ctor>.", className);
    }

    return aniObject;
}

ani_object StringToObject(ani_env *env, std::string value)
{
    return static_cast<ani_object>(AniStringUtils::ToAni(env, value));
}

ani_object Uint8ArrayToObject(ani_env *env, const std::vector<uint8_t> &values)
{
    ani_object aniObject = nullptr;
    ani_class arrayClass;
    if (values.size() == 0) {
        LOG_ERROR("values is empty");
        return aniObject;
    }
    ani_status retCode = env->FindClass("escompat.Uint8Array", &arrayClass);
    if (retCode != ANI_OK) {
        LOG_ERROR("Failed: env->FindClass()");
        return aniObject;
    }

    ani_method arrayCtor;
    retCode = env->Class_FindMethod(arrayClass, "<ctor>", "i:", &arrayCtor);
    if (retCode != ANI_OK) {
        LOG_ERROR("Failed: env->Class_FindMethod()");
        return aniObject;
    }

    auto valueSize = values.size();
    retCode = env->Object_New(arrayClass, arrayCtor, &aniObject, valueSize);
    if (retCode != ANI_OK) {
        LOG_ERROR("Failed: env->Object_New()");
        return aniObject;
    }

    ani_ref buffer;
    env->Object_GetFieldByName_Ref(aniObject, "buffer", &buffer);
    void *bufData;
    size_t bufLength;
    retCode = env->ArrayBuffer_GetInfo(static_cast<ani_arraybuffer>(buffer), &bufData, &bufLength);
    if (retCode != ANI_OK) {
        LOG_INFO("Failed: env->ArrayBuffer_GetInfo()");
    }

    auto ret = memcpy_s(bufData, bufLength, values.data(), values.size());
    if (ret != 0) {
        return nullptr;
    }

    return aniObject;
}  // namespace DataShare
}  // namespace OHOS
