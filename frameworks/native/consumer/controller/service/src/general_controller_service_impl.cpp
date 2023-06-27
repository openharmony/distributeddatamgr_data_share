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

#include "general_controller_service_impl.h"

#include "dataobs_mgr_client.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
constexpr int INVALID_VALUE = -1;
int GeneralControllerServiceImpl::Insert(const Uri &uri, const DataShareValuesBucket &value)
{
    auto proxy = DataShareManagerImpl::GetInstance().GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->Insert(uri, value);
}

int GeneralControllerServiceImpl::Update(const Uri &uri, const DataSharePredicates &predicates,
    const DataShareValuesBucket &value)
{
    auto proxy = DataShareManagerImpl::GetInstance().GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->Update(uri, predicates, value);
}

int GeneralControllerServiceImpl::Delete(const Uri &uri, const DataSharePredicates &predicates)
{
    auto proxy = DataShareManagerImpl::GetInstance().GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->Delete(uri, predicates);
}

std::shared_ptr<DataShareResultSet> GeneralControllerServiceImpl::Query(const Uri &uri,
    const DataSharePredicates &predicates, std::vector<std::string> &columns, DatashareBusinessError &businessError)
{
    auto proxy = DataShareManagerImpl::GetInstance().GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return nullptr;
    }
    return proxy->Query(uri, predicates, columns, businessError);
}

void GeneralControllerServiceImpl::RegisterObserver(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto obsMgrClient = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("get DataObsMgrClient failed");
        return;
    }
    ErrCode ret = obsMgrClient->RegisterObserver(uri, dataObserver);
    if (ret != ERR_OK) {
        LOG_ERROR("RegisterObserver failed");
    }
}

void GeneralControllerServiceImpl::UnregisterObserver(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto obsMgrClient = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("get DataObsMgrClient failed");
        return;
    }
    ErrCode ret = obsMgrClient->UnregisterObserver(uri, dataObserver);
    if (ret != ERR_OK) {
        LOG_ERROR("UnregisterObserver failed");
    }
}

void GeneralControllerServiceImpl::NotifyChange(const Uri &uri)
{
    auto proxy = DataShareManagerImpl::GetInstance().GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return;
    }
    proxy->Notify(uri.ToString());
}
} // namespace DataShare
} // namespace OHOS