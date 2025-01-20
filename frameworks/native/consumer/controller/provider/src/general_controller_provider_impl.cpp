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

#include "general_controller_provider_impl.h"

#include "datashare_log.h"
#include "datashare_string_utils.h"

namespace OHOS {
namespace DataShare {
int GeneralControllerProviderImpl::Insert(const Uri &uri, const DataShareValuesBucket &value)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return DATA_SHARE_ERROR;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return DATA_SHARE_ERROR;
    }
    return proxy->Insert(uri, value);
}

int GeneralControllerProviderImpl::Update(const Uri &uri, const DataSharePredicates &predicates,
    const DataShareValuesBucket &value)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return DATA_SHARE_ERROR;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return DATA_SHARE_ERROR;
    }
    return proxy->Update(uri, predicates, value);
}

int GeneralControllerProviderImpl::Delete(const Uri &uri, const DataSharePredicates &predicates)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return DATA_SHARE_ERROR;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return DATA_SHARE_ERROR;
    }
    return proxy->Delete(uri, predicates);
}

std::pair<int32_t, int32_t> GeneralControllerProviderImpl::InsertEx(const Uri &uri,
    const DataShareValuesBucket &value)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    return proxy->InsertEx(uri, value);
}

std::pair<int32_t, int32_t> GeneralControllerProviderImpl::UpdateEx(
    const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    return proxy->UpdateEx(uri, predicates, value);
}

std::pair<int32_t, int32_t> GeneralControllerProviderImpl::DeleteEx(const Uri &uri,
    const DataSharePredicates &predicates)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    return proxy->DeleteEx(uri, predicates);
}

std::shared_ptr<DataShareResultSet> GeneralControllerProviderImpl::Query(const Uri &uri,
    const DataSharePredicates &predicates, std::vector<std::string> &columns, DatashareBusinessError &businessError)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return nullptr;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return nullptr;
    }
    return proxy->Query(uri, predicates, columns, businessError);
}

int GeneralControllerProviderImpl::RegisterObserver(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return E_PROVIDER_CONN_NULL;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return E_PROVIDER_NOT_CONNECTED;
    }
    int ret = proxy->RegisterObserver(uri, dataObserver);
    LOG_INFO("Register non-silent observer ret: %{public}d, uri: %{public}s", ret,
        DataShareStringUtils::Anonymous(uri.ToString()).c_str());
    return ret;
}

int GeneralControllerProviderImpl::UnregisterObserver(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return E_PROVIDER_CONN_NULL;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return E_PROVIDER_NOT_CONNECTED;
    }
    int ret = proxy->UnregisterObserver(uri, dataObserver);
    LOG_INFO("Unregister non-silent observer ret: %{public}d, uri: %{public}s", ret,
        DataShareStringUtils::Anonymous(uri.ToString()).c_str());
    return ret;
}

void GeneralControllerProviderImpl::NotifyChange(const Uri &uri)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return;
    }
    proxy->NotifyChange(uri);
}

GeneralControllerProviderImpl::GeneralControllerProviderImpl(std::shared_ptr<DataShareConnection> connection,
    const Uri &uri, const sptr<IRemoteObject> &token) : connection_(connection), token_(token), uri_(uri)
{
}
} // namespace DataShare
} // namespace OHOS