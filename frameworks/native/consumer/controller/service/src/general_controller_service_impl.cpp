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
#include <thread>

#include "dataobs_mgr_client.h"
#include "dataobs_mgr_errors.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"

namespace OHOS {
namespace DataShare {
GeneralControllerServiceImpl::GeneralControllerServiceImpl(const std::string &ext)
{
    extUri_ = ext;
}

GeneralControllerServiceImpl::~GeneralControllerServiceImpl()
{
    auto manager = DataShareManagerImpl::GetInstance();
    manager->RemoveRegisterCallback(this);
}

int GeneralControllerServiceImpl::Insert(const Uri &uri, const DataShareValuesBucket &value)
{
    auto manager = DataShareManagerImpl::GetInstance();
    if (manager == nullptr) {
        LOG_ERROR("Manager is nullptr");
        return DATA_SHARE_ERROR;
    }
    manager->SetCallCount(__FUNCTION__, uri.ToString());
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return DATA_SHARE_ERROR;
    }
    return proxy->Insert(uri, Uri(extUri_), value);
}

int GeneralControllerServiceImpl::Update(const Uri &uri, const DataSharePredicates &predicates,
    const DataShareValuesBucket &value)
{
    auto manager = DataShareManagerImpl::GetInstance();
    if (manager == nullptr) {
        LOG_ERROR("Manager is nullptr");
        return DATA_SHARE_ERROR;
    }
    manager->SetCallCount(__FUNCTION__, uri.ToString());
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return DATA_SHARE_ERROR;
    }
    return proxy->Update(uri, Uri(extUri_), predicates, value);
}

int GeneralControllerServiceImpl::Delete(const Uri &uri, const DataSharePredicates &predicates)
{
    auto manager = DataShareManagerImpl::GetInstance();
    if (manager == nullptr) {
        LOG_ERROR("Manager is nullptr");
        return DATA_SHARE_ERROR;
    }
    manager->SetCallCount(__FUNCTION__, uri.ToString());
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return DATA_SHARE_ERROR;
    }
    return proxy->Delete(uri, Uri(extUri_), predicates);
}

std::pair<int32_t, int32_t> GeneralControllerServiceImpl::InsertEx(const Uri &uri, const DataShareValuesBucket &value)
{
    auto manager = DataShareManagerImpl::GetInstance();
    if (manager == nullptr) {
        LOG_ERROR("Manager is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    manager->SetCallCount(__FUNCTION__, uri.ToString());
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    return proxy->InsertEx(uri, Uri(extUri_), value);
}

std::pair<int32_t, int32_t> GeneralControllerServiceImpl::UpdateEx(
    const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value)
{
    auto manager = DataShareManagerImpl::GetInstance();
    if (manager == nullptr) {
        LOG_ERROR("Manager is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    manager->SetCallCount(__FUNCTION__, uri.ToString());
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    return proxy->UpdateEx(uri, Uri(extUri_), predicates, value);
}

std::pair<int32_t, int32_t> GeneralControllerServiceImpl::DeleteEx(const Uri &uri,
    const DataSharePredicates &predicates)
{
    auto manager = DataShareManagerImpl::GetInstance();
    if (manager == nullptr) {
        LOG_ERROR("Manager is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    manager->SetCallCount(__FUNCTION__, uri.ToString());
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    return proxy->DeleteEx(uri, Uri(extUri_), predicates);
}

std::shared_ptr<DataShareResultSet> GeneralControllerServiceImpl::Query(const Uri &uri,
    const DataSharePredicates &predicates, std::vector<std::string> &columns, DatashareBusinessError &businessError)
{
    auto manager = DataShareManagerImpl::GetInstance();
    if (manager == nullptr) {
        LOG_ERROR("Manager is nullptr");
        return nullptr;
    }
    manager->SetCallCount(__FUNCTION__, uri.ToString());
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return nullptr;
    }
    auto resultSet = proxy->Query(uri, Uri(extUri_), predicates, columns, businessError);
    int retryCount = 0;
    while (resultSet == nullptr && businessError.GetCode() == E_RESULTSET_BUSY && retryCount++ < MAX_RETRY_COUNT) {
        LOG_ERROR("resultSet busy retry, uri: %{public}s", DataShareStringUtils::Anonymous(uri.ToString()).c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(
            DataShareStringUtils::GetRandomNumber(RANDOM_MIN, RANDOM_MAX)));
        resultSet = proxy->Query(uri, Uri(extUri_), predicates, columns, businessError);
    }
    return resultSet;
}

int GeneralControllerServiceImpl::RegisterObserver(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto manager = DataShareManagerImpl::GetInstance();
    if (manager == nullptr) {
        LOG_ERROR("Manager is nullptr");
        return E_DATA_SHARE_NOT_READY;
    }
    manager->SetCallCount(__FUNCTION__, uri.ToString());
    auto obsMgrClient = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("get DataObsMgrClient failed");
        return E_DATA_OBS_NOT_READY;
    }
    ErrCode ret = obsMgrClient->RegisterObserver(uri, dataObserver);
    LOG_INFO("Register silent observer ret: %{public}d, uri: %{public}s", ret,
        DataShareStringUtils::Anonymous(uri.ToString()).c_str());
    return ret;
}

int GeneralControllerServiceImpl::UnregisterObserver(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto obsMgrClient = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("get DataObsMgrClient failed");
        return E_DATA_OBS_NOT_READY;
    }
    ErrCode ret = obsMgrClient->UnregisterObserver(uri, dataObserver);
    LOG_INFO("Unregister silent observer ret: %{public}d, uri: %{public}s", ret,
        DataShareStringUtils::Anonymous(uri.ToString()).c_str());
    return ret;
}

void GeneralControllerServiceImpl::NotifyChange(const Uri &uri)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return;
    }
    proxy->Notify(uri.ToString());
}

void GeneralControllerServiceImpl::SetRegisterCallback()
{
    auto manager = DataShareManagerImpl::GetInstance();
    if (manager == nullptr) {
        LOG_ERROR("Manager is nullptr");
        return;
    }
    auto registerCallback = [this]() {
        ReRegisterObserver();
    };
    manager->SetRegisterCallback(this, registerCallback);
}

void GeneralControllerServiceImpl::ReRegisterObserver()
{
    LOG_INFO("Distributeddata service on start, reRegister observer.");
    decltype(observers_) observers(std::move(observers_));
    observers_.Clear();
    observers.ForEach([this](const auto &key, const auto &value) {
        for (const auto &uri : value) {
            RegisterObserver(uri, key);
        }
        return false;
    });
}
} // namespace DataShare
} // namespace OHOS