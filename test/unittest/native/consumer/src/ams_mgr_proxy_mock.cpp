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

#include "ams_mgr_proxy.h"
#include "ams_mgr_proxy_mock.h"

#include <atomic>
#include <mutex>

namespace OHOS {
namespace DataShare {
namespace {
std::atomic<int> g_mockConnectCount { 0 };
std::atomic<int> g_mockDisConnectCount { 0 };
std::atomic<int> g_mockConnectResult { 0 };
std::atomic<bool> g_mockGetInstanceNull { false };
} // namespace

int AmsMgrProxyMockGetConnectCount()
{
    return g_mockConnectCount.load();
}

int AmsMgrProxyMockGetDisConnectCount()
{
    return g_mockDisConnectCount.load();
}

void AmsMgrProxyMockSetConnectResult(int result)
{
    g_mockConnectResult = result;
}

void AmsMgrProxyMockSetGetInstanceNull(bool isNull)
{
    g_mockGetInstanceNull = isNull;
}

void AmsMgrProxyMockReset()
{
    g_mockConnectCount = 0;
    g_mockDisConnectCount = 0;
    g_mockConnectResult = 0;
    g_mockGetInstanceNull = false;
}

AmsMgrProxy *AmsMgrProxy::GetInstance()
{
    if (g_mockGetInstanceNull.load()) {
        return nullptr;
    }
    static AmsMgrProxy instance;
    return &instance;
}

int AmsMgrProxy::Connect(const std::string &serviceName, const sptr<IRemoteObject> &callback,
    const sptr<IRemoteObject> &token)
{
    g_mockConnectCount++;
    return g_mockConnectResult.load();
}

int AmsMgrProxy::DisConnect(sptr<IRemoteObject> callback)
{
    g_mockDisConnectCount++;
    return 0;
}

AmsMgrProxy::~AmsMgrProxy()
{
}

std::mutex AmsMgrProxy::pmutex_;
} // namespace DataShare
} // namespace OHOS
