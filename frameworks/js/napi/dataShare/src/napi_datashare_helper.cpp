/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "data_proxy_observer_stub.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "datashare_predicates_proxy.h"
#include "datashare_result_set_proxy.h"
#include "datashare_valuebucket_convert.h"
#include "napi_base_context.h"
#include "napi_common_util.h"
#include "napi_datashare_values_bucket.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace DataShare {
static constexpr int MAX_ARGC = 6;
static bool GetSilentUri(napi_env env, napi_value jsValue, std::string &uri)
{
    napi_valuetype valuetype = napi_undefined;
    napi_typeof(env, jsValue, &valuetype);
    if (valuetype == napi_undefined || valuetype == napi_null) {
        return true;
    }
    if (valuetype == napi_string) {
        uri = DataShareJSUtils::Convert2String(env, jsValue);
        return true;
    }
    return false;
}

static bool GetUri(napi_env env, napi_value jsValue, std::string &uri)
{
    napi_valuetype valuetype = napi_undefined;
    napi_typeof(env, jsValue, &valuetype);
    if (valuetype != napi_string) {
        return false;
    }
    uri = DataShareJSUtils::Convert2String(env, jsValue);
    return true;
}

bool NapiDataShareHelper::GetOptions(napi_env env, napi_value jsValue, CreateOptions &options)
{
    napi_valuetype type = napi_undefined;
    napi_typeof(env, jsValue, &type);
    if (type != napi_object) {
        LOG_ERROR("CreateOptions is not object");
        return false;
    }
    napi_value isProxyJs = nullptr;
    napi_status status = napi_get_named_property(env, jsValue, "isProxy", &isProxyJs);
    if (status != napi_ok) {
        LOG_ERROR("napi_get_named_property failed %{public}d", status);
        return false;
    }
    napi_typeof(env, isProxyJs, &type);
    if (type != napi_boolean) {
        LOG_ERROR("CreateOptions.isProxy is not bool");
        return false;
    }
    status = napi_get_value_bool(env, isProxyJs, &options.isProxy_);
    if (status != napi_ok) {
        LOG_ERROR("napi_get_value_bool failed %{public}d", status);
        return false;
    }
    options.enabled_ = true;
    return true;
}

napi_value NapiDataShareHelper::Napi_CreateDataShareHelper(napi_env env, napi_callback_info info)
{
    auto ctxInfo = std::make_shared<CreateContextInfo>();
    auto input = [ctxInfo](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        NAPI_ASSERT_CALL_ERRCODE(env, argc == 2 || argc == 3 || argc == 4,
            ctxInfo->error = std::make_shared<ParametersNumError>("2 or 3 or 4"), napi_invalid_arg);
        ctxInfo->contextS = OHOS::AbilityRuntime::GetStageModeContext(env, argv[0]);
        NAPI_ASSERT_CALL_ERRCODE(env, ctxInfo->contextS != nullptr,
            ctxInfo->error = std::make_shared<ParametersTypeError>("contextS", "not nullptr"), napi_invalid_arg);
        NAPI_ASSERT_CALL_ERRCODE(env, GetUri(env, argv[1], ctxInfo->strUri),
            ctxInfo->error = std::make_shared<ParametersTypeError>("uri", "string"), napi_invalid_arg);
        Uri uri(ctxInfo->strUri);
        if (uri.GetScheme() == "datashareproxy") {
            NAPI_ASSERT_CALL_ERRCODE(env, argc == 3 || argc == 4,
                ctxInfo->error = std::make_shared<ParametersNumError>("3 or 4"), napi_invalid_arg);
            NAPI_ASSERT_CALL_ERRCODE(env, GetOptions(env, argv[2], ctxInfo->options),
                ctxInfo->error = std::make_shared<ParametersTypeError>("option", "CreateOption"), napi_invalid_arg);
        }
        napi_value helperProxy = nullptr;
        napi_status status = napi_new_instance(env, GetConstructor(env), argc, argv, &helperProxy);
        NAPI_ASSERT_CALL_ERRCODE(env, helperProxy != nullptr && status == napi_ok,
            ctxInfo->error = std::make_shared<DataShareHelperInitError>(), napi_generic_failure);
        napi_create_reference(env, helperProxy, 1, &(ctxInfo->ref));
        ctxInfo->env = env;
        return napi_ok;
    };
    auto output = [ctxInfo](napi_env env, napi_value *result) -> napi_status {
        NAPI_ASSERT_CALL_ERRCODE(env, ctxInfo->dataShareHelper != nullptr,
            ctxInfo->error = std::make_shared<DataShareHelperInitError>(), napi_generic_failure);
        napi_status status = napi_get_reference_value(env, ctxInfo->ref, result);
        NAPI_ASSERT_CALL_ERRCODE(env, result != nullptr,
            ctxInfo->error = std::make_shared<DataShareHelperInitError>(), napi_generic_failure);
        NapiDataShareHelper *proxy = nullptr;
        status = napi_unwrap(env, *result, reinterpret_cast<void **>(&proxy));
        NAPI_ASSERT_CALL_ERRCODE(env, proxy != nullptr, ctxInfo->error = std::make_shared<DataShareHelperInitError>(),
            status);
        proxy->jsRdbObsManager_ = std::make_shared<NapiRdbSubscriberManager>(ctxInfo->dataShareHelper);
        proxy->jsPublishedObsManager_ = std::make_shared<NapiPublishedSubscriberManager>(ctxInfo->dataShareHelper);
        proxy->SetHelper(std::move(ctxInfo->dataShareHelper));
        return status;
    };
    auto exec = [ctxInfo](AsyncCall::Context *ctx) {
        if (ctxInfo->options.enabled_) {
            ctxInfo->options.token_ = ctxInfo->contextS->GetToken();
            ctxInfo->dataShareHelper = DataShareHelper::Creator(ctxInfo->strUri, ctxInfo->options);
        } else {
            ctxInfo->dataShareHelper = DataShareHelper::Creator(ctxInfo->contextS->GetToken(), ctxInfo->strUri);
        }
    };
    ctxInfo->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, ctxInfo);
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
        DECLARE_NAPI_FUNCTION("batchUpdate", Napi_BatchUpdate),
        DECLARE_NAPI_FUNCTION("normalizeUri", Napi_NormalizeUri),
        DECLARE_NAPI_FUNCTION("denormalizeUri", Napi_DenormalizeUri),
        DECLARE_NAPI_FUNCTION("notifyChange", Napi_NotifyChange),
        DECLARE_NAPI_FUNCTION("addTemplate", Napi_AddTemplate),
        DECLARE_NAPI_FUNCTION("delTemplate", Napi_DelTemplate),
        DECLARE_NAPI_FUNCTION("publish", Napi_Publish),
        DECLARE_NAPI_FUNCTION("getPublishedData", Napi_GetPublishedData),
        DECLARE_NAPI_FUNCTION("close", Napi_Close),
    };
    NAPI_CALL(env, napi_define_class(env, "DataShareHelper", NAPI_AUTO_LENGTH, Initialize, nullptr,
        sizeof(clzDes) / sizeof(napi_property_descriptor), clzDes, &cons));
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
    auto *proxy = new (std::nothrow) NapiDataShareHelper();
    if (proxy == nullptr) {
        return nullptr;
    }
    auto finalize = [](napi_env env, void *data, void *hint) {
        NapiDataShareHelper *proxy = reinterpret_cast<NapiDataShareHelper *>(data);
        delete proxy;
    };
    if (napi_wrap(env, self, proxy, finalize, nullptr, nullptr) != napi_ok) {
        finalize(env, proxy, nullptr);
        return nullptr;
    }
    return self;
}

napi_value NapiDataShareHelper::Napi_Insert(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2 && argc != 3) {
            context->error = std::make_shared<ParametersNumError>("2 or 3");
            return napi_invalid_arg;
        }

        if (!GetUri(env, argv[0], context->uri)) {
            context->error = std::make_shared<ParametersTypeError>("uri", "string");
            return napi_invalid_arg;
        }

        context->valueBucket.Clear();
        if (!GetValueBucketObject(context->valueBucket, env, argv[1])) {
            context->error = std::make_shared<ParametersTypeError>("valueBucket",
                "[string|number|boolean|null|Uint8Array]");
            return napi_invalid_arg;
        }

        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        if (context->resultNumber < 0) {
            context->error = std::make_shared<InnerError>();
            return napi_generic_failure;
        }
        napi_create_int32(env, context->resultNumber, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto helper = context->proxy->GetHelper();
        if (helper != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            context->resultNumber = helper->Insert(uri, context->valueBucket);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                helper == nullptr, context->uri.empty());
            context->error = std::make_shared<HelperAlreadyClosedError>();
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_Delete(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2 && argc != 3) {
            context->error = std::make_shared<ParametersNumError>("2 or 3");
            return napi_invalid_arg;
        }

        if (!GetUri(env, argv[0], context->uri)) {
            context->error = std::make_shared<ParametersTypeError>("uri", "string");
            return napi_invalid_arg;
        }

        if (!DataShareJSUtils::UnwrapDataSharePredicates(env, argv[1], context->predicates)) {
            context->error = std::make_shared<ParametersTypeError>("predicates", "DataSharePredicates");
            return napi_invalid_arg;
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        if (context->resultNumber < 0) {
            context->error = std::make_shared<InnerError>();
            return napi_generic_failure;
        }
        napi_create_int32(env, context->resultNumber, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto helper = context->proxy->GetHelper();
        if (helper != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            context->resultNumber = helper->Delete(uri, context->predicates);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                helper == nullptr, context->uri.empty());
            context->error = std::make_shared<HelperAlreadyClosedError>();
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_Query(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 3 && argc != 4) {
            context->error = std::make_shared<ParametersNumError>("3 or 4");
            return napi_invalid_arg;
        }

        if (!GetUri(env, argv[0], context->uri)) {
            context->error = std::make_shared<ParametersTypeError>("uri", "string");
            return napi_invalid_arg;
        }

        if (!DataShareJSUtils::UnwrapDataSharePredicates(env, argv[1], context->predicates)) {
            context->error = std::make_shared<ParametersTypeError>("predicates", "DataSharePredicates");
            return napi_invalid_arg;
        }

        context->columns = DataShareJSUtils::Convert2StrVector(env, argv[2], DataShareJSUtils::DEFAULT_BUF_SIZE);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        if (context->businessError.GetCode() != 0) {
            LOG_DEBUG("query failed, errorCode : %{public}d", context->businessError.GetCode());
            context->error = std::make_shared<BusinessError>(context->businessError.GetCode(),
                context->businessError.GetMessage());
            return napi_generic_failure;
        }

        if (context->resultObject == nullptr) {
            context->error = std::make_shared<InnerError>();
            return napi_generic_failure;
        }
        *result = DataShareResultSetProxy::NewInstance(env, context->resultObject);
        context->resultObject = nullptr;
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto helper = context->proxy->GetHelper();
        if (helper != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            context->resultObject = helper->Query(uri, context->predicates, context->columns,
                &(context->businessError));
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                helper == nullptr, context->uri.empty());
            context->error = std::make_shared<HelperAlreadyClosedError>();
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_Update(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 3 && argc != 4) {
            context->error = std::make_shared<ParametersNumError>("3 or 4");
            return napi_invalid_arg;
        }

        if (!GetUri(env, argv[0], context->uri)) {
            context->error = std::make_shared<ParametersTypeError>("uri", "string");
            return napi_invalid_arg;
        }

        if (!DataShareJSUtils::UnwrapDataSharePredicates(env, argv[1], context->predicates)) {
            context->error = std::make_shared<ParametersTypeError>("predicates", "DataSharePredicates");
            return napi_invalid_arg;
        }

        context->valueBucket.Clear();
        if (!GetValueBucketObject(context->valueBucket, env, argv[2])) {
            context->error = std::make_shared<ParametersTypeError>("valueBucket",
                "[string|number|boolean|null|Uint8Array]");
            return napi_invalid_arg;
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        if (context->resultNumber < 0) {
            context->error = std::make_shared<InnerError>();
            return napi_generic_failure;
        }
        napi_create_int32(env, context->resultNumber, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto helper = context->proxy->GetHelper();
        if (helper != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            context->resultNumber = helper->Update(uri, context->predicates, context->valueBucket);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                helper == nullptr, context->uri.empty());
            context->error = std::make_shared<HelperAlreadyClosedError>();
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_BatchUpdate(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 1) {
            context->error = std::make_shared<ParametersNumError>("1");
            return napi_invalid_arg;
        }
        if (DataShareJSUtils::Convert2Value(env, argv[0], context->updateOperations) != napi_ok) {
            context->error = std::make_shared<ParametersTypeError>("operations",
                "Record<string, Array<UpdateOperation>>");
            return napi_invalid_arg;
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        if (context->resultNumber < 0) {
            context->error = std::make_shared<InnerError>();
            return napi_generic_failure;
        }
        *result = DataShareJSUtils::Convert2JSValue(env, context->batchUpdateResult);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto helper = context->proxy->GetHelper();
        if (helper != nullptr && !context->updateOperations.empty()) {
            context->resultNumber = helper->BatchUpdate(context->updateOperations, context->batchUpdateResult);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->updateOperations is empty : %{public}d",
                helper == nullptr, context->updateOperations.empty());
            context->error = std::make_shared<HelperAlreadyClosedError>();
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_BatchInsert(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2 && argc != 3) {
            context->error = std::make_shared<ParametersNumError>("2 or 3");
            return napi_invalid_arg;
        }

        if (!GetUri(env, argv[0], context->uri)) {
            context->error = std::make_shared<ParametersTypeError>("uri", "string");
            return napi_invalid_arg;
        }
        if (DataShareJSUtils::Convert2Value(env, argv[1], context->values) != napi_ok) {
            context->error = std::make_shared<ParametersTypeError>("valueBucket",
                "[string|number|boolean|null|Uint8Array]");
            return napi_invalid_arg;
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        if (context->resultNumber < 0) {
            context->error = std::make_shared<InnerError>();
            return napi_generic_failure;
        }
        napi_create_int32(env, context->resultNumber, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto helper = context->proxy->GetHelper();
        if (helper != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            context->resultNumber = helper->BatchInsert(uri, context->values);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                helper == nullptr, context->uri.empty());
            context->error = std::make_shared<HelperAlreadyClosedError>();
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_NormalizeUri(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 1 && argc != 2) {
            context->error = std::make_shared<ParametersNumError>("1 or 2");
            return napi_invalid_arg;
        }
        if (!GetUri(env, argv[0], context->uri)) {
            context->error = std::make_shared<ParametersTypeError>("uri", "string");
            return napi_invalid_arg;
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_create_string_utf8(env, context->resultString.c_str(), NAPI_AUTO_LENGTH, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto helper = context->proxy->GetHelper();
        if (helper != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            Uri uriValue = helper->NormalizeUri(uri);
            context->resultString = uriValue.ToString();
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                helper == nullptr, context->uri.empty());
            context->error = std::make_shared<HelperAlreadyClosedError>();
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_DenormalizeUri(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 1 && argc != 2) {
            context->error = std::make_shared<ParametersNumError>("1 or 2");
            return napi_invalid_arg;
        }
        if (!GetUri(env, argv[0], context->uri)) {
            context->error = std::make_shared<ParametersTypeError>("uri", "string");
            return napi_invalid_arg;
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_create_string_utf8(env, context->resultString.c_str(), NAPI_AUTO_LENGTH, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto helper = context->proxy->GetHelper();
        if (helper != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            Uri uriValue = helper->DenormalizeUri(uri);
            context->resultString = uriValue.ToString();
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                helper == nullptr, context->uri.empty());
            context->error = std::make_shared<HelperAlreadyClosedError>();
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

void NapiDataShareHelper::Notify(const std::shared_ptr<NapiDataShareHelper::ContextInfo> context,
    std::shared_ptr<DataShareHelper> helper)
{
    if (!context->isNotifyDetails) {
        if (!context->uri.empty()) {
            Uri uri(context->uri);
            helper->NotifyChange(uri);
            context->status = napi_ok;
            return;
        }
        LOG_ERROR("context->isNotifyDetails is false, but context->uri is empty");
        context->error = std::make_shared<ParametersTypeError>("uri", "not empty");
        return;
    }
    helper->NotifyChangeExt(context->changeInfo);
    context->status = napi_ok;
    return;
}

napi_value NapiDataShareHelper::Napi_NotifyChange(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 1 && argc != 2) {
            context->error = std::make_shared<ParametersNumError>("1 or 2");
            return napi_invalid_arg;
        }
        napi_valuetype valueType;
        napi_typeof(env, argv[0], &valueType);
        if (valueType != napi_string) {
            context->isNotifyDetails = true;
            if (DataShareJSUtils::Convert2Value(env, argv[0], context->changeInfo) != napi_ok) {
                context->error = std::make_shared<ParametersTypeError>("ChangeInfo", "valid");
                return napi_invalid_arg;
            }
        } else {
            GetUri(env, argv[0], context->uri);
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_get_null(env, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto helper = context->proxy->GetHelper();
        if (helper != nullptr) {
            Notify(context, helper);
        } else {
            LOG_ERROR("helper == nullptr : %{public}d", helper == nullptr);
            context->error = std::make_shared<HelperAlreadyClosedError>();
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_AddTemplate(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    size_t argc = MAX_ARGC;
    napi_value argv[MAX_ARGC] = { nullptr };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    std::shared_ptr<Error> error = nullptr;
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, argc == ARGS_THREE, error = std::make_shared<ParametersNumError>("3"), error,
        nullptr);

    NapiDataShareHelper *proxy = nullptr;
    NAPI_CALL_BASE(env, napi_unwrap(env, self, reinterpret_cast<void **>(&proxy)), nullptr);
    NAPI_ASSERT_BASE(env, proxy != nullptr, "there is no NapiDataShareHelper instance", nullptr);
    auto helper = proxy->GetHelper();
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, helper != nullptr, error = std::make_shared<HelperAlreadyClosedError>(), error,
        nullptr);

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_string,
        error = std::make_shared<ParametersTypeError>("uri", "string"), error, nullptr);
    std::string uri = DataShareJSUtils::Convert2String(env, argv[0]);
    NAPI_ASSERT_BASE(env, !uri.empty(), "convert uri failed", nullptr);

    NAPI_CALL(env, napi_typeof(env, argv[1], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_string,
        error = std::make_shared<ParametersTypeError>("subscriberId", "string"), error, nullptr);
    std::string subscriberId = DataShareJSUtils::Convert2String(env, argv[1]);

    NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_object,
        error = std::make_shared<ParametersTypeError>("template", "Template"), error, nullptr);
    Template tpl = DataShareJSUtils::Convert2Template(env, argv[PARAM2]);

    auto res = helper->AddQueryTemplate(uri, atoll(subscriberId.c_str()), tpl);
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, res != E_URI_NOT_EXIST && res != E_BUNDLE_NAME_NOT_EXIST,
        error = std::make_shared<UriNotExistError>(), error, nullptr);
    return DataShareJSUtils::Convert2JSValue(env, res);
}

napi_value NapiDataShareHelper::Napi_DelTemplate(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    size_t argc = MAX_ARGC;
    napi_value argv[MAX_ARGC] = { nullptr };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    std::shared_ptr<Error> error = nullptr;
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, argc == ARGS_TWO, error = std::make_shared<ParametersNumError>("2"), error,
        nullptr);

    NapiDataShareHelper *proxy = nullptr;
    NAPI_CALL_BASE(env, napi_unwrap(env, self, reinterpret_cast<void **>(&proxy)), nullptr);
    NAPI_ASSERT_BASE(env, proxy != nullptr, "there is no NapiDataShareHelper instance", nullptr);
    auto helper = proxy->GetHelper();
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, helper != nullptr, error = std::make_shared<HelperAlreadyClosedError>(), error,
        nullptr);

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_string,
        error = std::make_shared<ParametersTypeError>("uri", "string"), error, nullptr);
    std::string uri = DataShareJSUtils::Convert2String(env, argv[0]);
    NAPI_ASSERT_BASE(env, !uri.empty(), "convert uri failed", nullptr);

    NAPI_CALL(env, napi_typeof(env, argv[1], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_string,
        error = std::make_shared<ParametersTypeError>("subscriberId", "string"), error, nullptr);
    std::string subscriberId = DataShareJSUtils::Convert2String(env, argv[1]);

    auto res = helper->DelQueryTemplate(uri, atoll(subscriberId.c_str()));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, res != E_URI_NOT_EXIST && res != E_BUNDLE_NAME_NOT_EXIST,
        error = std::make_shared<UriNotExistError>(), error, nullptr);
    return DataShareJSUtils::Convert2JSValue(env, res);
}

napi_value NapiDataShareHelper::Napi_Publish(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2 && argc != 3 && argc != 4) {
            context->error = std::make_shared<ParametersNumError>("2 or 3 or 4");
            return napi_invalid_arg;
        }
        napi_valuetype valueType;
        NAPI_CALL_BASE(env, napi_typeof(env, argv[0], &valueType), napi_invalid_arg);
        NAPI_ASSERT_CALL_ERRCODE(env, valueType == napi_object,
            context->error = std::make_shared<ParametersTypeError>("data", "Data"), napi_invalid_arg);
        NAPI_CALL_BASE(env, napi_typeof(env, argv[1], &valueType), napi_invalid_arg);
        NAPI_ASSERT_CALL_ERRCODE(env, valueType == napi_string,
            context->error = std::make_shared<ParametersTypeError>("bundleName", "string"), napi_invalid_arg);
        context->publishData = DataShareJSUtils::Convert2PublishedData(env, argv[0]);
        context->bundleName = DataShareJSUtils::Convert2String(env, argv[1]);
        if (argc > 2) {
            NAPI_CALL_BASE(env, napi_typeof(env, argv[PARAM2], &valueType), napi_invalid_arg);
            if (valueType == napi_number) {
                napi_get_value_int32(env, argv[PARAM2], &(context->publishData.version_));
            }
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        NAPI_ASSERT_BASE(env, context->status == napi_ok, "exec failed", napi_generic_failure);
        for (auto &operationResult : context->results) {
            if (operationResult.errCode_ == E_BUNDLE_NAME_NOT_EXIST) {
                context->error = std::make_shared<DataAreaNotExistError>();
                return napi_generic_failure;
            }
        }
        *result = DataShareJSUtils::Convert2JSValue(env, context->results);
        context->results.clear();
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto helper = context->proxy->GetHelper();
        if (helper == nullptr) {
            LOG_ERROR("dataShareHelper_ is nullptr");
            context->error = std::make_shared<HelperAlreadyClosedError>();
            return;
        }
        context->results = helper->Publish(context->publishData, context->bundleName);
        context->status = napi_ok;
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_GetPublishedData(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 1 && argc != 2) {
            context->error = std::make_shared<ParametersNumError>("1 or 2");
            return napi_invalid_arg;
        }
        napi_valuetype valueType;
        NAPI_CALL_BASE(env, napi_typeof(env, argv[0], &valueType), napi_invalid_arg);
        NAPI_ASSERT_CALL_ERRCODE(env, valueType == napi_string,
            context->error = std::make_shared<ParametersTypeError>("bundleName", "string"), napi_invalid_arg);
        context->bundleName = DataShareJSUtils::Convert2String(env, argv[0]);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        NAPI_ASSERT_BASE(env, context->status == napi_ok, "exec failed", napi_generic_failure);
        if (context->resultNumber == E_BUNDLE_NAME_NOT_EXIST) {
            context->error = std::make_shared<DataAreaNotExistError>();
            return napi_generic_failure;
        }
        *result = DataShareJSUtils::Convert2JSValue(env, context->publishData.datas_);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto helper = context->proxy->GetHelper();
        if (helper == nullptr) {
            LOG_ERROR("dataShareHelper_ is nullptr");
            context->error = std::make_shared<HelperAlreadyClosedError>();
            return;
        }
        context->publishData = helper->GetPublishedData(context->bundleName,
            context->resultNumber);
        context->status = napi_ok;
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_On(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    size_t argc = MAX_ARGC;
    napi_value argv[MAX_ARGC] = { nullptr };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    std::shared_ptr<Error> error = nullptr;
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, argc == ARGS_THREE || argc == ARGS_FOUR,
        error = std::make_shared<ParametersNumError>("3 or 4"), error, nullptr);
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
    if (valueType != napi_string) {
        LOG_ERROR("type is not string");
        return nullptr;
    }
    std::string type = DataShareJSUtils::Convert2String(env, argv[0]);
    if (type == "rdbDataChange") {
        return Napi_SubscribeRdbObserver(env, argc, argv, self);
    } else if (type == "publishedDataChange") {
        return Napi_SubscribePublishedObserver(env, argc, argv, self);
    } else if (type == "dataChange") {
        return Napi_RegisterObserver(env, argc, argv, self);
    }
    LOG_ERROR("wrong register type : %{public}s", type.c_str());
    return nullptr;
}
napi_value NapiDataShareHelper::Napi_RegisterObserver(napi_env env, size_t argc, napi_value *argv, napi_value self)
{
    NapiDataShareHelper* proxy = nullptr;
    std::shared_ptr<Error> error = nullptr;
    napi_valuetype valueType;
    NAPI_CALL_BASE(env, napi_unwrap(env, self, reinterpret_cast<void**>(&proxy)), nullptr);
    NAPI_ASSERT_BASE(env, proxy != nullptr, "there is no NapiDataShareHelper instance", nullptr);
    auto helper = proxy->GetHelper();
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, helper != nullptr, error = std::make_shared<HelperAlreadyClosedError>(), error,
        nullptr);
    if (argc == ARGS_THREE) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valueType));
        NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_string,
            error = std::make_shared<ParametersTypeError>("uri", "string"), error, nullptr);
        std::string uri = DataShareJSUtils::Convert2String(env, argv[PARAM1]);
        NAPI_ASSERT_CALL_ERRCODE_SYNC(env, !uri.empty(),
            error = std::make_shared<ParametersTypeError>("uri", "not empty"), error, nullptr);
        NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valueType));
        NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_function,
            error = std::make_shared<ParametersTypeError>("callback", "function"), error, nullptr);
        proxy->RegisteredObserver(env, uri, argv[PARAM2], std::move(helper));
        return nullptr;
    }
    if (argc == ARGS_FOUR) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valueType));
        NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_number,
            error = std::make_shared<ParametersTypeError>("SubscriptionType", "number"), error, nullptr);
        int32_t value;
        napi_get_value_int32(env, argv[PARAM1], &value);
        NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valueType));
        NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_string,
            error = std::make_shared<ParametersTypeError>("uri", "string"), error, nullptr);
        std::string uri = DataShareJSUtils::Convert2String(env, argv[PARAM2]);
        NAPI_CALL(env, napi_typeof(env, argv[PARAM3], &valueType));
        NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_function,
            error = std::make_shared<ParametersTypeError>("callback", "function"), error, nullptr);
        proxy->RegisteredObserver(env, uri, argv[PARAM3], std::move(helper), true);
        return nullptr;
    }
    return nullptr;
}

napi_value NapiDataShareHelper::Napi_Off(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    size_t argc = MAX_ARGC;
    napi_value argv[MAX_ARGC] = { nullptr };
    std::shared_ptr<Error> error = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    NAPI_ASSERT(env, argc == ARGS_TWO || argc == ARGS_THREE || argc == ARGS_FOUR, "wrong count of args");

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
    if (valueType != napi_string) {
        LOG_ERROR("type is not string");
        return nullptr;
    }
    std::string type = DataShareJSUtils::Convert2String(env, argv[0]);
    if (type == "rdbDataChange") {
        return Napi_UnsubscribeRdbObserver(env, argc, argv, self);
    } else if (type == "publishedDataChange") {
        return Napi_UnsubscribePublishedObserver(env, argc, argv, self);
    } else if (type == "dataChange") {
        return Napi_UnregisterObserver(env, argc, argv, self);
    }
    LOG_ERROR("wrong register type : %{public}s", type.c_str());
    return nullptr;
}

napi_value NapiDataShareHelper::Napi_UnregisterObserver(napi_env env, size_t argc, napi_value *argv, napi_value self)
{
    std::shared_ptr<Error> error = nullptr;
    NapiDataShareHelper* proxy = nullptr;
    napi_valuetype type;
    NAPI_CALL_BASE(env, napi_unwrap(env, self, reinterpret_cast<void**>(&proxy)), nullptr);
    NAPI_ASSERT_BASE(env, proxy != nullptr, "there is no NapiDataShareHelper instance", nullptr);
    auto helper = proxy->GetHelper();
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, helper != nullptr, error = std::make_shared<HelperAlreadyClosedError>(), error,
        nullptr);
    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &type));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, type == napi_string || type == napi_number,
        error = std::make_shared<ParametersTypeError>("argv[1]", "string or number"), error, nullptr);
    if (type == napi_string) {
        NAPI_ASSERT_CALL_ERRCODE_SYNC(env, argc == ARGS_TWO || argc == ARGS_THREE,
            error = std::make_shared<ParametersNumError>("2 or 3"), error, nullptr);
        std::string uri = DataShareJSUtils::Convert2String(env, argv[PARAM1]);
        if (argc == ARGS_THREE) {
            NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &type));
            NAPI_ASSERT_CALL_ERRCODE_SYNC(env, type == napi_function || type == napi_undefined || type == napi_null,
                error = std::make_shared<ParametersTypeError>("callback", "function"), error, nullptr);
            if (type == napi_function) {
                proxy->UnRegisteredObserver(env, uri, argv[PARAM2], std::move(helper));
                return nullptr;
            }
        }
        proxy->UnRegisteredObserver(env, uri, std::move(helper));
        return nullptr;
    }
    if (type == napi_number) {
        NAPI_ASSERT_CALL_ERRCODE_SYNC(env, argc == ARGS_THREE || argc == ARGS_FOUR,
            error = std::make_shared<ParametersNumError>("3 or 4"), error, nullptr);
        NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &type));
        NAPI_ASSERT_CALL_ERRCODE_SYNC(env, type == napi_string,
            error = std::make_shared<ParametersTypeError>("uri", "string"), error, nullptr);
        std::string uriStr = DataShareJSUtils::Convert2String(env, argv[PARAM2]);
        if (argc == ARGS_FOUR) {
            NAPI_CALL(env, napi_typeof(env, argv[PARAM3], &type));
            NAPI_ASSERT_CALL_ERRCODE_SYNC(env, type == napi_function || type == napi_undefined || type == napi_null,
                error = std::make_shared<ParametersTypeError>("callback", "function"), error, nullptr);
            if (type == napi_function) {
                proxy->UnRegisteredObserver(env, uriStr, argv[PARAM3], std::move(helper), true);
                return nullptr;
            }
        }
        proxy->UnRegisteredObserver(env, uriStr, std::move(helper), true);
        return nullptr;
    }
    return nullptr;
}

napi_value NapiDataShareHelper::Napi_Close(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_get_null(env, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto helper = context->proxy->GetHelper();
        if (helper == nullptr) {
            context->status = napi_ok;
            return;
        }
        if (!helper->Release()) {
            context->error = std::make_shared<InnerError>();
            return;
        }
        context->proxy->SetHelper(nullptr);
        LOG_INFO("Close dataShareHelper succeed.");
        context->status = napi_ok;
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

bool NapiDataShareHelper::HasRegisteredObserver(napi_env env, std::list<sptr<NAPIDataShareObserver>> &list,
    napi_value callback)
{
    for (auto &it : list) {
        if (DataShareJSUtils::Equals(env, callback, it->observer_->GetCallback())) {
            LOG_DEBUG("The observer has already subscribed.");
            return true;
        }
    }
    return false;
}

void NapiDataShareHelper::RegisteredObserver(napi_env env, const std::string &uri, napi_value callback,
    std::shared_ptr<DataShareHelper> helper, bool isNotifyDetails)
{
    std::lock_guard<std::mutex> lck(listMutex_);
    observerMap_.try_emplace(uri);

    auto &list = observerMap_.find(uri)->second;
    if (HasRegisteredObserver(env, list, callback)) {
        LOG_DEBUG("has registered observer");
        return;
    }
    auto innerObserver = std::make_shared<NAPIInnerObserver>(env, callback);
    sptr<NAPIDataShareObserver> observer(new (std::nothrow) NAPIDataShareObserver(innerObserver));
    if (observer == nullptr) {
        LOG_ERROR("observer is nullptr");
        return;
    }
    if (!isNotifyDetails) {
        helper->RegisterObserver(Uri(uri), observer);
    } else {
        helper->RegisterObserverExt(Uri(uri),
            std::shared_ptr<DataShareObserver>(observer.GetRefPtr(), [holder = observer](const auto*) {}), false);
    }
    list.push_back(observer);
}

void NapiDataShareHelper::UnRegisteredObserver(napi_env env, const std::string &uri, napi_value callback,
    std::shared_ptr<DataShareHelper> helper, bool isNotifyDetails)
{
    std::lock_guard<std::mutex> lck(listMutex_);
    auto obs = observerMap_.find(uri);
    if (obs == observerMap_.end()) {
        LOG_DEBUG("this uri hasn't been registered");
        return;
    }
    auto &list = obs->second;
    auto it = list.begin();
    while (it != list.end()) {
        if (!DataShareJSUtils::Equals(env, callback, (*it)->observer_->GetCallback())) {
            ++it;
            continue;
        }
        if (!isNotifyDetails) {
            helper->UnregisterObserver(Uri(uri), *it);
        } else {
            helper->UnregisterObserverExt(Uri(uri),
                std::shared_ptr<DataShareObserver>((*it).GetRefPtr(), [holder = *it](const auto*) {}));
        }
        (*it)->observer_->DeleteReference();
        it = list.erase(it);
        break;
    }
    if (list.empty()) {
        observerMap_.erase(uri);
    }
}

void NapiDataShareHelper::UnRegisteredObserver(napi_env env, const std::string &uri,
    std::shared_ptr<DataShareHelper> helper, bool isNotifyDetails)
{
    std::lock_guard<std::mutex> lck(listMutex_);
    auto obs = observerMap_.find(uri);
    if (obs == observerMap_.end()) {
        LOG_DEBUG("this uri hasn't been registered");
        return;
    }
    auto &list = obs->second;
    auto it = list.begin();
    while (it != list.end()) {
        if (!isNotifyDetails) {
            helper->UnregisterObserver(Uri(uri), *it);
        } else {
            helper->UnregisterObserverExt(Uri(uri),
                std::shared_ptr<DataShareObserver>((*it).GetRefPtr(), [holder = *it](const auto*) {}));
        }
        (*it)->observer_->DeleteReference();
        it = list.erase(it);
    }
    observerMap_.erase(uri);
}

napi_value NapiDataShareHelper::Napi_SubscribeRdbObserver(napi_env env, size_t argc, napi_value *argv, napi_value self)
{
    std::vector<OperationResult> results;
    napi_value jsResults = DataShareJSUtils::Convert2JSValue(env, results);
    std::shared_ptr<Error> error = nullptr;
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, argc == ARGS_FOUR, error = std::make_shared<ParametersNumError>("4"), error,
        jsResults);

    NapiDataShareHelper *proxy = nullptr;
    NAPI_CALL_BASE(env, napi_unwrap(env, self, reinterpret_cast<void **>(&proxy)), jsResults);
    NAPI_ASSERT_BASE(env, proxy != nullptr, "there is no NapiDataShareHelper instance", jsResults);
    auto helper = proxy->GetHelper();
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, helper != nullptr, error = std::make_shared<HelperAlreadyClosedError>(), error,
        jsResults);

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[1], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_object,
        error = std::make_shared<ParametersTypeError>("uris", "Array<String>"), error, jsResults);
    std::vector<std::string> uris =
        DataShareJSUtils::Convert2StrVector(env, argv[1], DataShareJSUtils::DEFAULT_BUF_SIZE);

    NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_object,
        error = std::make_shared<ParametersTypeError>("templateId", "TemplateId"), error, jsResults);
    TemplateId templateId = DataShareJSUtils::Convert2TemplateId(env, argv[PARAM2]);

    NAPI_CALL(env, napi_typeof(env, argv[PARAM3], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_function,
        error = std::make_shared<ParametersTypeError>("callback", "function"), error, jsResults);

    if (proxy->jsRdbObsManager_ == nullptr) {
        LOG_ERROR("proxy->jsManager_ is nullptr");
        return jsResults;
    }
    results = proxy->jsRdbObsManager_->AddObservers(env, argv[PARAM3], uris, templateId);
    return DataShareJSUtils::Convert2JSValue(env, results);
}

napi_value NapiDataShareHelper::Napi_UnsubscribeRdbObserver(napi_env env, size_t argc, napi_value *argv,
    napi_value self)
{
    std::vector<OperationResult> results;
    napi_value jsResults = DataShareJSUtils::Convert2JSValue(env, results);
    std::shared_ptr<Error> error = nullptr;
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, argc == ARGS_THREE || argc == ARGS_FOUR,
        error = std::make_shared<ParametersNumError>("3 or 4"), error, jsResults);

    NapiDataShareHelper *proxy = nullptr;
    NAPI_CALL_BASE(env, napi_unwrap(env, self, reinterpret_cast<void **>(&proxy)), jsResults);
    NAPI_ASSERT_BASE(env, proxy != nullptr, "there is no NapiDataShareHelper instance", jsResults);
    auto helper = proxy->GetHelper();
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, helper != nullptr, error = std::make_shared<HelperAlreadyClosedError>(), error,
        jsResults);

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[1], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_object,
        error = std::make_shared<ParametersTypeError>("uris", "Array<String>"), error, jsResults);
    std::vector<std::string> uris =
        DataShareJSUtils::Convert2StrVector(env, argv[1], DataShareJSUtils::DEFAULT_BUF_SIZE);

    NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_object,
        error = std::make_shared<ParametersTypeError>("templateId", "TemplateId"), error, jsResults);
    TemplateId templateId = DataShareJSUtils::Convert2TemplateId(env, argv[2]);

    if (proxy->jsRdbObsManager_ == nullptr) {
        LOG_ERROR("proxy->jsManager_ is nullptr");
        return jsResults;
    }

    if (argc == ARGS_FOUR) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM3], &valueType));
        NAPI_ASSERT_CALL_ERRCODE_SYNC(env,
            valueType == napi_function || valueType == napi_undefined || valueType == napi_null,
            error = std::make_shared<ParametersTypeError>("callback", "function"), error, jsResults);
        if (valueType == napi_function) {
            results = proxy->jsRdbObsManager_->DelObservers(env, argv[PARAM3], uris, templateId);
            return DataShareJSUtils::Convert2JSValue(env, results);
        }
    }
    results = proxy->jsRdbObsManager_->DelObservers(env, nullptr, uris, templateId);
    return DataShareJSUtils::Convert2JSValue(env, results);
}

napi_value NapiDataShareHelper::Napi_SubscribePublishedObserver(napi_env env, size_t argc, napi_value *argv,
    napi_value self)
{
    std::vector<OperationResult> results;
    napi_value jsResults = DataShareJSUtils::Convert2JSValue(env, results);
    std::shared_ptr<Error> error = nullptr;
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, argc == ARGS_FOUR, error = std::make_shared<ParametersNumError>("4"), error,
        jsResults);

    NapiDataShareHelper *proxy = nullptr;
    NAPI_CALL_BASE(env, napi_unwrap(env, self, reinterpret_cast<void **>(&proxy)), jsResults);
    NAPI_ASSERT_BASE(env, proxy != nullptr, "there is no NapiDataShareHelper instance", jsResults);
    auto helper = proxy->GetHelper();
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, helper != nullptr, error = std::make_shared<HelperAlreadyClosedError>(), error,
        jsResults);

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[1], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_object,
        error = std::make_shared<ParametersTypeError>("uris", "Array<String>"), error, jsResults);
    std::vector<std::string> uris =
        DataShareJSUtils::Convert2StrVector(env, argv[1], DataShareJSUtils::DEFAULT_BUF_SIZE);

    NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_string,
        error = std::make_shared<ParametersTypeError>("subscriberId", "string"), error, jsResults);
    std::string subscriberId = DataShareJSUtils::Convert2String(env, argv[PARAM2]);

    NAPI_CALL(env, napi_typeof(env, argv[PARAM3], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_function,
        error = std::make_shared<ParametersTypeError>("callback", "function"), error, jsResults);

    if (proxy->jsPublishedObsManager_ == nullptr) {
        LOG_ERROR("proxy->jsPublishedObsManager_ is nullptr");
        return jsResults;
    }
    results = proxy->jsPublishedObsManager_->AddObservers(env, argv[PARAM3], uris, atoll(subscriberId.c_str()));
    return DataShareJSUtils::Convert2JSValue(env, results);
}

napi_value NapiDataShareHelper::Napi_UnsubscribePublishedObserver(napi_env env, size_t argc, napi_value *argv,
    napi_value self)
{
    std::vector<OperationResult> results;
    napi_value jsResults = DataShareJSUtils::Convert2JSValue(env, results);
    std::shared_ptr<Error> error = nullptr;
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, argc == ARGS_THREE || argc == ARGS_FOUR,
        error = std::make_shared<ParametersNumError>("3 or 4"), error, jsResults);

    NapiDataShareHelper *proxy = nullptr;
    NAPI_CALL_BASE(env, napi_unwrap(env, self, reinterpret_cast<void **>(&proxy)), jsResults);
    NAPI_ASSERT_BASE(env, proxy != nullptr, "there is no NapiDataShareHelper instance", jsResults);
    auto helper = proxy->GetHelper();
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, helper != nullptr, error = std::make_shared<HelperAlreadyClosedError>(), error,
        jsResults);

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[1], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_object,
        error = std::make_shared<ParametersTypeError>("uris", "Array<String>"), error, jsResults);
    std::vector<std::string> uris =
        DataShareJSUtils::Convert2StrVector(env, argv[1], DataShareJSUtils::DEFAULT_BUF_SIZE);

    NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_string,
        error = std::make_shared<ParametersTypeError>("subscriberId", "string"), error, jsResults);
    std::string subscriberId = DataShareJSUtils::Convert2String(env, argv[PARAM2]);
    if (proxy->jsPublishedObsManager_ == nullptr) {
        LOG_ERROR("proxy->jsPublishedObsManager_ is nullptr");
        return jsResults;
    }

    if (argc == ARGS_FOUR) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM3], &valueType));
        NAPI_ASSERT_CALL_ERRCODE_SYNC(env,
            valueType == napi_function || valueType == napi_undefined || valueType == napi_null,
            error = std::make_shared<ParametersTypeError>("callback", "function"), error, jsResults);
        if (valueType == napi_function) {
            results = proxy->jsPublishedObsManager_->DelObservers(env, argv[PARAM3], uris, atoll(subscriberId.c_str()));
            return DataShareJSUtils::Convert2JSValue(env, results);
        }
    }
    results = proxy->jsPublishedObsManager_->DelObservers(env, nullptr, uris, atoll(subscriberId.c_str()));
    return DataShareJSUtils::Convert2JSValue(env, results);
}

napi_value NapiDataShareHelper::EnableSilentProxy(napi_env env, napi_callback_info info)
{
    return SetSilentSwitch(env, info, true);
}

napi_value NapiDataShareHelper::DisableSilentProxy(napi_env env, napi_callback_info info)
{
    return SetSilentSwitch(env, info, false);
}

napi_value NapiDataShareHelper::SetSilentSwitch(napi_env env, napi_callback_info info, bool enable)
{
    auto context = std::make_shared<CreateContextInfo>();
    context->silentSwitch = enable;
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 1 && argc != 2) {
            context->error = std::make_shared<ParametersNumError>("1 or 2");
            return napi_invalid_arg;
        }
        context->contextS = OHOS::AbilityRuntime::GetStageModeContext(env, argv[0]);
        NAPI_ASSERT_CALL_ERRCODE(env, context->contextS != nullptr,
            context->error = std::make_shared<ParametersTypeError>("contextS", "not nullptr"), napi_invalid_arg);
        if (argc > 1) {
            NAPI_ASSERT_CALL_ERRCODE(env, GetSilentUri(env, argv[1], context->strUri),
                context->error = std::make_shared<ParametersTypeError>("uri", "string"), napi_invalid_arg);
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        OHOS::Uri uri(context->strUri);
        DataShareHelper::SetSilentSwitch(uri, context->silentSwitch);
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

void NapiDataShareHelper::NapiDataShareHelper::SetHelper(std::shared_ptr<DataShareHelper> dataShareHelper)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    datashareHelper_ = std::move(dataShareHelper);
}
std::shared_ptr<DataShareHelper> NapiDataShareHelper::GetHelper()
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return datashareHelper_;
}
} // namespace DataShare
} // namespace OHOS
