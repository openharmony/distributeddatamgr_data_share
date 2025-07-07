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

#include "napi_dataproxy_handle.h"
#include "dataproxy_handle_common.h"
#include "datashare_error_impl.h"
#include "datashare_js_utils.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"
#include "js_native_api_types.h"

namespace OHOS {
namespace DataShare {
static thread_local napi_ref constructor_ = nullptr;
static constexpr int MAX_ARGC = 4;
using DataProxyHandle = OHOS::DataShare::DataProxyHandle;
napi_value NapiDataProxyHandle::GetConstructor(napi_env env)
{
    napi_value cons = nullptr;
    if (constructor_ != nullptr) {
        napi_status status = napi_get_reference_value(env, constructor_, &cons);
        if (status != napi_ok) {
            LOG_ERROR("napi get reference value failed. napi_status: %{public}d", status);
        }
        return cons;
    }
    napi_property_descriptor clzDes[] = {
        DECLARE_NAPI_FUNCTION("publish", Napi_Publish),
        DECLARE_NAPI_FUNCTION("delete", Napi_Delete),
        DECLARE_NAPI_FUNCTION("get", Napi_Get),
        DECLARE_NAPI_FUNCTION("on", Napi_On),
        DECLARE_NAPI_FUNCTION("off", Napi_Off),
    };
    NAPI_CALL(env, napi_define_class(env, "DataProxyHandle", NAPI_AUTO_LENGTH, Initialize, nullptr,
        sizeof(clzDes) / sizeof(napi_property_descriptor), clzDes, &cons));
    napi_status status = napi_create_reference(env, cons, 1, &constructor_);
    if (status != napi_ok) {
        LOG_ERROR("napi create reference failed. napi_status: %{public}d", status);
    }
    return cons;
}

napi_value NapiDataProxyHandle::Initialize(napi_env env, napi_callback_info info)
{
    LOG_INFO("Start");
    napi_value self = nullptr;
    size_t argc = ARGS_MAX_COUNT;
    napi_value argv[ARGS_MAX_COUNT] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));

    auto *proxy = new (std::nothrow) NapiDataProxyHandle();
    if (proxy == nullptr) {
        return nullptr;
    }
    auto finalize = [](napi_env env, void *data, void *hint) {
        NapiDataProxyHandle *proxy = reinterpret_cast<NapiDataProxyHandle *>(data);
        delete proxy;
    };
    if (napi_wrap(env, self, proxy, finalize, nullptr, nullptr) != napi_ok) {
        finalize(env, proxy, nullptr);
        return nullptr;
    }
    return self;
}

napi_value NapiDataProxyHandle::Napi_CreateDataProxyHandle(napi_env env, napi_callback_info info)
{
    auto ctxInfo = std::make_shared<HandleContextInfo>();
    auto input = [ctxInfo](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        napi_value handleProxy = nullptr;
        napi_status status = napi_new_instance(env, GetConstructor(env), argc, argv, &handleProxy);
        if (status != napi_ok) {
            LOG_ERROR("napi new instance failed. napi_status: %{public}d", status);
        }
        NAPI_ASSERT_CALL_ERRCODE(env, handleProxy != nullptr && status == napi_ok,
            ctxInfo->error = std::make_shared<InnerError>(), napi_generic_failure);
        napi_create_reference(env, handleProxy, 1, &(ctxInfo->ref));
        ctxInfo->env = env;
        return napi_ok;
    };
    auto output = [ctxInfo](napi_env env, napi_value *result) -> napi_status {
        NAPI_ASSERT_CALL_ERRCODE(env, ctxInfo->dataProxyHandle != nullptr,
            ctxInfo->error = std::make_shared<InnerError>(), napi_generic_failure);
        napi_status status = napi_get_reference_value(env, ctxInfo->ref, result);
        NAPI_ASSERT_CALL_ERRCODE(env, result != nullptr,
            ctxInfo->error = std::make_shared<InnerError>(), napi_generic_failure);
        NapiDataProxyHandle *proxy = nullptr;
        status = napi_unwrap(env, *result, reinterpret_cast<void **>(&proxy));
        NAPI_ASSERT_CALL_ERRCODE(env, proxy != nullptr, ctxInfo->error = std::make_shared<InnerError>(),
            status);
        proxy->jsProxyDataObsManager_ = std::make_shared<NapiProxyDataSubscriberManager>(ctxInfo->dataProxyHandle);
        proxy->SetHandle(std::move(ctxInfo->dataProxyHandle));
        return status;
    };
    auto exec = [ctxInfo](AsyncCall::Context *ctx) {
        auto ret = DataProxyHandle::Create();
        ctxInfo->dataProxyHandle = ret.second;
    };
    ctxInfo->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, ctxInfo);
    return asyncCall.Call(env, exec);
}

bool NapiDataProxyHandle::CheckIsParameterExceed(const std::vector<DataShareProxyData> &proxyDatas)
{
    if (proxyDatas.size() > PROXY_DATA_MAX_COUNT || proxyDatas.empty()) {
        return false;
    }
    for (const auto &data : proxyDatas) {
        // value's limit is 4096 bytes
        if (data.value_.index() == DataProxyValueType::VALUE_STRING) {
            std::string valStr = std::get<std::string>(data.value_);
            if (valStr.size() > VALUE_MAX_SIZE) {
                LOG_ERROR("ProxyData's value is over limit, uri: %{public}s",
                    DataShareStringUtils::Anonymous(data.uri_).c_str());
                return false;
            }
        }
        if (data.uri_.size() > URI_MAX_SIZE) {
            LOG_ERROR("the size of uri %{public}s is over limit", DataShareStringUtils::Anonymous(data.uri_).c_str());
            return false;
        }
    }
    return true;
}

bool NapiDataProxyHandle::CheckIsParameterExceed(const std::vector<std::string> &uris)
{
    for (const auto &uri : uris) {
        if (uri.size() > URI_MAX_SIZE) {
            LOG_ERROR("the size of uri %{public}s is over limit", DataShareStringUtils::Anonymous(uri).c_str());
            return false;
        }
    }
    return true;
}

napi_value NapiDataProxyHandle::Napi_Publish(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2) {
            context->error = std::make_shared<ParametersNumError>("2");
            return napi_invalid_arg;
        }
        napi_valuetype valueType;
        NAPI_CALL_BASE(env, napi_typeof(env, argv[0], &valueType), napi_invalid_arg);

        NAPI_ASSERT_CALL_ERRCODE(env, valueType == napi_object,
            context->error = std::make_shared<ParametersTypeError>("proxyData", "Array<ProxyData>"), napi_invalid_arg);
        context->proxyDatas = DataShareJSUtils::Convert2ProxyData(env, argv[0]);
        NAPI_ASSERT_CALL_ERRCODE(env, CheckIsParameterExceed(context->proxyDatas), context->error =
            std::make_shared<DataProxyHandleParamError>(), napi_invalid_arg);
        NAPI_CALL_BASE(env, napi_typeof(env, argv[1], &valueType), napi_invalid_arg);
        NAPI_ASSERT_CALL_ERRCODE(env, valueType == napi_object,
            context->error = std::make_shared<ParametersTypeError>("config", "DataProxyConfig"), napi_invalid_arg);
        DataShareJSUtils::UnwrapDataProxyConfig(env, argv[1], context->config);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        NAPI_ASSERT_BASE(env, context->status == napi_ok, "exec failed", napi_generic_failure);
        *result = DataShareJSUtils::Convert2JSValue(env, context->proxyResult);
        context->proxyResult.clear();
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto handle = context->proxy->GetHandle();
        NAPI_ASSERT_CALL_ERRCODE(env, handle != nullptr,
            context->error = std::make_shared<InnerError>(), napi_generic_failure);

        context->proxyResult = handle->PublishProxyData(context->proxyDatas, context->config);
        context->status = napi_ok;
        return napi_ok;
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataProxyHandle::Napi_Delete(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2) {
            context->error = std::make_shared<ParametersNumError>("2");
            return napi_invalid_arg;
        }

        napi_valuetype valueType;
        NAPI_CALL_BASE(env, napi_typeof(env, argv[0], &valueType), napi_invalid_arg);
        NAPI_ASSERT_CALL_ERRCODE(env, valueType == napi_object,
            context->error = std::make_shared<ParametersTypeError>("uris", "Array<String>"), napi_invalid_arg);
        context->uris =
            DataShareJSUtils::Convert2StrVector(env, argv[0], DataShareJSUtils::DEFAULT_BUF_SIZE);
        NAPI_ASSERT_CALL_ERRCODE(env, !(context->uris.empty()),
            context->error = std::make_shared<ParametersTypeError>("uris", "not empty"), napi_invalid_arg);
        NAPI_ASSERT_CALL_ERRCODE(env, context->uris.size() <= URI_MAX_COUNT &&
            CheckIsParameterExceed(context->uris), context->error =
            std::make_shared<DataProxyHandleParamError>(), napi_invalid_arg);
        NAPI_CALL_BASE(env, napi_typeof(env, argv[1], &valueType), napi_invalid_arg);
        NAPI_ASSERT_CALL_ERRCODE(env, valueType == napi_object,
            context->error = std::make_shared<ParametersTypeError>("config", "DataProxyConfig"), napi_invalid_arg);
        if (!DataShareJSUtils::UnwrapDataProxyConfig(env, argv[1], context->config)) {
            context->error = std::make_shared<ParametersTypeError>("config", "DataProxyConfig");
            return napi_invalid_arg;
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        NAPI_ASSERT_BASE(env, context->status == napi_ok, "exec failed", napi_generic_failure);
        *result = DataShareJSUtils::Convert2JSValue(env, context->proxyResult);
        context->proxyResult.clear();
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto handle = context->proxy->GetHandle();
        NAPI_ASSERT_CALL_ERRCODE(env, handle != nullptr,
            context->error = std::make_shared<InnerError>(), napi_generic_failure);

        context->proxyResult = handle->DeleteProxyData(context->uris, context->config);
        context->status = napi_ok;
        return napi_ok;
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataProxyHandle::Napi_Get(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        if (argc != 2) {
            context->error = std::make_shared<ParametersNumError>("2");
            return napi_invalid_arg;
        }

        napi_valuetype valueType;
        NAPI_CALL_BASE(env, napi_typeof(env, argv[0], &valueType), napi_invalid_arg);
        NAPI_ASSERT_CALL_ERRCODE(env, valueType == napi_object,
            context->error = std::make_shared<ParametersTypeError>("uris", "Array<String>"), napi_invalid_arg);
        context->uris =
            DataShareJSUtils::Convert2StrVector(env, argv[0], DataShareJSUtils::DEFAULT_BUF_SIZE);
        NAPI_ASSERT_CALL_ERRCODE(env, !(context->uris.empty()),
            context->error = std::make_shared<ParametersTypeError>("uris", "not empty"), napi_invalid_arg);
        NAPI_ASSERT_CALL_ERRCODE(env, context->uris.size() <= URI_MAX_COUNT &&
            CheckIsParameterExceed(context->uris), context->error =
            std::make_shared<DataProxyHandleParamError>(), napi_invalid_arg);
        if (!DataShareJSUtils::UnwrapDataProxyConfig(env, argv[1], context->config)) {
            context->error = std::make_shared<ParametersTypeError>("config", "DataProxyConfig");
            return napi_invalid_arg;
        }
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        NAPI_ASSERT_BASE(env, context->status == napi_ok, "exec failed", napi_generic_failure);
        *result = DataShareJSUtils::Convert2JSValue(env, context->proxyGetResult);
        context->proxyResult.clear();
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        auto handle = context->proxy->GetHandle();
        NAPI_ASSERT_CALL_ERRCODE(env, handle != nullptr,
            context->error = std::make_shared<InnerError>(), napi_generic_failure);

        context->proxyGetResult = handle->GetProxyData(context->uris, context->config);
        context->status = napi_ok;
        return napi_ok;
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context);
    return asyncCall.Call(env, exec);
}

napi_value NapiDataProxyHandle::Napi_On(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    size_t argc = MAX_ARGC;
    napi_value argv[MAX_ARGC] = { nullptr };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    std::shared_ptr<Error> error = nullptr;
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, argc == ARGS_FOUR,
        error = std::make_shared<ParametersNumError>("4"), error, nullptr);
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_string,
        error = std::make_shared<ParametersTypeError>("event", "string"), error, nullptr);
    std::string type = DataShareJSUtils::Convert2String(env, argv[0]);
    if (type == "dataChange") {
        return Napi_SubscribeProxyData(env, argc, argv, self);
    }
    LOG_ERROR("wrong register type : %{public}s", type.c_str());
    return nullptr;
}

napi_value NapiDataProxyHandle::Napi_Off(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    size_t argc = MAX_ARGC;
    napi_value argv[MAX_ARGC] = { nullptr };
    std::shared_ptr<Error> error = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, argc == ARGS_THREE || argc == ARGS_FOUR,
        error = std::make_shared<ParametersNumError>("3 or 4"), error, nullptr);

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_string,
        error = std::make_shared<ParametersTypeError>("event", "string"), error, nullptr);
    std::string type = DataShareJSUtils::Convert2String(env, argv[0]);
    if (type == "dataChange") {
        return Napi_UnSubscribeProxyData(env, argc, argv, self);
    }
    LOG_ERROR("wrong register type : %{public}s", type.c_str());
    return nullptr;
}

void NapiDataProxyHandle::NapiDataProxyHandle::SetHandle(std::shared_ptr<DataProxyHandle> dataProxyHandle)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    dataProxyHandle_ = std::move(dataProxyHandle);
}

std::shared_ptr<DataProxyHandle> NapiDataProxyHandle::GetHandle()
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return dataProxyHandle_;
}

napi_value NapiDataProxyHandle::Napi_SubscribeProxyData(napi_env env, size_t argc, napi_value *argv, napi_value self)
{
    std::vector<DataProxyResult> results;
    napi_value jsResults = DataShareJSUtils::Convert2JSValue(env, results);
    std::shared_ptr<Error> error = nullptr;
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, argc == ARGS_FOUR, error = std::make_shared<ParametersNumError>("4"), error,
        jsResults);

    NapiDataProxyHandle *proxy = nullptr;
    NAPI_CALL_BASE(env, napi_unwrap(env, self, reinterpret_cast<void **>(&proxy)), jsResults);
    NAPI_ASSERT_BASE(env, proxy != nullptr, "there is no NapiDataProxyHandle instance", jsResults);
    auto handle = proxy->GetHandle();
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, handle != nullptr, error = std::make_shared<InnerError>(), error,
        jsResults);

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_object,
        error = std::make_shared<ParametersTypeError>("uris", "Array<String>"), error, jsResults);
    std::vector<std::string> uris =
        DataShareJSUtils::Convert2StrVector(env, argv[PARAM1], DataShareJSUtils::DEFAULT_BUF_SIZE);
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, !(uris.empty()),
        error = std::make_shared<ParametersTypeError>("uris", "not empty"), error, jsResults);
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, uris.size() <= URI_MAX_COUNT &&
        CheckIsParameterExceed(uris), error = std::make_shared<DataProxyHandleParamError>(), error, jsResults);

    NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_object,
        error = std::make_shared<ParametersTypeError>("config", "DataProxyConfig"), error, jsResults);
    DataProxyConfig config;
    DataShareJSUtils::UnwrapDataProxyConfig(env, argv[PARAM2], config);

    NAPI_CALL(env, napi_typeof(env, argv[PARAM3], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_function,
        error = std::make_shared<ParametersTypeError>("callback", "function"), error, jsResults);

    if (proxy->jsProxyDataObsManager_ == nullptr) {
        LOG_ERROR("proxy->jsManager_ is nullptr");
        return jsResults;
    }
    results = proxy->jsProxyDataObsManager_->AddObservers(env, argv[PARAM3], uris);
    return DataShareJSUtils::Convert2JSValue(env, results);
}

napi_value NapiDataProxyHandle::Napi_UnSubscribeProxyData(napi_env env, size_t argc, napi_value *argv, napi_value self)
{
    std::vector<DataProxyResult> results;
    napi_value jsResults = DataShareJSUtils::Convert2JSValue(env, results);
    std::shared_ptr<Error> error = nullptr;
    NapiDataProxyHandle* proxy = nullptr;
    napi_valuetype valueType;
    NAPI_CALL_BASE(env, napi_unwrap(env, self, reinterpret_cast<void**>(&proxy)), nullptr);
    NAPI_ASSERT_BASE(env, proxy != nullptr, "there is no NapiDataProxyHandle instance", nullptr);
    auto handle = proxy->GetHandle();
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, handle != nullptr, error = std::make_shared<InnerError>(), error,
        nullptr);
    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_object,
        error = std::make_shared<ParametersTypeError>("uris", "Array<String>"), error, nullptr);
    auto uris = DataShareJSUtils::Convert2StrVector(env, argv[PARAM1], DataShareJSUtils::DEFAULT_BUF_SIZE);
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, !(uris.empty()),
        error = std::make_shared<ParametersTypeError>("uris", "not empty"), error, jsResults);
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, uris.size() <= URI_MAX_COUNT &&
        CheckIsParameterExceed(uris), error = std::make_shared<DataProxyHandleParamError>(), error, jsResults);

    NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valueType));
    NAPI_ASSERT_CALL_ERRCODE_SYNC(env, valueType == napi_object,
        error = std::make_shared<ParametersTypeError>("config", "DataProxyConfig"), error, jsResults);
    DataProxyConfig config;
    DataShareJSUtils::UnwrapDataProxyConfig(env, argv[PARAM2], config);
    if (proxy->jsProxyDataObsManager_ == nullptr) {
        LOG_ERROR("proxy->jsManager_ is nullptr");
        return jsResults;
    }

    if (argc == ARGS_FOUR) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM3], &valueType));
        NAPI_ASSERT_CALL_ERRCODE_SYNC(env,
            valueType == napi_function || valueType == napi_undefined || valueType == napi_null,
            error = std::make_shared<ParametersTypeError>("callback", "function"), error, nullptr);
        results = proxy->jsProxyDataObsManager_->DelObservers(env, argv[PARAM3], uris);
    }

    results = proxy->jsProxyDataObsManager_->DelObservers(env, nullptr, uris);
    return DataShareJSUtils::Convert2JSValue(env, results);
}
} // namespace DataShare
} // namespace OHOS