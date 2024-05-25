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

#include "rdb_subscriber_manager.h"

#include <cinttypes>

#include "data_proxy_observer_stub.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"

namespace OHOS {
namespace DataShare {
RdbSubscriberManager &RdbSubscriberManager::GetInstance()
{
    static RdbSubscriberManager manager;
    return manager;
}

RdbSubscriberManager::RdbSubscriberManager()
{
    serviceCallback_ = nullptr;
}

std::vector<OperationResult> RdbSubscriberManager::AddObservers(void *subscriber,
    std::shared_ptr<DataShareServiceProxy> proxy,
    const std::vector<std::string> &uris, const TemplateId &templateId, const RdbCallback &callback)
{
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &templateId](auto &uri) {
        keys.emplace_back(uri, templateId);
    });
    return BaseCallbacks::AddObservers(
        keys, subscriber, std::make_shared<Observer>(callback),
        [this](const std::vector<Key> &localRegisterKeys, const std::shared_ptr<Observer> observer) {
            Emit(localRegisterKeys, observer);
        },
        [&proxy, subscriber, &templateId, this](const std::vector<Key> &firstAddKeys,
            const std::shared_ptr<Observer> observer, std::vector<OperationResult> &opResult) {
            std::vector<std::string> firstAddUris;
            std::for_each(firstAddKeys.begin(), firstAddKeys.end(), [&firstAddUris](auto &result) {
                firstAddUris.emplace_back(result);
            });
            if (firstAddUris.empty()) {
                return;
            }

            Init();
            auto subResults = proxy->SubscribeRdbData(firstAddUris, templateId, serviceCallback_);
            std::vector<Key> failedKeys;
            for (auto &subResult : subResults) {
                opResult.emplace_back(subResult);
                if (subResult.errCode_ != E_OK) {
                    failedKeys.emplace_back(subResult.key_, templateId);
                    LOG_WARN("registered failed, uri is %{public}s", subResult.key_.c_str());
                }
            }
            if (!failedKeys.empty()) {
                BaseCallbacks::DelObservers(failedKeys, subscriber);
            }
            Destroy();
        });
}

std::vector<OperationResult> RdbSubscriberManager::DelObservers(void *subscriber,
    std::shared_ptr<DataShareServiceProxy> proxy, const std::vector<std::string> &uris, const TemplateId &templateId)
{
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    if (uris.empty()) {
        return DelObservers(subscriber, proxy);
    }

    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &templateId](auto &uri) {
        keys.emplace_back(uri, templateId);
    });
    return BaseCallbacks::DelObservers(keys, subscriber,
        [&proxy, &templateId, this](const std::vector<Key> &lastDelKeys, std::vector<OperationResult> &opResult) {
            std::vector<std::string> lastDelUris;
            std::for_each(lastDelKeys.begin(), lastDelKeys.end(), [&lastDelUris, this](auto &result) {
                lastDelUris.emplace_back(result);
                lastChangeNodeMap_.erase(result);
            });
            if (lastDelUris.empty()) {
                return;
            }
            auto unsubResult = proxy->UnSubscribeRdbData(lastDelUris, templateId);
            if (BaseCallbacks::GetEnabledSubscriberSize() == 0) {
                LOG_INFO("no valid subscriber, delete callback");
                serviceCallback_ = nullptr;
            }
            opResult.insert(opResult.end(), unsubResult.begin(), unsubResult.end());
            Destroy();
        });
}

std::vector<OperationResult> RdbSubscriberManager::DelObservers(void *subscriber,
    std::shared_ptr<DataShareServiceProxy> proxy)
{
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    return BaseCallbacks::DelObservers(subscriber,
        [&proxy, this](const std::vector<Key> &lastDelKeys, std::vector<OperationResult> &opResult) {
            // delete all obs by subscriber
            for (const auto &key : lastDelKeys) {
                lastChangeNodeMap_.erase(key);
                auto unsubResult = proxy->UnSubscribeRdbData(std::vector<std::string>(1, key.uri_), key.templateId_);
                opResult.insert(opResult.end(), unsubResult.begin(), unsubResult.end());
            }
            Destroy();
        });
}

std::vector<OperationResult> RdbSubscriberManager::EnableObservers(void *subscriber,
    std::shared_ptr<DataShareServiceProxy> proxy, const std::vector<std::string> &uris, const TemplateId &templateId)
{
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &templateId](auto &uri) {
        keys.emplace_back(uri, templateId);
    });
    return BaseCallbacks::EnableObservers(keys, subscriber,
        [this](std::map<Key, std::vector<ObserverNodeOnEnabled>> &obsMap) {
            EmitOnEnable(obsMap);
        },
        [&proxy, subscriber, &templateId, this](const std::vector<Key> &firstAddKeys,
        std::vector<OperationResult> &opResult) {
            std::vector<std::string> firstAddUris;
            std::for_each(firstAddKeys.begin(), firstAddKeys.end(), [&firstAddUris](auto &result) {
                firstAddUris.emplace_back(result);
            });
            if (firstAddUris.empty()) {
                return;
            }
            auto subResults = proxy->EnableSubscribeRdbData(firstAddUris, templateId);
            std::vector<Key> failedKeys;
            for (auto &subResult : subResults) {
                opResult.emplace_back(subResult);
                if (subResult.errCode_ != E_OK) {
                    failedKeys.emplace_back(subResult.key_, templateId);
                    LOG_WARN("registered failed, uri is %{public}s", subResult.key_.c_str());
                }
            }
            if (!failedKeys.empty()) {
                BaseCallbacks::DisableObservers(failedKeys, subscriber);
            }
        });
}

std::vector<OperationResult> RdbSubscriberManager::DisableObservers(void *subscriber,
    std::shared_ptr<DataShareServiceProxy> proxy, const std::vector<std::string> &uris, const TemplateId &templateId)
{
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &templateId](auto &uri) {
        keys.emplace_back(uri, templateId);
    });
    return BaseCallbacks::DisableObservers(keys, subscriber,
        [&proxy, &templateId, this](const std::vector<Key> &lastDisabledKeys, std::vector<OperationResult> &opResult) {
            std::vector<std::string> lastDisabledUris;
            std::for_each(lastDisabledKeys.begin(), lastDisabledKeys.end(), [&lastDisabledUris](auto &result) {
                lastDisabledUris.emplace_back(result);
            });
            if (lastDisabledUris.empty()) {
                return;
            }

            auto results = proxy->DisableSubscribeRdbData(lastDisabledUris, templateId);
            opResult.insert(opResult.end(), results.begin(), results.end());
        });
}

void RdbSubscriberManager::RecoverObservers(std::shared_ptr<DataShareServiceProxy> proxy)
{
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return;
    }
    std::map<TemplateId, std::vector<std::string>> keysMap;
    std::vector<Key> keys = CallbacksManager::GetKeys();
    for (const auto& key : keys) {
        keysMap[key.templateId_].emplace_back(key.uri_);
    }
    for (const auto& [templateId, uris] : keysMap) {
        auto results = proxy->SubscribeRdbData(uris, templateId, serviceCallback_);
        for (const auto& result : results) {
            if (result.errCode_ != E_OK) {
                LOG_WARN("RecoverObservers failed, uri is %{public}s, errCode is %{public}d", result.key_.c_str(),
                    result.errCode_);
            }
        }
    }
}

void RdbSubscriberManager::Emit(const RdbChangeNode &changeNode)
{
    RdbObserverMapKey key(changeNode.uri_, changeNode.templateId_);
    lastChangeNodeMap_[key] = changeNode;
    auto callbacks = BaseCallbacks::GetEnabledObservers(key);
    for (auto &obs : callbacks) {
        if (obs != nullptr) {
            LOG_INFO("Client send data to form, uri is %{public}s, subscriberId is %{public}" PRId64,
                DataShareStringUtils::Anonymous(key.uri_).c_str(), key.templateId_.subscriberId_);
            obs->OnChange(changeNode);
        }
    }
    BaseCallbacks::SetObserversNotifiedOnEnabled(key);
}

void RdbSubscriberManager::Emit(const std::vector<Key> &keys, const std::shared_ptr<Observer> &observer)
{
    for (auto const &key : keys) {
        auto it = lastChangeNodeMap_.find(key);
        if (it != lastChangeNodeMap_.end()) {
            observer->OnChange(it->second);
        }
    }
}

void RdbSubscriberManager::EmitOnEnable(std::map<Key, std::vector<ObserverNodeOnEnabled>> &obsMap)
{
    for (auto &[key, obsVector] : obsMap) {
        auto it = lastChangeNodeMap_.find(key);
        if (it == lastChangeNodeMap_.end()) {
            continue;
        }
        for (auto &obs : obsVector) {
            if (obs.isNotifyOnEnabled_) {
                obs.observer_->OnChange(it->second);
            }
        }
    }
}

bool RdbSubscriberManager::Init()
{
    if (serviceCallback_ == nullptr) {
        LOG_INFO("callback init");
        serviceCallback_ = new RdbObserverStub([this](const RdbChangeNode &changeNode) {
            Emit(changeNode);
        });
    }
    return true;
}

void RdbSubscriberManager::Destroy()
{
    if (BaseCallbacks::GetEnabledSubscriberSize() == 0) {
        if (serviceCallback_ != nullptr) {
            serviceCallback_->ClearCallback();
        }
        LOG_INFO("no valid subscriber, delete callback");
        serviceCallback_ = nullptr;
    }
}

RdbObserver::RdbObserver(const RdbCallback &callback) : callback_(callback) {}

void RdbObserver::OnChange(const RdbChangeNode &changeNode)
{
    callback_(changeNode);
}

bool RdbObserver::operator==(const RdbObserver &rhs) const
{
    return false;
}

bool RdbObserver::operator!=(const RdbObserver &rhs) const
{
    return !(rhs == *this);
}
} // namespace DataShare
} // namespace OHOS
