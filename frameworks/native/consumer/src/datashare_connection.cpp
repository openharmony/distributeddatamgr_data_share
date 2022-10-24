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

#include "ability_manager_client.h"
#include "datashare_proxy.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
using namespace AppExecFwk;
sptr<DataShareConnection> DataShareConnection::instance_ = nullptr;
constexpr int WAIT_TIME = 3;

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
    LOG_DEBUG("Start");
    if (remoteObject == nullptr) {
        LOG_ERROR("remote is nullptr");
        return;
    }
    std::unique_lock<std::mutex> lock(condition_.mutex);
    dataShareProxy_ = iface_cast<DataShareProxy>(remoteObject);
    isConnected_.store(true);
    condition_.condition.notify_all();
    LOG_DEBUG("End");
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
    LOG_DEBUG("Start");
    std::unique_lock<std::mutex> lock(condition_.mutex);
    dataShareProxy_ = nullptr;
    isConnected_.store(false);
    condition_.condition.notify_all();
    LOG_DEBUG("End");
}

/**
 * @brief connect remote ability of DataShareExtAbility.
 */
void DataShareConnection::ConnectDataShareExtAbility(const Uri &uri, const sptr<IRemoteObject> &token)
{
    LOG_DEBUG("Start");
    std::unique_lock<std::mutex> lock(condition_.mutex);
    AAFwk::Want want;
    if (uri_.ToString().empty()) {
        want.SetUri(uri);
    } else {
        want.SetUri(uri_);
    }
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, this, token);
    if (condition_.condition.wait_for(lock, std::chrono::seconds(WAIT_TIME),
        [this] { return dataShareProxy_ != nullptr; })) {
        LOG_INFO("connect ability ended successfully");
    }
    LOG_INFO("called end, ret=%{public}d", ret);
}

/**
 * @brief disconnect remote ability of DataShareExtAbility.
 */
void DataShareConnection::DisconnectDataShareExtAbility()
{
    LOG_DEBUG("Start");
    std::unique_lock<std::mutex> lock(condition_.mutex);
    isConnected_.store(false);
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(this);
    if (condition_.condition.wait_for(lock, std::chrono::seconds(WAIT_TIME),
        [this] { return dataShareProxy_ == nullptr; })) {
        LOG_INFO("disconnect ability ended successfully");
    }
    LOG_INFO("called end, ret=%{public}d", ret);
}

/**
 * @brief check whether connected to remote extension ability.
 *
 * @return bool true if connected, otherwise false.
 */
bool DataShareConnection::IsExtAbilityConnected()
{
    return isConnected_.load();
}

/**
 * @brief check whether connected to remote extension ability.
 *
 * @return bool true if connected, otherwise false.
 */
bool DataShareConnection::TryReconnect(const Uri &uri, const sptr<IRemoteObject> &token)
{
    LOG_INFO("Reconnect begin");
    ConnectDataShareExtAbility(uri, token);
    if (dataShareProxy_ == nullptr) {
        LOG_ERROR("Reconnect failed");
        DisconnectDataShareExtAbility();
    }
    return dataShareProxy_ != nullptr;
}

sptr<IDataShare> DataShareConnection::GetDataShareProxy()
{
    return dataShareProxy_;
}
}  // namespace DataShare
}  // namespace OHOS