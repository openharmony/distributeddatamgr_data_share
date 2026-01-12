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

#include "uri.h"
#include "ani_utils.h"
#include "datashare_predicates.h"
#include "datashare_template.h"
#include "ani_base_context.h"
#include "ani_datashare_helper.h"
#include "datashare_business_error.h"
#include "ipc_skeleton.h"
#include "tokenid_kit.h"

using namespace OHOS;
using namespace OHOS::DataShare;
using namespace OHOS::Security::AccessToken;
using Uri = OHOS::Uri;

static constexpr int EXCEPTION_SYSTEMAPP_CHECK = 202;
static std::map<std::string, std::list<sptr<ANIDataShareObserver>>> observerMap_;

static bool IsSystemApp()
{
    uint64_t tokenId = IPCSkeleton::GetSelfTokenID();
    return TokenIdKit::IsSystemAppByFullTokenID(tokenId);
}

static void ThrowBusinessError(ani_env *env, int errCode, std::string&& errMsg)
{
    LOG_DEBUG("Begin ThrowBusinessError.");
    static const char *errorClsName = "@ohos.base.BusinessError";
    ani_class cls {};
    if (ANI_OK != env->FindClass(errorClsName, &cls)) {
        LOG_ERROR("find class BusinessError %{public}s failed", errorClsName);
        return;
    }
    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", ":", &ctor)) {
        LOG_ERROR("find method BusinessError.constructor failed");
        return;
    }
    ani_object errorObject;
    if (ANI_OK != env->Object_New(cls, ctor, &errorObject)) {
        LOG_ERROR("create BusinessError object failed");
        return;
    }
    ani_double aniErrCode = static_cast<ani_double>(errCode);
    ani_string errMsgStr;
    if (ANI_OK != env->String_NewUTF8(errMsg.c_str(), errMsg.size(), &errMsgStr)) {
        LOG_ERROR("convert errMsg to ani_string failed");
        return;
    }
    if (ANI_OK != env->Object_SetFieldByName_Double(errorObject, "code", aniErrCode)) {
        LOG_ERROR("set error code failed");
        return;
    }
    if (ANI_OK != env->Object_SetPropertyByName_Ref(errorObject, "message", errMsgStr)) {
        LOG_ERROR("set error message failed");
        return;
    }
    env->ThrowError(static_cast<ani_error>(errorObject));
    return;
}

static bool getNameSpace(ani_env *env, ani_namespace &ns)
{
    const char *spaceName = "@ohos.data.dataShare.dataShare";
    if (ANI_OK != env->FindNamespace(spaceName, &ns)) {
        LOG_ERROR("Not found space name '%{public}s'", spaceName);
        return false;
    }

    return true;
}

static bool getClass(ani_env *env, ani_class &cls)
{
    const char *className = "@ohos.data.dataShare.dataShare.DataShareHelperInner";
    if (ANI_OK != env->FindClass(className, &cls)) {
        LOG_ERROR("Not found class name '%{public}s'", className);
        return false;
    }

    return true;
}

static bool getUri(ani_env *env, ani_string uri, std::string &strUri)
{
    strUri = AniStringUtils::ToStd(env, uri);
    if (strUri.empty()) {
        LOG_ERROR("uri is empty");
        return false;
    }

    return true;
}

static bool getType(ani_env *env, ani_string type, std::string &strType)
{
    strType = AniStringUtils::ToStd(env, type);
    if (strType.empty()) {
        LOG_ERROR("type is empty");
        return false;
    }

    return true;
}

static bool getEvent(ani_env *env, ani_string event, std::string &strEvent)
{
    strEvent = AniStringUtils::ToStd(env, event);
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

    auto helperHolder = reinterpret_cast<SharedPtrHolder<DataShareHelper> *>(nativePtr);
    if (helperHolder == nullptr) {
        LOG_ERROR("SharedPtrHolder is nullptr");
        return false;
    }

    helper = helperHolder->Get().get();
    if (helper == nullptr) {
        LOG_ERROR("DataShareHelper is nullptr");
        return false;
    }

    return true;
}

static ani_status GetIsProxy(ani_env *env, ani_object options, bool &isProxy)
{
    ani_ref proxyObj;
    ani_status result = env->Object_GetPropertyByName_Ref(options, "isProxy", &proxyObj);
    if (ANI_OK != result) {
        LOG_ERROR("options Failed to get property named type: %{public}d", result);
        return result;
    }

    ani_boolean isUndefined;
    result = env->Reference_IsUndefined(static_cast<ani_object>(proxyObj), &isUndefined);
    if (ANI_OK != result) {
        LOG_ERROR("options Object_GetFieldByName_Ref isProxyField Failed");
        return result;
    }

    if (isUndefined) {
        LOG_ERROR("options isProxyField is Undefined Now");
        isProxy = false;
        return ANI_OK;
    }

    ani_boolean proxy;
    result = env->Object_CallMethodByName_Boolean(static_cast<ani_object>(proxyObj), "toBoolean", nullptr, &proxy);
    if (ANI_OK != result) {
        LOG_ERROR("options Failed to get property named isProxy: %{public}d", result);
        return result;
    }

    isProxy = proxy;
    return ANI_OK;
}

static std::shared_ptr<DataShareHelper> CreateDataShareHelper(ani_env *env, ani_object options, std::string &strUri,
    std::shared_ptr<AbilityRuntime::Context> &ctx)
{
    bool isProxy = false;
    if (GetIsProxy(env, options, isProxy) != ANI_OK) {
        LOG_ERROR("Get isProxy Failed");
        return nullptr;
    }
    CreateOptions opts = {
        isProxy,
        ctx->GetToken(),
        Uri(strUri).GetScheme() == "datashareproxy",
    };

    return DataShareHelper::Creator(strUri, opts);
}

static ani_object ANI_Create([[maybe_unused]] ani_env *env, ani_object context, ani_string uri, ani_object options)
{
    if (env == nullptr) {
        LOG_ERROR("env is nullptr %{public}s", __func__);
        return nullptr;
    }

    if (!IsSystemApp()) {
        ThrowBusinessError(env, EXCEPTION_SYSTEMAPP_CHECK, "not system app");
    }

    std::string strUri;
    ani_class cls;
    if (!getUri(env, uri, strUri) || !getClass(env, cls)) {
        return nullptr;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(options, &isUndefined)) {
        LOG_ERROR("Get is undefined Failed");
        return nullptr;
    }

    auto ctx = AbilityRuntime::GetStageModeContext(env, context);
    if (ctx == nullptr) {
        LOG_ERROR("Get Context Failed");
        return nullptr;
    }
    std::shared_ptr<DataShareHelper> dataShareHelper;
    if (isUndefined) {
        dataShareHelper = DataShareHelper::Creator(ctx->GetToken(), strUri);
    } else {
        dataShareHelper = CreateDataShareHelper(env, options, strUri, ctx);
    }

    if (dataShareHelper == nullptr) {
        LOG_ERROR("Create Object DataShareHelper is null");
        return nullptr;
    }

    auto shareptrData = new (std::nothrow) SharedPtrHolder<DataShareHelper>(dataShareHelper);
    if (shareptrData == nullptr) {
        LOG_ERROR("Create Object SharedPtrHolder is null");
        return nullptr;
    }

    const char *spaceName = "@ohos.data.dataShare.dataShare";
    const char *className = "DataShareHelperInner";
    ani_object dataShareObj = AniObjectUtils::Create(env, spaceName, className,
        reinterpret_cast<ani_long>(shareptrData));

    return dataShareObj;
}

static void ANI_OnType([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_string type, ani_string uri, ani_object callback)
{
    if (env == nullptr) {
        LOG_ERROR("env is nullptr %{public}s", __func__);
        return;
    }
    std::string strType;
    std::string strUri;
    DataShareHelper *helper = nullptr;
    if (!getType(env, type, strType) || !getUri(env, uri, strUri) || !getDataShareHelper(env, object, helper)) {
        return;
    }

    ani_ref callbackRef;
    if (ANI_OK != env->GlobalReference_Create(callback, &callbackRef)) {
        LOG_ERROR("Create callback failed");
        return;
    }

    if (strType == "dataChange") {
        observerMap_.try_emplace(strUri);

        auto &list = observerMap_.find(strUri)->second;
        for (auto &it : list) {
            if (callbackRef == it->observer_->GetCallback()) {
                LOG_ERROR("The observer has already subscribed.");
                return;
            }
        }

        ani_vm *vm = nullptr;
        if (ANI_OK != env->GetVM(&vm)) {
            LOG_ERROR("GetVM failed.");
            return;
        }

        auto innerObserver = std::make_shared<ANIInnerObserver>(vm, callbackRef);
        sptr<ANIDataShareObserver> observer(new (std::nothrow) ANIDataShareObserver(innerObserver));
        if (observer == nullptr) {
            LOG_ERROR("observer is nullptr");
            return;
        }

        helper->RegisterObserver(Uri(strUri), observer);
        list.push_back(observer);
    }
}

static void DataChangeOffEvent(ani_env *env, ani_ref &callbackRef, std::string &strUri, DataShareHelper *&helper)
{
    auto obs = observerMap_.find(strUri);
    if (obs == observerMap_.end()) {
        LOG_ERROR("this uri hasn't been registered");
        return;
    }

    auto &list = obs->second;
    auto it = list.begin();
    while (it != list.end()) {
        ani_boolean isEquals = false;
        if (ANI_OK != env->Reference_StrictEquals(callbackRef, (*it)->observer_->GetCallback(), &isEquals)) {
            LOG_ERROR("%{public}s: check observer equal failed!", __func__);
            return;
        }

        if (callbackRef != nullptr && !isEquals) {
            ++it;
            continue;
        }

        helper->UnregisterObserverExt(Uri(strUri),
            std::shared_ptr<DataShareObserver>((*it).GetRefPtr(), [holder = *it](const auto*) {}));
        it = list.erase(it);
    }

    if (list.empty()) {
        observerMap_.erase(strUri);
    }
}

static void DataChangeOffType(ani_env *env, ani_ref &callbackRef, std::string &strUri, DataShareHelper *&helper)
{
    auto obs = observerMap_.find(strUri);
    if (obs == observerMap_.end()) {
        LOG_ERROR("this uri hasn't been registered");
        return;
    }

    auto &list = obs->second;
    auto it = list.begin();
    while (it != list.end()) {
        ani_boolean isEquals = false;
        if (ANI_OK != env->Reference_StrictEquals(callbackRef, (*it)->observer_->GetCallback(), &isEquals)) {
            LOG_ERROR("%{public}s: check observer equal failed!", __func__);
            return;
        }

        if (callbackRef != nullptr && !isEquals) {
            ++it;
            continue;
        }

        helper->UnregisterObserver(Uri(strUri), *it);
        it = list.erase(it);
    }

    if (list.empty()) {
        observerMap_.erase(strUri);
    }
}

static void ANI_OffType([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_string type, ani_string uri, ani_object callback)
{
    if (env == nullptr) {
        LOG_ERROR("env is nullptr %{public}s", __func__);
        return;
    }
    std::string strType;
    std::string strUri;
    DataShareHelper *helper = nullptr;
    if (!getType(env, type, strType) || !getUri(env, uri, strUri) || !getDataShareHelper(env, object, helper)) {
        return;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(callback, &isUndefined)) {
        LOG_ERROR("Call Reference_IsUndefined failed");
        return;
    }

    ani_ref callbackRef;
    if (isUndefined) {
        LOG_ERROR("%{public}s: callback is undefined", __func__);
        callbackRef = nullptr;
    } else {
        if (ANI_OK != env->GlobalReference_Create(callback, &callbackRef)) {
            LOG_ERROR("Create callback failed");
            return;
        }
    }

    if (strType == "dataChange") {
        DataChangeOffType(env, callbackRef, strUri, helper);
    }
}

static void ANI_OnEvent([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_string event, ani_enum_item subscriptionType, ani_string uri, ani_object callback)
{
    if (env == nullptr) {
        LOG_ERROR("env is nullptr %{public}s", __func__);
        return;
    }
    std::string strEvent;
    std::string strUri;
    DataShareHelper *helper = nullptr;
    if (!getEvent(env, event, strEvent) || !getUri(env, uri, strUri) || !getDataShareHelper(env, object, helper)) {
        return;
    }

    ani_ref callbackRef;
    if (ANI_OK != env->GlobalReference_Create(callback, &callbackRef)) {
        LOG_ERROR("Create callback failed");
        return;
    }

    if (strEvent == "dataChange") {
        observerMap_.try_emplace(strUri);

        auto &list = observerMap_.find(strUri)->second;
        for (auto &it : list) {
            if (callbackRef == it->observer_->GetCallback()) {
                LOG_ERROR("The observer has already subscribed.");
                return;
            }
        }

        ani_vm *vm = nullptr;
        if (ANI_OK != env->GetVM(&vm)) {
            LOG_ERROR("GetVM failed.");
            return;
        }

        auto innerObserver = std::make_shared<ANIInnerObserver>(vm, callbackRef);
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

static void ANI_OffEvent([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_string event, ani_enum_item subscriptionType, ani_string uri, ani_object callback)
{
    if (env == nullptr) {
        LOG_ERROR("env is nullptr %{public}s", __func__);
        return;
    }
    std::string strEvent;
    std::string strUri;
    DataShareHelper *helper = nullptr;
    if (!getEvent(env, event, strEvent) || !getUri(env, uri, strUri) || !getDataShareHelper(env, object, helper)) {
        return;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(callback, &isUndefined)) {
        LOG_ERROR("Call Reference_IsUndefined failed");
        return;
    }

    ani_ref callbackRef;
    if (isUndefined) {
        LOG_ERROR("%{public}s: callback is undefined", __func__);
        callbackRef = nullptr;
    } else {
        if (ANI_OK != env->GlobalReference_Create(callback, &callbackRef)) {
            LOG_ERROR("Create callback failed");
            return;
        }
    }

    if (strEvent == "dataChange") {
        DataChangeOffEvent(env, callbackRef, strUri, helper);
    }
}

static ani_object ANI_Query([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_string uri, ani_object predicates, ani_object columns)
{
    if (env == nullptr) {
        LOG_ERROR("env is nullptr %{public}s", __func__);
        return nullptr;
    }
    std::string strUri;
    ani_class cls;
    DataShareHelper *helper = nullptr;
    if (!getUri(env, uri, strUri) || !getClass(env, cls) || !getDataShareHelper(env, object, helper)) {
        return nullptr;
    }

    auto pred = AniObjectUtils::Unwrap<DataSharePredicates>(env, predicates);
    if (pred == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr : %{public}d", pred == nullptr);
        return nullptr;
    }

    std::vector<std::string> strings;
    if (!UnionAccessor(env, columns).TryConvertArray<std::string>(strings)) {
        LOG_ERROR("TryConvertArray columns Failed");
        return nullptr;
    }

    Uri uriObj(strUri);
    DatashareBusinessError businessError;
    auto resultObject = helper->Query(uriObj, *pred, strings, &businessError);
    if (resultObject == nullptr) {
        LOG_ERROR("query failed, result is null!");
        return nullptr;
    }

    if (businessError.GetCode() != 0) {
        LOG_ERROR("query failed, errorCode : %{public}d", businessError.GetCode());
        return nullptr;
    }

    auto shareptrData = new (std::nothrow) SharedPtrHolder<DataShareResultSet>(resultObject);
    if (shareptrData == nullptr) {
        LOG_ERROR("Create Object SharedPtrHolder is null");
        return nullptr;
    }

    const char *className = "@ohos.data.DataShareResultSet.DataShareResultSetImpl";
    ani_object resultSetObj = AniObjectUtils::Create(env, className,
        reinterpret_cast<ani_long>(shareptrData));

    return resultSetObj;
}

using BucketMap = std::map<std::string, DataShareValueObject::Type>;
using BucketPtr = std::shared_ptr<BucketMap>;
using CallbackType = convertCallback<BucketMap>;

auto g_convertValuesBucket = [](ani_env *env, ani_ref &ani_key, ani_ref &object, BucketPtr records) {
    auto key = AniStringUtils::ToStd(env, static_cast<ani_string>(ani_key));
    auto unionObject = static_cast<ani_object>(object);
    UnionAccessor unionAccessor(env, unionObject);
    if (unionAccessor.IsInstanceOf("std.core.Double")) {
        double value;
        unionAccessor.TryConvert<double>(value);
        records->emplace(key, value);
        return true;
    }

    if (unionAccessor.IsInstanceOf("std.core.String")) {
        std::string value;
        unionAccessor.TryConvert<std::string>(value);
        records->emplace(key, value);
        return true;
    }

    if (unionAccessor.IsInstanceOf("std.core.Boolean")) {
        bool value;
        unionAccessor.TryConvert<bool>(value);
        records->emplace(key, value);
        return true;
    }

    std::vector<uint8_t> arrayUint8Values;
    if (unionAccessor.TryConvertArray<uint8_t>(arrayUint8Values)) {
        records->emplace(key, arrayUint8Values);
        return true;
    }

    if (OptionalAccessor(env, unionObject).IsNull()) {
        records->emplace(key, nullptr);
        return true;
    }

    LOG_ERROR("Unexpected object type");
    return false;
};

static ani_double ANI_Update([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_string uri, ani_object predicates, ani_object value)
{
    if (env == nullptr) {
        LOG_ERROR("env is nullptr %{public}s", __func__);
        return ani_double(DATA_SHARE_ERROR);
    }
    std::string strUri;
    ani_class cls;
    DataShareHelper *helper = nullptr;
    if (!getUri(env, uri, strUri) || !getClass(env, cls) || !getDataShareHelper(env, object, helper)) {
        return ani_double(DATA_SHARE_ERROR);
    }

    auto pred = AniObjectUtils::Unwrap<DataSharePredicates>(env, predicates);
    if (pred == nullptr) {
        LOG_ERROR("dataSharePredicates is nullptr : %{public}d", pred == nullptr);
        return ani_double(DATA_SHARE_ERROR);
    }

    auto bucket = std::make_shared<BucketMap>();
    if (!forEachMapEntry<CallbackType, BucketMap>(env, value, g_convertValuesBucket, bucket)) {
        LOG_ERROR("Is bucket null: %{public}d", bucket->empty());
        return ani_double(DATA_SHARE_ERROR);
    }

    Uri uriObj(strUri);
    DataShareValuesBucket dataShareValuesBucket(*bucket);

    return static_cast<ani_double>(helper->Update(uriObj, *pred, dataShareValuesBucket));
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        LOG_ERROR("Unsupported ANI_VERSION_1");
        return ANI_ERROR;
    }

    ani_namespace ns;
    if (!getNameSpace(env, ns)) {
        return ANI_ERROR;
    }

    std::array functions = {
        ani_native_function {"create", nullptr, reinterpret_cast<void *>(ANI_Create)},
    };

    if (ANI_OK != env->Namespace_BindNativeFunctions(ns, functions.data(), functions.size())) {
        LOG_ERROR("Cannot bind native functions to namespace");
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function {"on", "C{std.core.String}C{std.core.String}C{std.core.Function2}:",
            reinterpret_cast<void *>(ANI_OnType)},
        ani_native_function {"off", "C{std.core.String}C{std.core.String}C{std.core.Function2}:",
            reinterpret_cast<void *>(ANI_OffType)},
        ani_native_function {"on",
            "C{std.core.String}C{@ohos.data.dataShare.dataShare.SubscriptionType}C{std.core.String}C{std.core."
            "Function2}:",
            reinterpret_cast<void *>(ANI_OnEvent)},
        ani_native_function {"off",
            "C{std.core.String}C{@ohos.data.dataShare.dataShare.SubscriptionType}C{std.core.String}C{std.core."
            "Function2}:",
            reinterpret_cast<void *>(ANI_OffEvent)},
        ani_native_function {"ani_query", nullptr, reinterpret_cast<void *>(ANI_Query)},
        ani_native_function {"ani_update", nullptr, reinterpret_cast<void *>(ANI_Update)},
    };

    ani_class cls;
    if (!getClass(env, cls)) {
        return ANI_ERROR;
    }

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        LOG_ERROR("Cannot bind native methods to class");
        return ANI_ERROR;
    }

    static const char *cleanerName = "@ohos.data.dataShare.dataShare.DataShareHelperInner.Cleaner";
    auto cleanerCls = AniTypeFinder(env).FindClass(cleanerName);
    NativePtrCleaner(env).Bind(cleanerCls.value());

    *result = ANI_VERSION_1;
    return ANI_OK;
}