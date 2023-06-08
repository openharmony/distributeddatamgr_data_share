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
    std::vector<OperationResult> AddObservers(const std::vector<Key> &keys, void *subscriber,
        const std::shared_ptr<Observer> observer,
        std::function<void(const std::vector<Key> &, const std::shared_ptr<Observer> &observer,
            std::vector<OperationResult> &)>);

    std::vector<OperationResult> DelObservers(const std::vector<Key> &keys, void *subscriber,
        std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)> processOnLastDel =
            CallbacksManager::DefaultProcess);

    std::vector<OperationResult> DelObservers(void *subscriber,
        std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)> processOnLastDel =
            CallbacksManager::DefaultProcess);

    std::vector<OperationResult> EnableObservers(const std::vector<Key> &keys, void *subscriber,
        std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)>);

    std::vector<OperationResult> DisableObservers(const std::vector<Key> &keys, void *subscriber,
        std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)> processOnLastDel =
            CallbacksManager::DefaultProcess);

    std::vector<std::shared_ptr<Observer>> GetEnabledObservers(const Key &);

    int GetEnabledSubscriberSize();
    int GetEnabledSubscriberSize(const Key &key);
    void RecoverObservers(std::function<void(const std::vector<Key> &)> recoverObservers);

private:
    static void DefaultProcess(const std::vector<Key> &, std::vector<OperationResult> &){};
    struct ObserverNode {
        std::shared_ptr<Observer> observer_;
        bool enabled_;
        void *subscriber_;
        ObserverNode(const std::shared_ptr<Observer> &observer, void *subscriber)
            : observer_(observer), subscriber_(subscriber)
        {
            enabled_ = true;
        };
    };
    void DelLocalObservers(const Key &key, void *subscriber, std::vector<Key> &lastDelKeys,
        std::vector<OperationResult> &result);
    void DelLocalObservers(void *subscriber, std::vector<Key> &lastDelKeys, std::vector<OperationResult> &result);
    std::recursive_mutex mutex_{};
    std::map<Key, std::vector<ObserverNode>> callbacks_;
};

template<class Key, class Observer>
std::vector<OperationResult> CallbacksManager<Key, Observer>::AddObservers(const std::vector<Key> &keys,
    void *subscriber, const std::shared_ptr<Observer> observer, std::function<void(const std::vector<Key> &,
    const std::shared_ptr<Observer> &observer, std::vector<OperationResult> &)> processOnFirstAdd)
{
    std::vector<OperationResult> result;
    std::vector<Key> firstRegisterKey;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
        for (auto &key : keys) {
            std::vector<std::shared_ptr<Observer>> enabledObservers = GetEnabledObservers(key);
            if (enabledObservers.empty()) {
                callbacks_[key].emplace_back(observer, subscriber);
                firstRegisterKey.emplace_back(key);
                continue;
            }
            callbacks_[key].emplace_back(observer, subscriber);
            result.emplace_back(key, E_OK);
        }
    }
    processOnFirstAdd(firstRegisterKey, observer, result);
    return result;
}

template<class Key, class Observer>
void CallbacksManager<Key, Observer>::RecoverObservers(std::function<void(const std::vector<Key> &)> recoverObservers)
{
    std::vector<Key> keys;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
        for (auto &it : callbacks_) {
            if (GetEnabledSubscriberSize(it.first) > 0) {
                keys.emplace_back(it.first);
            }
        }
    }
    recoverObservers(keys);
}


template<class Key, class Observer>
void CallbacksManager<Key, Observer>::DelLocalObservers(void *subscriber, std::vector<Key> &lastDelKeys,
    std::vector<OperationResult> &result)
{
    for (auto &it : callbacks_) {
        DelLocalObservers(it.first, subscriber, lastDelKeys, result);
    }
}

template<class Key, class Observer>
void CallbacksManager<Key, Observer>::DelLocalObservers(const Key &key, void *subscriber,
    std::vector<Key> &lastDelKeys, std::vector<OperationResult> &result)
{
    auto it = callbacks_.find(key);
    if (it == callbacks_.end()) {
        result.emplace_back(key, E_UNREGISTERED_EMPTY);
        return;
    }
    std::vector<ObserverNode> &callbacks = it->second;
    auto callbackIt = callbacks.begin();
    while (callbackIt != callbacks.end()) {
        if (callbackIt->subscriber_ != subscriber) {
            callbackIt++;
            continue;
        }
        callbackIt = callbacks.erase(callbackIt);
    }
    if (!it->second.empty()) {
        result.emplace_back(key, E_OK);
        return;
    }
    lastDelKeys.emplace_back(key);
}

template<class Key, class Observer>
std::vector<OperationResult> CallbacksManager<Key, Observer>::DelObservers(void *subscriber,
    std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)> processOnLastDel)
{
    std::vector<OperationResult> result;
    std::vector<Key> lastDelKeys;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
        DelLocalObservers(subscriber, lastDelKeys, result);
        if (lastDelKeys.empty()) {
            return result;
        }
        for (auto &key : lastDelKeys) {
            callbacks_.erase(key);
        }
    }
    processOnLastDel(lastDelKeys, result);
    return result;
}

template<class Key, class Observer>
std::vector<OperationResult> CallbacksManager<Key, Observer>::DelObservers(const std::vector<Key> &keys,
    void *subscriber, std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)> processOnLastDel)
{
    std::vector<OperationResult> result;
    std::vector<Key> lastDelKeys;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
        for (auto &key : keys) {
            DelLocalObservers(key, subscriber, lastDelKeys, result);
        }
        if (lastDelKeys.empty()) {
            return result;
        }
        for (auto &key : lastDelKeys) {
            callbacks_.erase(key);
        }
    }
    processOnLastDel(lastDelKeys, result);
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
std::vector<OperationResult> CallbacksManager<Key, Observer>::EnableObservers(
    const std::vector<Key> &keys, void *subscriber,
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
            bool hasEnabled = false;
            for (auto &item : callbacks_[key]) {
                if (item.subscriber_ == subscriber) {
                    item.enabled_ = true;
                    hasEnabled = true;
                }
            }
            if (!hasEnabled) {
                result.emplace_back(key, E_SUBSCRIBER_NOT_EXIST);
                continue;
            }
            if (!enabledObservers.empty()) {
                result.emplace_back(key, E_OK);
                continue;
            }
            firstRegisterKey.emplace_back(key);
        }
    }
    processOnFirstEnabled(firstRegisterKey, result);
    return result;
}

template<class Key, class Observer>
std::vector<OperationResult> CallbacksManager<Key, Observer>::DisableObservers(
    const std::vector<Key> &keys, void *subscriber,
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
            if (enabledObservers.empty()) {
                result.emplace_back(key, E_OK);
                continue;
            }

            bool hasDisabled = false;
            for (auto &item : callbacks_[key]) {
                if (item.subscriber_ == subscriber) {
                    item.enabled_ = false;
                    hasDisabled = true;
                }
            }
            if (!hasDisabled) {
                result.emplace_back(key, E_SUBSCRIBER_NOT_EXIST);
                continue;
            }
            enabledObservers = GetEnabledObservers(key);
            if (!enabledObservers.empty()) {
                result.emplace_back(key, E_OK);
                continue;
            }
            lastDisabledKeys.emplace_back(key);
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
        count += GetEnabledSubscriberSize(key);
    }
    return count;
}

template<class Key, class Observer>
int CallbacksManager<Key, Observer>::GetEnabledSubscriberSize(const Key &key)
{
    std::lock_guard<decltype(mutex_)> lck(mutex_);
    int count = 0;
    auto it = callbacks_.find(key);
    if (it == callbacks_.end()) {
        return count;
    }
    std::vector<ObserverNode> &callbacks = it->second;
    for (const auto &callback : callbacks) {
        if (callback.enabled_) {
            count++;
        }
    }
    return count;
}
} // namespace OHOS::DataShare
#endif // DATA_SHARE_CALLBACKS_MANAGER_H
