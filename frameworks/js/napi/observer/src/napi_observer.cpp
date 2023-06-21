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

#include "napi_observer.h"

#include "datashare_js_utils.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
NapiObserver::NapiObserver(napi_env env, napi_value callback) : env_(env)
{
    napi_create_reference(env, callback, 1, &ref_);
    napi_get_uv_event_loop(env, &loop_);
}

void NapiObserver::CallbackFunc(uv_work_t *work, int status)
{
    LOG_DEBUG("RdbObsCallbackFunc start");
    std::shared_ptr<ObserverWorker> innerWorker(reinterpret_cast<ObserverWorker *>(work->data));
    std::shared_ptr<NapiObserver> observer = innerWorker->observer_.lock();
    if (observer == nullptr || observer->ref_ == nullptr) {
        delete work;
        LOG_ERROR("rdbObserver->ref_ is nullptr");
        return;
    }
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(observer->env_, &scope);
    if (scope == nullptr) {
        delete work;
        return;
    }
    napi_value callback = nullptr;
    napi_value param[2];
    napi_value global = nullptr;
    napi_value result;
    napi_get_reference_value(observer->env_, observer->ref_, &callback);
    napi_get_global(observer->env_, &global);
    napi_get_undefined(observer->env_, &param[0]);
    param[1] = innerWorker->getParam(observer->env_);
    napi_status callStatus = napi_call_function(observer->env_, global, callback, 2, param, &result);
    napi_close_handle_scope(observer->env_, scope);
    if (callStatus != napi_ok) {
        LOG_ERROR("napi_call_function failed status : %{public}d", callStatus);
    }
    delete work;
}

NapiObserver::~NapiObserver()
{
    if (ref_ != nullptr) {
        napi_delete_reference(env_, ref_);
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

void NapiRdbObserver::OnChange(const RdbChangeNode &changeNode)
{
    LOG_DEBUG("NapiRdbObserver onchange Start");
    if (ref_ == nullptr) {
        LOG_ERROR("ref_ is nullptr");
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

    uv_work_t *work = new (std::nothrow) uv_work_t();
    if (work == nullptr) {
        delete observerWorker;
        LOG_ERROR("Failed to create uv work");
        return;
    }
    work->data = observerWorker;
    int ret = uv_queue_work(
        loop_, work, [](uv_work_t *work) {}, CallbackFunc);
    if (ret != 0) {
        LOG_ERROR("uv_queue_work failed");
        delete observerWorker;
        delete work;
    }
}

void NapiPublishedObserver::OnChange(PublishedDataChangeNode &changeNode)
{
    LOG_DEBUG("NapiPublishedObserver onchange Start");
    if (ref_ == nullptr) {
        LOG_ERROR("ref_ is nullptr");
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

    uv_work_t *work = new (std::nothrow) uv_work_t();
    if (work == nullptr) {
        delete observerWorker;
        LOG_ERROR("Failed to create uv work");
        return;
    }
    work->data = observerWorker;
    int ret = uv_queue_work(
        loop_, work, [](uv_work_t *work) {}, CallbackFunc);
    if (ret != 0) {
        LOG_ERROR("uv_queue_work failed");
        delete observerWorker;
        delete work;
    }
}
} // namespace DataShare
} // namespace OHOS
