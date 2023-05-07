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

#include "data_proxy_observer_stub.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
RdbSubscriberManager::RdbSubscriberManager()
{
    serviceCallback_ = nullptr;
}

std::vector<OperationResult> RdbSubscriberManager::AddObservers(std::shared_ptr<BaseProxy> proxy,
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
    return BaseCallbacks::AddObservers(keys, std::make_shared<Observer>(callback),
        [&proxy, &templateId, this](const std::vector<Key> &firstAddKeys,
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
                BaseCallbacks::DelObservers(failedKeys, observer);
            }
            Destroy();
        });
}

std::vector<OperationResult> RdbSubscriberManager::DelObservers(std::shared_ptr<BaseProxy> proxy,
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &templateId](auto &uri) {
        keys.emplace_back(uri, templateId);
    });
    return BaseCallbacks::DelObservers(keys, nullptr,
        [&proxy, &templateId, &uris, this](const std::vector<Key> &lastDelKeys,
            const std::shared_ptr<Observer> &observer, std::vector<OperationResult> &opResult) {
            // delete all obs
            if (uris.empty()) {
                for (const auto& key : lastDelKeys) {
                    proxy->UnSubscribeRdbData(std::vector<std::string>(1, key.uri_), key.templateId_);
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
            auto unsubResult = proxy->UnSubscribeRdbData(lastDelUris, templateId);
            if (BaseCallbacks::GetEnabledSubscriberSize() == 0) {
                LOG_INFO("no valid subscriber, delete callback");
                serviceCallback_ = nullptr;
            }
            opResult.insert(opResult.end(), unsubResult.begin(), unsubResult.end());
            Destroy();
        });
}

std::vector<OperationResult> RdbSubscriberManager::EnableObservers(std::shared_ptr<BaseProxy> proxy,
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<OperationResult>();
    }
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &templateId](auto &uri) {
        keys.emplace_back(uri, templateId);
    });
    return BaseCallbacks::EnableObservers(keys,
        [&proxy, &templateId, this](const std::vector<Key> &firstAddKeys, std::vector<OperationResult> &opResult) {
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
                BaseCallbacks::DisableObservers(failedKeys);
            }
        });
}

std::vector<OperationResult> RdbSubscriberManager::DisableObservers(std::shared_ptr<BaseProxy> proxy,
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    std::vector<OperationResult> results;
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return results;
    }
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &templateId](auto &uri) {
        keys.emplace_back(uri, templateId);
    });
    return BaseCallbacks::DisableObservers(keys,
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
            Destroy();
        });
}

void RdbSubscriberManager::Emit(const RdbChangeNode &changeNode)
{
    RdbObserverMapKey key(changeNode.uri_, changeNode.templateId_);
    auto callbacks = BaseCallbacks::GetEnabledObservers(key);
    for (auto &obs : callbacks) {
        if (obs != nullptr) {
            obs->OnChange(changeNode);
        }
    }
}

bool RdbSubscriberManager::Init()
{
    if (serviceCallback_ == nullptr) {
        LOG_INFO("callback init");
        serviceCallback_ = new RdbObserverStub([this](const RdbChangeNode &changeNode) { Emit(changeNode); });
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
