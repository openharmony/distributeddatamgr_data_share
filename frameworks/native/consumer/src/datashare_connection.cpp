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

#define LOG_TAG "datashare_connection"

#include <cinttypes>
#include "datashare_connection.h"

#include "ams_mgr_proxy.h"
#include "datashare_common.h"
#include "datashare_errno.h"
#include "datashare_log.h"
#include "datashare_proxy.h"
#include "datashare_radar_reporter.h"
#include "datashare_string_utils.h"

namespace OHOS {
namespace DataShare {
using namespace AppExecFwk;
const std::chrono::milliseconds TIME_THRESHOLD = std::chrono::milliseconds(200);
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
    LOG_INFO("uri:%{public}s, ret=%{public}d", DataShareStringUtils::Change(element.GetURI()).c_str(), resultCode);
    if (remoteObject == nullptr) {
        LOG_ERROR("remote is nullptr");
        condition_.condition.notify_all();
        return;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        sptr<DataShareProxy> proxy = new (std::nothrow) DataShareProxy(remoteObject);
        if (proxy == nullptr) {
            LOG_ERROR("Create DataShareProxy failed");
            condition_.condition.notify_all();
            return;
        }
        dataShareProxy_ = std::shared_ptr<DataShareProxy>(proxy.GetRefPtr(), [holder = proxy](const auto *) {});
        condition_.condition.notify_all();
    }
    if (isInvalid_.load()) {
        LOG_ERROR("connect is invalid, uri:%{public}s, ret=%{public}d",
            DataShareStringUtils::Change(element.GetURI()).c_str(), resultCode);
        Disconnect();
        // when inInvalid is true, it means that this connection has been disconnected and does not need to be
        // re-registered
        return;
    }
    // Entering this function for the first time indicates a normal connection, and re-registration is not required.
    // Entering this function for the second time or later indicates an abnormal reconnection, and re-registration is
    // necessary. The atomic operation of the exchange ensures that only the thread that first connect can get false
    // while all other thread attempting to reconnect will get true.
    if (isReconnect_.exchange(true)) {
        ReRegisterObserverExtProvider();
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
    LOG_INFO("uri:%{public}s, ret=%{public}d", DataShareStringUtils::Change(element.GetURI()).c_str(), resultCode);
    std::string uri;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        dataShareProxy_ = nullptr;
        uri = uri_.ToString();
    }
    if (uri.empty()) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pool_ == nullptr) {
            pool_ = std::make_shared<ExecutorPool>(MAX_THREADS, MIN_THREADS, DATASHARE_EXECUTOR_NAME);
        }
    }
    ReconnectExtAbility(uri);
}

void DataShareConnection::ReconnectExtAbility(const std::string &uri)
{
    if (reConnects_.count.load() == 0) {
        AmsMgrProxy* instance = AmsMgrProxy::GetInstance();
        if (instance == nullptr) {
            LOG_ERROR("get proxy failed uri:%{public}s", DataShareStringUtils::Change(uri).c_str());
            return;
        }
    ErrCode ret = instance->Connect(uri, callback_, token_);
        LOG_INFO("reconnect ability, uri:%{public}s, ret = %{public}d",
            DataShareStringUtils::Change(uri).c_str(), ret);
        if (ret == E_OK) {
            auto curr = std::chrono::system_clock::now().time_since_epoch();
            reConnects_.count.store(1);
            reConnects_.firstTime.store(std::chrono::duration_cast<std::chrono::milliseconds>(curr).count());
            reConnects_.prevTime.store(std::chrono::duration_cast<std::chrono::milliseconds>(curr).count());
        }
        return;
    }
    return DelayConnectExtAbility(uri);
}

void DataShareConnection::DelayConnectExtAbility(const std::string &uri)
{
    int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    if (now - reConnects_.prevTime.load() >= MAX_RECONNECT_TIME_INTERVAL.count()) {
        reConnects_.count.store(0);
        reConnects_.firstTime.store(now);
        reConnects_.prevTime.store(now);
    }
    if (reConnects_.count.load() >= MAX_RECONNECT) {
        return;
    }
    auto delay = RECONNECT_TIME_INTERVAL;
    if (now - reConnects_.prevTime.load() >= RECONNECT_TIME_INTERVAL.count()) {
        delay = std::chrono::seconds(0);
    }
    std::weak_ptr<DataShareConnection> self = weak_from_this();
    auto taskid = pool_->Schedule(delay, [uri, self]() {
        auto selfSharedPtr = self.lock();
        if (selfSharedPtr == nullptr) {
            return;
        }
        AmsMgrProxy* instance = AmsMgrProxy::GetInstance();
        if (instance == nullptr) {
            LOG_ERROR("get proxy failed uri:%{public}s", DataShareStringUtils::Change(uri).c_str());
            return;
        }
        ErrCode ret = instance->Connect(uri, selfSharedPtr->callback_, selfSharedPtr->token_);
        LOG_INFO("reconnect ability, uri:%{public}s, ret = %{public}d",
            DataShareStringUtils::Change(uri).c_str(), ret);
        if (ret == E_OK) {
            selfSharedPtr->reConnects_.count.fetch_add(1);
            selfSharedPtr->reConnects_.prevTime.store(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
        }
    });
    if (taskid == ExecutorPool::INVALID_TASK_ID) {
        LOG_ERROR("create scheduler failed, over the max capacity");
        return;
    }
    LOG_DEBUG("create scheduler success");
    return;
}

// store observer when it was successfully registered
void DataShareConnection::UpdateObserverExtsProviderMap(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver, bool isDescendants)
{
    std::lock_guard<std::mutex> lock(mutex_);
    observerExtsProvider_.Compute(dataObserver, [&uri, isDescendants](const auto &key, auto &value) {
        value.emplace_back(uri, isDescendants);
        return true;
    });
}

// remove observer when it was successfully unregistered
void DataShareConnection::DeleteObserverExtsProviderMap(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    std::lock_guard<std::mutex> lock(mutex_);
    observerExtsProvider_.Compute(dataObserver, [&uri](const auto &key, auto &value) {
        value.remove_if([&uri](const auto &param) {
            return uri == param.uri;
        });
        return !value.empty();
    });
}

// re-register observer by dataShareProxy
void DataShareConnection::ReRegisterObserverExtProvider()
{
    LOG_INFO("ReRegisterObserverExtProvider start");
    decltype(observerExtsProvider_) observerExtsProvider;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        observerExtsProvider = std::move(observerExtsProvider_);
        observerExtsProvider_.Clear();
    }
    auto dataShareProxy = GetDataShareProxy();
    if (dataShareProxy == nullptr) {
        LOG_ERROR("dataShareProxy_ is nullptr");
        return;
    }
    observerExtsProvider.ForEach([this, &dataShareProxy](const auto &key, const auto &value) {
        for (const auto &param : value) {
            auto ret = dataShareProxy->RegisterObserverExtProvider(param.uri, key, param.isDescendants, { true });
            if (ret != E_OK) {
                LOG_ERROR(
                    "RegisterObserverExt failed, param.uri:%{public}s, ret:%{public}d, param.isDescendants:%{public}d",
                    DataShareStringUtils::Anonymous(param.uri.ToString()).c_str(), ret, param.isDescendants
                );
            } else {
                UpdateObserverExtsProviderMap(param.uri, key, param.isDescendants);
            }
        }
        return false;
    });
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
    ErrCode ret = instance->Connect(reqUri, callback_, token);
    LOG_INFO("uri = %{public}s. ret = %{public}d", DataShareStringUtils::Change(reqUri).c_str(), ret);
    if (ret != ERR_OK) {
        return nullptr;
    }
    std::unique_lock<std::mutex> condLock(mutex_);
    std::shared_ptr<DataShareProxy> proxy = dataShareProxy_;
    if (proxy != nullptr) {
        return proxy;
    }
    auto start = std::chrono::steady_clock::now();
    if (condition_.condition.wait_for(condLock, std::chrono::seconds(waitTime_),
        [this] { return dataShareProxy_ != nullptr; })) {
        auto finish = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
        if (duration >= TIME_THRESHOLD) {
            int64_t milliseconds = duration.count();
            LOG_WARN("over time connecting ability, uri:%{public}s, time:%{public}" PRIi64 "ms",
                DataShareStringUtils::Change(reqUri).c_str(), milliseconds);
        }
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

    ErrCode ret = Disconnect();
    LOG_INFO("uri:%{public}s, ret = %{public}d", DataShareStringUtils::Change(uri).c_str(), ret);
    if (ret == E_OK) {
        return;
    }
}

DataShareConnection::~DataShareConnection()
{
    SetConnectInvalid();
    DisconnectDataShareExtAbility();
}

bool DataShareConnection::Init()
{
    callback_ = new (std::nothrow) ConnectionCallback(weak_from_this());
    if (callback_ == nullptr) {
        LOG_ERROR("Create DataShareConnection::ConnectionCallback failed, uri:%{public}s",
            DataShareStringUtils::Change(uri_.ToString()).c_str());
        return false;
    }
    return true;
}

DataShareConnection::ConnectionCallback::ConnectionCallback(std::weak_ptr<DataShareConnection> target)
    : target_(std::move(target)) {}

void DataShareConnection::ConnectionCallback::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    auto target = target_.lock();
    if (target == nullptr) {
        LOG_WARN("DataShareConnection target is gone before OnAbilityConnectDone");
        AmsMgrProxy* instance = AmsMgrProxy::GetInstance();
        if (instance != nullptr) {
            int ret = instance->DisConnect(this);
            LOG_INFO("disconnect after target gone, uri:%{public}s, ret:%{public}d",
                DataShareStringUtils::Change(element.GetURI()).c_str(), ret);
        }
        return;
    }
    target->OnAbilityConnectDone(element, remoteObject, resultCode);
}

void DataShareConnection::ConnectionCallback::OnAbilityDisconnectDone(
    const AppExecFwk::ElementName &element, int resultCode)
{
    auto target = target_.lock();
    if (target == nullptr) {
        LOG_DEBUG("DataShareConnection target is gone before OnAbilityDisconnectDone");
        return;
    }
    target->OnAbilityDisconnectDone(element, resultCode);
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
    return instance->DisConnect(callback_);
}

std::shared_ptr<DataShareProxy> DataShareConnection::GetDataShareProxy()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return dataShareProxy_;
}
}  // namespace DataShare
}  // namespace OHOS