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

//#define LOG_TAG "DataShareManagerImpl"

#include "data_share_manager_impl.h"

#include "data_share_manager.h"

#include <thread>

#include "data_share_service_proxy.h"
#include "ipc_skeleton.h"
#include "idata_share_service.h"
#include "iservice_registry.h"
#include "datashare_log.h"
#include "system_ability_definition.h"

namespace OHOS::DataShare {
sptr<DistributedKv::IKvStoreDataService> DataShareManagerImpl::GetDistributedDataManager()
{
    int retry = 0;
    while (++retry <= GET_SA_RETRY_TIMES) {
        auto manager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (manager == nullptr) {
            LOG_ERROR("get system ability manager failed");
            return nullptr;
        }
        LOG_INFO("get distributed data manager %{public}d", retry);
        auto remoteObject = manager->CheckSystemAbility(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID);
        if (remoteObject == nullptr) {
            std::this_thread::sleep_for(std::chrono::seconds(RETRY_INTERVAL));
            continue;
        }
        LOG_INFO("get distributed data manager success");
        return iface_cast<DistributedKv::IKvStoreDataService>(remoteObject);
    }

    LOG_ERROR("get distributed data manager failed");
    return nullptr;
}

sptr<IDataShareService> DataShareManagerImpl::GetDataShareServiceProxy()
{
    if (dataMgrService_ == nullptr) {
        dataMgrService_ = GetDistributedDataManager();
    }
    if (dataMgrService_ == nullptr) {
        LOG_ERROR("Get distributed data manager failed!");
        return nullptr;
    }

    auto remote = dataMgrService_->GetDataShareService();
    if (remote == nullptr) {
        LOG_ERROR("Get DataShare service failed!");
        return nullptr;
    }
    return iface_cast<IDataShareService>(remote);
}

std::shared_ptr<IDataShareService> DataShareManagerImpl::GetDataShareService()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (dataShareService_ != nullptr) {
        return dataShareService_;
    }
    auto service = GetDataShareServiceProxy();
    if (service == nullptr) {
        return nullptr;
    }

    sptr<IDataShareService> serviceBase = service;
    dataShareService_ = std::shared_ptr<IDataShareService>(
        service.GetRefPtr(), [holder = service](const auto *) {});
    return dataShareService_;
}

DataShareManagerImpl& DataShareManagerImpl::GetInstance()
{
    static DataShareManagerImpl manager;
    return manager;
}

DataShareManagerImpl::DataShareManagerImpl()
{
    LOG_INFO("construct");
}

DataShareManagerImpl::~DataShareManagerImpl()
{
    LOG_INFO("destroy");
}


void DataShareManagerImpl::ResetServiceHandle()
{
    LOG_INFO("enter");
    std::lock_guard<std::mutex> lock(mutex_);
    dataMgrService_ = nullptr;
    dataShareService_ = nullptr;
}
} // namespace OHOS::DataShare
