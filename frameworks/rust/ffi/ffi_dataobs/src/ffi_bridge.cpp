// Copyright (c) 2026 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ffi_dataobs_bridge.h"
#include "wrapper.rs.h"
#include "dataobs_mgr_client.h"
#include "dataobs_mgr_changeinfo.h"
#include "data_ability_observer_stub.h"
#include "uri.h"

#include <map>
#include <mutex>

namespace ffi_dataobs {

class CppDataAbilityObserverImpl : public OHOS::AAFwk::DataAbilityObserverStub {
public:
    explicit CppDataAbilityObserverImpl(rust::Box<DataObsCallback> callback)
        : callback_(std::move(callback)) {}

    void OnChange() override
    {
        callback_->on_change();
    }

    void OnChangeExt(const OHOS::AAFwk::ChangeInfo &info) override
    {
        FfiChangeInfo ffiInfo;
        ffiInfo.change_type = static_cast<ChangeType>(info.changeType_);
        if (!info.uris_.empty()) {
            ffiInfo.uri = rust::String(info.uris_.front().ToString());
        }
        callback_->on_change_ext(ffiInfo);
    }

private:
    rust::Box<DataObsCallback> callback_;
};

struct ObserverEntry {
    OHOS::sptr<OHOS::AAFwk::IDataAbilityObserver> observer;
    std::string uri;
    int32_t userId;
};

static std::mutex g_mutex;
static std::map<int64_t, ObserverEntry> g_observers;
static int64_t g_nextHandle = 1;

int64_t register_observer(rust::Str uri, rust::Box<DataObsCallback> callback, int32_t user_id)
{
    OHOS::sptr<OHOS::AAFwk::IDataAbilityObserver> observer =
        new CppDataAbilityObserverImpl(std::move(callback));
    std::string uriStr(uri);
    OHOS::Uri uriObj{uriStr};
    auto client = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (client == nullptr) {
        return -1;
    }
    auto err = client->RegisterObserver(uriObj, observer, user_id);
    if (err != 0) {
        return -static_cast<int64_t>(err);
    }

    std::lock_guard<std::mutex> lock(g_mutex);
    int64_t handle = g_nextHandle++;
    g_observers[handle] = {observer, uriStr, user_id};
    return handle;
}

int32_t unregister_observer(int64_t handle)
{
    ObserverEntry entry;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_observers.find(handle);
        if (it == g_observers.end()) {
            return -1;
        }
        entry = std::move(it->second);
        g_observers.erase(it);
    }
    OHOS::Uri uriObj{entry.uri};
    auto client = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (client == nullptr) {
        return -1;
    }
    return client->UnregisterObserver(uriObj, entry.observer, entry.userId);
}

int32_t notify_change(rust::Str uri, int32_t user_id)
{
    OHOS::Uri uriObj{std::string(uri)};
    auto client = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (client == nullptr) {
        return -1;
    }
    return client->NotifyChange(uriObj, user_id);
}

int64_t register_observer_ext(
    rust::Str uri, rust::Box<DataObsCallback> callback, bool is_descendants)
{
    OHOS::sptr<OHOS::AAFwk::IDataAbilityObserver> observer =
        new CppDataAbilityObserverImpl(std::move(callback));
    std::string uriStr(uri);
    OHOS::Uri uriObj{uriStr};
    auto client = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (client == nullptr) {
        return -1;
    }
    auto err = client->RegisterObserverExt(uriObj, observer, is_descendants);
    if (err != 0) {
        return -static_cast<int64_t>(err);
    }

    std::lock_guard<std::mutex> lock(g_mutex);
    int64_t handle = g_nextHandle++;
    g_observers[handle] = {observer, uriStr, 0};
    return handle;
}

int32_t unregister_observer_ext(int64_t handle)
{
    ObserverEntry entry;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_observers.find(handle);
        if (it == g_observers.end()) {
            return -1;
        }
        entry = std::move(it->second);
        g_observers.erase(it);
    }
    OHOS::Uri uriObj{entry.uri};
    auto client = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (client == nullptr) {
        return -1;
    }
    return client->UnregisterObserverExt(uriObj, entry.observer);
}

int32_t notify_change_ext(uint32_t change_type, rust::Str uri)
{
    OHOS::AAFwk::ChangeInfo changeInfo;
    changeInfo.changeType_ = static_cast<OHOS::AAFwk::ChangeInfo::ChangeType>(change_type);
    changeInfo.uris_.emplace_back(std::string(uri));
    auto client = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (client == nullptr) {
        return -1;
    }
    return client->NotifyChangeExt(changeInfo);
}

} // namespace ffi_dataobs
