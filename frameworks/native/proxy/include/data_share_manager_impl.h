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
#include "iremote_object.h"
#include "refbase.h"

namespace OHOS {
class ExecutorPool;
namespace DataShare {
class DataShareKvServiceProxy;
class DataShareManagerImpl {
public:
    DataShareManagerImpl();

    virtual ~DataShareManagerImpl();

    std::shared_ptr<DataShareServiceProxy> GetServiceProxy();

    void OnRemoteDied();

    void SetDeathCallback(std::function<void(std::shared_ptr<DataShareServiceProxy>)> deathCallback);

    void SetBundleName(const std::string &bundleName);

    class ServiceDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit ServiceDeathRecipient(DataShareManagerImpl *owner) : owner_(owner)
        {
        }
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
    static constexpr int MAX_THREADS = 2;
    static constexpr int MIN_THREADS = 0;
    std::shared_ptr<ExecutorPool> pool_;
    std::function<void(std::shared_ptr<DataShareServiceProxy>)> deathCallback_ = {};
};
}
} // namespace OHOS::DataShare
#endif
