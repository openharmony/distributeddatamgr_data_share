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
#include "ams_mgr_proxy.h"

#include "datashare_log.h"
#include "datashare_string_utils.h"
#include "extension_ability_info.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "want.h"

namespace OHOS::DataShare {
std::mutex AmsMgrProxy::pmutex_;

void AmsMgrProxy::OnProxyDied()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (sa_ != nullptr) {
        sa_->RemoveDeathRecipient(deathRecipient_);
    }
    deathRecipient_ = nullptr;
    proxy_ = nullptr;
}

AmsMgrProxy::~AmsMgrProxy()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (sa_ != nullptr) {
        sa_->RemoveDeathRecipient(deathRecipient_);
    }
}

AmsMgrProxy* AmsMgrProxy::GetInstance()
{
    static AmsMgrProxy* proxy = nullptr;
    if (proxy != nullptr) {
        return proxy;
    }
    std::lock_guard<std::mutex> lock(pmutex_);
    if (proxy != nullptr) {
        return proxy;
    }
    proxy = new AmsMgrProxy();
    if (proxy == nullptr) {
        LOG_ERROR("new proxy failed");
    }
    return proxy;
}

__attribute__ ((no_sanitize("cfi"))) int AmsMgrProxy::Connect(
    const std::string &uri, const sptr<IRemoteObject> &connect, const sptr<IRemoteObject> &callerToken)
{
    AAFwk::Want want;
    want.SetUri(uri);
    std::lock_guard<std::mutex> lock(mutex_);
    if (ConnectSA()) {
        LOG_INFO("connect start, uri = %{public}s", DataShareStringUtils::Change(uri).c_str());
        return proxy_->ConnectAbilityCommon(want, connect, callerToken, AppExecFwk::ExtensionAbilityType::DATASHARE);
    }
    return -1;
}

bool AmsMgrProxy::ConnectSA()
{
    if (proxy_ != nullptr) {
        return true;
    }
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        LOG_ERROR("Failed to get system ability mgr.");
        return false;
    }

    sa_ = systemAbilityManager->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
    if (sa_ == nullptr) {
        LOG_ERROR("get ability manager service failed.");
        return false;
    }

    deathRecipient_ = new (std::nothrow) AmsMgrProxy::ServiceDeathRecipient(this);
    if (deathRecipient_ == nullptr) {
        LOG_ERROR("new death recipient failed.");
        return false;
    }
    sa_->AddDeathRecipient(deathRecipient_);
    proxy_ = new (std::nothrow)Proxy(sa_);
    if (proxy_ == nullptr) {
        LOG_ERROR("new proxy failed");
        return false;
    }
    return true;
}

int AmsMgrProxy::DisConnect(sptr<IRemoteObject> connect)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (ConnectSA()) {
        return proxy_->DisconnectAbility(connect);
    }
    return -1;
}
} // namespace OHOS::DataShare