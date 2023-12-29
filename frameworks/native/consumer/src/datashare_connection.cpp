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

#include "datashare_connection.h"

#include "ams_mgr_proxy.h"
#include "datashare_proxy.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
using namespace AppExecFwk;
constexpr int WAIT_TIME = 2;
/**
 * @brief This method is called back to receive the connection result after an ability calls the
 * ConnectAbility method to connect it to an extension ability.
 *
 * @param element: Indicates information about the connected extension ability.
 * @param remote: Indicates the remote proxy object of the extension ability.
 * @param resultCode: Indicates the connection result code. The value 0 indicates a successful connection, and any
 * other value indicates a connection failure.
 */
void DataShareConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    LOG_INFO("on connect done, req uri:%{public}s, rev uri:%{public}s, ret=%{public}d", uri_.ToString().c_str(),
             element.GetURI().c_str(), resultCode);
    if (remoteObject == nullptr) {
        LOG_ERROR("remote is nullptr");
        return;
    }
    std::unique_lock<std::mutex> lock(condition_.mutex);
    SetDataShareProxy(new (std::nothrow) DataShareProxy(remoteObject));
    condition_.condition.notify_all();
}

/**
 * @brief This method is called back to receive the disconnection result after the connected extension ability crashes
 * or is killed. If the extension ability exits unexpectedly, all its connections are disconnected, and each ability
 * previously connected to it will call onAbilityDisconnectDone.
 *
 * @param element: Indicates information about the disconnected extension ability.
 * @param resultCode: Indicates the disconnection result code. The value 0 indicates a successful disconnection, and
 * any other value indicates a disconnection failure.
 */
void DataShareConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    LOG_INFO("on disconnect done, req uri:%{public}s, rev uri:%{public}s, ret=%{public}d", uri_.ToString().c_str(),
             element.GetURI().c_str(), resultCode);
    {
        std::unique_lock<std::mutex> lock(condition_.mutex);
        SetDataShareProxy(nullptr);
        condition_.condition.notify_all();
    }
    if (!uri_.ToString().empty()) {
        ConnectDataShareExtAbility(uri_, token_);
    }
}

/**
 * @brief connect remote ability of DataShareExtAbility.
 */
std::shared_ptr<DataShareProxy> DataShareConnection::ConnectDataShareExtAbility(const Uri &uri,
    const sptr<IRemoteObject> &token)
{
    if (dataShareProxy_ != nullptr) {
        return dataShareProxy_;
    }
    auto reqUri = uri_.ToString().empty() ? uri.ToString() : uri_.ToString();
    AmsMgrProxy* instance = AmsMgrProxy::GetInstance();
    if (instance == nullptr) {
        LOG_ERROR("Connect: AmsMgrProxy::GetInstance failed uri:%{public}s", reqUri.c_str());
        return nullptr;
    }
    ErrCode ret = instance->Connect(reqUri, this, token);
    if (ret != ERR_OK) {
        LOG_ERROR("connect ability failed, uri:%{public}s, ret = %{public}d", reqUri.c_str(), ret);
        return nullptr;
    }
    std::unique_lock<std::mutex> lock(condition_.mutex);
    if (condition_.condition.wait_for(lock, std::chrono::seconds(WAIT_TIME),
        [this] { return dataShareProxy_ != nullptr; })) {
        LOG_DEBUG("connect ability ended successfully uri:%{public}s", reqUri.c_str());
    } else {
        LOG_WARN("connect timeout uri:%{public}s", reqUri.c_str());
    }
    return dataShareProxy_;
}

/**
 * @brief disconnect remote ability of DataShareExtAbility.
 */
void DataShareConnection::DisconnectDataShareExtAbility()
{
    auto uri = uri_.ToString();
    uri_ = Uri("");
    if (dataShareProxy_ == nullptr) {
        return;
    }
    AmsMgrProxy* instance = AmsMgrProxy::GetInstance();
    if (instance == nullptr) {
        LOG_ERROR("Disconnect: AmsMgrProxy::GetInstance failed uri:%{public}s", uri.c_str());
        return;
    }

    LOG_INFO("disconnect uri:%{public}s", uri.c_str());
    ErrCode ret = instance->DisConnect(this);
    if (ret != ERR_OK) {
        LOG_ERROR("disconnect ability failed, uri:%{public}s ret = %{public}d", uri.c_str(), ret);
        return;
    }
    std::unique_lock<std::mutex> lock(condition_.mutex);
    if (condition_.condition.wait_for(lock, std::chrono::seconds(WAIT_TIME),
        [this] { return dataShareProxy_ == nullptr; })) {
        LOG_DEBUG("disconnect ability successfully uri:%{public}s", uri.c_str());
    } else {
        LOG_INFO("disconnect timeout uri:%{public}s", uri.c_str());
    }
}

void DataShareConnection::SetDataShareProxy(sptr<DataShareProxy> proxy)
{
    if (proxy == nullptr) {
        dataShareProxy_ = nullptr;
        return;
    }

    dataShareProxy_ =
        std::shared_ptr<DataShareProxy>(proxy.GetRefPtr(), [holder = proxy](const auto *) {});
}

DataShareConnection::~DataShareConnection()
{
}

std::shared_ptr<DataShareProxy> DataShareConnection::GetDataShareProxy(const Uri &uri,
    const sptr<IRemoteObject> &token)
{
    return ConnectDataShareExtAbility(uri, token);
}
}  // namespace DataShare
}  // namespace OHOS