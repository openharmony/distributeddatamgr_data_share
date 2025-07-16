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

#include <securec.h>
#include <chrono>
#include <cinttypes>

#include "ani_utils.h"
#include "datashare_valuebucket_convert.h"
#include "ani_datashare_observer.h"

namespace OHOS {
namespace DataShare {
using namespace std::chrono;
ANIInnerObserver::ANIInnerObserver(ani_vm *vm, ani_ref callback) : vm_(vm), callback_(callback)
{
}

ANIInnerObserver::~ANIInnerObserver()
{
    vm_->DetachCurrentThread();
}

ani_object ANIInnerObserver::Convert2TSValue(ani_env *env, const std::monostate &value)
{
    (void)env;
    (void)value;
    return nullptr;
}

ani_object ANIInnerObserver::Convert2TSValue(ani_env *env, const std::string &value)
{
    return StringToObject(env, value);
}

ani_object ANIInnerObserver::Convert2TSValue(ani_env *env, const std::vector<uint8_t> &values)
{
    return Uint8ArrayToObject(env, values);
}

ani_object ANIInnerObserver::Convert2TSValue(ani_env *env, int64_t value)
{
    return DoubleToObject(env, value);
}

ani_object ANIInnerObserver::Convert2TSValue(ani_env *env, double value)
{
    return DoubleToObject(env, value);
}

ani_object ANIInnerObserver::Convert2TSValue(ani_env *env, bool value)
{
    return BoolToObject(env, value);
}

template<typename _VTp>
ani_object ANIInnerObserver::ReadVariant(ani_env *env, size_t step, size_t index, const _VTp &value)
{
    return nullptr;
}

template<typename _VTp, typename _First, typename ..._Rest>
ani_object ANIInnerObserver::ReadVariant(ani_env *env, size_t step, size_t index, const _VTp &value)
{
    if (env == nullptr) {
        LOG_ERROR("env is nullptr %{public}s", __func__);
        return nullptr;
    }
    if (step == index) {
        auto *realValue = std::get_if<_First>(&value);
        if (realValue == nullptr) {
            return nullptr;
        }

        return Convert2TSValue(env, *realValue);
    }

    return ReadVariant<_VTp, _Rest...>(env, step + 1, index, value);
}

template<class... Types>
ani_object ANIInnerObserver::Convert2TSValue(ani_env *env, const std::variant<Types...> &value)
{
    return ReadVariant<decltype(value), Types...>(env, 0, value.index(), value);
}

ani_object ANIInnerObserver::Convert2TSValue(ani_env *env, const DataShareValuesBucket &valueBucket)
{
    ani_object valuesBucketList = nullptr;
    static const char *className = "Lescompat/Record;";

    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        LOG_ERROR("Not found '%{public}s'.", className);
        return valuesBucketList;
    }

    ani_method aniCtor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", ":V", &aniCtor)) {
        LOG_ERROR("Class_GetMethod <ctor> Failed '%{public}s'.", className);
        return valuesBucketList;
    }

    if (ANI_OK != env->Object_New(cls, aniCtor, &valuesBucketList)) {
        LOG_ERROR("Object_New Failed '%{public}s'.", className);
        return valuesBucketList;
    }

    ani_method setter;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", nullptr, &setter)) {
        LOG_ERROR("Class_GetMethod set Failed '%{public}s'.", className);
        return valuesBucketList;
    }

    const auto &valuesMap = valueBucket.valuesMap;
    for (auto& value : valuesMap) {
        ani_string aniKey = AniStringUtils::ToAni(env, value.first);
        ani_object aniObjVal = Convert2TSValue(env, value.second);
        if (ANI_OK != env->Object_CallMethod_Void(valuesBucketList, setter, aniKey, aniObjVal)) {
            LOG_ERROR("Object_CallMethodByName_Void  $_set Faild ");
            break;
        }
    }

    return valuesBucketList;
}

template<typename T>
ani_object ANIInnerObserver::Convert2TSValue(ani_env *env, const std::vector<T> &values)
{
    ani_class arrayCls;
    if (ANI_OK != env->FindClass("Lescompat/Array;", &arrayCls)) {
        LOG_ERROR("FindClass Lescompat/Array; Failed");
        return nullptr;
    }

    ani_method arrayCtor;
    if (ANI_OK != env->Class_FindMethod(arrayCls, "<ctor>", "I:V", &arrayCtor)) {
        LOG_ERROR("Class_FindMethod <ctor> Failed");
        return nullptr;
    }

    ani_object arrayObj;
    if (ANI_OK != env->Object_New(arrayCls, arrayCtor, &arrayObj, values.size())) {
        LOG_ERROR("Object_New Array Faild");
        return nullptr;
    }

    ani_size index = 0;
    for (auto value : values) {
        if (ANI_OK != env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index,
            Convert2TSValue(env, value))) {
            LOG_ERROR("Object_CallMethodByName_Void  $_set Faild ");
            break;
        }

        index++;
    }

    return arrayObj;
}

ani_enum_item ANIInnerObserver::GetEnumItem(ani_env *env, int32_t type)
{
    ani_enum aniEnum{};
    const char *enumName = "@ohos.data.dataShare.dataShare.ChangeType";
    if (ANI_OK != env->FindEnum(enumName, &aniEnum)) {
        LOG_ERROR("Not found '%{public}s'", enumName);
        return nullptr;
    }

    int32_t index = 0U;
    ani_enum_item enumItem{};
    while (ANI_OK == env->Enum_GetEnumItemByIndex(aniEnum, index++, &enumItem)) {
        ani_int intValue = -1;
        if (ANI_OK != env->EnumItem_GetValue_Int(enumItem, &intValue)) {
            LOG_ERROR("EnumItem_GetValue_Int failed.");
            return nullptr;
        }

        if (intValue == type) {
            return enumItem;
        }
    }

    LOG_ERROR("Get enumItem by %{public}d failed.", type);
    return nullptr;
}

ani_object ANIInnerObserver::GetNewChangeInfo(ani_env *env)
{
    if (env == nullptr) {
        LOG_ERROR("env is nullptr %{public}s", __func__);
        return nullptr;
    }

    ani_class cls;
    const char *className = "@ohos.data.dataShare.dataShare.ChangeInfoInner";
    if (ANI_OK != env->FindClass(className, &cls)) {
        LOG_ERROR("Not found class name '%{public}s'", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        LOG_ERROR("Get ctor Failed");
        return nullptr;
    }

    ani_object object;
    if (ANI_OK != env->Object_New(cls, ctor, &object)) {
        LOG_ERROR("Create GetNewChangeInfo Object Failed");
        return nullptr;
    }

    return object;
}

ani_object ANIInnerObserver::Convert2TSValue(ani_env *env, const DataShareObserver::ChangeInfo& changeInfo)
{
    ani_object infoObj = GetNewChangeInfo(env);
    if (infoObj == nullptr) {
        LOG_ERROR("Create ts new ChangeInfo object failed!");
        return nullptr;
    }

    ani_enum_item aniType = GetEnumItem(env, ANIDataShareObserver::UPDATE);
    if (aniType == nullptr) {
        LOG_ERROR("GetEnumItem failed");
        return nullptr;
    }

    ani_status status = env->Object_SetPropertyByName_Ref(infoObj, "type", aniType);
    if (ANI_OK != status) {
        LOG_ERROR("Object_SetFieldByName_Int failed status : %{public}d", status);
        return nullptr;
    }

    ani_string uri;
    env->String_NewUTF8(changeInfo.uris_.front().ToString().c_str(), changeInfo.uris_.front().ToString().size(), &uri);
    status = env->Object_SetPropertyByName_Ref(infoObj, "uri", uri);
    if (ANI_OK != status) {
        LOG_ERROR("Object_SetPropertyByName_Ref failed status : %{public}d", status);
        return nullptr;
    }

    auto &valBucket = const_cast<DataShareObserver::ChangeInfo &>(changeInfo);
    std::vector<DataShareValuesBucket> VBuckets = ValueProxy::Convert(std::move(valBucket.valueBuckets_));
    ani_object valueBuckets = Convert2TSValue(env, VBuckets);
    status = env->Object_SetPropertyByName_Ref(infoObj, "values", valueBuckets);
    if (ANI_OK != status) {
        LOG_ERROR("Object_SetPropertyByName_Ref failed status : %{public}d", status);
        return nullptr;
    }

    return infoObj;
}

void ANIInnerObserver::OnChange(const DataShareObserver::ChangeInfo& changeInfo, bool isNotifyDetails)
{
    auto time = static_cast<uint64_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    LOG_INFO("ANIInnerObserver datashare callback start, times %{public}" PRIu64 ".", time);
    if (callback_ == nullptr) {
        LOG_ERROR("callback_ is nullptr");
        return;
    }

    ani_env *env = nullptr;
    ani_options aniArgs {0, nullptr};
    if (ANI_ERROR == vm_->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env)) {
        if (ANI_OK != vm_->GetEnv(ANI_VERSION_1, &env)) {
            LOG_ERROR("GetEnv failed");
            return;
        }
    }

    ani_ref result;
    auto fnObj = reinterpret_cast<ani_fn_object>(callback_);
    if (fnObj == nullptr) {
        LOG_ERROR("%{public}s: fnObj == nullptr", __func__);
        return;
    }

    std::vector<ani_ref> args = { nullptr };
    if (isNotifyDetails) {
        auto argsObj = Convert2TSValue(env, changeInfo);
        if (argsObj == nullptr) {
            LOG_ERROR("%{public}s: argsObj is null", __func__);
            return;
        }

        args.push_back(argsObj);
    }

    ani_status callStatus = env->FunctionalObject_Call(fnObj, args.size(), args.data(), &result);
    if (ANI_OK != callStatus) {
        LOG_ERROR("ani_call_function failed status : %{public}d", callStatus);
        return;
    }

    LOG_INFO("ANIInnerObserver datashare callback end, times %{public}" PRIu64 ".", time);
}

ani_ref ANIInnerObserver::GetCallback()
{
    return callback_;
}
}  // namespace DataShare
}  // namespace OHOS