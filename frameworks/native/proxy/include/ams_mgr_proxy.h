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

#ifndef DATA_PROXY_AMS_PROXY_H
#define DATA_PROXY_AMS_PROXY_H

#include <memory>
#include <string>

#include "extension_manager_proxy.h"
namespace OHOS::DataShare {
class AmsMgrProxy final : public std::enable_shared_from_this<AmsMgrProxy> {
public:
    ~AmsMgrProxy();
    static std::shared_ptr<AmsMgrProxy> GetInstance();
    int Connect(const std::string &uri, const sptr<IRemoteObject> &connect, const sptr<IRemoteObject> &callerToken);
    int DisConnect(sptr<IRemoteObject> connect);
private:
    using Proxy = AAFwk::ExtensionManagerProxy;
    AmsMgrProxy() = default;
    class ServiceDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit ServiceDeathRecipient(std::weak_ptr<AmsMgrProxy> owner) : owner_(owner)
        {
        }
        void OnRemoteDied(const wptr<IRemoteObject> &object) override
        {
            auto owner = owner_.lock();
            if (owner != nullptr) {
                owner->OnProxyDied();
            }
        }

    private:
        std::weak_ptr<AmsMgrProxy> owner_;
    };
    void OnProxyDied();
    bool ConnectSA();
    std::mutex mutex_;
    sptr<IRemoteObject> sa_;
    sptr<Proxy> proxy_;
    sptr<AmsMgrProxy::ServiceDeathRecipient> deathRecipient_;
};
} // namespace OHOS::DataShare
#endif // DATASHARESERVICE_BUNDLEMGR_PROXY_H