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

#ifndef DATA_SHARE_CALLBACKS_MANAGER_H
#define DATA_SHARE_CALLBACKS_MANAGER_H
#include <map>
#include <mutex>
#include <vector>

#include "datashare_errno.h"
#include "datashare_template.h"

namespace OHOS::DataShare {
template<class Key, class Observer>
class CallbacksManager {
public:
    std::vector<OperationResult> AddObservers(const std::vector<Key> &keys, const std::shared_ptr<Observer> observer,
        std::function<void(
            const std::vector<Key> &, const std::shared_ptr<Observer> &observer, std::vector<OperationResult> &)>);
    std::vector<OperationResult> DelObservers(const std::vector<Key> &keys,
        const std::shared_ptr<Observer> observer = nullptr,
        std::function<void(
            const std::vector<Key> &, const std::shared_ptr<Observer> &observer, std::vector<OperationResult> &)>
            processOnLastDel = CallbacksManager::DefaultProcess);
    std::vector<OperationResult> EnableObservers(
        const std::vector<Key> &keys, std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)>);

    std::vector<OperationResult> DisableObservers(const std::vector<Key> &keys,
        std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)> processOnLastDel =
            CallbacksManager::DefaultProcess2);
    std::vector<std::shared_ptr<Observer>> GetEnabledObservers(const Key &);
    void DelAllObservers(std::function<void(const std::vector<Key> &)>);
    int GetEnabledSubscriberSize();
private:
    static void DefaultProcess2(const std::vector<Key> &, std::vector<OperationResult> &){};
    static void DefaultProcess(
        const std::vector<Key> &, const std::shared_ptr<Observer> &observer, std::vector<OperationResult> &){};
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
std::vector<OperationResult> CallbacksManager<Key, Observer>::AddObservers(const std::vector<Key> &keys,
    const std::shared_ptr<Observer> observer,
    std::function<void(const std::vector<Key> &, const std::shared_ptr<Observer> &observer,
        std::vector<OperationResult> &)> processOnFirstAdd)
{
    std::vector<OperationResult> result;
    std::vector<Key> firstRegisterKey;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
        for (auto &key : keys) {
            std::vector<std::shared_ptr<Observer>> enabledObservers = GetEnabledObservers(key);
            if (enabledObservers.size() == 0) {
                callbacks_[key].emplace_back(ObserverNode(observer));
                firstRegisterKey.emplace_back(key);
                continue;
            }
            if (IsRegistered(*observer, callbacks_[key])) {
                result.emplace_back(static_cast<std::string>(key), E_REGISTERED_REPEATED);
                continue;
            }
            callbacks_[key].emplace_back(observer);
            result.emplace_back(key, E_OK);
        }
    }
    processOnFirstAdd(firstRegisterKey, observer, result);
    return result;
}

template<class Key, class Observer>
bool CallbacksManager<Key, Observer>::IsRegistered(const Observer &observer, const std::vector<ObserverNode> &observers)
{
    for (auto &item : observers) {
        if (*(item.observer_) == observer) {
            return true;
        }
    }
    return false;
}

template<class Key, class Observer>
std::vector<OperationResult> CallbacksManager<Key, Observer>::DelObservers(const std::vector<Key> &keys,
    const std::shared_ptr<Observer> observer,
    std::function<void(const std::vector<Key> &, const std::shared_ptr<Observer> &, std::vector<OperationResult> &)>
        processOnLastDel)
{
    std::vector<OperationResult> result;
    std::vector<Key> lastDelKeys;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
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
std::vector<std::shared_ptr<Observer>> CallbacksManager<Key, Observer>::GetEnabledObservers(const Key &inputKey)
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
std::vector<OperationResult> CallbacksManager<Key, Observer>::EnableObservers(const std::vector<Key> &keys,
    std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)> processOnFirstEnabled)
{
    std::vector<OperationResult> result;
    std::vector<Key> firstRegisterKey;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
        for (auto &key : keys) {
            auto it = callbacks_.find(key);
            if (it == callbacks_.end()) {
                result.emplace_back(key, E_SUBSCRIBER_NOT_EXIST);
                continue;
            }
            std::vector<std::shared_ptr<Observer>> enabledObservers = GetEnabledObservers(key);
            if (enabledObservers.size() == 0) {
                firstRegisterKey.emplace_back(key);
            }
            for (auto &item: callbacks_[key]) {
                item.enabled_ = true;
            }
            result.emplace_back(key, E_OK);
        }
    }
    processOnFirstEnabled(firstRegisterKey, result);
    return result;
}

template<class Key, class Observer>
std::vector<OperationResult> CallbacksManager<Key, Observer>::DisableObservers(const std::vector<Key> &keys,
    std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)> processOnLastDisable)
{
    std::vector<OperationResult> result;
    std::vector<Key> lastDisabledKeys;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
        for (auto &key : keys) {
            auto it = callbacks_.find(key);
            if (it == callbacks_.end()) {
                result.emplace_back(key, E_SUBSCRIBER_NOT_EXIST);
                continue;
            }
            std::vector<std::shared_ptr<Observer>> enabledObservers = GetEnabledObservers(key);
            if (enabledObservers.size() > 0) {
                lastDisabledKeys.emplace_back(key);
            }
            for (auto &item: callbacks_[key]) {
                item.enabled_ = false;
            }
            result.emplace_back(key, E_OK);
        }
    }
    processOnLastDisable(lastDisabledKeys, result);
    return result;
}
template<class Key, class Observer>
int CallbacksManager<Key, Observer>::GetEnabledSubscriberSize()
{
    int count = 0;
    std::lock_guard<decltype(mutex_)> lck(mutex_);
    for (auto &[key, value] : callbacks_) {
        for (const auto &callback:value) {
            if (callback.enabled_) {
                count++;
            }
        }
    }
    return count;
}

template<class Key, class Observer>
void CallbacksManager<Key, Observer>::DelAllObservers(std::function<void(const std::vector<Key> &)> processOnLastDel)
{
    std::vector<Key> lastDelKeys;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
        for (auto &it : callbacks_) {
            lastDelKeys.emplace_back(it.first);
        }
        callbacks_.clear();
    }
    processOnLastDel(lastDelKeys);
}
} // namespace OHOS::DataShare
#endif // DATA_SHARE_CALLBACKS_MANAGER_H
