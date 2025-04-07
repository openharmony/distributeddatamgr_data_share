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
#include "uri.h"
#include "datashare_predicates.h"
#include "datashare_template.h"
#include "ani_base_context.h"
#include "ani_datashare_helper.h"
#include "datashare_business_error.h"

using namespace OHOS;
using namespace OHOS::DataShare;
using Uri = OHOS::Uri;

static std::map<std::string, std::list<sptr<ANIDataShareObserver>>> observerMap_;

static bool getClass(ani_env *env, ani_class &cls)
{
    ani_namespace ns;
    static const char *spaceName = "L@ohos/data/dataShare/dataShare;";
    if (ANI_OK != env->FindNamespace(spaceName, &ns)) {
        LOG_ERROR("Not found space name '%{public}s'", spaceName);
        return false;
    }

    const char *className = "LDataShareHelperInner;";
    if (ANI_OK != env->Namespace_FindClass(ns, className, &cls)) {
        LOG_ERROR("Not found class name '%{public}s'", className);
        return false;
    }

    return true;
}

static bool getUri(ani_env *env, ani_string uri, std::string &strUri)
{
    strUri = AniStringUtils::ToStd(env, uri);
    std::cout << "On print uri:" << strUri << std::endl;
    if (strUri.empty()) {
        LOG_ERROR("uri is empty");
        return false;
    }

    return true;
}

static bool getType(ani_env *env, ani_string type, std::string &strType)
{
    strType = AniStringUtils::ToStd(env, type);
    std::cout << "On print type:" << strType << std::endl;
    if (strType.empty()) {
        LOG_ERROR("type is empty");
        return false;
    }

    return true;
}

static bool getEvent(ani_env *env, ani_string event, std::string &strEvent)
{
    strEvent = AniStringUtils::ToStd(env, event);
    std::cout << "On print event:" << strEvent << std::endl;
    if (strEvent.empty()) {
        LOG_ERROR("event is empty");
        return false;
    }

    return true;
}

static bool getDataShareHelper(ani_env *env, ani_object object, DataShareHelper *&helper)
{
    ani_long nativePtr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativePtr", &nativePtr)) {
        LOG_ERROR("nativePtr is nullptr");
        return false;
    }

    helper = reinterpret_cast<DataShareHelper *>(nativePtr);
    if (helper == nullptr) {
        LOG_ERROR("dataShareHelper is nullptr");
        return false;
    }

    return true;
}

static ani_object ANI_Create([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_object context, ani_string uri, ani_object options)
{
    std::cout << "ANI_Create enter" << std::endl;
    LOG_INFO("ANI_Create enter");

    std::string strUri;
    ani_class cls;
    if (!getUri(env, uri, strUri) || !getClass(env, cls)) {
        return {};
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        LOG_ERROR("Get ctor Failed");
        return {};
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(options, &isUndefined)) {
        LOG_ERROR("Get is undefined Failed");
        return {};
    }

    auto ctx = AbilityRuntime::GetStageModeContext(env, context);
    std::shared_ptr<DataShareHelper> dataShareHelper;
    if (isUndefined) {
        dataShareHelper = DataShareHelper::Creator(ctx->GetToken(), strUri);
    } else {
        CreateOptions opts = {
            OptionalAccessor(env, options).Convert<bool>().value_or(false),
            ctx->GetToken(),
            Uri(strUri).GetScheme() == "datashareproxy",
        };

        dataShareHelper = DataShareHelper::Creator(strUri, opts);
    }
    LOG_INFO("DataShareHelper::Creator complete");

    ani_object dataShareObj;
    if (ANI_OK != env->Object_New(cls, ctor, &dataShareObj, reinterpret_cast<ani_long>(dataShareHelper.get()))) {
        LOG_ERROR("Create Object Failed");
        return {};
    }

    return dataShareObj;
}

static void ANI_On(ani_env *env, ani_object object, ani_string type, ani_string uri)
{
    std::string strType;
    std::string strUri;
    ani_class cls;
    DataShareHelper *helper = nullptr;
    if (!getType(env, type, strType) || !getUri(env, uri, strUri) || !getClass(env, cls) ||
        !getDataShareHelper(env, object, helper)) {
        return;
    }

    ani_method callback;
    const char *methodName = "LonCallback;";
    if (ANI_OK != env->Class_FindMethod(cls, methodName, nullptr, &callback)) {
        LOG_ERROR("Not found method name '%{public}s'", methodName);
        return;
    }

    if (strType == "dataChange") {
        observerMap_.try_emplace(strUri);

        auto &list = observerMap_.find(strUri)->second;
        for (auto &it : list) {
            if (callback == it->observer_->GetCallback()) {
                LOG_ERROR("The observer has already subscribed.");
                return;
            }
        }

        auto innerObserver = std::make_shared<ANIInnerObserver>(env, cls, callback);
        sptr<ANIDataShareObserver> observer(new (std::nothrow) ANIDataShareObserver(innerObserver));
        if (observer == nullptr) {
            LOG_ERROR("observer is nullptr");
            return;
        }

        helper->RegisterObserverExt(Uri(strUri),
            std::shared_ptr<DataShareObserver>(observer.GetRefPtr(), [holder = observer](const auto*) {}), false);
        list.push_back(observer);
    }
}

static void ANI_Off(ani_env *env, ani_object object, ani_string type, ani_string uri)
{
    std::string strType;
    std::string strUri;
    ani_class cls;
    DataShareHelper *helper = nullptr;
    if (!getType(env, type, strType) || !getUri(env, uri, strUri) || !getClass(env, cls) ||
        !getDataShareHelper(env, object, helper)) {
        return;
    }

    ani_method callback;
    const char *methodName = "LoffCallback;";
    if (ANI_OK != env->Class_FindMethod(cls, methodName, nullptr, &callback)) {
        LOG_ERROR("Not found method name '%{public}s'", methodName);
        return;
    }

    if (strType == "dataChange") {
        auto obs = observerMap_.find(strUri);
        if (obs == observerMap_.end()) {
            LOG_ERROR("this uri hasn't been registered");
            return;
        }

        auto &list = obs->second;
        auto it = list.begin();
        while (it != list.end()) {
            if (callback != (*it)->observer_->GetCallback()) {
                ++it;
                continue;
            }

            helper->UnregisterObserverExt(Uri(strUri),
                std::shared_ptr<DataShareObserver>((*it).GetRefPtr(), [holder = *it](const auto*) {}));
            it = list.erase(it);
            break;
        }

        if (list.empty()) {
            observerMap_.erase(strUri);
        }
    }
}

static void ANI_OnEvent(ani_env *env, ani_object object, ani_string event, ani_int enumIndex, ani_string uri)
{
    std::string strEvent;
    std::string strUri;
    ani_class cls;
    DataShareHelper *helper = nullptr;
    if (!getEvent(env, event, strEvent) || !getUri(env, uri, strUri) || !getClass(env, cls) ||
        !getDataShareHelper(env, object, helper)) {
        return;
    }

    ani_method callback;
    const char *methodName = "LonEventCallback;";
    if (ANI_OK != env->Class_FindMethod(cls, methodName, nullptr, &callback)) {
        LOG_ERROR("Not found method name '%{public}s'", methodName);
        return;
    }

    if (strEvent == "dataChange") {
        observerMap_.try_emplace(strUri);

        auto &list = observerMap_.find(strUri)->second;
        for (auto &it : list) {
            if (callback == it->observer_->GetCallback()) {
                LOG_ERROR("The observer has already subscribed.");
                return;
            }
        }

        auto innerObserver = std::make_shared<ANIInnerObserver>(env, cls, callback);
        sptr<ANIDataShareObserver> observer(new (std::nothrow) ANIDataShareObserver(innerObserver));
        if (observer == nullptr) {
            LOG_ERROR("observer is nullptr");
            return;
        }

        helper->RegisterObserverExt(Uri(strUri),
            std::shared_ptr<DataShareObserver>(observer.GetRefPtr(), [holder = observer](const auto*) {}), false);
        list.push_back(observer);
    }
}

static void ANI_OffEvent(ani_env *env, ani_object object, ani_string event, ani_int enumIndex, ani_string uri)
{
    std::string strEvent;
    std::string strUri;
    ani_class cls;
    DataShareHelper *helper = nullptr;
    if (!getEvent(env, event, strEvent) || !getUri(env, uri, strUri) || !getClass(env, cls) ||
        !getDataShareHelper(env, object, helper)) {
        return;
    }

    ani_method callback;
    const char *methodName = "LoffEventCallback;";
    if (ANI_OK != env->Class_FindMethod(cls, methodName, nullptr, &callback)) {
        LOG_ERROR("Not found method name '%{public}s'", methodName);
        return;
    }

    if (strEvent == "dataChange") {
        auto obs = observerMap_.find(strUri);
        if (obs == observerMap_.end()) {
            LOG_ERROR("this uri hasn't been registered");
            return;
        }

        auto &list = obs->second;
        auto it = list.begin();
        while (it != list.end()) {
            if (callback != (*it)->observer_->GetCallback()) {
                ++it;
                continue;
            }

            helper->UnregisterObserverExt(Uri(strUri),
                std::shared_ptr<DataShareObserver>((*it).GetRefPtr(), [holder = *it](const auto*) {}));
            it = list.erase(it);
            break;
        }

        if (list.empty()) {
            observerMap_.erase(strUri);
        }
    }
}

static ani_object ANI_Query(ani_env *env, ani_object object, ani_string uri, ani_object predicates, ani_object columns)
{
    std::string strUri;
    ani_class cls;
    DataShareHelper *helper = nullptr;
    if (!getUri(env, uri, strUri) || !getClass(env, cls) || !getDataShareHelper(env, object, helper)) {
        return {};
    }

    auto pred = AniObjectUtils::Unwrap<DataSharePredicates>(env, predicates);
    std::cout << "Print data share predicates order:" << pred->GetOrder() << std::endl;
    if (pred == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr : %{public}d", pred == nullptr);
        return {};
    }

    std::vector<std::string> strings;
    UnionAccessor unionAccessor(env, columns);
    if (!unionAccessor.TryConvertArray<std::string>(strings)) {
        LOG_ERROR("TryConvertArray columns Failed");
        return {};
    }

    Uri uriObj(strUri);
    DatashareBusinessError businessError;
    auto resultObject = helper->Query(uriObj, *pred, strings, &businessError);
    if (resultObject == nullptr) {
        LOG_ERROR("query failed, result is null!");
        return {};
    }

    if (businessError.GetCode() != 0) {
        LOG_ERROR("query failed, errorCode : %{public}d", businessError.GetCode());
        return {};
    }

    const char *className = "LDataShareResultSetImp;";
    if (ANI_OK != env->FindClass(className, &cls)) {
        LOG_ERROR("Not found class name '%{public}s'", className);
        return {};
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        LOG_ERROR("Get ctor Failed");
        return {};
    }

    ani_object resultSetObj = {};
    if (ANI_OK != env->Object_New(cls, ctor, &resultSetObj, reinterpret_cast<ani_long>(resultObject.get()))) {
        LOG_ERROR("Create Object Failed");
        return {};
    }

    return resultSetObj;
}

using BucketMap = std::map<std::string, DataShareValueObject::Type>;
using BucketPtr = std::shared_ptr<BucketMap>;
using CallbackType = convertCallback<BucketMap>;

auto g_convertValuesBucket = [](ani_env *env, ani_ref &ani_key, ani_ref &object, BucketPtr records) {
    auto key = AniStringUtils::ToStd(env, static_cast<ani_string>(ani_key));
    auto unionObject = static_cast<ani_object>(object);
    UnionAccessor unionAccessor(env, unionObject);
    if (unionAccessor.IsInstanceOf("Lstd/core/Double;")) {
        double value;
        unionAccessor.TryConvert<double>(value);
        records->emplace(key, value);
        std::cout << "Object is double Content:" << value << std::endl;
        return true;
    }

    if (unionAccessor.IsInstanceOf("Lstd/core/String;")) {
        std::string value;
        unionAccessor.TryConvert<std::string>(value);
        records->emplace(key, value);
        std::cout << "Object is String Object Content:" << value.c_str() << std::endl;
        return true;
    }

    if (unionAccessor.IsInstanceOf("Lstd/core/Boolean;")) {
        bool value;
        unionAccessor.TryConvert<bool>(value);
        records->emplace(key, value);
        std::cout << "Object is Boolean Content:" << value << std::endl;
        return true;
    }

    std::vector<uint8_t> arrayUint8Values;
    if (unionAccessor.TryConvertArray<uint8_t>(arrayUint8Values)) {
        records->emplace(key, arrayUint8Values);
        std::cout << "Object is uint8_t array Content" << std::endl;
        return true;
    }

    if (OptionalAccessor(env, unionObject).IsNull()) {
        records->emplace(key, nullptr);
        std::cout << "Object is null" << std::endl;
        return true;
    }

    std::cout << "Unexpected object type" << std::endl;
    return false;
};

static ani_int ANI_Update(ani_env *env, ani_object object, ani_string uri, ani_object predicates, ani_object value)
{
    std::string strUri;
    ani_class cls;
    DataShareHelper *helper = nullptr;
    if (!getUri(env, uri, strUri) || !getClass(env, cls) || !getDataShareHelper(env, object, helper)) {
        return ani_int(DATA_SHARE_ERROR);
    }

    auto pred = AniObjectUtils::Unwrap<DataSharePredicates>(env, predicates);
    std::cout << "Print data share predicates order:" << pred->GetOrder() << std::endl;
    if (pred == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr : %{public}d", pred == nullptr);
        return ani_int(DATA_SHARE_ERROR);
    }

    auto bucket = std::make_shared<BucketMap>();
    if (!forEachMapEntry<CallbackType, BucketMap>(env, value, g_convertValuesBucket, bucket)) {
        std::cout << "Is bucket null:" << bucket->empty() << std::endl;
        return ani_int(DATA_SHARE_ERROR);
    }

    Uri uriObj(strUri);
    DataShareValuesBucket dataShareValuesBucket(*bucket);
    return static_cast<ani_int>(helper->Update(uriObj, *pred, dataShareValuesBucket));
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    std::cout << "ANI_Constructor start" << std::endl;

    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        LOG_ERROR("Unsupported ANI_VERSION_1");
        return ANI_ERROR;
    }

    ani_namespace ns;
    static const char *spaceName = "L@ohos/data/dataShare/dataShare;";
    if (ANI_OK != env->FindNamespace(spaceName, &ns)) {
        LOG_ERROR("Not found space name '%{public}s'", spaceName);
        return ANI_ERROR;
    }

    std::array functions = {
        ani_native_function {"native_create", nullptr, reinterpret_cast<void *>(ANI_Create)},
        ani_native_function {"native_on", nullptr, reinterpret_cast<void *>(ANI_On)},
        ani_native_function {"native_off", nullptr, reinterpret_cast<void *>(ANI_Off)},
        ani_native_function {"native_onEvent", nullptr, reinterpret_cast<void *>(ANI_OnEvent)},
        ani_native_function {"native_offEvent", nullptr, reinterpret_cast<void *>(ANI_OffEvent)},
        ani_native_function {"native_query", nullptr, reinterpret_cast<void *>(ANI_Query)},
        ani_native_function {"native_update", nullptr, reinterpret_cast<void *>(ANI_Update)},
    };

    if (ANI_OK != env->Namespace_BindNativeFunctions(ns, functions.data(), functions.size())) {
        LOG_ERROR("Cannot bind native functions to '%{public}s'", spaceName);
        return ANI_ERROR;
    }

    *result = ANI_VERSION_1;
    return ANI_OK;
}