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

#ifndef RDB_SUBSCRIBER_MANAGER_H
#define RDB_SUBSCRIBER_MANAGER_H

#include <memory>

#include "callbacks_manager.h"
#include "data_proxy_observer.h"
#include "data_proxy_observer_stub.h"
#include "datashare_template.h"
#include "idatashare.h"
#include "iremote_stub.h"
#include "data_share_service_proxy.h"

namespace OHOS {
namespace DataShare {
struct RdbObserverMapKey {
    std::string uri_;
    std::string clearUri_;
    TemplateId templateId_;
    RdbObserverMapKey(const std::string &uri, const TemplateId &templateId) : uri_(uri), templateId_(templateId)
    {
        auto pos = uri_.find_first_of('?');
        if (pos != std::string::npos) {
            clearUri_ = uri_.substr(0, pos);
        } else {
            clearUri_ = uri_;
        }
    }
    bool operator==(const RdbObserverMapKey &node) const
    {
        return clearUri_ == node.clearUri_ && templateId_ == node.templateId_;
    }
    bool operator!=(const RdbObserverMapKey &node) const
    {
        return !(node == *this);
    }
    bool operator<(const RdbObserverMapKey &node) const
    {
        if (clearUri_ != node.clearUri_) {
            return clearUri_ < node.clearUri_;
        }
        return templateId_ < node.templateId_;
    }
    operator std::string() const
    {
        return uri_;
    }
};

class RdbObserver {
public:
    RdbObserver(const RdbCallback &callback);
    void OnChange(const RdbChangeNode &changeNode);
    bool operator==(const RdbObserver &rhs) const;
    bool operator!=(const RdbObserver &rhs) const;

private:
    RdbCallback callback_;
};

class RdbSubscriberManager : public CallbacksManager<RdbObserverMapKey, RdbObserver> {
public:
    using Key = RdbObserverMapKey;
    using Observer = RdbObserver;
    using BaseCallbacks = CallbacksManager<RdbObserverMapKey, RdbObserver>;
    static RdbSubscriberManager &GetInstance();

    std::vector<OperationResult> AddObservers(void *subscriber, std::shared_ptr<DataShareServiceProxy> proxy,
        const std::vector<std::string> &uris, const TemplateId &templateId, const RdbCallback &callback);
    std::vector<OperationResult> DelObservers(void *subscriber, std::shared_ptr<DataShareServiceProxy> proxy,
        const std::vector<std::string> &uris, const TemplateId &templateId);
    std::vector<OperationResult> DelObservers(void *subscriber, std::shared_ptr<DataShareServiceProxy> proxy);
    std::vector<OperationResult> EnableObservers(void *subscriber, std::shared_ptr<DataShareServiceProxy> proxy,
        const std::vector<std::string> &uris, const TemplateId &templateId);
    std::vector<OperationResult> DisableObservers(void *subscriber, std::shared_ptr<DataShareServiceProxy> proxy,
        const std::vector<std::string> &uris, const TemplateId &templateId);
    void RecoverObservers(std::shared_ptr<DataShareServiceProxy> proxy);
    void Emit(const RdbChangeNode &changeNode);

private:
    void Emit(const std::vector<Key> &keys, const std::shared_ptr<Observer> &observer);
    void EmitOnEnable(std::map<Key, std::vector<ObserverNodeOnEnabled>> &obsMap);
    RdbSubscriberManager();
    bool Init();
    void Destroy();
    sptr<RdbObserverStub> serviceCallback_;
    std::map<Key, RdbChangeNode> lastChangeNodeMap_;
};
} // namespace DataShare
} // namespace OHOS
#endif // RDB_SUBSCRIBER_MANAGER_H
