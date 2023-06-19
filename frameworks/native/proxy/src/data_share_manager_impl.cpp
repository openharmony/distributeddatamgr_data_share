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

#include "data_share_manager_impl.h"

#include <thread>

#include "datashare_log.h"
#include "ikvstore_data_service.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "executor_pool.h"

namespace OHOS {
namespace DataShare {
std::shared_ptr<DataShareKvServiceProxy> DataShareManagerImpl::GetDistributedDataManager()
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
        return std::make_shared<DataShareKvServiceProxy>(remoteObject);
    }

    LOG_ERROR("get distributed data manager failed");
    return nullptr;
}

void DataShareManagerImpl::LinkToDeath(const sptr<IRemoteObject> remote)
{
    sptr<DataShareManagerImpl::ServiceDeathRecipient> deathRecipient = new (std::nothrow)
        DataShareManagerImpl::ServiceDeathRecipient(this);
    if (deathRecipient == nullptr) {
        LOG_ERROR("DataShareManagerImpl::LinkToDeath new ServiceDeathRecipient error.");
        return;
    }
    if (!remote->AddDeathRecipient(deathRecipient)) {
        LOG_ERROR("add death recipient failed");
    }
    LOG_ERROR("link to death success");
}

sptr<DataShareServiceProxy> DataShareManagerImpl::GetDataShareServiceProxy()
{
    if (dataMgrService_ == nullptr) {
        dataMgrService_ = GetDistributedDataManager();
    }
    if (dataMgrService_ == nullptr) {
        LOG_ERROR("Get distributed data manager failed!");
        return nullptr;
    }
    auto remote = dataMgrService_->GetFeatureInterface("data_share");
    if (remote == nullptr) {
        LOG_ERROR("Get DataShare service failed!");
        return nullptr;
    }
    return iface_cast<DataShareServiceProxy>(remote);
}

DataShareManagerImpl::DataShareManagerImpl()
{
    LOG_INFO("construct");
    pool_ = std::make_shared<ExecutorPool>(MAX_THREADS, MIN_THREADS);
}

DataShareManagerImpl::~DataShareManagerImpl()
{
    LOG_INFO("destroy");
}

std::shared_ptr<DataShareServiceProxy> DataShareManagerImpl::GetServiceProxy()
{
    if (dataShareService_ != nullptr) {
        return dataShareService_;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (dataShareService_ != nullptr) {
        return dataShareService_;
    }

    auto service = GetDataShareServiceProxy();
    if (service == nullptr) {
        return nullptr;
    }
    sptr<IDataShareService> serviceBase = service;
    LinkToDeath(serviceBase->AsObject().GetRefPtr());
    dataShareService_ = std::shared_ptr<DataShareServiceProxy>(
            service.GetRefPtr(), [holder = service](const auto *) {});
    return dataShareService_;
}

void DataShareManagerImpl::ResetServiceHandle()
{
    LOG_INFO("enter");
    std::lock_guard<std::mutex> lock(mutex_);
    dataMgrService_ = nullptr;
    dataShareService_ = nullptr;
}

void DataShareManagerImpl::SetDeathCallback(std::function<void(std::shared_ptr<DataShareServiceProxy>)> deathCallback)
{
    deathCallback_ = deathCallback;
}

void DataShareManagerImpl::SetBundleName(const std::string &bundleName)
{
    bundleName_ = bundleName;
}

void DataShareManagerImpl::OnRemoteDied()
{
    LOG_INFO("#######datashare service has dead");
    ResetServiceHandle();
    auto taskid = pool_->Schedule(std::chrono::seconds(WAIT_TIME), [this]() {
        if (GetServiceProxy() != nullptr) {
            deathCallback_(dataShareService_);
        }
    });
    if (taskid == ExecutorPool::INVALID_TASK_ID) {
        LOG_ERROR("create scheduler failed, over the max capacity");
        return;
    }
    LOG_DEBUG("create scheduler success");
}
}
}