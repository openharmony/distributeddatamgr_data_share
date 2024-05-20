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
#include "datashare_log.h"
#include "datashare_proxy.h"
#include "datashare_string_utils.h"

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
    LOG_INFO("on connect done, req uri:%{public}s, rev uri:%{public}s, ret=%{public}d",
        DataShareStringUtils::Change(uri_.ToString()).c_str(),
        DataShareStringUtils::Change(element.GetURI()).c_str(), resultCode);
    if (remoteObject == nullptr) {
        LOG_ERROR("remote is nullptr");
        condition_.condition.notify_all();
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    sptr<DataShareProxy> proxy = new (std::nothrow) DataShareProxy(remoteObject);
    dataShareProxy_ = std::shared_ptr<DataShareProxy>(proxy.GetRefPtr(), [holder = proxy](const auto *) {});
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
    LOG_INFO("on disconnect done, req uri:%{public}s, rev uri:%{public}s, ret=%{public}d",
        DataShareStringUtils::Change(uri_.ToString()).c_str(),
        DataShareStringUtils::Change(element.GetURI()).c_str(), resultCode);
    std::string uri;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        dataShareProxy_ = nullptr;
        uri = uri_.ToString();
    }
    if (uri.empty()) {
        return;
    }
    AmsMgrProxy* instance = AmsMgrProxy::GetInstance();
    if (instance == nullptr) {
        LOG_ERROR("get proxy failed uri:%{public}s", DataShareStringUtils::Change(uri_.ToString()).c_str());
        return;
    }
    ErrCode ret = instance->Connect(uri, this, token_);
    LOG_INFO("reconnect ability, uri:%{public}s, ret = %{public}d",
        DataShareStringUtils::Change(uri_.ToString()).c_str(), ret);
}

/**
 * @brief connect remote ability of DataShareExtAbility.
 */
std::shared_ptr<DataShareProxy> DataShareConnection::ConnectDataShareExtAbility(const Uri &uri,
    const sptr<IRemoteObject> &token)
{
    std::string reqUri;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (dataShareProxy_ != nullptr) {
            return dataShareProxy_;
        }
        reqUri = uri_.ToString().empty() ? uri.ToString() : uri_.ToString();
    }

    AmsMgrProxy* instance = AmsMgrProxy::GetInstance();
    if (instance == nullptr) {
        LOG_ERROR("get proxy failed uri:%{public}s", DataShareStringUtils::Change(reqUri).c_str());
        return nullptr;
    }
    ErrCode ret = instance->Connect(reqUri, this, token);
    LOG_INFO("connect ability, uri = %{public}s. ret = %{public}d", DataShareStringUtils::Change(reqUri).c_str(), ret);
    if (ret != ERR_OK) {
        return nullptr;
    }
    std::unique_lock<std::mutex> condLock(condition_.mutex);
    if (condition_.condition.wait_for(condLock, std::chrono::seconds(WAIT_TIME),
        [this] { return dataShareProxy_ != nullptr; })) {
        LOG_DEBUG("connect ability ended successfully uri:%{public}s", DataShareStringUtils::Change(reqUri).c_str());
    } else {
        LOG_WARN("connect timeout uri:%{public}s", DataShareStringUtils::Change(reqUri).c_str());
    }
    return dataShareProxy_;
}

/**
 * @brief disconnect remote ability of DataShareExtAbility.
 */
void DataShareConnection::DisconnectDataShareExtAbility()
{
    std::string uri;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        uri = uri_.ToString();
        uri_ = Uri("");
        if (dataShareProxy_ == nullptr) {
            return;
        }
    }

    AmsMgrProxy* instance = AmsMgrProxy::GetInstance();
    if (instance == nullptr) {
        LOG_ERROR("get proxy failed uri:%{public}s", DataShareStringUtils::Change(uri).c_str());
        return;
    }

    ErrCode ret = instance->DisConnect(this);
    LOG_INFO("disconnect uri:%{public}s, ret = %{public}d", DataShareStringUtils::Change(uri).c_str(), ret);
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