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

#include "ani_datashare_observer.h"

#include <chrono>
#include <cinttypes>

namespace OHOS {
namespace DataShare {
using namespace std::chrono;
ANIInnerObserver::ANIInnerObserver(ani_env *env, ani_class cls, ani_method callback) : env_(env), cls_(cls),
    callback_(callback)
{
}

void ANIInnerObserver::OnChange(const DataShareObserver::ChangeInfo& changeInfo, bool isNotifyDetails)
{
    auto time =
        static_cast<uint64_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    LOG_INFO("ANIInnerObserver datashare callback start, times %{public}" PRIu64 ".", time);
    if (callback_ == nullptr) {
        LOG_ERROR("callback_ is nullptr");
        return;
    }

    switch (changeInfo.changeType_) {
        case ANIDataShareObserver::ChangeType::INSERT:
        case ANIDataShareObserver::ChangeType::DELETE:
        case ANIDataShareObserver::ChangeType::UPDATE: {
                ani_class cls;
                const char *className = "LChangeInfoInner;";
                if (ANI_OK != env_->FindClass(className, &cls)) {
                    LOG_ERROR("Not found class name '%{public}s'", className);
                    return;
                }

                ani_method ctor;
                if (ANI_OK != env_->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
                    LOG_ERROR("Get ctor Failed");
                    return;
                }

                ani_object infoObj = {};
                if (ANI_OK != env_->Object_New(cls, ctor, &infoObj, reinterpret_cast<ani_long>(&changeInfo))) {
                    LOG_ERROR("Create Object Failed");
                    return;
                }

                ani_status callStatus = env_->Object_CallMethod_Void(cls_, callback_, infoObj);
                if (ANI_OK != callStatus) {
                    LOG_DEBUG("ani_call_function succeed status : %{public}d", callStatus);
                    return;
                }
            }
            break;
        default: {
                ani_status callStatus = env_->Object_CallMethod_Void(cls_, callback_);
                if (ANI_OK != callStatus) {
                    LOG_DEBUG("ani_call_function succeed status : %{public}d", callStatus);
                    return;
                }
            }
            break;
    }

    LOG_INFO("ANIInnerObserver datashare callback end, times %{public}" PRIu64 ".", time);
}

ani_method ANIInnerObserver::GetCallback()
{
    return callback_;
}
}  // namespace DataShare
}  // namespace OHOS