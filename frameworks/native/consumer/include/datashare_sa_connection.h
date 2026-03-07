/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef DATASHARE_SA_CONNECTION_H
#define DATASHARE_SA_CONNECTION_H

#include "datashare_common.h"
#include "datashare_connection_base.h"
#include "datashare_sa_provider_info.h"
#include "if_local_ability_manager.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_load_callback_stub.h"

namespace OHOS {
namespace DataShare {
struct ConnectResult {
    bool isFinish_;
    int errorCode_;
    ConnectionInterfaceInfo interfaceInfo_;
    sptr<IRemoteObject> proxy_;

    explicit ConnectResult(bool isFinish, int errorCode, ConnectionInterfaceInfo info,
        sptr<IRemoteObject> proxy) : isFinish_(isFinish), errorCode_(errorCode),
        interfaceInfo_(info), proxy_(proxy) {}
};
class DataShareSAConnection : public DataShareConnectionBase,
                              public std::enable_shared_from_this<DataShareSAConnection> {
public:
    DataShareSAConnection(const Uri &uri, int32_t saId, int32_t waitTime = 2);
    ~DataShareSAConnection() override;

    /**
     * @brief get the proxy of datashare provider.
     *
     * @return the proxy of datashare provider.
     */
    std::shared_ptr<DataShareProxy> GetDataShareProxy(const Uri &uri, const sptr<IRemoteObject> &token) override;
    void UpdateObserverExtsProviderMap(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver,
        bool isDescendants) override;
    void DeleteObserverExtsProviderMap(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;
    void OnRemoteDied();
    class SALoadCallback : public SystemAbilityLoadCallbackStub {
    public:
        void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject) override;
        void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;

    public:
        std::mutex mutex_{};
        std::condition_variable proxyConVar_;
        std::atomic<bool> isLoadSuccess_ = {false};
        sptr<IRemoteObject> remoteObject_ = nullptr;
    };
private:
    class ServiceDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit ServiceDeathRecipient(std::weak_ptr<DataShareSAConnection> owner) : owner_(owner) {}
        void OnRemoteDied(const wptr<IRemoteObject> &object) override
        {
            auto owner = owner_.lock();
            if (owner != nullptr) {
                owner->OnRemoteDied();
            }
        }
    private:
        std::weak_ptr<DataShareSAConnection> owner_;
    };
    static ConnectResult GetConnectResult(const Uri &uri, ConnectionInterfaceInfo interfaceInfo, int32_t saId,
        uint32_t waitTime);
    static sptr<IRemoteObject> ConnectToProvider(const Uri &uri,
        ConnectionInterfaceInfo interfaceInfo, int32_t saId, uint32_t waitTime);
    static std::pair<int32_t, ConnectionInterfaceInfo> GetConnectionInterfaceInfo(int32_t saId, uint32_t waitTime);
    static sptr<IRemoteObject> CheckAndLoadSystemAbility(int32_t systemAbilityId, uint32_t waitTime);
    std::shared_ptr<DataShareProxy> GetDataShareProxy();
    bool LinkToDeath(const sptr<IRemoteObject> remote);
    static constexpr int32_t INVALID_SA_ID = 0;
    Uri uri_;
    int32_t saId_ = INVALID_SA_ID;
    uint32_t waitTime_ = 0;
    ConnectionInterfaceInfo interfaceInfo_ = ConnectionInterfaceInfo();
    std::mutex mutex_{};
    std::shared_ptr<DataShareProxy> dataShareProxy_ = nullptr;
    std::shared_ptr<ExecutorPool> pool_ = nullptr;
};
}  // namespace DataShare
}  // namespace OHOS
#endif  // DATASHARE_SA_CONNECTION_H