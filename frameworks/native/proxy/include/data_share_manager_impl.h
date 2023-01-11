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

#ifndef DATASHARESERVICE_DATA_SHARE_MANAGER_IMPL_H
#define DATASHARESERVICE_DATA_SHARE_MANAGER_IMPL_H

#include <map>
#include <memory>
#include <mutex>

#include "data_share_service_proxy.h"
#include "data_share_errno.h"
#include "idata_share_service.h"
#include "iremote_object.h"
#include "refbase.h"
#include "base_connection.h"

namespace OHOS::DataShare {
class DataShareKvServiceProxy;
class DataShareManagerImpl : public BaseConnection {
public:
    std::shared_ptr<BaseProxy> GetDataShareService();

    DataShareManagerImpl();
    virtual ~DataShareManagerImpl();
    void OnRemoteDied();
    std::shared_ptr<BaseProxy> GetDataShareProxy() override;
    bool ConnectDataShare(const Uri &uri, const sptr<IRemoteObject> token) override;
    bool IsConnected() override;

    class ServiceDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit ServiceDeathRecipient(DataShareManagerImpl *owner) : owner_(owner) {}
        void OnRemoteDied(const wptr<IRemoteObject> &object) override
        {
            if (owner_ != nullptr) {
                owner_->OnRemoteDied();
            }
        }

    private:
        DataShareManagerImpl *owner_;
    };

private:
    void LinkToDeath(const sptr<IRemoteObject> remote);

    sptr<DataShareServiceProxy> GetDataShareServiceProxy();

    void ResetServiceHandle();

    static std::shared_ptr<DataShareKvServiceProxy> GetDistributedDataManager();

    std::mutex mutex_;
    std::shared_ptr<DataShareKvServiceProxy> dataMgrService_;
    std::shared_ptr<DataShareServiceProxy> dataShareService_;
    std::string bundleName_;
    static constexpr int GET_SA_RETRY_TIMES = 3;
    static constexpr int RETRY_INTERVAL = 1;
    static constexpr int WAIT_TIME = 2;
};

class DataShareKvServiceProxy : public IRemoteProxy<DataShare::IKvStoreDataService> {
public:
    explicit DataShareKvServiceProxy(const sptr<IRemoteObject> &impl);
    ~DataShareKvServiceProxy() = default;
    sptr<IRemoteObject> GetFeatureInterface(const std::string &name) override;
};
} // namespace OHOS::DataShare
#endif
