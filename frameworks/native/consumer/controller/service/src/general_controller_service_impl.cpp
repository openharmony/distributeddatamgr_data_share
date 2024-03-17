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
#include "dataobs_mgr_errors.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"

namespace OHOS {
namespace DataShare {
constexpr int INVALID_VALUE = -1;
GeneralControllerServiceImpl::~GeneralControllerServiceImpl()
{
    auto manager = DataShareManagerImpl::GetInstance();
    manager->RemoveRegisterCallback(this);
}

int GeneralControllerServiceImpl::Insert(const Uri &uri, const DataShareValuesBucket &value)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->Insert(uri, value);
}

int GeneralControllerServiceImpl::Update(const Uri &uri, const DataSharePredicates &predicates,
    const DataShareValuesBucket &value)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->Update(uri, predicates, value);
}

int GeneralControllerServiceImpl::Delete(const Uri &uri, const DataSharePredicates &predicates)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->Delete(uri, predicates);
}

std::shared_ptr<DataShareResultSet> GeneralControllerServiceImpl::Query(const Uri &uri,
    const DataSharePredicates &predicates, std::vector<std::string> &columns, DatashareBusinessError &businessError)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return nullptr;
    }
    return proxy->Query(uri, predicates, columns, businessError);
}

void GeneralControllerServiceImpl::RegisterObserver(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    SetRegisterCallback();
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("Proxy is nullptr, uri: %{public}s", DataShareStringUtils::Anonymous(uri.ToString()).c_str());
        observers_.Compute(dataObserver, [&uri](const auto &, auto &value) {
            value.emplace_back(uri);
            return true;
        });
        return;
    }
    auto ret = proxy->RegisterObserver(uri, dataObserver->AsObject());
    LOG_INFO("Register observer ret: %{public}d, uri: %{public}s", ret,
        DataShareStringUtils::Anonymous(uri.ToString()).c_str());
    if (ret == E_OK || ret == AAFwk::OBS_EXIST) {
        observers_.Compute(dataObserver, [&uri](const auto &, auto &value) {
            value.emplace_back(uri);
            return true;
        });
    }
    return;
}

void GeneralControllerServiceImpl::UnregisterObserver(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("Proxy is nullptr");
        return;
    }
    auto ret = proxy->UnRegisterObserver(uri, dataObserver->AsObject());
    LOG_INFO("UnRegister observer ret: %{public}d, uri: %{public}s", ret,
        DataShareStringUtils::Anonymous(uri.ToString()).c_str());
    if (ret == E_OK) {
        observers_.Compute(dataObserver, [&uri](const auto &, auto &value) {
            value.remove_if([&uri](const auto &val) {
                return uri == val;
            });
            return !value.empty();
        });
    }
    return;
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