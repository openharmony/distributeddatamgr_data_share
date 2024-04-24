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

#ifndef NAPI_DATASHARE_HELPER_H
#define NAPI_DATASHARE_HELPER_H

#include <memory>
#include <shared_mutex>

#include "async_call.h"
#include "data_share_common.h"
#include "datashare_helper.h"
#include "napi_datashare_observer.h"
#include "napi_subscriber_manager.h"

namespace OHOS {
namespace DataShare {
class NapiDataShareHelper {
public:
    static napi_value Napi_CreateDataShareHelper(napi_env env, napi_callback_info info);
    static napi_value Napi_On(napi_env env, napi_callback_info info);
    static napi_value Napi_Off(napi_env env, napi_callback_info info);
    static napi_value Napi_Insert(napi_env env, napi_callback_info info);
    static napi_value Napi_Delete(napi_env env, napi_callback_info info);
    static napi_value Napi_Query(napi_env env, napi_callback_info info);
    static napi_value Napi_Update(napi_env env, napi_callback_info info);
    static napi_value Napi_BatchUpdate(napi_env env, napi_callback_info info);
    static napi_value Napi_BatchInsert(napi_env env, napi_callback_info info);
    static napi_value Napi_NormalizeUri(napi_env env, napi_callback_info info);
    static napi_value Napi_DenormalizeUri(napi_env env, napi_callback_info info);
    static napi_value Napi_NotifyChange(napi_env env, napi_callback_info info);
    static napi_value Napi_AddTemplate(napi_env env, napi_callback_info info);
    static napi_value Napi_DelTemplate(napi_env env, napi_callback_info info);
    static napi_value Napi_Publish(napi_env env, napi_callback_info info);
    static napi_value Napi_GetPublishedData(napi_env env, napi_callback_info info);
    static napi_value EnableSilentProxy(napi_env env, napi_callback_info info);
    static napi_value DisableSilentProxy(napi_env env, napi_callback_info info);
    static napi_value Napi_Close(napi_env env, napi_callback_info info);

private:
    static napi_value GetConstructor(napi_env env);
    static napi_value Initialize(napi_env env, napi_callback_info info);
    static napi_value Napi_SubscribeRdbObserver(napi_env env, size_t argc, napi_value *argv, napi_value self);
    static napi_value Napi_UnsubscribeRdbObserver(napi_env env, size_t argc, napi_value *argv, napi_value self);
    static napi_value Napi_RegisterObserver(napi_env env, size_t argc, napi_value *argv, napi_value self);
    static napi_value Napi_UnregisterObserver(napi_env env, size_t argc, napi_value *argv, napi_value self);
    static napi_value Napi_SubscribePublishedObserver(napi_env env, size_t argc, napi_value *argv, napi_value self);
    static napi_value Napi_UnsubscribePublishedObserver(napi_env env, size_t argc, napi_value *argv, napi_value self);
    static napi_value SetSilentSwitch(napi_env env, napi_callback_info info, bool enable);

    bool HasRegisteredObserver(napi_env env, std::list<sptr<NAPIDataShareObserver>> &list, napi_value callback);
    void RegisteredObserver(napi_env env, const std::string &uri, napi_value callback,
        std::shared_ptr<DataShareHelper> helper, bool isNotifyDetails = false);
    void UnRegisteredObserver(napi_env env, const std::string &uri, napi_value callback,
        std::shared_ptr<DataShareHelper> helper, bool isNotifyDetails = false);
    void UnRegisteredObserver(napi_env env, const std::string& uri, std::shared_ptr<DataShareHelper> helper,
        bool isNotifyDetails = false);
    static bool GetOptions(napi_env env, napi_value jsValue, CreateOptions &options);
    void SetHelper(std::shared_ptr<DataShareHelper> dataShareHelper);
    std::shared_ptr<DataShareHelper> GetHelper();
    std::shared_ptr<DataShareHelper> datashareHelper_ = nullptr;
    std::map<std::string, std::list<sptr<NAPIDataShareObserver>>> observerMap_;
    std::mutex listMutex_{};
    std::shared_mutex mutex_;
    std::shared_ptr<NapiRdbSubscriberManager> jsRdbObsManager_ = nullptr;
    std::shared_ptr<NapiPublishedSubscriberManager> jsPublishedObsManager_ = nullptr;

    struct CreateContextInfo : public AsyncCall::Context {
        napi_env env = nullptr;
        napi_ref ref = nullptr;
        bool isStageMode = true;
        CreateOptions options;
        std::string strUri;
        std::shared_ptr<AppExecFwk::Context> contextF = nullptr;
        std::shared_ptr<OHOS::AbilityRuntime::Context> contextS = nullptr;
        std::shared_ptr<DataShareHelper> dataShareHelper = nullptr;
        bool silentSwitch = false;
        CreateContextInfo() : Context(nullptr, nullptr) {};
        CreateContextInfo(InputAction input, OutputAction output) : Context(std::move(input), std::move(output)) {};
        ~CreateContextInfo()
        {
            if (env != nullptr && ref != nullptr) {
                napi_delete_reference(env, ref);
            }
        }
    };

    struct ContextInfo : public AsyncCall::Context {
        NapiDataShareHelper *proxy = nullptr;
        napi_status status = napi_generic_failure;
        int resultNumber = 0;
        std::shared_ptr<DataShareResultSet> resultObject = nullptr;
        std::string resultString = "";
        std::vector<std::string> resultStrArr;
        std::string uri;
        std::string mode;
        DataShareValuesBucket valueBucket;
        DataSharePredicates predicates;
        std::vector<std::string> columns;
        std::vector<DataShareValuesBucket> values;
        std::string mimeTypeFilter;
        DatashareBusinessError businessError;
        Data publishData;
        std::string bundleName;
        std::vector<OperationResult> results;
        UpdateOperations updateOperations;
        std::vector<BatchUpdateResult> batchUpdateResult;
        DataShareObserver::ChangeInfo changeInfo;
        bool isNotifyDetails = false;

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
    static void Notify(const std::shared_ptr<NapiDataShareHelper::ContextInfo> context,
        std::shared_ptr<DataShareHelper> helper);
};
} // namespace DataShare
} // namespace OHOS
#endif /* NAPI_DATASHARE_HELPER_H */
