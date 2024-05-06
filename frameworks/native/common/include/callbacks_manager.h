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
#include "datashare_log.h"
#include "datashare_template.h"

namespace OHOS::DataShare {
template<class Key, class Observer>
class CallbacksManager {
public:
    struct ObserverNodeOnEnabled {
        ObserverNodeOnEnabled(const std::shared_ptr<Observer> &observer, bool isNotifyOnEnabled = false)
            : observer_(observer), isNotifyOnEnabled_(isNotifyOnEnabled) {};
        std::shared_ptr<Observer> observer_;
        bool isNotifyOnEnabled_;
    };

    std::vector<OperationResult> AddObservers(const std::vector<Key> &keys, void *subscriber,
        const std::shared_ptr<Observer> observer,
        std::function<void(const std::vector<Key> &, const std::shared_ptr<Observer> &observer)>,
        std::function<void(const std::vector<Key> &, const std::shared_ptr<Observer> &observer,
            std::vector<OperationResult> &)>);

    std::vector<OperationResult> DelObservers(const std::vector<Key> &keys, void *subscriber,
        std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)> processOnLastDel =
            CallbacksManager::DefaultProcess);

    std::vector<OperationResult> DelObservers(void *subscriber,
        std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)> processOnLastDel =
            CallbacksManager::DefaultProcess);

    std::vector<OperationResult> EnableObservers(const std::vector<Key> &keys, void *subscriber,
        std::function<void(std::map<Key, std::vector<ObserverNodeOnEnabled>> &)> processOnLocalEnabled,
        std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)>);

    std::vector<OperationResult> DisableObservers(const std::vector<Key> &keys, void *subscriber,
        std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)> processOnLastDel =
            CallbacksManager::DefaultProcess);

    std::vector<std::shared_ptr<Observer>> GetEnabledObservers(const Key &);

    int GetEnabledSubscriberSize();
    int GetEnabledSubscriberSize(const Key &key);
    std::vector<Key> GetKeys();
    void SetObserversNotifiedOnEnabled(const Key &key);

private:
    static void DefaultProcess(const std::vector<Key> &, std::vector<OperationResult> &){};
    struct ObserverNode {
        std::shared_ptr<Observer> observer_;
        bool enabled_;
        void *subscriber_;
        bool isNotifyOnEnabled_;
        ObserverNode(const std::shared_ptr<Observer> &observer, void *subscriber)
            : observer_(observer), subscriber_(subscriber)
        {
            enabled_ = true;
            isNotifyOnEnabled_ = false;
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
    void *subscriber, const std::shared_ptr<Observer> observer,
    std::function<void(const std::vector<Key> &, const std::shared_ptr<Observer> &observer)> processOnLocalAdd,
    std::function<void(const std::vector<Key> &,
        const std::shared_ptr<Observer> &observer, std::vector<OperationResult> &)> processOnFirstAdd)
{
    std::vector<OperationResult> result;
    std::vector<Key> firstRegisterKey;
    std::vector<Key> localRegisterKey;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
        for (auto &key : keys) {
            std::vector<std::shared_ptr<Observer>> enabledObservers = GetEnabledObservers(key);
            if (enabledObservers.empty()) {
                callbacks_[key].emplace_back(observer, subscriber);
                firstRegisterKey.emplace_back(key);
                continue;
            }
            localRegisterKey.emplace_back(key);
            callbacks_[key].emplace_back(observer, subscriber);
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
std::vector<Key> CallbacksManager<Key, Observer>::GetKeys()
{
    std::vector<Key> keys;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
        for (auto &it : callbacks_) {
            keys.emplace_back(it.first);
        }
    }
    return keys;
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
    std::function<void(std::map<Key, std::vector<ObserverNodeOnEnabled>> &)> enableLocalFunc,
    std::function<void(const std::vector<Key> &, std::vector<OperationResult> &)> enableServiceFunc)
{
    std::vector<OperationResult> result;
    std::vector<Key> sendServiceKeys;
    std::map<Key, std::vector<ObserverNodeOnEnabled>> refreshObservers;
    {
        std::lock_guard<decltype(mutex_)> lck(mutex_);
        for (auto &key : keys) {
            auto it = callbacks_.find(key);
            if (it == callbacks_.end()) {
                result.emplace_back(key, E_SUBSCRIBER_NOT_EXIST);
                continue;
            }

            auto& allObservers = it->second;
            auto iterator = std::find_if(allObservers.begin(), allObservers.end(), [&subscriber](ObserverNode node) {
                if (node.subscriber_ == subscriber) {
                    return true;
                }
                return false;
            });
            if (iterator == allObservers.end()) {
                result.emplace_back(key, E_SUBSCRIBER_NOT_EXIST);
                continue;
            }
            if (iterator->enabled_) {
                result.emplace_back(key, E_OK);
                continue;
            }

            std::vector<std::shared_ptr<Observer>> enabledObservers = GetEnabledObservers(key);
            if (enabledObservers.empty()) {
                sendServiceKeys.emplace_back(key);
            }
            refreshObservers[key].emplace_back(iterator->observer_, iterator->isNotifyOnEnabled_);
            iterator->enabled_ = true;
        }
    }
    enableServiceFunc(sendServiceKeys, result);
    enableLocalFunc(refreshObservers);

    return result;
}

template<class Key, class Observer>
std::vector<OperationResult> CallbacksManager<Key, Observer>::DisableObservers(const std::vector<Key> &keys,
    void *subscriber,
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
                result.emplace_back(key, E_SUBSCRIBER_NOT_EXIST);
                continue;
            }

            bool hasDisabled = false;
            for (auto &item : callbacks_[key]) {
                if (item.subscriber_ == subscriber) {
                    item.enabled_ = false;
                    item.isNotifyOnEnabled_ = false;
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

template<class Key, class Observer>
void CallbacksManager<Key, Observer>::SetObserversNotifiedOnEnabled(const Key &key)
{
    std::lock_guard<decltype(mutex_)> lck(mutex_);
    auto it = callbacks_.find(key);
    if (it == callbacks_.end()) {
        return;
    }
    std::vector<ObserverNode> &callbacks = it->second;
    uint32_t num = 0;
    for (auto &observerNode : callbacks) {
        if (!observerNode.enabled_) {
            num++;
            observerNode.isNotifyOnEnabled_ = true;
        }
    }
    if (num > 0) {
        LOG_INFO("total %{public}zu, not refreshed %{public}u", callbacks.size(), num);
    }
}
} // namespace OHOS::DataShare
#endif // DATA_SHARE_CALLBACKS_MANAGER_H
