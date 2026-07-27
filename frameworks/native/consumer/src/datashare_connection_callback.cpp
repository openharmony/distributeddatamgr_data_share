/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#define LOG_TAG "datashare_connection_callback"

#include "datashare_connection_callback.h"

#include "datashare_connection.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
DataShareConnectionCallback::DataShareConnectionCallback() : target_() {}

void DataShareConnectionCallback::SetTarget(std::weak_ptr<DataShareConnection> target)
{
    target_ = std::move(target);
}

bool DataShareConnectionCallback::TargetExpired() const
{
    return target_.expired();
}

void DataShareConnectionCallback::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    auto target = target_.lock();
    if (target == nullptr) {
        LOG_WARN("DataShareConnection target is gone before OnAbilityConnectDone");
        return;
    }
    target->OnAbilityConnectDone(element, remoteObject, resultCode);
}

void DataShareConnectionCallback::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    auto target = target_.lock();
    if (target == nullptr) {
        LOG_WARN("DataShareConnection target is gone before OnAbilityDisconnectDone");
        return;
    }
    target->OnAbilityDisconnectDone(element, resultCode);
}
} // namespace DataShare
} // namespace OHOS