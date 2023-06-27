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

#include "napi_subscriber_manager.h"

#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
std::vector<OperationResult> NapiRdbSubscriberManager::AddObservers(napi_env env, napi_value callback,
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    auto datashareHelper = dataShareHelper_.lock();
    if (datashareHelper == nullptr) {
        LOG_ERROR("datashareHelper is nullptr");
        return std::vector<OperationResult>();
    }

    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &templateId](auto &uri) {
        keys.emplace_back(uri, templateId);
    });
    return BaseCallbacks::AddObservers(
        keys, std::make_shared<Observer>(env, callback),
        [this](const std::vector<Key> &localRegisterKeys, const std::shared_ptr<Observer> observer) {
            Emit(localRegisterKeys, observer);
        },
        [&datashareHelper, &templateId, this](const std::vector<Key> &firstAddKeys,
            const std::shared_ptr<Observer> observer, std::vector<OperationResult> &opResult) {
            std::vector<std::string> firstAddUris;
            std::for_each(firstAddKeys.begin(), firstAddKeys.end(), [&firstAddUris](auto &result) {
                firstAddUris.emplace_back(result);
            });
            if (firstAddUris.empty()) {
                return;
            }
            auto subResults =
                datashareHelper->SubscribeRdbData(firstAddUris, templateId, [this](const RdbChangeNode &changeNode) {
                    Emit(changeNode);
                });
            std::vector<Key> failedKeys;
            for (auto &subResult : subResults) {
                opResult.emplace_back(subResult);
                if (subResult.errCode_ != E_OK) {
                    failedKeys.emplace_back(subResult.key_, templateId);
                    LOG_WARN("registered failed, uri is %{public}s", subResult.key_.c_str());
                }
            }
            if (failedKeys.size() > 0) {
                BaseCallbacks::DelObservers(failedKeys, observer);
            }
        });
}

std::vector<OperationResult> NapiRdbSubscriberManager::DelObservers(napi_env env, napi_value callback,
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    auto dataShareHelper = dataShareHelper_.lock();
    if (dataShareHelper == nullptr) {
        LOG_ERROR("nativeManager is nullptr");
        return std::vector<OperationResult>();
    }
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &templateId](auto &uri) {
        keys.emplace_back(uri, templateId);
    });
    return BaseCallbacks::DelObservers(keys, callback == nullptr ? nullptr : std::make_shared<Observer>(env, callback),
        [&dataShareHelper, &templateId, this](const std::vector<Key> &lastDelKeys,
            const std::shared_ptr<Observer> &observer, std::vector<OperationResult> &opResult) {
            std::vector<std::string> lastDelUris;
            std::for_each(lastDelKeys.begin(), lastDelKeys.end(), [&lastDelUris, this](auto &result) {
                lastChangeNodeMap_.erase(result);
                lastDelUris.emplace_back(result);
            });
            if (lastDelUris.empty()) {
                return;
            }
            auto unsubResult = dataShareHelper->UnsubscribeRdbData(lastDelUris, templateId);
            opResult.insert(opResult.end(), unsubResult.begin(), unsubResult.end());
        });
}

void NapiRdbSubscriberManager::Emit(const RdbChangeNode &changeNode)
{
    Key key(changeNode.uri_, changeNode.templateId_);
    lastChangeNodeMap_[key] = changeNode;
    auto callbacks = BaseCallbacks::GetEnabledObservers(key);
    for (auto &obs : callbacks) {
        if (obs != nullptr) {
            obs->OnChange(changeNode);
        }
    }
}

void NapiRdbSubscriberManager::Emit(const std::vector<Key> &keys, const std::shared_ptr<Observer> &observer)
{
    for (auto const &key : keys) {
        auto it = lastChangeNodeMap_.find(key);
        if (it != lastChangeNodeMap_.end()) {
            observer->OnChange(it->second);
        }
    }
}

std::vector<OperationResult> NapiPublishedSubscriberManager::AddObservers(napi_env env, napi_value callback,
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    auto dataShareHelper = dataShareHelper_.lock();
    if (dataShareHelper == nullptr) {
        LOG_ERROR("datashareHelper is nullptr");
        return std::vector<OperationResult>();
    }

    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &subscriberId](auto &uri) {
        keys.emplace_back(uri, subscriberId);
    });
    return BaseCallbacks::AddObservers(
        keys, std::make_shared<Observer>(env, callback),
        [this](const std::vector<Key> &localRegisterKeys, const std::shared_ptr<Observer> observer) {
            Emit(localRegisterKeys, observer);
        },
        [&dataShareHelper, &subscriberId, this](const std::vector<Key> &firstAddKeys,
            const std::shared_ptr<Observer> observer, std::vector<OperationResult> &opResult) {
            std::vector<std::string> firstAddUris;
            std::for_each(firstAddKeys.begin(), firstAddKeys.end(), [&firstAddUris](auto &result) {
                firstAddUris.emplace_back(result);
            });
            if (firstAddUris.empty()) {
                return;
            }
            auto subResults = dataShareHelper->SubscribePublishedData(firstAddUris, subscriberId,
                [this](const PublishedDataChangeNode &changeNode) {
                    Emit(changeNode);
                });
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
        });
}

std::vector<OperationResult> NapiPublishedSubscriberManager::DelObservers(napi_env env, napi_value callback,
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    auto dataShareHelper = dataShareHelper_.lock();
    if (dataShareHelper == nullptr) {
        LOG_ERROR("nativeManager is nullptr");
        return std::vector<OperationResult>();
    }
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &subscriberId](auto &uri) {
        keys.emplace_back(uri, subscriberId);
    });
    return BaseCallbacks::DelObservers(keys, callback == nullptr ? nullptr : std::make_shared<Observer>(env, callback),
        [&dataShareHelper, &subscriberId, &callback, &uris, this](const std::vector<Key> &lastDelKeys,
            const std::shared_ptr<Observer> &observer, std::vector<OperationResult> &opResult) {
            std::vector<std::string> lastDelUris;
            std::for_each(lastDelKeys.begin(), lastDelKeys.end(), [&lastDelUris, this](auto &result) {
                lastChangeNodeMap_.erase(result);
                lastDelUris.emplace_back(result);
            });
            if (lastDelUris.empty()) {
                return;
            }
            auto unsubResult = dataShareHelper->UnsubscribePublishedData(lastDelUris, subscriberId);
            opResult.insert(opResult.end(), unsubResult.begin(), unsubResult.end());
        });
}

void NapiPublishedSubscriberManager::Emit(const PublishedDataChangeNode &changeNode)
{
    for (auto &data : changeNode.datas_) {
        Key key(data.key_, data.subscriberId_);
        lastChangeNodeMap_[key].datas_.clear();
    }
    std::map<std::shared_ptr<Observer>, PublishedDataChangeNode> results;
    for (auto &data : changeNode.datas_) {
        Key key(data.key_, data.subscriberId_);
        auto callbacks = BaseCallbacks::GetEnabledObservers(key);
        if (callbacks.empty()) {
            LOG_WARN("%{private}s nobody subscribe, but still notify", data.key_.c_str());
            continue;
        }
        lastChangeNodeMap_[key].datas_.emplace_back(data.key_, data.subscriberId_, data.GetData());
        lastChangeNodeMap_[key].ownerBundleName_ = changeNode.ownerBundleName_;
        for (auto const &obs : callbacks) {
            results[obs].datas_.emplace_back(data.key_, data.subscriberId_, data.GetData());
        }
    }
    for (auto &[callback, node] : results) {
        node.ownerBundleName_ = changeNode.ownerBundleName_;
        callback->OnChange(node);
    }
}

void NapiPublishedSubscriberManager::Emit(const std::vector<Key> &keys, const std::shared_ptr<Observer> &observer)
{
    PublishedDataChangeNode node;
    for (auto &key : keys) {
        auto it = lastChangeNodeMap_.find(key);
        if (it == lastChangeNodeMap_.end()) {
            continue;
        }
        for (auto &data : it->second.datas_) {
            node.datas_.emplace_back(data.key_, data.subscriberId_, data.GetData());
        }
        node.ownerBundleName_ = it->second.ownerBundleName_;
    }
    observer->OnChange(node);
}
} // namespace DataShare
} // namespace OHOS
