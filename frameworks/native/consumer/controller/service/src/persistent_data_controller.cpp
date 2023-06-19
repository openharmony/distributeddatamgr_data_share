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
#include "persistent_data_controller.h"

#include "datashare_log.h"
#include "rdb_subscriber_manager.h"

namespace OHOS {
namespace DataShare {
constexpr int INVALID_VALUE = -1;
PersistentDataController::PersistentDataController(std::shared_ptr<DataShareManagerImpl> service) : service_(service)
{
}

int PersistentDataController::AddQueryTemplate(const std::string &uri, int64_t subscriberId, Template &tpl)
{
    auto service = service_;
    if (service == nullptr) {
        LOG_ERROR("service is nullptr");
        return INVALID_VALUE;
    }
    auto proxy = service->GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->AddQueryTemplate(uri, subscriberId, tpl);
}

int PersistentDataController::DelQueryTemplate(const std::string &uri, int64_t subscriberId)
{
    auto service = service_;
    if (service == nullptr) {
        LOG_ERROR("service is nullptr");
        return INVALID_VALUE;
    }
    auto proxy = service->GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->DelQueryTemplate(uri, subscriberId);
}

std::vector<OperationResult> PersistentDataController::SubscribeRdbData(void *subscriber,
    const std::vector<std::string> &uris, const TemplateId &templateId,
    std::function<void(const RdbChangeNode &)> callback)
{
    auto service = service_;
    if (service == nullptr) {
        LOG_ERROR("service is nullptr");
        return std::vector<OperationResult>();
    }
    auto proxy = service->GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    return RdbSubscriberManager::GetInstance().AddObservers(subscriber, proxy, uris, templateId, callback);
}

std::vector<OperationResult> PersistentDataController::UnSubscribeRdbData(void *subscriber,
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    auto service = service_;
    if (service == nullptr) {
        LOG_ERROR("service is nullptr");
        return std::vector<OperationResult>();
    }
    auto proxy = service->GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    if (uris.empty()) {
        return RdbSubscriberManager::GetInstance().DelObservers(subscriber, proxy);
    }
    return RdbSubscriberManager::GetInstance().DelObservers(subscriber, proxy, uris, templateId);
}

std::vector<OperationResult> PersistentDataController::EnableSubscribeRdbData(void *subscriber,
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    auto service = service_;
    if (service == nullptr) {
        LOG_ERROR("service is nullptr");
        return std::vector<OperationResult>();
    }
    auto proxy = service->GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    return RdbSubscriberManager::GetInstance().EnableObservers(subscriber, proxy, uris, templateId);
}

std::vector<OperationResult> PersistentDataController::DisableSubscribeRdbData(void *subscriber,
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    auto service = service_;
    if (service == nullptr) {
        LOG_ERROR("service is nullptr");
        return std::vector<OperationResult>();
    }
    auto proxy = service->GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    return RdbSubscriberManager::GetInstance().DisableObservers(subscriber, proxy, uris, templateId);
}
} // namespace DataShare
} // namespace OHOS
