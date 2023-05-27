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

#ifndef DATASHARE_CONNECTION_H
#define DATASHARE_CONNECTION_H

#include <memory>
#include <condition_variable>

#include "ability_connect_callback_stub.h"
#include "event_handler.h"
#include "idatashare.h"
#include "want.h"

namespace OHOS {
namespace DataShare {
using namespace AppExecFwk;
class DataShareConnection : public AAFwk::AbilityConnectionStub {
public:
    DataShareConnection(const Uri &uri) : uri_(uri) {}
    virtual ~DataShareConnection();

    /**
     * @brief This method is called back to receive the connection result after an ability calls the
     * ConnectAbility method to connect it to an extension ability.
     *
     * @param element: Indicates information about the connected extension ability.
     * @param remote: Indicates the remote proxy object of the extension ability.
     * @param resultCode: Indicates the connection result code. The value 0 indicates a successful connection, and any
     * other value indicates a connection failure.
     */
    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override;

    /**
     * @brief This method is called back to receive the disconnection result after the connected extension ability
     * crashes or is killed. If the extension ability exits unexpectedly, all its connections are disconnected, and
     * each ability previously connected to it will call onAbilityDisconnectDone.
     *
     * @param element: Indicates information about the disconnected extension ability.
     * @param resultCode: Indicates the disconnection result code. The value 0 indicates a successful disconnection,
     * and any other value indicates a disconnection failure.
     */
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;

    /**
     * @brief connect remote ability of DataShareExtAbility.
     */
    bool ConnectDataShareExtAbility(const Uri &uri, const sptr<IRemoteObject> &token);

    /**
     * @brief get the proxy of datashare extension ability.
     *
     * @return the proxy of datashare extension ability.
     */
    sptr<IDataShare> GetDataShareProxy();

    struct ConnectCondition {
        std::condition_variable condition;
        std::mutex mutex;
    };

    /**
     * @brief disconnect remote ability of DataShareExtAbility.
     */
    void DisconnectDataShareExtAbility();

private:

    void SetDataShareProxy(sptr<IDataShare> proxy);
    bool IsExtAbilityConnected();

    static sptr<DataShareConnection> instance_;
    std::mutex mutex_;
    sptr<IDataShare> dataShareProxy_;
    ConnectCondition condition_;
    Uri uri_;
};
}  // namespace DataShare
}  // namespace OHOS
#endif  // DATASHARE_CONNECTION_H