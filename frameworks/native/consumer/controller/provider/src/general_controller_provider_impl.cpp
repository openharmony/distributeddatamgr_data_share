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

#include "general_controller_porvider_impl.h"

#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
constexpr int INVALID_VALUE = -1;
int GeneralControllerProviderImpl::Insert(const Uri &uri, const DataShareValuesBucket &value)
{
    int index = INVALID_VALUE;
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return index;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return index;
    }
    return proxy->Insert(uri, value);
}

int GeneralControllerProviderImpl::Update(const Uri &uri, const DataSharePredicates &predicates,
    const DataShareValuesBucket &value)
{
    int index = INVALID_VALUE;
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return index;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return index;
    }
    return proxy->Update(uri, predicates, value);
}

int GeneralControllerProviderImpl::Delete(const Uri &uri, const DataSharePredicates &predicates)
{
    int index = INVALID_VALUE;
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return index;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return index;
    }
    return proxy->Delete(uri, predicates);
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

void GeneralControllerProviderImpl::RegisterObserver(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
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
    proxy->RegisterObserver(uri, dataObserver);
}

void GeneralControllerProviderImpl::UnregisterObserver(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
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
    proxy->UnregisterObserver(uri, dataObserver);
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

void GeneralControllerProviderImpl::Release()
{
    connection_ = nullptr;
    uri_ = Uri("");
}
} // namespace DataShare
} // namespace OHOS