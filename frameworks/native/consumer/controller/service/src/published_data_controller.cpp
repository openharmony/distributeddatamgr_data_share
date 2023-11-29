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

#include "published_data_controller.h"

#include "datashare_log.h"
#include "published_data_subscriber_manager.h"

namespace OHOS {
namespace DataShare {
std::vector<OperationResult> PublishedDataController::Publish(const Data &data, const std::string &bundleName)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    return proxy->Publish(data, bundleName);
}

Data PublishedDataController::GetPublishedData(const std::string &bundleName, int &resultCode)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return Data();
    }
    return proxy->GetPublishedData(bundleName, resultCode);
}

std::vector<OperationResult> PublishedDataController::SubscribePublishedData(void *subscriber,
    const std::vector<std::string> &uris, int64_t subscriberId,
    const std::function<void(const PublishedDataChangeNode &changeNode)> &callback)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    return PublishedDataSubscriberManager::GetInstance().AddObservers(subscriber, proxy, uris, subscriberId, callback);
}

std::vector<OperationResult> PublishedDataController::UnSubscribePublishedData(void *subscriber,
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    if (uris.empty()) {
        return PublishedDataSubscriberManager::GetInstance().DelObservers(subscriber, proxy);
    }
    return PublishedDataSubscriberManager::GetInstance().DelObservers(subscriber, proxy, uris, subscriberId);
}

std::vector<OperationResult> PublishedDataController::EnableSubscribePublishedData(void *subscriber,
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    return PublishedDataSubscriberManager::GetInstance().EnableObservers(subscriber, proxy, uris, subscriberId);
}

std::vector<OperationResult> PublishedDataController::DisableSubscribePublishedData(void *subscriber,
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    return PublishedDataSubscriberManager::GetInstance().DisableObservers(subscriber, proxy, uris, subscriberId);
}
} // namespace DataShare
} // namespace OHOS