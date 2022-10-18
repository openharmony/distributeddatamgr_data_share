/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "napi_datashare_helper.h"

#include "napi_common_util.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "napi_base_context.h"
#include "napi_datashare_values_bucket.h"
#include "datashare_predicates_proxy.h"
#include "datashare_result_set_proxy.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace DataShare {
constexpr int MAX_ARGC = 6;

std::list<std::shared_ptr<DataShareHelper>> g_dataShareHelperList;

static DataSharePredicates UnwrapDataSharePredicates(napi_env env, napi_value value)
{
    auto predicates = DataSharePredicatesProxy::GetNativePredicates(env, value);
    if (predicates == nullptr) {
        LOG_ERROR("GetNativePredicates is nullptr.");
        return {};
    }
    return DataSharePredicates(predicates->GetOperationList());
}

static std::string UnwrapValuesBucketArrayFromJS(napi_env env, napi_value param,
    std::vector<DataShareValuesBucket> &value)
{
    LOG_DEBUG("Start");
    uint32_t arraySize = 0;
    napi_value jsValue = nullptr;
    std::string strValue = "";

    if (!IsArrayForNapiValue(env, param, arraySize)) {
        LOG_ERROR("IsArrayForNapiValue is false");
        return "Parameters error. The type of 'values' must be 'Array'";
    }

    value.clear();
    for (uint32_t i = 0; i < arraySize; i++) {
        jsValue = nullptr;
        if (napi_get_element(env, param, i, &jsValue) != napi_ok) {
            LOG_ERROR("napi_get_element is false");
            return "Parameters error. The type of 'values' must be 'Array'";
        }

        DataShareValuesBucket valueBucket;
        valueBucket.Clear();
        std::string msg = GetValueBucketObject(valueBucket, env, jsValue);
        if (!msg.empty()) {
            return msg;
        }

        value.push_back(valueBucket);
    }
    return "";
}

static std::vector<DataShareValuesBucket> GetValuesBucketArray(napi_env env, napi_value param, std::string &msg)
{
    LOG_DEBUG("Start");
    std::vector<DataShareValuesBucket> result;
    msg = UnwrapValuesBucketArrayFromJS(env, param, result);
    return result;
}

static std::string GetUri(napi_env env, napi_value jsValue, std::string &uri)
{
    LOG_DEBUG("Start");
    napi_valuetype valuetype = napi_undefined;
    napi_typeof(env, jsValue, &valuetype);
    if (valuetype != napi_string) {
        return "Parameters error. The type of 'uri' must be 'string'";
    }
    uri = DataShareJSUtils::Convert2String(env, jsValue);
    return "";
}

napi_value NapiDataShareHelper::Napi_CreateDataShareHelper(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto ctxInfo = std::make_shared<CreateContextInfo>();
    auto input = [ctxInfo](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2 && argc != 3) {
            LOG_ERROR("Parameters error, should 2 or 3 parameters!");
            ctxInfo->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            ctxInfo->errorMsg = "Parameters error, should 2 or 3 parameters!";
            return napi_invalid_arg;
        }
        LOG_INFO("after check Parameters.");
        bool isStageMode = false;
        napi_status status = AbilityRuntime::IsStageContext(env, argv[PARAM0], isStageMode);
        if (status != napi_ok || !isStageMode) {
            ctxInfo->isStageMode = false;
            auto ability = OHOS::AbilityRuntime::GetCurrentAbility(env);
            std::string msg = GetUri(env, argv[PARAM0], ctxInfo->strUri);
            if (!msg.empty()) {
                LOG_ERROR("getUri failed.");
                ctxInfo->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
                ctxInfo->errorMsg = msg;
                return napi_invalid_arg;
            }
            LOG_INFO("after geturi.");
            if (ability == nullptr) {
                LOG_ERROR("ability is nullptr.");
                ctxInfo->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
                ctxInfo->errorMsg = "Parameters error, failed to get native ability, ability can't be nullptr";
                return napi_invalid_arg;
            }
            LOG_INFO("after check ability.");
            ctxInfo->contextF = ability->GetContext();
        } else {
            ctxInfo->contextS = OHOS::AbilityRuntime::GetStageModeContext(env, argv[PARAM0]);
            LOG_INFO("before geturi.");
            std::string msg = GetUri(env, argv[PARAM1], ctxInfo->strUri);
            LOG_INFO("after geturi.");
            if (!msg.empty()) {
                LOG_ERROR("getUri failed.");
                ctxInfo->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
                ctxInfo->errorMsg = msg;
                return napi_invalid_arg;
            }
            LOG_INFO("after geturi check.");
            if (ctxInfo->contextS == nullptr) {
                LOG_ERROR("contextS is nullptr");
                ctxInfo->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
                ctxInfo->errorMsg = "Parameters error, failed to get native contextS, contextS can't be nullptr";
                return napi_invalid_arg;
            }
            LOG_INFO("after contextS check.");
        }

        napi_value helperProxy = nullptr;
        LOG_INFO("before napi_new_instance");
        status = napi_new_instance(env, GetConstructor(env), argc, argv, &helperProxy);
        LOG_INFO("after napi_new_instance");
        if ((helperProxy == nullptr) || (status != napi_ok)) {
            LOG_ERROR("helperProxy == nullptr) || (status != napi_ok)");
            ctxInfo->errorCode = DataShareJSUtils::EXCEPTION_HELPER_UNINITIALIZED;
            ctxInfo->errorMsg = DataShareJSUtils::MESSAGE_HELPER_UNINITIALIZED;
            return napi_generic_failure;
        }
        LOG_INFO("(helperProxy == nullptr) || (status != napi_ok)");
        napi_create_reference(env, helperProxy, 1, &(ctxInfo->ref));
        ctxInfo->env = env;
        return napi_ok;
    };
    auto output = [ctxInfo](napi_env env, napi_value *result) -> napi_status {
        LOG_INFO("output start.");
        if (ctxInfo->dataShareHelper == nullptr) {
            LOG_INFO("ctxInfo->dataShareHelper == nullptr.");
            ctxInfo->errorCode = DataShareJSUtils::EXCEPTION_HELPER_UNINITIALIZED;
            ctxInfo->errorMsg = DataShareJSUtils::MESSAGE_HELPER_UNINITIALIZED;
            return napi_generic_failure;
        }
        LOG_INFO("ctxInfo->dataShareHelper != nullptr.");
        g_dataShareHelperList.emplace_back(ctxInfo->dataShareHelper);
        LOG_INFO("before napi_get_reference_value");
        napi_status status = napi_get_reference_value(env, ctxInfo->ref, result);
        LOG_INFO("after napi_get_reference_value");
        NapiDataShareHelper *proxy = nullptr;
        LOG_INFO("before napi_unwrap");
        status = napi_unwrap(env, *result, reinterpret_cast<void **>(&proxy));
        LOG_INFO("after napi_unwrap");
        if (proxy == nullptr) {
            LOG_ERROR("proxy is nullptr");
            return status;
        }
        LOG_INFO("before move");
        proxy->datashareHelper_ = std::move(ctxInfo->dataShareHelper);
        LOG_INFO("after move");
        return status;
    };
    auto exec = [ctxInfo](AsyncCall::Context *ctx) {
        if (ctxInfo->isStageMode && ctxInfo->contextS != nullptr) {
            ctxInfo->dataShareHelper = DataShareHelper::Creator(ctxInfo->contextS, ctxInfo->strUri);
        } else if (!ctxInfo->isStageMode && ctxInfo->contextF != nullptr) {
            ctxInfo->dataShareHelper = DataShareHelper::Creator(ctxInfo->contextF, ctxInfo->strUri);
        }
    };
    ctxInfo->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(ctxInfo));
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::GetConstructor(napi_env env)
{
    napi_value cons = nullptr;
    napi_property_descriptor clzDes[] = {
        DECLARE_NAPI_FUNCTION("on", Napi_On),
        DECLARE_NAPI_FUNCTION("off", Napi_Off),
        DECLARE_NAPI_FUNCTION("insert", Napi_Insert),
        DECLARE_NAPI_FUNCTION("delete", Napi_Delete),
        DECLARE_NAPI_FUNCTION("query", Napi_Query),
        DECLARE_NAPI_FUNCTION("update", Napi_Update),
        DECLARE_NAPI_FUNCTION("batchInsert", Napi_BatchInsert),
        DECLARE_NAPI_FUNCTION("normalizeUri", Napi_NormalizeUri),
        DECLARE_NAPI_FUNCTION("denormalizeUri", Napi_DenormalizeUri),
        DECLARE_NAPI_FUNCTION("notifyChange", Napi_NotifyChange),
    };
    NAPI_CALL(env, napi_define_class(env, "DataShareHelper", NAPI_AUTO_LENGTH, Initialize, nullptr,
        sizeof(clzDes) / sizeof(napi_property_descriptor), clzDes, &cons));
    LOG_INFO("after napi_call");
    g_dataShareHelperList.clear();
    return cons;
}

napi_value NapiDataShareHelper::Initialize(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    napi_value self = nullptr;
    size_t argc = ARGS_MAX_COUNT;
    napi_value argv[ARGS_MAX_COUNT] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    if (argc <= 1) {
        LOG_ERROR("Parameters error, need at least 2 parameters!");
        return nullptr;
    }
    LOG_INFO("after check Parameters");
    auto *proxy = new NapiDataShareHelper();
    auto finalize = [](napi_env env, void * data, void * hint) {
        NapiDataShareHelper *proxy = reinterpret_cast<NapiDataShareHelper *>(data);
        if (proxy != nullptr) {
            auto it = proxy->observerMap_.begin();
            while (it != proxy->observerMap_.end()) {
                if (proxy->datashareHelper_ != nullptr) {
                    proxy->datashareHelper_->UnregisterObserver(Uri(it->first), it->second);
                }
                it->second->DeleteReference();
            }
            proxy->observerMap_.clear();
            delete proxy;
        }
    };
    LOG_INFO("after finalize");
    if (napi_wrap(env, self, proxy, finalize, nullptr, nullptr) != napi_ok) {
        finalize(env, proxy, nullptr);
        return nullptr;
    }
    LOG_INFO("after napi_wrap");
    return self;
}

napi_value NapiDataShareHelper::Napi_Insert(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2 && argc != 3) {
            context->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            context->errorMsg = "Parameters error, should 2 or 3 parameters!";
            return napi_invalid_arg;
        }
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        std::string msg = GetUri(env, argv[PARAM0], context->uri);
        if (!msg.empty()) {
            context->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            context->errorMsg = msg;
            return napi_invalid_arg;
        }

        context->valueBucket.Clear();
        msg = GetValueBucketObject(context->valueBucket, env, argv[PARAM1]);
        if (!msg.empty()) {
            context->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            context->errorMsg = msg;
            return napi_invalid_arg;
        }

        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        if (context->resultNumber < 0) {
            context->errorCode = DataShareJSUtils::EXCEPTION_INNER;
            return napi_generic_failure;
        }
        napi_create_int32(env, context->resultNumber, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        if (context->proxy->datashareHelper_ != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            context->resultNumber = context->proxy->datashareHelper_->Insert(uri, context->valueBucket);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                context->proxy->datashareHelper_ == nullptr, context->uri.empty());
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context));
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_Delete(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2 && argc != 3) {
            context->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            context->errorMsg = "Parameters error, should 2 or 3 parameters!";
            return napi_invalid_arg;
        }
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        std::string msg = GetUri(env, argv[PARAM0], context->uri);
        if (!msg.empty()) {
            context->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            context->errorMsg = msg;
            return napi_invalid_arg;
        }

        context->predicates = UnwrapDataSharePredicates(env, argv[PARAM1]);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        if (context->resultNumber < 0) {
            context->errorCode = DataShareJSUtils::EXCEPTION_INNER;
            return napi_generic_failure;
        }
        napi_create_int32(env, context->resultNumber, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        if (context->proxy->datashareHelper_ != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            context->resultNumber = context->proxy->datashareHelper_->Delete(uri, context->predicates);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                context->proxy->datashareHelper_ == nullptr, context->uri.empty());
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context));
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_Query(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 3 && argc != 4) {
            context->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            context->errorMsg = "Parameters error, should 3 or 4 parameters!";
            return napi_invalid_arg;
        }
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        std::string msg = GetUri(env, argv[PARAM0], context->uri);
        if (!msg.empty()) {
            context->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            context->errorMsg = msg;
            return napi_invalid_arg;
        }

        context->predicates = UnwrapDataSharePredicates(env, argv[PARAM1]);

        context->columns = DataShareJSUtils::Convert2StrVector(env, argv[PARAM2], DataShareJSUtils::DEFAULT_BUF_SIZE);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        if (context->resultObject == nullptr) {
            context->errorCode = DataShareJSUtils::EXCEPTION_INNER;
            return napi_generic_failure;
        }
        *result = DataShareResultSetProxy::NewInstance(env, context->resultObject);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        if (context->proxy->datashareHelper_ != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            context->resultObject = context->proxy->datashareHelper_->Query(uri, context->predicates, context->columns);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                context->proxy->datashareHelper_ == nullptr, context->uri.empty());
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context));
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_Update(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 3 && argc != 4) {
            context->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            context->errorMsg = "Parameters error, should 3 or 4 parameters!";
            return napi_invalid_arg;
        }
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        std::string msg = GetUri(env, argv[PARAM0], context->uri);
        if (!msg.empty()) {
            context->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            context->errorMsg = msg;
            return napi_invalid_arg;
        }

        context->predicates = UnwrapDataSharePredicates(env, argv[PARAM1]);

        context->valueBucket.Clear();
        msg = GetValueBucketObject(context->valueBucket, env, argv[PARAM2]);
        if (!msg.empty()) {
            context->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            context->errorMsg = msg;
            return napi_invalid_arg;
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        if (context->resultNumber < 0) {
            context->errorCode = DataShareJSUtils::EXCEPTION_INNER;
            return napi_generic_failure;
        }
        napi_create_int32(env, context->resultNumber, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        if (context->proxy->datashareHelper_ != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            context->resultNumber =
                context->proxy->datashareHelper_->Update(uri, context->predicates, context->valueBucket);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                context->proxy->datashareHelper_ == nullptr, context->uri.empty());
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context));
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_BatchInsert(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2 && argc != 3) {
            context->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            context->errorMsg = "Parameters error, should 2 or 3 parameters!";
            return napi_invalid_arg;
        }
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        std::string msg = GetUri(env, argv[PARAM0], context->uri);
        if (!msg.empty()) {
            context->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            context->errorMsg = msg;
            return napi_invalid_arg;
        }

        context->values = GetValuesBucketArray(env, argv[PARAM1], msg);
        if (!msg.empty()) {
            context->errorCode = DataShareJSUtils::EXCEPTION_PARAMETER_CHECK;
            context->errorMsg = msg;
            return napi_invalid_arg;
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        if (context->resultNumber < 0) {
            context->errorCode = DataShareJSUtils::EXCEPTION_INNER;
            return napi_generic_failure;
        }
        napi_create_int32(env, context->resultNumber, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        if (context->proxy->datashareHelper_ != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            context->resultNumber = context->proxy->datashareHelper_->BatchInsert(uri, context->values);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                context->proxy->datashareHelper_ == nullptr, context->uri.empty());
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context));
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_NormalizeUri(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        NAPI_ASSERT_BASE(env, argc == 1 || argc == 2, " should 1 or 2 parameters!", napi_invalid_arg);
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        GetUri(env, argv[PARAM0], context->uri);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_create_string_utf8(env, context->resultString.c_str(), NAPI_AUTO_LENGTH, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        if (context->proxy->datashareHelper_ != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            Uri uriValue = context->proxy->datashareHelper_->NormalizeUri(uri);
            context->resultString = uriValue.ToString();
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                context->proxy->datashareHelper_ == nullptr, context->uri.empty());
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context));
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_DenormalizeUri(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        NAPI_ASSERT_BASE(env, argc == 1 || argc == 2, " should 1 or 2 parameters!", napi_invalid_arg);
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        GetUri(env, argv[PARAM0], context->uri);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_create_string_utf8(env, context->resultString.c_str(), NAPI_AUTO_LENGTH, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        if (context->proxy->datashareHelper_ != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            Uri uriValue = context->proxy->datashareHelper_->DenormalizeUri(uri);
            context->resultString = uriValue.ToString();
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                context->proxy->datashareHelper_ == nullptr, context->uri.empty());
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context));
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_NotifyChange(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        NAPI_ASSERT_BASE(env, argc == 1 || argc == 2, " should 1 or 2 parameters!", napi_invalid_arg);
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        GetUri(env, argv[PARAM0], context->uri);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_get_null(env, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        if (context->proxy->datashareHelper_ != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            context->proxy->datashareHelper_->NotifyChange(uri);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                context->proxy->datashareHelper_ == nullptr, context->uri.empty());
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context));
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_On(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    napi_value self = nullptr;
    size_t argc = MAX_ARGC;
    napi_value argv[MAX_ARGC] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    NAPI_ASSERT(env, argc == ARGS_THREE, "wrong count of args");

    NapiDataShareHelper *proxy = nullptr;
    NAPI_CALL_BASE(env, napi_unwrap(env, self, reinterpret_cast<void **>(&proxy)), nullptr);
    NAPI_ASSERT_BASE(env, proxy != nullptr, "there is no NapiDataShareHelper instance", nullptr);
    NAPI_ASSERT_BASE(env, proxy->datashareHelper_ != nullptr, "there is no DataShareHelper instance", nullptr);

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valueType));
    if (valueType != napi_string) {
        LOG_ERROR("type is not string");
        return nullptr;
    }
    std::string type = DataShareJSUtils::Convert2String(env, argv[PARAM0]);
    if (type != "dataChange") {
        LOG_ERROR("wrong register type : %{public}s", type.c_str());
        return nullptr;
    }

    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valueType));
    NAPI_ASSERT_BASE(env, valueType == napi_string, "uri is not string", nullptr);
    std::string uri = DataShareJSUtils::Convert2String(env, argv[PARAM1]);

    NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valueType));
    NAPI_ASSERT_BASE(env, valueType == napi_function, "callback is not a function", nullptr);
    sptr<NAPIDataShareObserver> observer(new (std::nothrow) NAPIDataShareObserver(env, argv[PARAM2]));

    auto obs = proxy->observerMap_.find(uri);
    if (obs != proxy->observerMap_.end()) {
        proxy->datashareHelper_->UnregisterObserver(Uri(uri), obs->second);
        obs->second->DeleteReference();
        proxy->observerMap_.erase(uri);
    }
    proxy->datashareHelper_->RegisterObserver(Uri(uri), observer);
    proxy->observerMap_.emplace(uri, observer);

    return nullptr;
}

napi_value NapiDataShareHelper::Napi_Off(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    napi_value self = nullptr;
    size_t argc = MAX_ARGC;
    napi_value argv[MAX_ARGC] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    NAPI_ASSERT(env, argc == ARGS_TWO || argc == ARGS_THREE, "wrong count of args");

    NapiDataShareHelper *proxy = nullptr;
    NAPI_CALL_BASE(env, napi_unwrap(env, self, reinterpret_cast<void **>(&proxy)), nullptr);
    NAPI_ASSERT_BASE(env, proxy != nullptr, "there is no NapiDataShareHelper instance", nullptr);
    NAPI_ASSERT_BASE(env, proxy->datashareHelper_ != nullptr, "there is no DataShareHelper instance", nullptr);

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valueType));
    if (valueType != napi_string) {
        LOG_ERROR("type is not string");
        return nullptr;
    }
    std::string type = DataShareJSUtils::Convert2String(env, argv[PARAM0]);
    if (type != "dataChange") {
        LOG_ERROR("wrong register type : %{public}s", type.c_str());
        return nullptr;
    }

    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valueType));
    NAPI_ASSERT_BASE(env, valueType == napi_string, "uri is not string", nullptr);
    std::string uri = DataShareJSUtils::Convert2String(env, argv[PARAM1]);

    if (argc == ARGS_THREE) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valueType));
        NAPI_ASSERT_BASE(env, valueType == napi_function, "callback is not a function", nullptr);
    }

    auto obs = proxy->observerMap_.find(uri);
    if (obs != proxy->observerMap_.end()) {
        proxy->datashareHelper_->UnregisterObserver(Uri(uri), obs->second);
        obs->second->DeleteReference();
        proxy->observerMap_.erase(uri);
    } else {
        LOG_DEBUG("this uri hasn't been registered");
    }

    return nullptr;
}
}  // namespace DataShare
}  // namespace OHOS
