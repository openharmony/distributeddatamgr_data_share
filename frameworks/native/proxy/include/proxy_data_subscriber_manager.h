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

#ifndef PROXY_DATA_SUBSCRIBER_MANAGER_H
#define PROXY_DATA_SUBSCRIBER_MANAGER_H
#include <memory>

#include "callbacks_manager.h"
#include "concurrent_map.h"
#include "data_proxy_observer.h"
#include "data_proxy_observer_stub.h"
#include "dataproxy_handle.h"
#include "dataproxy_handle_common.h"
#include "idatashare.h"
#include "iremote_stub.h"
#include "data_share_service_proxy.h"

namespace OHOS {
namespace DataShare {
struct ProxyDataObserverMapKey {
    std::string uri_;
    ProxyDataObserverMapKey(const std::string &uri) : uri_(uri) {};
    bool operator==(const ProxyDataObserverMapKey &node) const
    {
        return uri_ == node.uri_;
    }
    bool operator!=(const ProxyDataObserverMapKey &node) const
    {
        return !(node == *this);
    }
    bool operator<(const ProxyDataObserverMapKey &node) const
    {
        return uri_ < node.uri_;
    }
    operator std::string() const
    {
        return uri_;
    }
};

class ProxyDataObserver {
public:
    explicit ProxyDataObserver(const ProxyDataCallback &callback);
    void OnChange(std::vector<DataProxyChangeInfo> &changeNode);
    bool operator==(const ProxyDataObserver &rhs) const;
    bool operator!=(const ProxyDataObserver &rhs) const;

private:
    ProxyDataCallback callback_;
};

class ProxyDataSubscriberManager : public CallbacksManager<ProxyDataObserverMapKey, ProxyDataObserver> {
public:
    using Key = ProxyDataObserverMapKey;
    using Observer = ProxyDataObserver;
    using BaseCallbacks = CallbacksManager<ProxyDataObserverMapKey, ProxyDataObserver>;
    static ProxyDataSubscriberManager &GetInstance();

    std::vector<DataProxyResult> AddObservers(void *subscriber, std::shared_ptr<DataShareServiceProxy> proxy,
        const std::vector<std::string> &uris, const ProxyDataCallback &callback);
    std::vector<DataProxyResult> DelObservers(void *subscriber, std::shared_ptr<DataShareServiceProxy> proxy,
        const std::vector<std::string> &uris);
    std::vector<DataProxyResult> DelObservers(void *subscriber, std::shared_ptr<DataShareServiceProxy> proxy);

    void RecoverObservers(std::shared_ptr<DataShareServiceProxy> proxy);
    void Emit(std::vector<DataProxyChangeInfo> &changeInfo);

private:
    void Emit(const std::vector<Key> &keys, const std::shared_ptr<Observer> &observer);
    ProxyDataSubscriberManager();
    std::map<Key, std::vector<Observer>> callbacks_;
    sptr<ProxyDataObserverStub> serviceCallback_;
    std::recursive_mutex mutex_{};
};
} // namespace DataShare
} // namespace OHOS
#endif //PROXY_DATA_SUBSCRIBER_MANAGER_H
