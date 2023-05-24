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

#ifndef DATA_SHARE_BASE_CONNECTION_H
#define DATA_SHARE_BASE_CONNECTION_H

#include <memory>
#include "idatashare.h"

#include "base_proxy.h"

namespace OHOS {
namespace DataShare {

enum class ConnectionType {
    NORMAL = 0,
    SILENCE,
};

class BaseConnection {
public:
    BaseConnection(ConnectionType type = ConnectionType::NORMAL) : type_(type) {};
    virtual ~BaseConnection() = default;
    virtual std::shared_ptr<BaseProxy> GetDataShareProxy() = 0;
    virtual bool ConnectDataShare(const Uri &uri, const sptr<IRemoteObject> &token) = 0;
    ConnectionType GetType()
    {
        return type_;
    }
private:
    ConnectionType type_ = ConnectionType::NORMAL;
};
}}

#endif // DATA_SHARE_BASE_CONNECTION_H
