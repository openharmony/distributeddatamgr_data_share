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

#include "published_data_subscriber_manager.h"

#include "data_proxy_observer_stub.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
std::vector<OperationResult> PublishedDataSubscriberManager::AddObservers(std::shared_ptr<BaseProxy> proxy,
    const std::vector<std::string> &uris, int64_t subscriberId, const PublishedDataCallback &callback)
{
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &subscriberId](auto &uri) {
        keys.emplace_back(uri, subscriberId);
    });
    return BaseCallbacks::AddObservers(keys, std::make_shared<Observer>(callback),
        [&proxy, &subscriberId, this](const std::vector<Key> &firstAddKeys, const std::shared_ptr<Observer> observer,
            std::vector<OperationResult> &opResult) {
            std::vector<std::string> firstAddUris;
            std::for_each(firstAddKeys.begin(), firstAddKeys.end(), [&firstAddUris](auto &result) {
                firstAddUris.emplace_back(result);
            });
            if (firstAddUris.empty()) {
                return;
            }

            Init();
            auto subResults = proxy->SubscribePublishedData(firstAddUris, subscriberId, serviceCallback_);
            std::vector<Key> failedKeys;
            for (auto &subResult : subResults) {
                opResult.emplace_back(subResult);
                if (subResult.errCode_ != E_OK) {
                    failedKeys.emplace_back(subResult.key_, subscriberId);
                    LOG_WARN("registered failed, uri is %{public}s", subResult.key_.c_str());
                }
            }
            if (failedKeys.size() > 0) {
                BaseCallbacks::DelObservers(failedKeys, observer);
            }
            Destroy();
        });
}

std::vector<OperationResult> PublishedDataSubscriberManager::DelObservers(std::shared_ptr<BaseProxy> proxy,
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &subscriberId](auto &uri) {
        keys.emplace_back(uri, subscriberId);
    });
    return BaseCallbacks::DelObservers(keys, nullptr,
        [&proxy, &subscriberId, &uris, this](const std::vector<Key> &lastDelKeys,
            const std::shared_ptr<Observer> &observer, std::vector<OperationResult> &opResult) {
            // delete all obs
            if (uris.empty()) {
                for (const auto &key : lastDelKeys) {
                    proxy->UnSubscribePublishedData(std::vector<std::string>(1, key.uri_), key.subscriberId_);
                }
                Destroy();
                return;
            }
            std::vector<std::string> lastDelUris;
            std::for_each(lastDelKeys.begin(), lastDelKeys.end(), [&lastDelUris](auto &result) {
                lastDelUris.emplace_back(result);
            });
            if (lastDelUris.empty()) {
                return;
            }
            auto unsubResult = proxy->UnSubscribePublishedData(lastDelUris, subscriberId);
            if (BaseCallbacks::GetEnabledSubscriberSize() == 0) {
                LOG_INFO("no valid subscriber, delete callback");
                serviceCallback_ = nullptr;
            }
            opResult.insert(opResult.end(), unsubResult.begin(), unsubResult.end());
            Destroy();
        });
}

std::vector<OperationResult> PublishedDataSubscriberManager::EnableObservers(std::shared_ptr<BaseProxy> proxy,
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &subscriberId](auto &uri) {
        keys.emplace_back(uri, subscriberId);
    });
    return BaseCallbacks::EnableObservers(keys,
        [&proxy, &subscriberId, this](const std::vector<Key> &firstAddKeys, std::vector<OperationResult> &opResult) {
            std::vector<std::string> firstAddUris;
            std::for_each(firstAddKeys.begin(), firstAddKeys.end(), [&firstAddUris](auto &result) {
                firstAddUris.emplace_back(result);
            });
            if (firstAddUris.empty()) {
                return;
            }
            auto subResults = proxy->EnableSubscribePublishedData(firstAddUris, subscriberId);
            std::vector<Key> failedKeys;
            for (auto &subResult : subResults) {
                opResult.emplace_back(subResult);
                if (subResult.errCode_ != E_OK) {
                    failedKeys.emplace_back(subResult.key_, subscriberId);
                    LOG_WARN("registered failed, uri is %{public}s", subResult.key_.c_str());
                }
            }
            if (failedKeys.size() > 0) {
                BaseCallbacks::DisableObservers(failedKeys);
            }
        });
}

std::vector<OperationResult> PublishedDataSubscriberManager::DisableObservers(std::shared_ptr<BaseProxy> proxy,
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    std::vector<OperationResult> results;
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return results;
    }
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &subscriberId](auto &uri) {
        keys.emplace_back(uri, subscriberId);
    });
    return BaseCallbacks::DisableObservers(keys,
        [&proxy, &subscriberId, this](const std::vector<Key> &lastDisabledKeys,
                                      std::vector<OperationResult> &opResult) {
            std::vector<std::string> lastDisabledUris;
            std::for_each(lastDisabledKeys.begin(), lastDisabledKeys.end(), [&lastDisabledUris](auto &result) {
                lastDisabledUris.emplace_back(result);
            });
            if (lastDisabledUris.empty()) {
                return;
            }

            auto results = proxy->DisableSubscribePublishedData(lastDisabledUris, subscriberId);
            opResult.insert(opResult.end(), results.begin(), results.end());
            Destroy();
        });
}

void PublishedDataSubscriberManager::Emit(PublishedDataChangeNode &changeNode)
{
    std::map<std::shared_ptr<Observer>, PublishedDataChangeNode> results;
    for (auto &data : changeNode.datas_) {
        PublishedObserverMapKey key(data.key_, data.subscriberId_);
        auto callbacks = BaseCallbacks::GetEnabledObservers(key);
        if (callbacks.empty()) {
            LOG_WARN("%{private}s nobody subscribe, but still notify", data.key_.c_str());
            continue;
        }
        for (auto &obs : callbacks) {
            results[obs].datas_.emplace_back(data.key_, data.subscriberId_, data.GetData());
        }
    }
    for (auto &[callback, node] : results) {
        node.ownerBundleName_ = changeNode.ownerBundleName_;
        callback->OnChange(node);
    }
}

bool PublishedDataSubscriberManager::Init()
{
    if (serviceCallback_ == nullptr) {
        LOG_INFO("callback init");
        serviceCallback_ = new PublishedDataObserverStub([this](PublishedDataChangeNode &changeNode) {
            Emit(changeNode);
        });
    }
    return true;
}

void PublishedDataSubscriberManager::Destroy()
{
    if (BaseCallbacks::GetEnabledSubscriberSize() == 0) {
        if (serviceCallback_ != nullptr) {
            serviceCallback_->ClearCallback();
        }
        LOG_INFO("no valid subscriber, delete callback");
        serviceCallback_ = nullptr;
    }
}

PublishedDataSubscriberManager::PublishedDataSubscriberManager()
{
    serviceCallback_ = nullptr;
}

PublishedDataObserver::PublishedDataObserver(const PublishedDataCallback &callback) : callback_(callback) {}

void PublishedDataObserver::OnChange(PublishedDataChangeNode &changeNode)
{
    callback_(changeNode);
}

bool PublishedDataObserver::operator==(const PublishedDataObserver &rhs) const
{
    return false;
}

bool PublishedDataObserver::operator!=(const PublishedDataObserver &rhs) const
{
    return !(rhs == *this);
}
} // namespace DataShare
} // namespace OHOS
