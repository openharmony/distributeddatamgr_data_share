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

#include <memory>

#include "connection_factory.h"
#include "datashare_connection.h"
#include "datashare_log.h"

namespace OHOS::DataShare{
std::shared_ptr<BaseConnection> ConnectionFactory::GetConnection(Uri &uri,  const sptr<IRemoteObject> &token) {
    if (uri.GetQuery().find("Proxy=true") != std::string::npos && service_->ConnectDataShare(uri, token)) {
        return service_;
    }

    sptr<DataShareConnection> connection = new (std::nothrow) DataShareConnection(uri, token);
    if (connection == nullptr){
        LOG_ERROR("Factory Create DataShareConnection failed.");
        return nullptr;
    }
    return  std::shared_ptr<DataShareConnection>(
        connection.GetRefPtr(), [holder = connection](const auto *) {});
}

ConnectionFactory& ConnectionFactory::GetInstance()
{
    static ConnectionFactory manager;
    return manager;
}

ConnectionFactory::ConnectionFactory(){
    service_ = std::make_shared<DataShareManagerImpl>();
}

}