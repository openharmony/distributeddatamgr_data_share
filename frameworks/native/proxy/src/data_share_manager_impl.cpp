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
#include "rdb_subscriber_manager.h"
#include "published_data_subscriber_manager.h"

namespace OHOS {
namespace DataShare {

std::mutex DataShareManagerImpl::pmutex_;
DataShareManagerImpl* DataShareManagerImpl::manager_ = nullptr;

DataShareManagerImpl* DataShareManagerImpl::GetInstance()
{
    if (manager_ != nullptr) {
        return manager_;
    }
    std::lock_guard<std::mutex> lock(pmutex_);
    if (manager_ != nullptr) {
        return manager_;
    }
    manager_ = new DataShareManagerImpl();
    if (manager_ == nullptr) {
        LOG_ERROR("DataShareManagerImpl: GetInstance failed");
    }
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        LOG_ERROR("Failed to get saMgrProxy.");
        return manager_;
    }
    sptr<DataShareClientStatusChangeStub> callback(new DataShareClientStatusChangeStub(manager_));
    saManager->SubscribeSystemAbility(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID, callback);
    return manager_;
}


sptr<DataShareKvServiceProxy> DataShareManagerImpl::GetDistributedDataManager()
{
    auto manager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (manager == nullptr) {
        LOG_ERROR("get system ability manager failed");
        return nullptr;
    }
    auto remoteObject = manager->CheckSystemAbility(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID);
    if (remoteObject == nullptr) {
        LOG_ERROR("get distributed data manager failed");
        return nullptr;
    }
    sptr<DataShareKvServiceProxy> proxy = new (std::nothrow)DataShareKvServiceProxy(remoteObject);
    if (proxy == nullptr) {
        LOG_ERROR("new DataShareKvServiceProxy fail.");
        return nullptr;
    }
    return proxy;
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
    LOG_DEBUG("link to death success");
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
    RegisterClientDeathObserver();
    return iface_cast<DataShareServiceProxy>(remote);
}

void DataShareManagerImpl::RegisterClientDeathObserver()
{
    if (dataMgrService_ == nullptr || bundleName_.empty()) {
        return;
    }
    LOG_INFO("RegisterClientDeathObserver bundleName is %{public}s", bundleName_.c_str());
    if (clientDeathObserverPtr_ == nullptr) {
        clientDeathObserverPtr_ = new (std::nothrow) DataShareClientDeathObserverStub();
    }
    if (clientDeathObserverPtr_ == nullptr) {
        LOG_WARN("new KvStoreClientDeathObserver failed");
        return;
    }
    auto status = dataMgrService_->RegisterClientDeathObserver(bundleName_, clientDeathObserverPtr_);
    if (!status) {
        LOG_ERROR("RegisterClientDeathObserver failed, bundleName is %{public}s", bundleName_.c_str());
        return;
    }
}

DataShareManagerImpl::DataShareManagerImpl()
{
    pool_ = std::make_shared<ExecutorPool>(MAX_THREADS, MIN_THREADS);
    SetDeathCallback([](std::shared_ptr<DataShareServiceProxy> proxy) {
        LOG_INFO("RecoverObs start");
        RdbSubscriberManager::GetInstance().RecoverObservers(proxy);
        PublishedDataSubscriberManager::GetInstance().RecoverObservers(proxy);
    });
}

DataShareManagerImpl::~DataShareManagerImpl()
{
}

std::shared_ptr<DataShareServiceProxy> DataShareManagerImpl::GetProxy()
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

std::shared_ptr<DataShareServiceProxy> DataShareManagerImpl::GetServiceProxy()
{
    auto manager = DataShareManagerImpl::GetInstance();
    if (manager == nullptr) {
        LOG_ERROR("manager_ is nullptr");
        return nullptr;
    }
    return manager->GetProxy();
}

void DataShareManagerImpl::ResetServiceHandle()
{
    LOG_DEBUG("enter");
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

void DataShareManagerImpl::SetRegisterCallback(GeneralControllerServiceImpl* ptr,
    std::function<void()> registerCallback)
{
    observers_.ComputeIfAbsent(ptr, [&registerCallback](const GeneralControllerServiceImpl*) {
        return std::move(registerCallback);
    });
}

void DataShareManagerImpl::RemoveRegisterCallback(GeneralControllerServiceImpl* ptr)
{
    observers_.Erase(ptr);
}

void DataShareManagerImpl::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    if (systemAbilityId != DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID) {
        LOG_ERROR("SystemAbilityId must be DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID, but it is %{public}d",
            systemAbilityId);
        return;
    }
    observers_.ForEach([](const auto &, auto &callback) {
        callback();
        return false;
    });
}
}
}