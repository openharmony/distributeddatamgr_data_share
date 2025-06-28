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

#ifndef NAPI_DATAPROXY_HANDLE_H
#define NAPI_DATAPROXY_HANDLE_H

#include "async_call.h"
#include "dataproxy_handle.h"
#include "data_share_common.h"
#include "hap_module_info.h"
#include "dataproxy_handle_common.h"
#include "napi_datashare_observer.h"
#include "napi_subscriber_manager.h"

namespace OHOS {
namespace DataShare {
class NapiDataProxyHandle {
public:
    static napi_value Napi_CreateDataProxyHandle(napi_env env, napi_callback_info info);
    static napi_value Napi_Publish(napi_env env, napi_callback_info info);
    static napi_value Napi_Delete(napi_env env, napi_callback_info info);
    static napi_value Napi_Get(napi_env env, napi_callback_info info);
    static napi_value Napi_On(napi_env env, napi_callback_info info);
    static napi_value Napi_Off(napi_env env, napi_callback_info info);
private:
    static napi_value GetConstructor(napi_env env);
    static napi_value Initialize(napi_env env, napi_callback_info info);
    static napi_value Napi_SubscribeProxyData(napi_env env, size_t argc, napi_value *argv, napi_value self);
    static napi_value Napi_UnSubscribeProxyData(napi_env env, size_t argc, napi_value *argv, napi_value self);
    std::vector<DataProxyResult> SubscribeProxyData(napi_env env, const std::vector<std::string> &uris,
        napi_value callback, std::shared_ptr<DataProxyHandle> handle);
    std::vector<DataProxyResult> UnsubscribeProxyData(napi_env env, const std::vector<std::string> &uris,
        napi_value callback, std::shared_ptr<DataProxyHandle> handle);
    void SetHandle(std::shared_ptr<DataProxyHandle> dataProxyHandle);
    std::shared_ptr<DataProxyHandle> GetHandle();
    static bool CheckIsParameterExceed(const std::vector<DataShareProxyData> &proxyDatas);
    static bool CheckIsParameterExceed(const std::vector<std::string> &uris);
    std::shared_ptr<DataProxyHandle> dataProxyHandle_ = nullptr;
    std::shared_mutex mutex_;
    std::shared_ptr<NapiProxyDataSubscriberManager> jsProxyDataObsManager_ = nullptr;

    struct HandleContextInfo : public AsyncCall::Context {
        napi_env env = nullptr;
        napi_ref ref = nullptr;
        bool isStageMode = true;
        int32_t errCode;
        std::string strUri;
        std::shared_ptr<DataProxyHandle> dataProxyHandle = nullptr;

        HandleContextInfo() : Context(nullptr, nullptr) {};
        HandleContextInfo(InputAction input, OutputAction output) : Context(std::move(input), std::move(output)) {};
        ~HandleContextInfo()
        {
            if (env != nullptr && ref != nullptr) {
                napi_delete_reference(env, ref);
            }
        }
    };

    struct ContextInfo : public AsyncCall::Context {
        NapiDataProxyHandle *proxy = nullptr;
        napi_status status = napi_generic_failure;

        DatashareBusinessError businessError;
        std::vector<DataShareProxyData> proxyDatas;
        DataProxyType type = DataProxyType::SHARED_CONFIG;
        DataProxyConfig config;
        std::string bundleName;
        std::vector<std::string> uris;
        std::vector<DataProxyResult> proxyResult;
        std::vector<DataProxyGetResult> proxyGetResult;
        int32_t resultNumber = 0;

        ContextInfo() : Context(nullptr, nullptr) {};
        ContextInfo(InputAction input, OutputAction output) : Context(std::move(input), std::move(output)) {};
        virtual ~ContextInfo() {};

        napi_status operator()(napi_env env, size_t argc, napi_value *argv, napi_value self) override
        {
            NAPI_ASSERT_BASE(env, self != nullptr, "self is nullptr", napi_invalid_arg);
            NAPI_CALL_BASE(env, napi_unwrap(env, self, reinterpret_cast<void **>(&proxy)), napi_invalid_arg);
            NAPI_ASSERT_BASE(env, proxy != nullptr, "there is no native upload task", napi_invalid_arg);
            return Context::operator()(env, argc, argv, self);
        }
        napi_status operator()(napi_env env, napi_value *result) override
        {
            if (status != napi_ok) {
                return status;
            }
            return Context::operator()(env, result);
        }
    };
};
} // namespace DataShare
} // namespace OHOS
#endif /* NAPI_DATAPROXY_HANDLE_H */