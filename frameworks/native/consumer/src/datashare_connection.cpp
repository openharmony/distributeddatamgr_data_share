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
#include "datashare_errno.h"
#include "datashare_log.h"
#include "datashare_proxy.h"
#include "datashare_radar_reporter.h"
#include "datashare_string_utils.h"

namespace OHOS {
namespace DataShare {
using namespace AppExecFwk;
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
    {
        std::lock_guard<std::mutex> lock(mutex_);
        sptr<DataShareProxy> proxy = new (std::nothrow) DataShareProxy(remoteObject);
        dataShareProxy_ = std::shared_ptr<DataShareProxy>(proxy.GetRefPtr(), [holder = proxy](const auto *) {});
        condition_.condition.notify_all();
    }
    if (isInvalid_.load()) {
        LOG_ERROR("connect is invalid, req uri:%{public}s, rev uri:%{public}s, ret=%{public}d",
            DataShareStringUtils::Change(uri_.ToString()).c_str(),
            DataShareStringUtils::Change(element.GetURI()).c_str(), resultCode);
        Disconnect();
    }
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
    if (pool_ == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pool_ == nullptr) {
            pool_ = std::make_shared<ExecutorPool>(MAX_THREADS, MIN_THREADS);
        }
    }
    ReconnectExtAbility(uri);
}

void DataShareConnection::ReconnectExtAbility(const std::string &uri)
{
    if (reConnects_.count == 0) {
        AmsMgrProxy* instance = AmsMgrProxy::GetInstance();
        if (instance == nullptr) {
            LOG_ERROR("get proxy failed uri:%{public}s", DataShareStringUtils::Change(uri_.ToString()).c_str());
            return;
        }
        ErrCode ret = instance->Connect(uri, this, token_);
        LOG_INFO("reconnect ability, uri:%{public}s, ret = %{public}d",
            DataShareStringUtils::Change(uri).c_str(), ret);
        if (ret == E_OK) {
            auto curr = std::chrono::system_clock::now().time_since_epoch();
            reConnects_.count = 1;
            reConnects_.firstTime = std::chrono::duration_cast<std::chrono::milliseconds>(curr).count();
            reConnects_.prevTime = std::chrono::duration_cast<std::chrono::milliseconds>(curr).count();
        }
        return;
    }
    return DelayConnectExtAbility(uri);
}

void DataShareConnection::DelayConnectExtAbility(const std::string &uri)
{
    int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    if (now - reConnects_.prevTime >= MAX_RECONNECT_TIME_INTERVAL.count()) {
        reConnects_.count = 0;
        reConnects_.firstTime = now;
        reConnects_.prevTime = now;
    }
    if (reConnects_.count >= MAX_RECONNECT) {
        return;
    }
    auto delay = RECONNECT_TIME_INTERVAL;
    if (now - reConnects_.prevTime >= RECONNECT_TIME_INTERVAL.count()) {
        delay = std::chrono::seconds(0);
    }
    std::weak_ptr<DataShareConnection> self = weak_from_this();
    auto taskid = pool_->Schedule(delay, [uri, self]() {
        auto selfSharedPtr = self.lock();
        if (selfSharedPtr) {
            AmsMgrProxy* instance = AmsMgrProxy::GetInstance();
            if (instance == nullptr) {
                LOG_ERROR("get proxy failed uri:%{public}s", DataShareStringUtils::Change(uri).c_str());
                return;
            }
            ErrCode ret = instance->Connect(uri, selfSharedPtr.get(), selfSharedPtr->token_);
            LOG_INFO("reconnect ability, uri:%{public}s, ret = %{public}d",
                DataShareStringUtils::Change(uri).c_str(), ret);
            if (ret == E_OK) {
                selfSharedPtr->reConnects_.count++;
                selfSharedPtr->reConnects_.prevTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
            }
        }
    });
    if (taskid == ExecutorPool::INVALID_TASK_ID) {
        LOG_ERROR("create scheduler failed, over the max capacity");
        return;
    }
    LOG_DEBUG("create scheduler success");
    return;
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
    if (condition_.condition.wait_for(condLock, std::chrono::seconds(waitTime_),
        [this] { return dataShareProxy_ != nullptr; })) {
        LOG_DEBUG("connect ability ended successfully uri:%{public}s", DataShareStringUtils::Change(reqUri).c_str());
        RADAR_REPORT(__FUNCTION__, RadarReporter::CREATE_DATASHARE_HELPER,
            RadarReporter::CONNECT_EXT, RadarReporter::SUCCESS,
            RadarReporter::LOCAL_SESS_NAME, Str16ToStr8(token->GetObjectDescriptor()),
            RadarReporter::PEER_SESS_NAME, reqUri);
    } else {
        LOG_WARN("connect timeout uri:%{public}s", DataShareStringUtils::Change(reqUri).c_str());
        RADAR_REPORT(__FUNCTION__, RadarReporter::CREATE_DATASHARE_HELPER,
            RadarReporter::CONNECT_EXT, RadarReporter::FAILED,
            RadarReporter::ERROR_CODE, RadarReporter::EXT_CONNECT_TIMEOUT_ERROR,
            RadarReporter::LOCAL_SESS_NAME, Str16ToStr8(token->GetObjectDescriptor()),
            RadarReporter::PEER_SESS_NAME, reqUri);
    }
    return GetDataShareProxy();
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

    ErrCode ret = Disconnect();
    LOG_INFO("disconnect uri:%{public}s, ret = %{public}d", DataShareStringUtils::Change(uri).c_str(), ret);
    if (ret == E_OK) {
        RADAR_REPORT(__FUNCTION__, RadarReporter::CREATE_DATASHARE_HELPER,
            RadarReporter::DIS_CONNECT_EXT, RadarReporter::SUCCESS,
            RadarReporter::LOCAL_SESS_NAME, Str16ToStr8(token_->GetObjectDescriptor()),
            RadarReporter::PEER_SESS_NAME, uri);
        return;
    }
    RADAR_REPORT(__FUNCTION__, RadarReporter::CREATE_DATASHARE_HELPER,
        RadarReporter::DIS_CONNECT_EXT, RadarReporter::FAILED,
        RadarReporter::ERROR_CODE, RadarReporter::EXT_DIS_CONNECT_ERROR,
        RadarReporter::LOCAL_SESS_NAME, Str16ToStr8(token_->GetObjectDescriptor()),
        RadarReporter::PEER_SESS_NAME, uri);
}

DataShareConnection::~DataShareConnection()
{
}

std::shared_ptr<DataShareProxy> DataShareConnection::GetDataShareProxy(const Uri &uri,
    const sptr<IRemoteObject> &token)
{
    return ConnectDataShareExtAbility(uri, token);
}

void DataShareConnection::SetConnectInvalid()
{
    isInvalid_.store(true);
}

ErrCode DataShareConnection::Disconnect()
{
    AmsMgrProxy* instance = AmsMgrProxy::GetInstance();
    if (instance == nullptr) {
        return -1;
    }
    return instance->DisConnect(this);
}

std::shared_ptr<DataShareProxy> DataShareConnection::GetDataShareProxy()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return dataShareProxy_;
}
}  // namespace DataShare
}  // namespace OHOS