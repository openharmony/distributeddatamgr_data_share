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
#include <memory>
#include <vector>

#include "dataproxy_handle_common.h"
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
                lastChangeNodeMap_.Erase(result);
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
    lastChangeNodeMap_.InsertOrAssign(key, changeNode);
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
        bool isExist = false;
        RdbChangeNode node;
        lastChangeNodeMap_.ComputeIfPresent(key, [&node, &isExist](const Key &, const RdbChangeNode &value) {
            node = value;
            isExist = true;
            return true;
        });
        if (isExist) {
            observer->OnChange(node);
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
                lastChangeNodeMap_.Erase(result);
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
        lastChangeNodeMap_.Compute(key, [](const Key &, PublishedDataChangeNode &value) {
            value.datas_.clear();
            return true;
        });
    }
    std::map<std::shared_ptr<Observer>, PublishedDataChangeNode> results;
    for (auto &data : changeNode.datas_) {
        Key key(data.key_, data.subscriberId_);
        auto callbacks = BaseCallbacks::GetEnabledObservers(key);
        if (callbacks.empty()) {
            LOG_WARN("%{private}s nobody subscribe, but still notify", data.key_.c_str());
            continue;
        }
        lastChangeNodeMap_.Compute(key, [&data, &changeNode](const Key &, PublishedDataChangeNode &value) {
            value.datas_.emplace_back(data.key_, data.subscriberId_, data.GetData());
            value.ownerBundleName_ = changeNode.ownerBundleName_;
            return true;
        });
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
        lastChangeNodeMap_.ComputeIfPresent(key, [&node](const Key &, PublishedDataChangeNode &value) {
            for (auto &data : value.datas_) {
                node.datas_.emplace_back(data.key_, data.subscriberId_, data.GetData());
            }
            node.ownerBundleName_ = value.ownerBundleName_;
            return true;
        });
    }
    observer->OnChange(node);
}

std::vector<DataProxyResult> NapiProxyDataSubscriberManager::AddObservers(napi_env env, napi_value callback,
    const std::vector<std::string> &uris)
{
    std::vector<DataProxyResult> result = {};
    auto dataProxyHandle = dataProxyHandle_.lock();
    if (dataProxyHandle == nullptr) {
        LOG_ERROR("dataProxyHandle is nullptr");
        return result;
    }

    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys](auto &uri) {
        keys.emplace_back(uri);
    });
    return BaseCallbacks::AddObservers(
        keys, std::make_shared<Observer>(env, callback),
        [&dataProxyHandle, this](const std::vector<Key> &firstAddKeys,
            const std::shared_ptr<Observer> observer, std::vector<DataProxyResult> &opResult) {
            std::vector<std::string> firstAddUris;
            std::for_each(firstAddKeys.begin(), firstAddKeys.end(), [&firstAddUris](auto &result) {
                firstAddUris.emplace_back(result);
            });
            if (firstAddUris.empty()) {
                return;
            }
            auto subResults = dataProxyHandle->SubscribeProxyData(firstAddUris,
                [this](const std::vector<DataProxyChangeInfo> &changeInfo) {
                    Emit(changeInfo);
                });
            std::vector<Key> failedKeys;
            for (auto &subResult : subResults) {
                opResult.emplace_back(subResult);
                if (subResult.result_ != SUCCESS) {
                    failedKeys.emplace_back(subResult.uri_);
                    LOG_WARN("registered failed, uri is %{public}s", subResult.uri_.c_str());
                }
            }
            if (failedKeys.size() > 0) {
                BaseCallbacks::DelObservers(failedKeys, observer);
            }
        });
}

std::vector<DataProxyResult> NapiProxyDataSubscriberManager::DelObservers(napi_env env, napi_value callback,
    const std::vector<std::string> &uris)
{
    std::vector<DataProxyResult> result = {};
    auto dataProxyHandle = dataProxyHandle_.lock();
    if (dataProxyHandle == nullptr) {
        LOG_ERROR("dataProxyHandle is nullptr");
        return result;
    }

    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys](auto &uri) {
        keys.emplace_back(uri);
    });
    return BaseCallbacks::DelObservers(keys, callback == nullptr ? nullptr : std::make_shared<Observer>(env, callback),
        [&dataProxyHandle, &callback, &uris, this](const std::vector<Key> &lastDelKeys,
            const std::shared_ptr<Observer> &observer, std::vector<DataProxyResult> &opResult) {
            std::vector<std::string> lastDelUris;
            std::for_each(lastDelKeys.begin(), lastDelKeys.end(), [&lastDelUris, this](auto &result) {
                lastDelUris.emplace_back(result);
            });
            if (lastDelUris.empty()) {
                return;
            }
            auto unsubResult = dataProxyHandle->UnsubscribeProxyData(lastDelUris);
            opResult.insert(opResult.end(), unsubResult.begin(), unsubResult.end());
        });
}

void NapiProxyDataSubscriberManager::Emit(const std::vector<DataProxyChangeInfo> &changeInfo)
{
    std::map<std::shared_ptr<Observer>, std::vector<DataProxyChangeInfo>> results;
    for (const auto &info : changeInfo) {
        Key key(info.uri_);
        auto callbacks = BaseCallbacks::GetEnabledObservers(key);
        if (callbacks.empty()) {
            LOG_WARN("emit but nobody subscribe");
            continue;
        }
        for (const auto &obs : callbacks) {
            results[obs].emplace_back(info);
        }
    }
    for (const auto &[callback, infos] : results) {
        callback->OnChange(infos);
    }
}
} // namespace DataShare
} // namespace OHOS
