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

#ifndef DATA_PROXY_OBSERVER_STUB_H
#define DATA_PROXY_OBSERVER_STUB_H

#include "data_proxy_observer.h"
#include "datashare_template.h"
#include "iremote_stub.h"

namespace OHOS {
namespace DataShare {
using RdbCallback = std::function<void(const RdbChangeNode &changeNode)>;
class RdbObserverStub : public IRemoteStub<IDataProxyRdbObserver> {
public:
    RdbObserverStub(RdbCallback callback);
    virtual ~RdbObserverStub();
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    void OnChangeFromRdb(const RdbChangeNode &changeNode);
    void ClearCallback();
private:
    std::mutex mutex_;
    RdbCallback callback_;
};

using PublishedDataCallback = std::function<void(PublishedDataChangeNode &changeNode)>;
class PublishedDataObserverStub : public IRemoteStub<IDataProxyPublishedDataObserver> {
public:
    virtual ~PublishedDataObserverStub();
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    explicit PublishedDataObserverStub(PublishedDataCallback callback) : callback_(callback){};
    void OnChangeFromPublishedData(PublishedDataChangeNode &changeNode);
    void ClearCallback();
private:
    std::mutex mutex_;
    PublishedDataCallback callback_;
};
} // namespace DataShare
} // namespace OHOS
#endif //DATA_PROXY_OBSERVER_STUB_H
