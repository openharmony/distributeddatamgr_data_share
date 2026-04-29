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

#include "ffi_extension_connect_bridge.h"
#include "wrapper.rs.h"

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "ability_connect_callback_stub.h"
#include "executor_pool.h"
#include "extension_ability_info.h"
#include "extension_manager_proxy.h"
#include "if_system_ability_manager.h"
#include "int_wrapper.h"
#include "iservice_registry.h"
#include "string_wrapper.h"
#include "system_ability_definition.h"
#include "want.h"

extern "C" void ds_on_ability_connect_done(const char *bundle_name);
extern "C" void ds_on_ability_disconnect_done(const char *bundle_name);

namespace OHOS::DataShare {

namespace {

class ConnectCallback : public AAFwk::AbilityConnectionStub {
public:
    explicit ConnectCallback(std::string bundleName) : bundleName_(std::move(bundleName)) {}

    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &, const sptr<IRemoteObject> &, int) override
    {
        ds_on_ability_connect_done(bundleName_.c_str());
    }

    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &, int) override
    {
        ds_on_ability_disconnect_done(bundleName_.c_str());
    }

private:
    std::string bundleName_;
};

class ServiceDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    void OnRemoteDied(const wptr<IRemoteObject> &) override;
};

std::mutex g_mutex;
sptr<IRemoteObject> g_sa;
sptr<AAFwk::ExtensionManagerProxy> g_proxy;
sptr<IRemoteObject::DeathRecipient> g_death_recipient;
std::map<std::string, sptr<IRemoteObject>> g_callback_map;
std::shared_ptr<ExecutorPool> g_executor;

void ServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_sa != nullptr) {
        g_sa->RemoveDeathRecipient(g_death_recipient);
    }
    g_sa = nullptr;
    g_death_recipient = nullptr;
    g_proxy = nullptr;
}

bool ConnectSA()
{
    if (g_proxy != nullptr) {
        return true;
    }
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return false;
    }
    g_sa = samgr->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
    if (g_sa == nullptr) {
        return false;
    }
    g_death_recipient = new (std::nothrow) ServiceDeathRecipient();
    if (g_death_recipient == nullptr) {
        g_sa = nullptr;
        return false;
    }
    g_sa->AddDeathRecipient(g_death_recipient);
    g_proxy = new (std::nothrow) AAFwk::ExtensionManagerProxy(g_sa);
    if (g_proxy == nullptr) {
        g_sa->RemoveDeathRecipient(g_death_recipient);
        g_death_recipient = nullptr;
        g_sa = nullptr;
        return false;
    }
    return true;
}

void DisconnectInternal(const std::string &bundleName)
{
    sptr<IRemoteObject> callback;
    sptr<AAFwk::ExtensionManagerProxy> proxy;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_callback_map.find(bundleName);
        if (it == g_callback_map.end()) {
            return;
        }
        callback = it->second;
        g_callback_map.erase(it);
        proxy = g_proxy;
    }

    if (proxy != nullptr && callback != nullptr) {
        proxy->DisconnectAbility(callback);
    }
}

} // anonymous namespace

int32_t connect_extension(
    rust::Str uri, rust::Str bundle_name, int32_t user_id, size_t want_params_ptr)
{
    std::string bundleStr(bundle_name.data(), bundle_name.size());
    std::string uriStr(uri.data(), uri.size());

    sptr<ConnectCallback> callback = new (std::nothrow) ConnectCallback(bundleStr);
    if (callback == nullptr) {
        return -1;
    }
    sptr<IRemoteObject> remote = callback->AsObject();

    sptr<AAFwk::ExtensionManagerProxy> proxy;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_callback_map.count(bundleStr) > 0) {
            return -1;
        }
        g_callback_map.emplace(bundleStr, remote);
        if (!ConnectSA()) {
            g_callback_map.erase(bundleStr);
            return -1;
        }
        proxy = g_proxy;
    }

    AAFwk::Want want;
    want.SetUri(uriStr);
    if (want_params_ptr != 0) {
        auto *params = reinterpret_cast<AAFwk::WantParams *>(want_params_ptr);
        want.SetParams(*params);
    }

    int ret = proxy->ConnectAbilityCommon(
        want, remote, nullptr,
        AppExecFwk::ExtensionAbilityType::DATASHARE, user_id);
    if (ret != ERR_OK) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_callback_map.erase(bundleStr);
        return -1;
    }

    return 0;
}

int32_t disconnect_extension(rust::Str bundle_name)
{
    std::string bundleStr(bundle_name.data(), bundle_name.size());

    sptr<IRemoteObject> callback;
    sptr<AAFwk::ExtensionManagerProxy> proxy;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_callback_map.find(bundleStr);
        if (it == g_callback_map.end()) {
            return -1;
        }
        callback = it->second;
        g_callback_map.erase(it);
        proxy = g_proxy;
    }

    if (proxy != nullptr && callback != nullptr) {
        proxy->DisconnectAbility(callback);
    }
    return 0;
}

bool has_active_connection(rust::Str bundle_name)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    std::string bundleStr(bundle_name.data(), bundle_name.size());
    return g_callback_map.count(bundleStr) > 0;
}

void schedule_disconnect(rust::Str bundle_name, int32_t delay_secs)
{
    std::shared_ptr<ExecutorPool> executor;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        executor = g_executor;
    }
    if (executor == nullptr) {
        return;
    }
    std::string bundleStr(bundle_name.data(), bundle_name.size());
    executor->Schedule(std::chrono::seconds(delay_secs), [bundleStr]() {
        DisconnectInternal(bundleStr);
    });
}

void init_executor(size_t executor_ptr)
{
    auto ptr = reinterpret_cast<const std::shared_ptr<ExecutorPool> *>(executor_ptr);
    if (ptr == nullptr || *ptr == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    g_executor = *ptr;
}

size_t build_corruption_want_params(rust::Str bundle_name, rust::Str store_name)
{
    auto *params = new (std::nothrow) AAFwk::WantParams();
    if (params == nullptr) {
        return 0;
    }
    std::string bundleStr(bundle_name.data(), bundle_name.size());
    std::string storeStr(store_name.data(), store_name.size());
    params->SetParam("BundleName", AAFwk::String::Box(bundleStr));
    params->SetParam("StoreName", AAFwk::String::Box(storeStr));
    params->SetParam("StoreStatus", AAFwk::Integer::Box(1));
    return reinterpret_cast<size_t>(params);
}

void destroy_want_params(size_t ptr)
{
    delete reinterpret_cast<AAFwk::WantParams *>(ptr);
}

} // namespace OHOS::DataShare
