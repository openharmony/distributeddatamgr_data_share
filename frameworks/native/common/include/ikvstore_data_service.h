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


#ifndef IKVSTORE_DATA_SERVICE_H
#define IKVSTORE_DATA_SERVICE_H

#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "distributeddata_data_share_ipc_interface_code.h"

namespace OHOS {
namespace DataShare {
class IKvStoreDataService : public IRemoteBroker {
public:
    enum {
        DATA_SHARE_ERROR = -1,
        DATA_SHARE_OK = 0,
    };

    virtual sptr<IRemoteObject> GetFeatureInterface(const std::string &name) = 0;

    virtual uint32_t RegisterClientDeathObserver(const std::string &appId, sptr<IRemoteObject> observer) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DistributedKv.IKvStoreDataService");
};

class DataShareKvServiceProxy : public IRemoteProxy<IKvStoreDataService> {
public:
    explicit DataShareKvServiceProxy(const sptr<IRemoteObject> &impl);
    ~DataShareKvServiceProxy() = default;
    sptr<IRemoteObject> GetFeatureInterface(const std::string &name) override;
    uint32_t RegisterClientDeathObserver(const std::string &appId, sptr<IRemoteObject> observer) override;
};
}
}
#endif // IKVSTORE_DATA_SERVICE_H
