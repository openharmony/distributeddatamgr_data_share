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

#ifndef NAPI_RDB_SUBSCRIBER_MANAGER_H
#define NAPI_RDB_SUBSCRIBER_MANAGER_H

#include <memory>
#include <uv.h>

#include "napi_callbacks_manager.h"
#include "datashare_helper.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "napi_observer.h"

namespace OHOS {
namespace DataShare {
struct NapiRdbObserverMapKey {
    std::string uri_;
    TemplateId templateId_;
    NapiRdbObserverMapKey(const std::string &uri, const TemplateId &templateId) : uri_(uri), templateId_(templateId){};
    bool operator==(const NapiRdbObserverMapKey &node) const
    {
        return uri_ == node.uri_ && templateId_ == node.templateId_;
    }
    bool operator!=(const NapiRdbObserverMapKey &node) const
    {
        return !(node == *this);
    }
    bool operator<(const NapiRdbObserverMapKey &node) const
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

class NapiRdbSubscriberManager : public NapiCallbacksManager<NapiRdbObserverMapKey, NapiRdbObserver> {
public:
    using Key = NapiRdbObserverMapKey;
    using Observer = NapiRdbObserver;
    using BaseCallbacks = NapiCallbacksManager<NapiRdbObserverMapKey, NapiRdbObserver>;
    explicit NapiRdbSubscriberManager(std::weak_ptr<DataShareHelper> dataShareHelper)
        : dataShareHelper_(dataShareHelper){};
    std::vector<OperationResult> AddObservers(napi_env env, napi_value callback, const std::vector<std::string> &uris,
        const TemplateId &templateId);
    std::vector<OperationResult> DelObservers(napi_env env, napi_value callback,
        const std::vector<std::string> &uris, const TemplateId &templateId);
    void Emit(const RdbChangeNode &changeNode);

private:
    void Emit(const std::vector<Key> &keys, const std::shared_ptr<Observer> &observer);
    std::weak_ptr<DataShareHelper> dataShareHelper_;
    std::map<Key, RdbChangeNode> lastChangeNodeMap_;
};

struct NapiPublishedObserverMapKey {
    std::string uri_;
    int64_t subscriberId_;
    NapiPublishedObserverMapKey(const std::string &uri, int64_t subscriberId) : uri_(uri),
        subscriberId_(subscriberId){};
    bool operator==(const NapiPublishedObserverMapKey &node) const
    {
        return uri_ == node.uri_ && subscriberId_ == node.subscriberId_;
    }
    bool operator!=(const NapiPublishedObserverMapKey &node) const
    {
        return !(node == *this);
    }
    bool operator<(const NapiPublishedObserverMapKey &node) const
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

class NapiPublishedSubscriberManager : public NapiCallbacksManager<NapiPublishedObserverMapKey, NapiPublishedObserver> {
public:
    using Key = NapiPublishedObserverMapKey;
    using Observer = NapiPublishedObserver;
    using BaseCallbacks = NapiCallbacksManager<NapiPublishedObserverMapKey, NapiPublishedObserver>;
    explicit NapiPublishedSubscriberManager(std::weak_ptr<DataShareHelper> dataShareHelper)
        : dataShareHelper_(dataShareHelper){};
    std::vector<OperationResult> AddObservers(napi_env env, napi_value callback, const std::vector<std::string> &uris,
        int64_t subscriberId);
    std::vector<OperationResult> DelObservers(napi_env env, napi_value callback,
        const std::vector<std::string> &uris, int64_t subscriberId);
    void Emit(const PublishedDataChangeNode &changeNode);

private:
    void Emit(const std::vector<Key> &keys, const std::shared_ptr<Observer> &observer);
    std::weak_ptr<DataShareHelper> dataShareHelper_;
    std::map<Key, PublishedDataChangeNode> lastChangeNodeMap_;
};
} // namespace DataShare
} // namespace OHOS
#endif //NAPI_RDB_SUBSCRIBER_MANAGER_H
