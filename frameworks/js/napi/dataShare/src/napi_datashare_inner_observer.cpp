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

#include "napi_datashare_observer.h"

#include <memory>
#include <chrono>
#include <cinttypes>
#include "datashare_log.h"
#include "napi_datashare_values_bucket.h"

namespace OHOS {
namespace DataShare {
using namespace std::chrono;
NAPIInnerObserver::NAPIInnerObserver(napi_env env, napi_value callback)
    : env_(env)
{
    napi_create_reference(env, callback, 1, &ref_);
    napi_get_uv_event_loop(env, &loop_);
}

void NAPIInnerObserver::OnComplete(ObserverWorker* observerWorker)
{
    LOG_DEBUG("napi_send_event start");
    auto observer = observerWorker->observer_.lock();
    if (observer == nullptr || observer->ref_ == nullptr) {
        LOG_ERROR("innerWorker->observer_->ref_ is nullptr");
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
    napi_value args[2] = {0};
    napi_value global = nullptr;
    napi_get_reference_value(observer->env_, observer->ref_, &callback);
    napi_get_global(observer->env_, &global);
    napi_get_undefined(observer->env_, &args[0]);
    if (observerWorker->isNotifyDetails_) {
        args[1] = DataShareJSUtils::Convert2JSValue(observer->env_, observerWorker->result_);
    }
    napi_status callStatus = napi_call_function(observer->env_, global, callback, 2, args, nullptr);
    napi_close_handle_scope(observer->env_, scope);
    if (callStatus != napi_ok) {
        LOG_ERROR("napi_call_function failed status : %{public}d", callStatus);
    }
    LOG_DEBUG("napi_call_function succeed status : %{public}d", callStatus);
    delete observerWorker;
}

void NAPIInnerObserver::OnChange(const DataShareObserver::ChangeInfo& changeInfo, bool isNotifyDetails)
{
    auto time =
        static_cast<uint64_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    LOG_INFO("NAPIInnerObserver datashare callback start, times %{public}" PRIu64 ".", time);
    if (ref_ == nullptr) {
        LOG_ERROR("ref_ is nullptr");
        return;
    }
    ObserverWorker* observerWorker = new (std::nothrow) ObserverWorker(shared_from_this(), changeInfo);
    if (observerWorker == nullptr) {
        LOG_ERROR("Failed to create observerWorker");
        return;
    }
    observerWorker->isNotifyDetails_ = isNotifyDetails;
    auto task = [observerWorker]() {
        NAPIInnerObserver::OnComplete(observerWorker);
    };
    int ret = napi_send_event(env_, task, napi_eprio_immediate);
    if (ret != 0) {
        LOG_ERROR("napi_send_event failed: %{public}d", ret);
        delete observerWorker;
    }
    LOG_INFO("NAPIInnerObserver datashare callback end, times %{public}" PRIu64 ".", time);
}

void NAPIInnerObserver::DeleteReference()
{
    if (ref_ != nullptr) {
        napi_delete_reference(env_, ref_);
        ref_ = nullptr;
    }
}

napi_ref NAPIInnerObserver::GetCallback()
{
    return ref_;
}
}  // namespace DataShare
}  // namespace OHOS