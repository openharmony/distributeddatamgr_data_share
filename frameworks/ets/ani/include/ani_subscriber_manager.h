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

#ifndef ANI_SUBSCRIBER_MANAGER_H
#define ANI_SUBSCRIBER_MANAGER_H

#include <memory>

#include "cxx.h"
#include "concurrent_map.h"
#include "ani_callbacks_manager.h"
#include "datashare_helper.h"
#include "ani_subscriber.h"
#include "dataproxy_handle.h"

namespace OHOS {
using namespace DataShare;
namespace DataShareAni {
struct DataShareCallback;
struct AniRdbObserverMapKey {
    std::string uri_;
    DataShare::TemplateId templateId_;
    AniRdbObserverMapKey(const std::string &uri,
        const DataShare::TemplateId &templateId) : uri_(uri), templateId_(templateId) {};
    bool operator==(const AniRdbObserverMapKey &node) const
    {
        return uri_ == node.uri_ && templateId_ == node.templateId_;
    }
    bool operator!=(const AniRdbObserverMapKey &node) const
    {
        return !(node == *this);
    }
    bool operator<(const AniRdbObserverMapKey &node) const
    {
        if (uri_ != node.uri_) {
            return uri_ < node.uri_;
        }
        return templateId_ < node.templateId_;
    }
    operator std::string() const
    {
        return uri_;
    }
};

class AniRdbSubscriberManager : public OHOS::DataShareAni::AniCallbacksManager<AniRdbObserverMapKey, AniRdbObserver> {
public:
    using Key = AniRdbObserverMapKey;
    using Observer = AniRdbObserver;
    using AniBaseCallbacks = OHOS::DataShareAni::AniCallbacksManager<AniRdbObserverMapKey, AniRdbObserver>;
    explicit AniRdbSubscriberManager(std::shared_ptr<DataShareHelper> dataShareHelperPtr)
    {
        dataShareHelper_ = std::weak_ptr<DataShareHelper>(dataShareHelperPtr);
    }
    std::vector<OperationResult> AddObservers(rust::Box<DataShareCallback> &callback,
        const std::vector<std::string> &uris, const DataShare::TemplateId &templateId);
    std::vector<OperationResult> DelObservers(rust::Box<DataShareCallback> &callback,
        const std::vector<std::string> &uris, const DataShare::TemplateId &templateId);
    std::vector<OperationResult> DelObservers(const std::vector<std::string> &uris,
        const DataShare::TemplateId &templateId);
    void Emit(const RdbChangeNode &changeNode);

private:
    void Emit(const std::vector<Key> &keys, const std::shared_ptr<Observer> &observer);
    std::weak_ptr<DataShareHelper> dataShareHelper_;
    ConcurrentMap<Key, RdbChangeNode> lastChangeNodeMap_;
};

struct AniPublishedObserverMapKey {
    std::string uri_;
    int64_t subscriberId_;
    AniPublishedObserverMapKey(const std::string &uri, int64_t subscriberId) : uri_(uri),
        subscriberId_(subscriberId) {};
    bool operator==(const AniPublishedObserverMapKey &node) const
    {
        return uri_ == node.uri_ && subscriberId_ == node.subscriberId_;
    }
    bool operator!=(const AniPublishedObserverMapKey &node) const
    {
        return !(node == *this);
    }
    bool operator<(const AniPublishedObserverMapKey &node) const
    {
        if (uri_ != node.uri_) {
            return uri_ < node.uri_;
        }
        return subscriberId_ < node.subscriberId_;
    }
    operator std::string() const
    {
        return uri_;
    }
};

class AniPublishedSubscriberManager
    : public OHOS::DataShareAni::AniCallbacksManager<AniPublishedObserverMapKey, AniPublishedObserver> {
public:
    using Key = AniPublishedObserverMapKey;
    using Observer = AniPublishedObserver;
    using AniBaseCallbacks = OHOS::DataShareAni::AniCallbacksManager<AniPublishedObserverMapKey, AniPublishedObserver>;
    explicit AniPublishedSubscriberManager(std::shared_ptr<DataShareHelper> dataShareHelperPtr)
    {
        dataShareHelper_ = std::weak_ptr<DataShareHelper>(dataShareHelperPtr);
    }
    std::vector<OperationResult> AddObservers(rust::Box<DataShareCallback> &callback,
        const std::vector<std::string> &uris, int64_t subscriberId);
    std::vector<OperationResult> DelObservers(rust::Box<DataShareCallback> &callback,
        const std::vector<std::string> &uris, int64_t subscriberId);
    std::vector<OperationResult> DelObservers(const std::vector<std::string> &uris, int64_t subscriberId);
    void Emit(const DataShare::PublishedDataChangeNode &changeNode);

private:
    void Emit(const std::vector<Key> &keys, const std::shared_ptr<Observer> &observer);
    std::weak_ptr<DataShareHelper> dataShareHelper_;
    ConcurrentMap<Key, DataShare::PublishedDataChangeNode> lastChangeNodeMap_;
};

struct AniProxyDataObserverMapKey {
    std::string uri_;
    AniProxyDataObserverMapKey(const std::string &uri) : uri_(uri) {};
    bool operator==(const AniProxyDataObserverMapKey &node) const
    {
        return uri_ == node.uri_;
    }
    bool operator!=(const AniProxyDataObserverMapKey &node) const
    {
        return !(node == *this);
    }
    bool operator<(const AniProxyDataObserverMapKey &node) const
    {
        return uri_ < node.uri_;
    }
    operator std::string() const
    {
        return uri_;
    }
};

class AniProxyDataSubscriberManager
    : public OHOS::DataShareAni::AniCallbacksManager<AniProxyDataObserverMapKey, AniProxyDataObserver> {
public:
    using Key = AniProxyDataObserverMapKey;
    using Observer = AniProxyDataObserver;
    using AniBaseCallbacks = OHOS::DataShareAni::AniCallbacksManager<AniProxyDataObserverMapKey, AniProxyDataObserver>;
    explicit AniProxyDataSubscriberManager(std::weak_ptr<DataProxyHandle> dataProxyHandle)
        : dataProxyHandle_(dataProxyHandle){};
    std::vector<DataProxyResult> AddObservers(
        rust::Box<DataShareCallback> &callback, const std::vector<std::string> &uris);
    std::vector<DataProxyResult> DelObservers(
        rust::Box<DataShareCallback> &callback, const std::vector<std::string> &uris);
    std::vector<DataProxyResult> DelObservers(const std::vector<std::string> &uris);
    void Emit(const std::vector<DataProxyChangeInfo> &changeNode);

private:
    std::weak_ptr<DataProxyHandle> dataProxyHandle_;
};
} // namespace DataShareAni
} // namespace OHOS
#endif // ANI_SUBSCRIBER_MANAGER_H
