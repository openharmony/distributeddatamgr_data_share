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

#define LOG_TAG "napi_observer"

#include "napi_observer.h"

#include "dataproxy_handle_common.h"
#include "datashare_js_utils.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
NapiObserver::NapiObserver(napi_env env, napi_value callback) : env_(env)
{
    envMutexPtr_ = std::make_unique<std::mutex>();
    napi_create_reference(env, callback, 1, &ref_);
    napi_get_uv_event_loop(env, &loop_);
}

void NapiObserver::CallbackFunc(ObserverWorker *observerWorker)
{
    LOG_DEBUG("ObsCallbackFunc start");
    std::shared_ptr<NapiObserver> observer = observerWorker->observer_.lock();
    if (observer == nullptr || observer->ref_ == nullptr || observer->envMutexPtr_ == nullptr) {
        LOG_ERROR("rdbObserver->ref_ is nullptr");
        delete observerWorker;
        return;
    }
    std::lock_guard<std::mutex> lck(*observer->envMutexPtr_);
    if (observer->env_ == nullptr) {
        LOG_ERROR("rdbObserver->env_ is nullptr");
        delete observerWorker;
        return;
    }
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(observer->env_, &scope);
    if (scope == nullptr) {
        LOG_ERROR("scope is nullptr");
        delete observerWorker;
        return;
    }
    napi_value callback = nullptr;
    napi_value param[2];
    napi_value global = nullptr;
    napi_value result;
    napi_get_reference_value(observer->env_, observer->ref_, &callback);
    napi_get_global(observer->env_, &global);
    napi_get_undefined(observer->env_, &param[0]);
    param[1] = observerWorker->getParam(observer->env_);
    napi_status callStatus = napi_call_function(observer->env_, global, callback, 2, param, &result);
    napi_close_handle_scope(observer->env_, scope);
    if (callStatus != napi_ok) {
        LOG_ERROR("napi_call_function failed status : %{public}d", callStatus);
    }
    LOG_DEBUG("napi_call_function succeed status : %{public}d", callStatus);
    delete observerWorker;
}

NapiObserver::~NapiObserver()
{
    if (env_ == nullptr) {
        LOG_ERROR("env_ is nullptr");
        return;
    }
    
    if (ref_ != nullptr) {
        // SAFETY: observerEnvHookWorker will not be accessed in napi_remove_env_cleanup_hook.
        // Temporary workaround for timing-related crashes：pointer lifetime may be mishandled only if
        // napi_remove_env_cleanup_hook or napi_send_event fails.
        auto task = [env = env_, ref = ref_, observerEnvHookWorker = observerEnvHookWorker_]() {
            napi_delete_reference(env, ref);
            napi_status status = napi_remove_env_cleanup_hook(env, &CleanEnv, observerEnvHookWorker);
            if (status != napi_ok) {
                LOG_ERROR("remove hook failed: %{public}d, env:%{public}d, worker:%{public}d", status, env == nullptr,
                    observerEnvHookWorker == nullptr);
                return;
            }
            if (observerEnvHookWorker != nullptr) {
                delete observerEnvHookWorker;
            }
        };
        int ret = napi_send_event(env_, task, napi_eprio_immediate);
        if (ret != 0) {
            LOG_ERROR("napi_send_event failed: %{public}d, env_:%{public}d", ret, env_ == nullptr);
        }
        ref_ = nullptr;
    }
}

bool NapiObserver::operator==(const NapiObserver &rhs) const
{
    if (ref_ == nullptr) {
        return (rhs.ref_ == nullptr);
    }

    napi_value value1 = nullptr;
    napi_get_reference_value(env_, ref_, &value1);

    napi_value value2 = nullptr;
    napi_get_reference_value(env_, rhs.ref_, &value2);

    bool isEqual = false;
    napi_strict_equals(env_, value1, value2, &isEqual);
    return isEqual;
}

bool NapiObserver::operator!=(const NapiObserver &rhs) const
{
    return !(rhs == *this);
}

void NapiObserver::RegisterEnvCleanHook()
{
    observerEnvHookWorker_ = new (std::nothrow) ObserverEnvHookWorker(shared_from_this());
    if (observerEnvHookWorker_ == nullptr) {
        LOG_ERROR("Failed to create observerEnvHookWorker_");
        return;
    }
    napi_add_env_cleanup_hook(env_, &CleanEnv, observerEnvHookWorker_);
}

void NapiObserver::CleanEnv(void *obj)
{
    LOG_INFO("Napi env cleanup hook is executed, env is about to exit");
    auto observerEnvHookWorker = reinterpret_cast<ObserverEnvHookWorker *>(obj);
    // Prevent concurrency with NAPIInnerObserver destructors
    auto observer = observerEnvHookWorker->observer_.lock();
    if (observer == nullptr || observer->envMutexPtr_ == nullptr) {
        LOG_ERROR("observer or observer->envMutexPtr_ is nullptr");
        delete observerEnvHookWorker;
        return;
    }
    std::lock_guard<std::mutex> lck(*observer->envMutexPtr_);
    if (observer->ref_ != nullptr) {
        napi_delete_reference(observer->env_, observer->ref_);
        observer->ref_ = nullptr;
    }
    observer->env_ = nullptr;
}

void NapiRdbObserver::OnChange(const RdbChangeNode &changeNode)
{
    LOG_DEBUG("NapiRdbObserver onchange Start");
    if (ref_ == nullptr || envMutexPtr_ == nullptr) {
        LOG_ERROR("ref_ or envMutexPtr_ is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lck(*envMutexPtr_);
    if (env_ == nullptr) {
        LOG_ERROR("env_ is nullptr");
        return;
    }
    ObserverWorker *observerWorker = new (std::nothrow) ObserverWorker(shared_from_this());
    if (observerWorker == nullptr) {
        LOG_ERROR("Failed to create observerWorker");
        return;
    }
    observerWorker->getParam = [changeNode](napi_env env) {
        return DataShareJSUtils::Convert2JSValue(env, changeNode);
    };

    auto task = [observerWorker]() {
        NapiObserver::CallbackFunc(observerWorker);
    };
    int ret = napi_send_event(env_, task, napi_eprio_immediate);
    if (ret != 0) {
        LOG_ERROR("napi_send_event failed: %{public}d", ret);
        delete observerWorker;
    }
    LOG_DEBUG("NapiRdbObserver onchange End: %{public}d", ret);
}

void NapiPublishedObserver::OnChange(PublishedDataChangeNode &changeNode)
{
    LOG_DEBUG("NapiPublishedObserver onchange Start");
    if (ref_ == nullptr || envMutexPtr_ == nullptr) {
        LOG_ERROR("ref_ or envMutexPtr_ is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lck(*envMutexPtr_);
    if (env_ == nullptr) {
        LOG_ERROR("env_ is nullptr");
        return;
    }
    ObserverWorker *observerWorker = new (std::nothrow) ObserverWorker(shared_from_this());
    if (observerWorker == nullptr) {
        LOG_ERROR("Failed to create observerWorker");
        return;
    }
    std::shared_ptr<PublishedDataChangeNode> node = std::make_shared<PublishedDataChangeNode>(std::move(changeNode));
    observerWorker->getParam = [node](napi_env env) {
        return DataShareJSUtils::Convert2JSValue(env, *node);
    };

    auto task = [observerWorker]() {
        NapiObserver::CallbackFunc(observerWorker);
    };
    int ret = napi_send_event(env_, task, napi_eprio_immediate);
    if (ret != 0) {
        LOG_ERROR("napi_send_event failed: %{public}d", ret);
        delete observerWorker;
    }
    LOG_DEBUG("NapiRdbObserver onchange End: %{public}d", ret);
}

void NapiProxyDataObserver::OnChange(const std::vector<DataProxyChangeInfo> &changeNode)
{
    LOG_INFO("NapiProxyDataObserver onchange Start");
    if (ref_ == nullptr || envMutexPtr_ == nullptr) {
        LOG_ERROR("ref_ or envMutexPtr_ is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lck(*envMutexPtr_);
    if (env_ == nullptr) {
        LOG_ERROR("env_ is nullptr");
        return;
    }
    ObserverWorker *observerWorker = new (std::nothrow) ObserverWorker(shared_from_this());
    if (observerWorker == nullptr) {
        LOG_ERROR("Failed to create observerWorker");
        return;
    }
    std::shared_ptr<std::vector<DataProxyChangeInfo>> node =
        std::make_shared<std::vector<DataProxyChangeInfo>>(std::move(changeNode));
    observerWorker->getParam = [node](napi_env env) {
        return DataShareJSUtils::Convert2JSValue(env, *node);
    };

    auto task = [observerWorker]() {
        NapiObserver::CallbackFunc(observerWorker);
    };
    int ret = napi_send_event(env_, task, napi_eprio_immediate);
    if (ret != 0) {
        LOG_ERROR("napi_send_event failed: %{public}d", ret);
        delete observerWorker;
    }
    LOG_INFO("NapiProxyDataObserver onchange End: %{public}d", ret);
}
} // namespace DataShare
} // namespace OHOS
