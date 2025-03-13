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

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>
#include <type_traits>

template<typename T>
class NativeObjectWrapper {
public:
    static ani_long Create([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_class clazz)
    {
        T* nativePtr = new T;
        return reinterpret_cast<ani_long>(nativePtr);
    }

    static T* Unwrap(ani_env *env, ani_object object, const char* propName = "nativePtr")
    {
        ani_long nativePtr;
        if (ANI_OK != env->Object_GetFieldByName_Long(object, propName, &nativePtr)) {
            return nullptr;
        }
        return reinterpret_cast<T*>(nativePtr);
    }
};

class NativeObject {
public:
    virtual ~NativeObject() = default;
};

template<typename T>
class NativeObjectAdapter : public NativeObject {
public:
    explicit NativeObjectAdapter()
    {
        obj_ = std::make_shared<T>();
    }

    std::shared_ptr<T> Get()
    {
        return obj_;
    }

private:
    std::shared_ptr<T> obj_;
};

class NativeObjectManager {
public:
    static NativeObjectManager& GetInstance()
    {
        static NativeObjectManager instance;
        return instance;
    }

    template<typename T>
    std::shared_ptr<T> Get(ani_object &object)
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);

        auto iter = aniToNativeObjMapper_.find(object);
        if (iter != aniToNativeObjMapper_.end()) {
            return std::static_pointer_cast<NativeObjectAdapter<T>>(iter->second)->Get();
        }
        auto nativeObj = std::make_shared<NativeObjectAdapter<T>>();
        aniToNativeObjMapper_.emplace(object, nativeObj);
        return nativeObj->Get();
    }

private:
    std::unordered_map<ani_object, std::shared_ptr<NativeObject>> aniToNativeObjMapper_;
    std::mutex mutex_;
};

std::string ANIUtils_ANIStringToStdString(ani_env *env, ani_string ani_str)
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

bool ANIUtils_UnionIsInstanceOf(ani_env *env, ani_object union_obj, std::string& cls_name)
{
    ani_class cls;
    env->FindClass(cls_name.c_str(), &cls);

    ani_boolean ret;
    env->Object_InstanceOf(union_obj, cls, &ret);
    return ret;
}

class UnionAccessor {
public:
    UnionAccessor(ani_env *env, ani_object &obj) : env_(env), obj_(obj) {}

    bool IsInstanceOf(const std::string& cls_name)
    {
        ani_class cls;
        env_->FindClass(cls_name.c_str(), &cls);

        ani_boolean ret;
        env_->Object_InstanceOf(obj_, cls, &ret);
        return ret;
    }

    bool IsInstanceOf(std::string&& cls_name, ani_object obj)
    {
        ani_class cls;
        env_->FindClass(cls_name.c_str(), &cls);

        ani_boolean ret;
        env_->Object_InstanceOf(obj, cls, &ret);
        return ret;
    }

    template<typename T>
    bool TryConvert(T &value);

    template<typename T>
    bool TryConvertArray(std::vector<T> &value);

private:
    template<typename T>
    bool TryUnbox(ani_ref &ref, T &value);

private:
    ani_env *env_;
    ani_object obj_;
};

template<>
bool UnionAccessor::TryConvert<bool>(bool &value)
{
    if (!IsInstanceOf("Lstd/core/Boolean;")) {
        return false;
    }
    ani_boolean aniValue;
    auto ret = env_->Object_CallMethodByName_Boolean(obj_, "booleanValue", nullptr, &aniValue);
    if (ret != ANI_OK) {
        return false;
    }
    value = static_cast<bool>(aniValue);
    if (ret != ANI_OK) {
        return false;
    }
    value = static_cast<bool>(aniValue);
    return true;
}

template<>
bool UnionAccessor::TryConvert<int>(int &value)
{
    if (!IsInstanceOf("Lstd/core/Int;")) {
        return false;
    }
    ani_int aniValue;
    auto ret = env_->Object_CallMethodByName_Int(obj_, "unboxed", nullptr, &aniValue);
    if (ret != ANI_OK) {
        return false;
    }
    value = static_cast<int>(aniValue);
    return true;
}

template<>
bool UnionAccessor::TryConvert<double>(double &value)
{
    if (!IsInstanceOf("Lstd/core/Double;")) {
        return false;
    }
    ani_double aniValue;
    auto ret = env_->Object_CallMethodByName_Double(obj_, "unboxed", nullptr, &aniValue);
    if (ret != ANI_OK) {
        return false;
    }
    value = static_cast<double>(aniValue);
    return true;
}

template<>
bool UnionAccessor::TryConvert<ani_string>(ani_string &value)
{
    if (!IsInstanceOf("Lstd/core/String;")) {
        return false;
    }
    value = static_cast<ani_string>(obj_);
    return true;
}

template<>
bool UnionAccessor::TryConvert<std::string>(std::string &value)
{
    if (!IsInstanceOf("Lstd/core/String;")) {
        return false;
    }
    value = ANIUtils_ANIStringToStdString(env_, static_cast<ani_string>(obj_));
    return true;
}

template<>
bool UnionAccessor::TryConvertArray<bool>(std::vector<bool> &value)
{
    ani_double length;
    if (ANI_OK != env_->Object_GetPropertyByName_Double(obj_, "length", &length)) {
        std::cerr << "Object_GetPropertyByName_Double length Failed" << std::endl;
        return false;
    }
    for (int i = 0; i < int(length); i++) {
        ani_ref arrayRef;
        if (ANI_OK != env_->Object_CallMethodByName_Ref(obj_, "$_get", "I:Lstd/core/Object;", &arrayRef, (ani_int)i)) {
            std::cerr << "Object_GetPropertyByName_Ref Failed" << std::endl;
            return false;
        }
        if (!IsInstanceOf("Lstd/core/Boolean;", static_cast<ani_object>(arrayRef))) {
            std::cerr << "Not found 'Lstd/core/Boolean;'" << std::endl;
            return false;
        }
        ani_boolean boolValue;
        if (ANI_OK != env_->Object_CallMethodByName_Boolean(static_cast<ani_object>(arrayRef), "unboxed", nullptr,
            &boolValue)) {
            std::cerr << "Object_CallMethodByName_Boolean unbox Failed" << std::endl;
            return false;
        }
        value.push_back(static_cast<double>(boolValue));
    }
    return true;
}

template<>
bool UnionAccessor::TryConvertArray<int>(std::vector<int> &value)
{
    ani_double length;
    if (ANI_OK != env_->Object_GetPropertyByName_Double(obj_, "length", &length)) {
        std::cerr << "Object_GetPropertyByName_Double length failed" << std::endl;
        return false;
    }
    for (int i = 0; i < int(length); i++) {
        ani_ref arrayRef;
        if (ANI_OK != env_->Object_CallMethodByName_Ref(obj_, "$_get", "I:Lstd/core/Object;", &arrayRef, (ani_int)i)) {
            std::cerr << "Object_GetPropertyByName_Ref failed" << std::endl;
            return false;
        }
        if (!IsInstanceOf("Lstd/core/Int;", static_cast<ani_object>(arrayRef))) {
            std::cerr << "Not found 'Lstd/core/Double;'" << std::endl;
            return false;
        }
        ani_int intValue;
        if (ANI_OK != env_->Object_CallMethodByName_Int(static_cast<ani_object>(arrayRef), "unboxed", nullptr,
            &intValue)) {
            std::cerr << "Object_CallMethodByName_Double unbox failed" << std::endl;
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
        std::cerr << "Object_GetPropertyByName_Double length Failed" << std::endl;
        return false;
    }
    for (int i = 0; i < int(length); i++) {
        ani_ref arrayRef;
        if (ANI_OK != env_->Object_CallMethodByName_Ref(obj_, "$_get", "I:Lstd/core/Object;", &arrayRef, (ani_int)i)) {
            std::cerr << "Object_GetPropertyByName_Ref Failed" << std::endl;
            return false;
        }
        if (!IsInstanceOf("Lstd/core/Double;", static_cast<ani_object>(arrayRef))) {
            std::cerr << "Not found 'Lstd/core/Double;'" << std::endl;
            return false;
        }
        ani_double doubleValue;
        if (ANI_OK != env_->Object_CallMethodByName_Double(static_cast<ani_object>(arrayRef), "unboxed",
            nullptr, &doubleValue)) {
            std::cerr << "Object_CallMethodByName_Double unbox Failed" << std::endl;
            return false;
        }
        value.push_back(static_cast<double>(doubleValue));
    }
    return true;
}

template<>
bool UnionAccessor::TryConvertArray<std::string>(std::vector<std::string> &value)
{
    ani_double length;
    if (ANI_OK != env_->Object_GetPropertyByName_Double(obj_, "length", &length)) {
        std::cerr << "Object_GetPropertyByName_Double length failed" << std::endl;
        return false;
    }

    for (int i = 0; i < int(length); i++) {
        ani_ref stringEntryRef;
        if (ANI_OK != env_->Object_CallMethodByName_Ref(obj_, "$_get", "I:Lstd/core/Object;", &stringEntryRef,
            (ani_int)i)) {
            std::cerr << "Object_GetPropertyByName_Double length failed" << std::endl;
            return false;
        }

        if (!IsInstanceOf("Lstd/core/String;", static_cast<ani_object>(stringEntryRef))) {
            std::cerr << "Not found 'Lstd/core/String;'" << std::endl;
            return false;
        }
        value.push_back(ANIUtils_ANIStringToStdString(env_, static_cast<ani_string>(stringEntryRef)));
    }
    return true;
}

template<typename T>
std::vector<std::string> convertVector(const std::vector<T>& input)
{
    std::vector<std::string> result;
    result.reserve(input.size());

    for (const auto& elem : input) {
        if constexpr (std::is_same_v<T, bool>) {
            result.push_back(elem ? "true" : "false");
            continue;
        }
        std::ostringstream oss;
        if constexpr (std::is_same_v<T, double>) {
            oss << elem;
            std::string str = oss.str();
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            if (str.back() == '.') str.pop_back();
            result.push_back(str);
        } else {
            oss << elem;
            result.push_back(oss.str());
        }
    }
    return result;
}
#endif