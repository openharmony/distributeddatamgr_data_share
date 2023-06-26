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

#ifndef LDBPROJ_NAPI_CALLBACKS_MANAGER_H
#define LDBPROJ_NAPI_CALLBACKS_MANAGER_H
#include <map>
#include <mutex>
#include <vector>

#include "datashare_errno.h"
#include "datashare_template.h"

namespace OHOS::DataShare {
template<class Key, class Observer>
class NapiCallbacksManager {
public:
    std::vector<OperationResult> AddObservers(const std::vector<Key> &keys, const std::shared_ptr<Observer> observer,
        std::function<void(const std::vector<Key> &, const std::shared_ptr<Observer> &observer)>,
        std::function<void(const std::vector<Key> &, const std::shared_ptr<Observer> &observer,
            std::vector<OperationResult> &)>);

    std::vector<OperationResult> DelObservers(const std::vector<Key> &keys,
        const std::shared_ptr<Observer> observer = nullptr,
        std::function<void(const std::vector<Key> &, const std::shared_ptr<Observer> &observer,
            std::vector<OperationResult> &)> processOnLastDel = NapiCallbacksManager::DefaultProcess);

    std::vector<std::shared_ptr<Observer>> GetEnabledObservers(const Key &);

    int GetEnabledSubscriberSize();

private:
    static void DefaultProcess(const std::vector<Key> &, const std::shared_ptr<Observer> &observer,
        std::vector<OperationResult> &){};
    struct ObserverNode {
        std::shared_ptr<Observer> observer_;
        bool enabled_;
        ObserverNode(const std::shared_ptr<Observer> &observer) : observer_(observer)
        {
            enabled_ = true;
        };
        ObserverNode(const std::shared_ptr<Observer> &observer, bool enabled)
            : observer_(observer), enabled_(enabled){};
    };
    bool IsRegistered(const Observer &, const std::vector<ObserverNode> &);
    std::recursive_mutex mutex_{};
    std::map<Key, std::vector<ObserverNode>> callbacks_;
};

template<class Key, class Observer>
std::vector<OperationResult> NapiCallbacksManager<Key, Observer>::AddObservers(
    const std::vector<Key> &keys, const std::shared_ptr<Observer> observer,
    std::function<void(const std::vector<Key> &, const std::shared_ptr<Observer> &observer)> processOnLocalAdd,
    std::function<void(const std::vector<Key> &, const std::shared_ptr<Observer> &observer,
        std::vector<OperationResult> &)> processOnFirstAdd)
{
    std::vector<OperationResult> result;
    std::vector<Key> firstRegisterKey;
    std::vector<Key> localRegisterKey;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
        for (auto &key : keys) {
            std::vector<std::shared_ptr<Observer>> enabledObservers = GetEnabledObservers(key);
            if (enabledObservers.empty()) {
                callbacks_[key].emplace_back(ObserverNode(observer));
                firstRegisterKey.emplace_back(key);
                continue;
            }
            if (IsRegistered(*observer, callbacks_[key])) {
                result.emplace_back(static_cast<std::string>(key), E_REGISTERED_REPEATED);
                continue;
            }
            localRegisterKey.emplace_back(key);
            callbacks_[key].emplace_back(observer);
            result.emplace_back(key, E_OK);
        }
    }
    if (!localRegisterKey.empty()) {
        processOnLocalAdd(localRegisterKey, observer);
    }
    processOnFirstAdd(firstRegisterKey, observer, result);
    return result;
}

template<class Key, class Observer>
bool NapiCallbacksManager<Key, Observer>::IsRegistered(const Observer &observer,
    const std::vector<ObserverNode> &observers)
{
    for (auto &item : observers) {
        if (*(item.observer_) == observer) {
            return true;
        }
    }
    return false;
}

template<class Key, class Observer>
std::vector<OperationResult> NapiCallbacksManager<Key, Observer>::DelObservers(
    const std::vector<Key> &keys, const std::shared_ptr<Observer> observer,
    std::function<void(const std::vector<Key> &, const std::shared_ptr<Observer> &, std::vector<OperationResult> &)>
        processOnLastDel)
{
    std::vector<OperationResult> result;
    std::vector<Key> lastDelKeys;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
        if (keys.empty() && observer == nullptr) {
            for (auto &it : callbacks_) {
                lastDelKeys.emplace_back(it.first);
            }
            callbacks_.clear();
        }
        for (auto &key : keys) {
            auto it = callbacks_.find(key);
            if (it == callbacks_.end()) {
                result.emplace_back(key, E_UNREGISTERED_EMPTY);
                continue;
            }
            if (observer == nullptr) {
                callbacks_.erase(key);
                lastDelKeys.emplace_back(key);
                continue;
            }
            if (!IsRegistered(*observer, it->second)) {
                result.emplace_back(key, E_UNREGISTERED_EMPTY);
                continue;
            }
            std::vector<ObserverNode> &callbacks = it->second;
            auto callbackIt = callbacks.begin();
            while (callbackIt != callbacks.end()) {
                if (!(*(callbackIt->observer_) == *observer)) {
                    callbackIt++;
                    continue;
                }
                callbackIt = callbacks.erase(callbackIt);
            }
            if (!it->second.empty()) {
                result.emplace_back(key, E_OK);
                continue;
            }
            callbacks_.erase(key);
            lastDelKeys.emplace_back(key);
        }
        if (lastDelKeys.empty()) {
            return result;
        }
    }
    processOnLastDel(lastDelKeys, observer, result);
    return result;
}

template<class Key, class Observer>
std::vector<std::shared_ptr<Observer>> NapiCallbacksManager<Key, Observer>::GetEnabledObservers(const Key &inputKey)
{
    std::lock_guard<decltype(mutex_)> lck(mutex_);
    auto it = callbacks_.find(inputKey);
    if (it == callbacks_.end()) {
        return std::vector<std::shared_ptr<Observer>>();
    }
    std::vector<std::shared_ptr<Observer>> results;
    for (const auto &value : it->second) {
        if (value.enabled_ && value.observer_ != nullptr) {
            results.emplace_back(value.observer_);
        }
    }
    return results;
}

template<class Key, class Observer>
int NapiCallbacksManager<Key, Observer>::GetEnabledSubscriberSize()
{
    int count = 0;
    std::lock_guard<decltype(mutex_)> lck(mutex_);
    for (auto &[key, value] : callbacks_) {
        for (const auto &callback : value) {
            if (callback.enabled_) {
                count++;
            }
        }
    }
    return count;
}
} // namespace OHOS::DataShare
#endif //LDBPROJ_NAPI_CALLBACKS_MANAGER_H
