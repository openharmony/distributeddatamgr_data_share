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

static DataSharePredicates UnwrapDataSharePredicates(napi_env env, napi_value value)
{
    auto predicates = DataSharePredicatesProxy::GetNativePredicates(env, value);
    if (predicates == nullptr) {
        LOG_ERROR("GetNativePredicates is nullptr.");
        return {};
    }
    return DataSharePredicates(predicates->GetOperationList());
}

static bool UnwrapValuesBucketArrayFromJS(napi_env env, napi_value param,
    std::vector<DataShareValuesBucket> &value)
{
    LOG_DEBUG("Start");
    uint32_t arraySize = 0;
    napi_value jsValue = nullptr;
    std::string strValue = "";

    if (!IsArrayForNapiValue(env, param, arraySize)) {
        LOG_ERROR("IsArrayForNapiValue is false");
        return false;
    }

    value.clear();
    for (uint32_t i = 0; i < arraySize; i++) {
        jsValue = nullptr;
        if (napi_get_element(env, param, i, &jsValue) != napi_ok) {
            LOG_ERROR("napi_get_element is false");
            return false;
        }

        DataShareValuesBucket valueBucket;
        valueBucket.Clear();
        if (!GetValueBucketObject(valueBucket, env, jsValue)) {
            return false;
        }

        value.push_back(valueBucket);
    }
    return true;
}

static std::vector<DataShareValuesBucket> GetValuesBucketArray(napi_env env, napi_value param, bool &status)
{
    LOG_DEBUG("Start");
    std::vector<DataShareValuesBucket> result;
    status = UnwrapValuesBucketArrayFromJS(env, param, result);
    return result;
}

static bool GetUri(napi_env env, napi_value jsValue, std::string &uri)
{
    LOG_DEBUG("Start");
    napi_valuetype valuetype = napi_undefined;
    napi_typeof(env, jsValue, &valuetype);
    if (valuetype != napi_string) {
        return false;
    }
    uri = DataShareJSUtils::Convert2String(env, jsValue);
    return true;
}

napi_value NapiDataShareHelper::Napi_CreateDataShareHelper(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto ctxInfo = std::make_shared<CreateContextInfo>();
    auto input = [ctxInfo](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2 && argc != 3) {
            ctxInfo->error = std::make_shared<ParametersNumError>("2 or 3");
            return napi_invalid_arg;
        }
        bool isStageMode = false;
        napi_status status = AbilityRuntime::IsStageContext(env, argv[PARAM0], isStageMode);
        if (status != napi_ok || !isStageMode) {
            ctxInfo->isStageMode = false;
            auto ability = OHOS::AbilityRuntime::GetCurrentAbility(env);
            if (!GetUri(env, argv[PARAM0], ctxInfo->strUri)) {
                ctxInfo->error = std::make_shared<ParametersTypeError>("uri", "string");
                return napi_invalid_arg;
            }

            if (ability == nullptr) {
                ctxInfo->error = std::make_shared<ParametersTypeError>("ability", "not nullptr");
                return napi_invalid_arg;
            }
            ctxInfo->contextF = ability->GetContext();
        } else {
            ctxInfo->contextS = OHOS::AbilityRuntime::GetStageModeContext(env, argv[PARAM0]);
            if (!GetUri(env, argv[PARAM1], ctxInfo->strUri)) {
                ctxInfo->error = std::make_shared<ParametersTypeError>("uri", "string");
                return napi_invalid_arg;
            }

            if (ctxInfo->contextS == nullptr) {
                ctxInfo->error = std::make_shared<ParametersTypeError>("contextS", "not nullptr");
                return napi_invalid_arg;
            }
        }

        napi_value helperProxy = nullptr;
        status = napi_new_instance(env, GetConstructor(env), argc, argv, &helperProxy);
        if ((helperProxy == nullptr) || (status != napi_ok)) {
            ctxInfo->error = std::make_shared<DataShareHelperInitError>();
            return napi_generic_failure;
        }
        napi_create_reference(env, helperProxy, 1, &(ctxInfo->ref));
        ctxInfo->env = env;
        return napi_ok;
    };
    auto output = [ctxInfo](napi_env env, napi_value *result) -> napi_status {
        if (ctxInfo->dataShareHelper == nullptr) {
            ctxInfo->error = std::make_shared<DataShareHelperInitError>();
            return napi_generic_failure;
        }
        napi_status status = napi_get_reference_value(env, ctxInfo->ref, result);
        NapiDataShareHelper *proxy = nullptr;
        status = napi_unwrap(env, *result, reinterpret_cast<void **>(&proxy));
        if (proxy == nullptr) {
            LOG_ERROR("proxy is nullptr");
            return status;
        }
        proxy->datashareHelper_ = std::move(ctxInfo->dataShareHelper);
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
        DECLARE_NAPI_FUNCTION("normalizeUri", Napi_NormalizeUri),
        DECLARE_NAPI_FUNCTION("denormalizeUri", Napi_DenormalizeUri),
        DECLARE_NAPI_FUNCTION("notifyChange", Napi_NotifyChange),
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
    auto finalize = [](napi_env env, void * data, void * hint) {
        NapiDataShareHelper *proxy = reinterpret_cast<NapiDataShareHelper *>(data);
        delete proxy;
        LOG_INFO("NapiDataShareHelper has been deleted successfully!");
    };
    if (napi_wrap(env, self, proxy, finalize, nullptr, nullptr) != napi_ok) {
        finalize(env, proxy, nullptr);
        return nullptr;
    }
    return self;
}

napi_value NapiDataShareHelper::Napi_OpenFile(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        NAPI_ASSERT_BASE(env, argc == 2 || argc == 3, " should 2 or 3 parameters!", napi_invalid_arg);
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        GetUri(env, argv[PARAM0], context->uri);

        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, argv[PARAM1], &valuetype);
        if (valuetype == napi_string) {
            context->mode = DataShareJSUtils::Convert2String(env, argv[PARAM1]);
        } else {
            LOG_ERROR("wrong type, should be napi_string");
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_create_int32(env, context->resultNumber, result);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        if (context->proxy->datashareHelper_ != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            context->resultNumber = context->proxy->datashareHelper_->OpenFile(uri, context->mode);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                      context->proxy->datashareHelper_ == nullptr, context->uri.empty());
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_Insert(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2 && argc != 3) {
            context->error = std::make_shared<ParametersNumError>("2 or 3");
            return napi_invalid_arg;
        }
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        if (!GetUri(env, argv[PARAM0], context->uri)) {
            context->error = std::make_shared<ParametersTypeError>("uri", "string");
            return napi_invalid_arg;
        }

        context->valueBucket.Clear();
        if (!GetValueBucketObject(context->valueBucket, env, argv[PARAM1])) {
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
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_Delete(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2 && argc != 3) {
            context->error = std::make_shared<ParametersNumError>("2 or 3");
            return napi_invalid_arg;
        }
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        if (!GetUri(env, argv[PARAM0], context->uri)) {
            context->error = std::make_shared<ParametersTypeError>("uri", "string");
            return napi_invalid_arg;
        }

        context->predicates = UnwrapDataSharePredicates(env, argv[PARAM1]);
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
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_Query(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 3 && argc != 4) {
            context->error = std::make_shared<ParametersNumError>("3 or 4");
            return napi_invalid_arg;
        }
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        if (!GetUri(env, argv[PARAM0], context->uri)) {
            context->error = std::make_shared<ParametersTypeError>("uri", "string");
            return napi_invalid_arg;
        }

        context->predicates = UnwrapDataSharePredicates(env, argv[PARAM1]);

        context->columns = DataShareJSUtils::Convert2StrVector(env, argv[PARAM2], DataShareJSUtils::DEFAULT_BUF_SIZE);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        if (context->resultObject == nullptr) {
            context->error = std::make_shared<InnerError>();
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
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_Update(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 3 && argc != 4) {
            context->error = std::make_shared<ParametersNumError>("3 or 4");
            return napi_invalid_arg;
        }
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        if (!GetUri(env, argv[PARAM0], context->uri)) {
            context->error = std::make_shared<ParametersTypeError>("uri", "string");
            return napi_invalid_arg;
        }

        context->predicates = UnwrapDataSharePredicates(env, argv[PARAM1]);

        context->valueBucket.Clear();
        if (!GetValueBucketObject(context->valueBucket, env, argv[PARAM2])) {
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
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_BatchInsert(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2 && argc != 3) {
            context->error = std::make_shared<ParametersNumError>("2 or 3");
            return napi_invalid_arg;
        }
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        if (!GetUri(env, argv[PARAM0], context->uri)) {
            context->error = std::make_shared<ParametersTypeError>("uri", "string");
            return napi_invalid_arg;
        }
        bool status = false;
        context->values = GetValuesBucketArray(env, argv[PARAM1], status);
        if (!status) {
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
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_GetType(napi_env env, napi_callback_info info)
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
            context->resultString = context->proxy->datashareHelper_->GetType(uri);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                      context->proxy->datashareHelper_ == nullptr, context->uri.empty());
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataShareHelper::Napi_GetFileTypes(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("Start");
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        NAPI_ASSERT_BASE(env, argc == 2 || argc == 3, " should 2 or 3 parameters!", napi_invalid_arg);
        LOG_DEBUG("argc : %{public}d", static_cast<int>(argc));

        GetUri(env, argv[PARAM0], context->uri);

        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, argv[PARAM1], &valuetype);
        if (valuetype == napi_string) {
            context->mimeTypeFilter = DataShareJSUtils::Convert2String(env, argv[PARAM1]);
        } else {
            LOG_ERROR("wrong type, should be napi_string");
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        *result = DataShareJSUtils::Convert2JSValue(env, context->resultStrArr);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        if (context->proxy->datashareHelper_ != nullptr && !context->uri.empty()) {
            OHOS::Uri uri(context->uri);
            context->resultStrArr = context->proxy->datashareHelper_->GetFileTypes(uri, context->mimeTypeFilter);
            context->status = napi_ok;
        } else {
            LOG_ERROR("dataShareHelper_ is nullptr : %{public}d, context->uri is empty : %{public}d",
                      context->proxy->datashareHelper_ == nullptr, context->uri.empty());
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
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
    AsyncCall asyncCall(env, info, context);
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
    AsyncCall asyncCall(env, info, context);
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
    AsyncCall asyncCall(env, info, context);
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

    proxy->RegisteredObserver(env, uri, argv[PARAM2]);
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
        proxy->UnRegisteredObserver(env, uri, argv[PARAM2]);
        return nullptr;
    }
    proxy->UnRegisteredObserver(env, uri);
    return nullptr;
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

void NapiDataShareHelper::RegisteredObserver(napi_env env, const std::string &uri, napi_value callback)
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
    datashareHelper_->RegisterObserver(Uri(uri), observer);
    list.push_back(observer);
}

void NapiDataShareHelper::UnRegisteredObserver(napi_env env, const std::string &uri, napi_value callback)
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
        datashareHelper_->UnregisterObserver(Uri(uri), *it);
        (*it)->observer_->DeleteReference();
        it = list.erase(it);
        break;
    }
    if (list.empty()) {
        observerMap_.erase(uri);
    }
}

void NapiDataShareHelper::UnRegisteredObserver(napi_env env, const std::string &uri)
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
        datashareHelper_->UnregisterObserver(Uri(uri), *it);
        (*it)->observer_->DeleteReference();
        it = list.erase(it);
    }
    observerMap_.erase(uri);
}
}  // namespace DataShare
}  // namespace OHOS