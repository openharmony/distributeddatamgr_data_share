/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "connection_factory.h"

#include <memory>

#include "datashare_log.h"
#include "rdb_subscriber_manager.h"
#include "published_data_subscriber_manager.h"

namespace OHOS {
namespace DataShare {
std::shared_ptr<DataShareManagerImpl> ConnectionFactory::GetDataShareService(const std::string &bundleName)
{
    service_->SetBundleName(bundleName);
    return service_;
}

std::shared_ptr<DataShareConnection> ConnectionFactory::GetDataShareConnection(Uri &uri,
    const sptr<IRemoteObject> token)
{
    sptr<DataShareConnection> connection = new (std::nothrow) DataShareConnection(uri, token);
    if (connection == nullptr) {
        LOG_ERROR("Factory Create DataShareConnection failed.");
        return nullptr;
    }
    return std::shared_ptr<DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
        holder->DisconnectDataShareExtAbility();
    });
}

ConnectionFactory& ConnectionFactory::GetInstance()
{
    static ConnectionFactory manager;
    return manager;
}

ConnectionFactory::ConnectionFactory()
{
    service_ = std::make_shared<DataShareManagerImpl>();
    service_->SetDeathCallback([](std::shared_ptr<DataShareServiceProxy> proxy) {
        LOG_INFO("RecoverObs start");
        RdbSubscriberManager::GetInstance().RecoverObservers(proxy);
        PublishedDataSubscriberManager::GetInstance().RecoverObservers(proxy);
    });
}
}
}