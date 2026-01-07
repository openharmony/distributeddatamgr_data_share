/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#define LOG_TAG "ANISubscriberManager"

#include "wrapper.rs.h"
#include "datashare_string_utils.h"
#include "ani_subscriber_manager.h"
#include "datashare_log.h"

namespace OHOS {
using namespace DataShare;
namespace DataShareAni {
std::vector<OperationResult> AniRdbSubscriberManager::AddObservers(rust::box<DataShareCallback> &callback,
    const std::vector<std::string> &uris, const DataShare::TemplateId &templateId)
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
    return AniRdbSubscriberManager::AniBaseCallbacks::AddObservers(
        keys, std::make_shared<Observer>(std::move(callback)),
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
                AniRdbSubscriberManager::AniBaseCallbacks::DelObservers(failedKeys, observer);
            }
        });
}

std::vector<OperationResult> AniRdbSubscriberManager::DelObservers(rust::Box<DataShareCallback> &callback,
    const std::vector<std::string> &uris, const DataShare::TemplateId &templateId)
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
    return AniRdbSubscriberManager::AniBaseCallbacks::DelObservers(keys,
        std::make_shared<Observer>(std::move(callback)),
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

std::vector<OperationResult> AniRdbSubscriberManager::DelObservers(const std::vector<std::string> &uris,
    const DataShare::TemplateId &templateId)
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
    return AniRdbSubscriberManager::AniBaseCallbacks::DelObservers(keys, nullptr,
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

void AniRdbSubscriberManager::Emit(const RdbChangeNode &changeNode)
{
    Key key(changeNode.uri_, changeNode.templateId_);
    lastChangeNodeMap_.InsertOrAssign(key, changeNode);
    auto callbacks = AniRdbSubscriberManager::AniBaseCallbacks::GetEnabledObservers(key);
    rust::Box<DataShareAni::RdbDataChangeNode> node = rust_create_rdb_data_change_node(rust::String(changeNode.uri_),
        rust::String(std::to_string(changeNode.templateId_.subscriberId_)),
        rust::String(changeNode.templateId_.bundleName_));
    if (!changeNode.isSharedMemory_) {
        for (const auto &data : changeNode.data_) {
            rdb_data_change_node_push_data(*node, rust::String(data));
        }
    }
    for (auto &obs : callbacks) {
        if (obs != nullptr) {
            obs->OnChange(changeNode);
        }
    }
}

void AniRdbSubscriberManager::Emit(const std::vector<Key> &keys, const std::shared_ptr<Observer> &observer)
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

std::vector<OperationResult> AniPublishedSubscriberManager::AddObservers(rust::Box<DataShareCallback> &callback,
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
    return AniPublishedSubscriberManager::AniBaseCallbacks::AddObservers(
        keys, std::make_shared<Observer>(std::move(callback)),
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
                [this](const DataShare::PublishedDataChangeNode &changeNode) {
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
                AniPublishedSubscriberManager::AniBaseCallbacks::DelObservers(failedKeys, observer);
            }
        });
}

std::vector<OperationResult> AniPublishedSubscriberManager::DelObservers(rust::Box<DataShareCallback> &callback,
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
    return AniPublishedSubscriberManager::AniBaseCallbacks::DelObservers(keys,
        std::make_shared<Observer>(std::move(callback)),
        [&dataShareHelper, &subscriberId, this](const std::vector<Key> &lastDelKeys,
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

std::vector<OperationResult> AniPublishedSubscriberManager::DelObservers(const std::vector<std::string> &uris,
    int64_t subscriberId)
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
    return AniPublishedSubscriberManager::AniBaseCallbacks::DelObservers(keys, nullptr,
        [&dataShareHelper, &subscriberId, this](const std::vector<Key> &lastDelKeys,
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

void AniPublishedSubscriberManager::Emit(const DataShare::PublishedDataChangeNode &changeNode)
{
    for (auto &data : changeNode.datas_) {
        Key key(data.key_, data.subscriberId_);
        lastChangeNodeMap_.Compute(key, [](const Key &, DataShare::PublishedDataChangeNode &value) {
            value.datas_.clear();
            return true;
        });
    }
    std::map<std::shared_ptr<Observer>, DataShare::PublishedDataChangeNode> results;
    for (auto &data : changeNode.datas_) {
        Key key(data.key_, data.subscriberId_);
        auto callbacks = AniPublishedSubscriberManager::AniBaseCallbacks::GetEnabledObservers(key);
        if (callbacks.empty()) {
            LOG_WARN("%{private}s nobody subscribe, but still notify", data.key_.c_str());
            continue;
        }
        lastChangeNodeMap_.Compute(key, [&data, &changeNode](const Key &, DataShare::PublishedDataChangeNode &value) {
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

void AniPublishedSubscriberManager::Emit(const std::vector<Key> &keys, const std::shared_ptr<Observer> &observer)
{
    DataShare::PublishedDataChangeNode node;
    for (auto &key : keys) {
        lastChangeNodeMap_.ComputeIfPresent(key, [&node](const Key &, DataShare::PublishedDataChangeNode &value) {
            for (auto &data : value.datas_) {
                node.datas_.emplace_back(data.key_, data.subscriberId_, data.GetData());
            }
            node.ownerBundleName_ = value.ownerBundleName_;
            return true;
        });
    }
    observer->OnChange(node);
}

std::vector<DataProxyResult> AniProxyDataSubscriberManager::AddObservers(
    rust::Box<DataShareCallback> &callback, const std::vector<std::string> &uris)
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
    return AniBaseCallbacks::AddObservers(
        keys, std::make_shared<Observer>(std::move(callback)),
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
                [this](const std::vector<DataShare::DataProxyChangeInfo> &changeInfo) {
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
                AniBaseCallbacks::DelObservers(failedKeys, observer);
            }
        });
}

std::vector<DataProxyResult> AniProxyDataSubscriberManager::DelObservers(
    rust::Box<DataShareCallback> &callback, const std::vector<std::string> &uris)
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
    return AniBaseCallbacks::DelObservers(keys,
        std::make_shared<Observer>(std::move(callback)),
        [&dataProxyHandle, this](const std::vector<Key> &lastDelKeys,
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

std::vector<DataProxyResult> AniProxyDataSubscriberManager::DelObservers(const std::vector<std::string> &uris)
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
    return AniBaseCallbacks::DelObservers(keys, nullptr,
        [&dataProxyHandle, this](const std::vector<Key> &lastDelKeys,
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

void AniProxyDataSubscriberManager::Emit(const std::vector<DataShare::DataProxyChangeInfo> &changeInfo)
{
    std::map<std::shared_ptr<Observer>, std::vector<DataShare::DataProxyChangeInfo>> results;
    for (const auto &info : changeInfo) {
        Key key(info.uri_);
        auto callbacks = AniBaseCallbacks::GetEnabledObservers(key);
        if (callbacks.empty()) {
            LOG_WARN("emit but nobody subscribe, uri is %{public}s",
                DataShareStringUtils::Anonymous(info.uri_).c_str());
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
} // namespace DataShareAni
} // namespace OHOS
