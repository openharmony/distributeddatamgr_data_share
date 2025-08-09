/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NAPI_RDB_OBSERVER_H
#define NAPI_RDB_OBSERVER_H

#include <uv.h>

#include "datashare_template.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "dataproxy_handle_common.h"

namespace OHOS {
namespace DataShare {
class NapiObserver : public std::enable_shared_from_this<NapiObserver> {
public:
    NapiObserver(napi_env env, napi_value callback);
    void RegisterEnvCleanHook();
    virtual ~NapiObserver();
    virtual bool operator==(const NapiObserver &rhs) const;
    virtual bool operator!=(const NapiObserver &rhs) const;
    NapiObserver& operator=(NapiObserver &&rhs) = default;
protected:
    struct ObserverWorker {
        std::weak_ptr<NapiObserver> observer_;
        std::function<napi_value(napi_env)> getParam;
        explicit ObserverWorker(std::shared_ptr<NapiObserver> observerIn) : observer_(observerIn) {}
    };
    struct ObserverEnvHookWorker {
        std::weak_ptr<NapiObserver> observer_;
        ObserverEnvHookWorker(std::shared_ptr<NapiObserver> observerIn): observer_(observerIn) {}
    };
    static void CallbackFunc(ObserverWorker *observerWorker);
    static void CleanEnv(void *obj);
    napi_env env_ = nullptr;
    napi_ref ref_ = nullptr;
    uv_loop_s *loop_ = nullptr;
    std::unique_ptr<std::mutex> envMutexPtr_;
    ObserverEnvHookWorker* observerEnvHookWorker_ = nullptr;
};

class NapiRdbObserver final: public NapiObserver {
public:
    NapiRdbObserver(napi_env env, napi_value callback) : NapiObserver(env, callback) {};
    void OnChange(const RdbChangeNode &changeNode);
};

class NapiPublishedObserver final: public NapiObserver {
public:
    NapiPublishedObserver(napi_env env, napi_value callback) : NapiObserver(env, callback) {};
    void OnChange(PublishedDataChangeNode &changeNode);
};

class NapiProxyDataObserver final: public NapiObserver {
public:
    NapiProxyDataObserver(napi_env env, napi_value callback) : NapiObserver(env, callback) {};
    void OnChange(const std::vector<DataProxyChangeInfo> &changeNode);
};
} // namespace DataShare
} // namespace OHOS
#endif //NAPI_RDB_OBSERVER_H
