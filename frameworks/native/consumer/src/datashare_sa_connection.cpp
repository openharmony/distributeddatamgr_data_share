/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#define LOG_TAG "datashare_sa_connection"

#include <cinttypes>
#include "block_data.h"
#include "datashare_sa_connection.h"
#include "datashare_errno.h"
#include "datashare_itypes_utils.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"
#include "data_share_manager_impl.h"

namespace OHOS {
namespace DataShare {
DataShareSAConnection::DataShareSAConnection(const Uri &uri, int32_t saId, int32_t waitTime) : uri_(uri),
    saId_(saId)
{
    pool_ = std::make_shared<ExecutorPool>(MAX_THREADS, MIN_THREADS, DATASHARE_EXECUTOR_NAME);
    if (waitTime < 0) {
        waitTime_ = 0;
    } else {
        waitTime_ = static_cast<uint32_t>(waitTime);
    }
}

DataShareSAConnection::~DataShareSAConnection()
{
    std::lock_guard<std::mutex> lock(mutex_);
    dataShareProxy_ = nullptr;
    saId_ = INVALID_SA_ID;
}

std::shared_ptr<DataShareProxy> DataShareSAConnection::GetDataShareProxy()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (dataShareProxy_ != nullptr) {
        return dataShareProxy_;
    }
    if (saId_ == INVALID_SA_ID) {
        LOG_ERROR("connection may have been destructed");
        return nullptr;
    }
    auto connectResult = std::make_shared<OHOS::BlockData<ConnectResult, std::chrono::seconds>>(
        waitTime_, ConnectResult{false, E_TIMEOUT_ERROR, ConnectionInterfaceInfo(), nullptr});
    auto task = [connectResult, saId = saId_, uri = uri_, interfaceInfo = interfaceInfo_, waitTime = waitTime_]() {
        auto result = DataShareSAConnection::GetConnectResult(uri, interfaceInfo, saId, waitTime);
        connectResult->SetValue(result);
    };
    if (pool_ == nullptr) {
        LOG_ERROR("pool is nullptr");
        return nullptr;
    }
    auto taskId = pool_->Execute(task);
    auto res = connectResult->GetValue();
    if (!res.isFinish_) {
        LOG_ERROR("connect sa provider time out, waited time: %{public}d, uri: %{public}s", waitTime_,
            DataShareStringUtils::Anonymous(uri_.ToString()).c_str());
        pool_->Remove(taskId);
        return nullptr;
    }
    if (res.errorCode_ != E_OK) {
        LOG_ERROR("connect sa provider failed, e: %{public}d, uri: %{public}s", res.errorCode_,
            DataShareStringUtils::Anonymous(uri_.ToString()).c_str());
        return nullptr;
    }
    if (!LinkToDeath(res.proxy_)) {
        return nullptr;
    }
    sptr<DataShareProxy> proxy = new (std::nothrow) DataShareProxy(std::move(res.proxy_));
    if (proxy == nullptr) {
        LOG_ERROR("new DataShareProxy failed");
        return nullptr;
    }
    dataShareProxy_ = std::shared_ptr<DataShareProxy>(proxy.GetRefPtr(), [holder = proxy](const auto *) {});
    interfaceInfo_ = res.interfaceInfo_;
    return dataShareProxy_;
}

std::shared_ptr<DataShareProxy> DataShareSAConnection::GetDataShareProxy(const Uri &uri,
    const sptr<IRemoteObject> &token)
{
    (void)uri;
    (void)token;
    return GetDataShareProxy();
}

ConnectResult DataShareSAConnection::GetConnectResult(const Uri &uri, ConnectionInterfaceInfo interfaceInfo,
    int32_t saId, uint32_t waitTime)
{
    ConnectResult result = ConnectResult(true, E_PROVIDER_NOT_CONNECTED,
        interfaceInfo, nullptr);
    if (interfaceInfo.code_ == INVALID_INTERFACE_CODE) {
        LOG_INFO("get connectioninterfaceInfo, saId: %{public}d", saId);
        auto [errorCode, res] = DataShareSAConnection::GetConnectionInterfaceInfo(saId, waitTime);
        if (errorCode != E_OK) {
            LOG_ERROR("get connectioninterfaceInfo failed, e: %{public}d, saId: %{public}d", errorCode, saId);
            result.errorCode_ = errorCode;
            return result;
        }
        LOG_INFO("get connectioninterfaceInfo success, code: %{public}d, descriptor: %{public}s", res.code_,
            Str16ToStr8(res.descriptor_).c_str());
        result.interfaceInfo_ = res;
    }
    LOG_INFO("connect to provider, uri: %{public}s", DataShareStringUtils::Anonymous(uri.ToString()).c_str());
    auto dataShareProxy = DataShareSAConnection::ConnectToProvider(uri, result.interfaceInfo_, saId, waitTime);
    if (dataShareProxy != nullptr) {
        result.proxy_ = dataShareProxy;
        result.errorCode_ = E_OK;
    }
    return result;
}

sptr<IRemoteObject> DataShareSAConnection::CheckAndLoadSystemAbility(int32_t systemAbilityId, uint32_t waitTime)
{
    auto manager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (manager == nullptr) {
        LOG_ERROR("get system ability manager failed");
        return nullptr;
    }
    auto remoteObject = manager->CheckSystemAbility(systemAbilityId);
    if (remoteObject == nullptr) {
        // check SA failed, try load SA
        LOG_WARN("get system ability failed, saId: %{public}d", systemAbilityId);
        // callback of load
        sptr<SALoadCallback> loadCallback(new SALoadCallback());
        if (loadCallback == nullptr) {
            LOG_ERROR("Create load callback failed.");
            return nullptr;
        }
        int32_t errCode = manager->LoadSystemAbility(systemAbilityId, loadCallback);
        // start load failed
        if (errCode != ERR_OK) {
            LOG_ERROR("load sa failed, err: %{public}d", errCode);
            return nullptr;
        }
        std::unique_lock<std::mutex> lock(loadCallback->mutex_);
        if (loadCallback->proxyConVar_.wait_for(lock, std::chrono::seconds(waitTime),
            [loadCallback]() { return loadCallback->isLoadSuccess_.load(); })) {
            LOG_INFO("load sa success, saId: %{public}d", systemAbilityId);
            return loadCallback->remoteObject_;
        } else {
            LOG_ERROR("Load sa timeout, saId:%{public}d", systemAbilityId);
            return nullptr;
        }
    }
    return remoteObject;
}

sptr<IRemoteObject> DataShareSAConnection::ConnectToProvider(const Uri &uri,
    ConnectionInterfaceInfo interfaceInfo, int32_t saId, uint32_t waitTime)
{
    auto remoteObject = CheckAndLoadSystemAbility(saId, waitTime);
    if (remoteObject == nullptr) {
        LOG_ERROR("SystemAbility haven't been loaded, saId: %{public}d", saId);
        return nullptr;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(interfaceInfo.descriptor_)) {
        LOG_ERROR("Write descriptor failed!");
        return nullptr;
    }

    if (!ITypesUtil::Marshal(data, uri.ToString())) {
        LOG_ERROR("Write to message parcel failed!");
        return nullptr;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = remoteObject->SendRequest(interfaceInfo.code_, data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return nullptr;
    }

    int32_t errCode = reply.ReadInt32();
    if (errCode != E_OK) {
        LOG_ERROR("failed to get connect, errCode: %{public}d", errCode);
        return nullptr;
    }

    sptr<IRemoteObject> remoter = reply.ReadRemoteObject();
    if (remoter == nullptr) {
        LOG_ERROR("remoter is nullptr");
        return nullptr;
    }
    
    return remoter;
}

bool DataShareSAConnection::LinkToDeath(const sptr<IRemoteObject> remote)
{
    sptr<DataShareSAConnection::ServiceDeathRecipient> deathRecipient = new (std::nothrow)
        DataShareSAConnection::ServiceDeathRecipient(weak_from_this());
    if (deathRecipient == nullptr) {
        LOG_ERROR("new ServiceDeathRecipient error.");
        return false;
    }
    if (remote == nullptr || !remote->AddDeathRecipient(deathRecipient)) {
        LOG_ERROR("add death recipient failed");
        return false;
    }
    LOG_INFO("link to death success");
    return true;
}

std::pair<int32_t, ConnectionInterfaceInfo> DataShareSAConnection::GetConnectionInterfaceInfo(int32_t saId,
    uint32_t waitTime)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, ConnectionInterfaceInfo());
    }

    return proxy->GetConnectionInterfaceInfo(saId, waitTime);
}

void DataShareSAConnection::OnRemoteDied()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        dataShareProxy_ = nullptr;
    }
    LOG_INFO("remote died, try to reconnect, saId: %{public}d", saId_);
    GetDataShareProxy();
}

void DataShareSAConnection::UpdateObserverExtsProviderMap(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver, bool isDescendants)
{
}

void DataShareSAConnection::DeleteObserverExtsProviderMap(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
}

void DataShareSAConnection::SALoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
    const OHOS::sptr<IRemoteObject> &remoteObject)
{
    LOG_INFO("Load sa success, saId: %{public}d, remoteObject:%{public}s", systemAbilityId,
        (remoteObject != nullptr) ? "true" : "false");
    std::lock_guard<std::mutex> lock(mutex_);
    remoteObject_ = remoteObject;
    isLoadSuccess_.store(remoteObject != nullptr);
    proxyConVar_.notify_one();
}

void DataShareSAConnection::SALoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    LOG_ERROR("Load sa failed, saId:%{public}d", systemAbilityId);
    std::lock_guard<std::mutex> lock(mutex_);
    isLoadSuccess_.store(false);
    proxyConVar_.notify_one();
}
}  // namespace DataShare
}  // namespace OHOS