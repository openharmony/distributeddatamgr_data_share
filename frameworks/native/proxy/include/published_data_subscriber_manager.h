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

#ifndef PUBLISHED_DATA_SUBSCRIBER_MANAGER_H
#define PUBLISHED_DATA_SUBSCRIBER_MANAGER_H
#include <memory>

#include "callbacks_manager.h"
#include "data_proxy_observer.h"
#include "data_proxy_observer_stub.h"
#include "datashare_template.h"
#include "idatashare.h"
#include "iremote_stub.h"

namespace OHOS {
namespace DataShare {
struct PublishedObserverMapKey {
    std::string uri_;
    int64_t subscriberId_;
    PublishedObserverMapKey(const std::string &uri, int64_t subscriberId)
        : uri_(uri), subscriberId_(subscriberId){};
    bool operator==(const PublishedObserverMapKey &node) const
    {
        return uri_ == node.uri_ && subscriberId_ == node.subscriberId_;
    }
    bool operator!=(const PublishedObserverMapKey &node) const
    {
        return !(node == *this);
    }
    bool operator<(const PublishedObserverMapKey &node) const
    {
        if (uri_ != node.uri_) {
            return uri_ < node.uri_;
        }
        return subscriberId_ < node.subscriberId_;
    }
    operator std::string () const
    {
        return uri_;
    }
};

class PublishedDataObserver {
public:
    explicit PublishedDataObserver(const PublishedDataCallback &callback);
    void OnChange(PublishedDataChangeNode &changeNode);
    bool operator==(const PublishedDataObserver &rhs) const;
    bool operator!=(const PublishedDataObserver &rhs) const;

private:
    PublishedDataCallback callback_;
};

class PublishedDataSubscriberManager : public CallbacksManager<PublishedObserverMapKey, PublishedDataObserver>  {
public:
    using Key = PublishedObserverMapKey;
    using Observer = PublishedDataObserver;
    using BaseCallbacks = CallbacksManager<PublishedObserverMapKey, PublishedDataObserver>;
    PublishedDataSubscriberManager();
    std::vector<OperationResult> AddObservers(std::shared_ptr<BaseProxy> proxy,
        const std::vector<std::string> &uris, int64_t subscriberId, const PublishedDataCallback &callback);
    std::vector<OperationResult> DelObservers(std::shared_ptr<BaseProxy> proxy, const std::vector<std::string> &uris,
        int64_t subscriberId);
    std::vector<OperationResult> EnableObservers(std::shared_ptr<BaseProxy> proxy,
        const std::vector<std::string> &uris, int64_t subscriberId);
    std::vector<OperationResult> DisableObservers(std::shared_ptr<BaseProxy> proxy,
        const std::vector<std::string> &uris, int64_t subscriberId);
    void DelAllObservers(std::shared_ptr<BaseProxy> proxy);
    void Emit(PublishedDataChangeNode &changeNode);

private:
    bool Init();
    void Destroy();
    sptr<PublishedDataObserverStub> serviceCallback_;
};
} // namespace DataShare
} // namespace OHOS
#endif //PUBLISHED_DATA_SUBSCRIBER_MANAGER_H
