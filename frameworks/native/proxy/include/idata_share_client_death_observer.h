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

#ifndef I_DATA_SHARE_CLIENT_DEATH_OBSERVER_H
#define I_DATA_SHARE_CLIENT_DEATH_OBSERVER_H

#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS {
namespace DataShare {
class IDataShareClientDeathObserver : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DataShare.IDataShareClientDeathObserver");
};

class DataShareClientDeathObserverStub : public IRemoteStub<IDataShareClientDeathObserver> {
public:
    DataShareClientDeathObserverStub();

    virtual ~DataShareClientDeathObserverStub();
};

class DataShareClientDeathObserverProxy : public IRemoteProxy<IDataShareClientDeathObserver> {
public:
    explicit DataShareClientDeathObserverProxy(const sptr<IRemoteObject> &impl)
        : IRemoteProxy<IDataShareClientDeathObserver>(impl){};
    ~DataShareClientDeathObserverProxy() = default;

private:
    static inline BrokerDelegator<DataShareClientDeathObserverProxy> delegator_;
};
} // namespace DataShare
} // namespace OHOS

#endif // I_DATA_SHARE_CLIENT_DEATH_OBSERVER_H
