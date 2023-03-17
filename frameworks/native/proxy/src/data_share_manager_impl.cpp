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

#include "data_share_manager.h"
#include "data_share_service_proxy.h"
#include "datashare_log.h"
#include "idata_share_service.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS::DataShare {
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

static void LinkToDeath(const sptr<IRemoteObject> &remote)
{
    auto &manager = DataShareManagerImpl::GetInstance();
    sptr<DataShareManagerImpl::ServiceDeathRecipient> deathRecipient = new (std::nothrow)
        DataShareManagerImpl::ServiceDeathRecipient(&manager);
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
    LinkToDeath(serviceBase->AsObject().GetRefPtr());
    dataShareService_ =
        std::shared_ptr<DataShareServiceProxy>(service.GetRefPtr(), [holder = service](const auto *) {});
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
void DataShareManagerImpl::OnRemoteDied()
{
    LOG_INFO("datashare service has dead");
    ResetServiceHandle();
}

DataShareKvServiceProxy::DataShareKvServiceProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<DataShare::IKvStoreDataService>(impl)
{
    LOG_DEBUG("Init data service proxy.");
}

sptr<IRemoteObject> DataShareKvServiceProxy::GetFeatureInterface(const std::string &name)
{
    LOG_INFO("GetDataShareService enter.");
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareKvServiceProxy::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed");
        return nullptr;
    }
    if (!data.WriteString(name)) {
        LOG_ERROR("Write name failed");
        return nullptr;
    }

    MessageParcel reply;
    MessageOption mo { MessageOption::TF_SYNC };
    int32_t error = Remote()->SendRequest(GET_FEATURE_INTERFACE, data, reply, mo);
    if (error != 0) {
        LOG_ERROR("SendRequest returned %{public}d", error);
        return nullptr;
    }
    auto remoteObject = reply.ReadRemoteObject();
    if (remoteObject == nullptr) {
        LOG_ERROR("Remote object is nullptr!");
        return nullptr;
    }
    return remoteObject;
}

} // namespace OHOS::DataShare