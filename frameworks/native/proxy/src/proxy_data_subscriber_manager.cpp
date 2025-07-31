/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "proxy_data_subscriber_manager.h"

#include <cinttypes>

#include "data_proxy_observer_stub.h"
#include "dataproxy_handle_common.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"

namespace OHOS {
namespace DataShare {
std::vector<DataProxyResult> ProxyDataSubscriberManager::AddObservers(void *subscriber,
    std::shared_ptr<DataShareServiceProxy> proxy, const std::vector<std::string> &uris,
    const ProxyDataCallback &callback)
{
    std::vector<DataProxyResult> result = {};
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return result;
    }
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys](auto &uri) {
        keys.emplace_back(uri);
    });
    return BaseCallbacks::AddObservers(
        keys, subscriber, std::make_shared<Observer>(callback),
        [&proxy, subscriber, this](const std::vector<Key> &firstAddKeys,
            const std::shared_ptr<Observer> observer, std::vector<DataProxyResult> &opResult) {
            std::vector<std::string> firstAddUris;
            std::for_each(firstAddKeys.begin(), firstAddKeys.end(), [&firstAddUris](auto &result) {
                firstAddUris.emplace_back(result);
            });
            if (firstAddUris.empty()) {
                return;
            }

            auto subResults = proxy->SubscribeProxyData(firstAddUris, serviceCallback_);
            std::vector<Key> failedKeys;
            for (auto &subResult : subResults) {
                opResult.emplace_back(subResult);
                if (subResult.result_ != SUCCESS) {
                    failedKeys.emplace_back(subResult.uri_);
                    LOG_WARN("registered failed, uri is %{public}s, errCode",
                        DataShareStringUtils::Anonymous(subResult.uri_).c_str());
                }
            }

            if (failedKeys.size() > 0) {
                BaseCallbacks::DelProxyDataObservers(failedKeys, subscriber);
            }
        });
}

std::vector<DataProxyResult> ProxyDataSubscriberManager::DelObservers(void *subscriber,
    std::shared_ptr<DataShareServiceProxy> proxy)
{
    std::vector<DataProxyResult> result = {};
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return result;
    }
    return BaseCallbacks::DelObservers(subscriber,
        [&proxy, this](const std::vector<Key> &lastDelKeys, std::vector<DataProxyResult> &opResult) {
            // delete all obs by subscriber
            std::vector<std::string> uris;
            std::for_each(lastDelKeys.begin(), lastDelKeys.end(), [&uris](const Key &key) {
                uris.emplace_back(key.uri_);
            });
            auto results = proxy->UnsubscribeProxyData(uris);
            opResult.insert(opResult.end(), results.begin(), results.end());
        });
}

std::vector<DataProxyResult> ProxyDataSubscriberManager::DelObservers(void *subscriber,
    std::shared_ptr<DataShareServiceProxy> proxy, const std::vector<std::string> &uris)
{
    std::vector<DataProxyResult> result = {};
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return result;
    }
    if (uris.empty()) {
        return DelObservers(subscriber, proxy);
    }

    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys](auto &uri) {
        keys.emplace_back(uri);
    });
    return BaseCallbacks::DelProxyDataObservers(keys, subscriber,
        [&proxy, this](const std::vector<Key> &lastDelKeys, std::vector<DataProxyResult> &opResult) {
            std::vector<std::string> lastDelUris;
            std::for_each(lastDelKeys.begin(), lastDelKeys.end(), [&lastDelUris, this](auto &result) {
                lastDelUris.emplace_back(result);
            });
            if (lastDelUris.empty()) {
                return;
            }
            auto unsubResult = proxy->UnsubscribeProxyData(lastDelUris);
            opResult.insert(opResult.end(), unsubResult.begin(), unsubResult.end());
        });
}

void ProxyDataSubscriberManager::RecoverObservers(std::shared_ptr<DataShareServiceProxy> proxy)
{
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return;
    }

    std::vector<std::string> uris;
    std::vector<Key> keys = CallbacksManager::GetKeys();
    for (const auto& key : keys) {
        uris.emplace_back(key.uri_);
    }
    auto results = proxy->SubscribeProxyData(uris, serviceCallback_);
    for (const auto& result : results) {
        if (result.result_ != SUCCESS) {
            LOG_WARN("RecoverObservers failed, uri is %{public}s, errCode is %{public}d",
                DataShareStringUtils::Anonymous(result.uri_).c_str(), result.result_);
        }
    }
}

void ProxyDataSubscriberManager::Emit(std::vector<DataProxyChangeInfo> &changeInfo)
{
    std::map<std::shared_ptr<Observer>, std::vector<DataProxyChangeInfo>> results;
    for (auto &data : changeInfo) {
        ProxyDataObserverMapKey key(data.uri_);
        auto callbacks = BaseCallbacks::GetEnabledObservers(key);
        for (auto const &obs : callbacks) {
            results[obs].emplace_back(data.changeType_, data.uri_, data.value_);
        }
    }
    for (auto &[callback, node] : results) {
        callback->OnChange(node);
    }
}

ProxyDataSubscriberManager &ProxyDataSubscriberManager::GetInstance()
{
    static ProxyDataSubscriberManager manager;
    return manager;
}

ProxyDataSubscriberManager::ProxyDataSubscriberManager()
{
    serviceCallback_ = new ProxyDataObserverStub([this](std::vector<DataProxyChangeInfo> &changeInfo) {
        Emit(changeInfo);
    });
}

ProxyDataObserver::ProxyDataObserver(const ProxyDataCallback &callback) : callback_(callback) {}

void ProxyDataObserver::OnChange(std::vector<DataProxyChangeInfo> &changeInfo)
{
    callback_(changeInfo);
}

bool ProxyDataObserver::operator==(const ProxyDataObserver &rhs) const
{
    return false;
}

bool ProxyDataObserver::operator!=(const ProxyDataObserver &rhs) const
{
    return !(rhs == *this);
}
} // namespace DataShare
} // namespace OHOS
