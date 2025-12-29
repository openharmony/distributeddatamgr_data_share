/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#define LOG_TAG "mock_token"

#include <gtest/gtest.h>
#include "datashare_log.h"
#include "mock_token.h"
#include "token_setproc.h"

namespace OHOS {
namespace DataShare {
using namespace OHOS::DataShare;
using namespace OHOS::Security::AccessToken;

std::mutex g_lockSetToken;
uint64_t g_shellTokenId = 0;
const uint64_t SHELL_TOKEN_ID = 0;
const std::string FOUNDATION_PROC_NAME = "foundation";
// An arbitrary err code, avoding repeating with others' err
const int ERR = -1001001;

void MockToken::SetTestEnvironment()
{
    std::lock_guard<std::mutex> lock(g_lockSetToken);
    g_shellTokenId = GetSelfTokenID();
}

void MockToken::ResetTestEnvironment()
{
    std::lock_guard<std::mutex> lock(g_lockSetToken);
    // Shell token id is 0
    g_shellTokenId = SHELL_TOKEN_ID;
}

uint64_t GetShellTokenID()
{
    std::lock_guard<std::mutex> lock(g_lockSetToken);
    return g_shellTokenId;
}

MockNativeToken::MockNativeToken(const std::string& process)
{
    selfToken_ = GetSelfTokenID();
    uint32_t tokenId = MockToken::GetTokenByProcName(process);
    int ret = SetSelfTokenID(tokenId);
    // Should be 0 (ok)
    LOG_INFO("MockNativeToken SetSelfTokenID: %{public}d", ret);
}

MockNativeToken::~MockNativeToken()
{
    // Restore back to original token
    int ret = SetSelfTokenID(selfToken_);
    // Should be 0 (ok)
    LOG_INFO("~MockNativeToken SetSelfTokenID: %{public}d", ret);
}

// Adapted from PrivacyTestCommon::GetNativeTokenIdFromProcess. Rename according to the purpose.
AccessTokenID MockToken::GetTokenByProcName(const std::string& process)
{
    uint64_t selfTokenId = GetSelfTokenID();
    // Set shell token
    int ret = SetSelfTokenID(GetShellTokenID());
    // Should be 0 (ok)
    LOG_INFO("GetTokenByProcName SetSelfTokenID: %{public}d", ret);

    std::string dumpInfo;
    AtmToolsParamInfo info;
    info.processName = process;
    AccessTokenKit::DumpTokenInfo(info, dumpInfo);
    size_t pos = dumpInfo.find("\"tokenID\": ");
    if (pos == std::string::npos) {
        return 0;
    }
    pos += std::string("\"tokenID\": ").length();
    std::string numStr;
    while (pos < dumpInfo.length() && std::isdigit(dumpInfo[pos])) {
        numStr += dumpInfo[pos];
        ++pos;
    }
    // Restore
    ret = SetSelfTokenID(selfTokenId);
    // Should be 0 (ok)
    LOG_INFO("GetTokenByProcName restore SetSelfTokenID: %{public}d", ret);

    std::istringstream iss(numStr);
    AccessTokenID tokenID;
    iss >> tokenID;
    return tokenID;
}

AccessTokenIDEx MockToken::AllocTestHapToken(const HapInfoParams& hapInfo, HapPolicyParams& hapPolicy)
{
    // Output
    AccessTokenIDEx tokenIdEx = {0};
    uint64_t selfTokenId = GetSelfTokenID();
    for (auto& permissionStateFull : hapPolicy.permStateList) {
        PermissionDef permDefResult;
        if (AccessTokenKit::GetDefPermission(permissionStateFull.permissionName, permDefResult) != RET_SUCCESS) {
            continue;
        }
        if (permDefResult.availableLevel > hapPolicy.apl) {
            hapPolicy.aclRequestedList.emplace_back(permissionStateFull.permissionName);
        }
    }
    // Only **foundation** can alloc permission
    int ret = ERR;
    if (MockToken::GetTokenByProcName(FOUNDATION_PROC_NAME) == selfTokenId) {
        ret = AccessTokenKit::InitHapToken(hapInfo, hapPolicy, tokenIdEx);
        LOG_INFO("AllocTestHapToken InitHapToken: %{public}d", ret);
    } else {
        // Set sh token for self
        MockNativeToken mock(FOUNDATION_PROC_NAME);
        ret = AccessTokenKit::InitHapToken(hapInfo, hapPolicy, tokenIdEx);
        LOG_INFO("AllocTestHapToken Not foundation. InitHapToken: %{public}d", ret);

        // Restore. No influence on permissions because they have been alloced
        ret = SetSelfTokenID(selfTokenId);
        // Should be 0 (ok)
        LOG_INFO("AllocTestHapToken SetSelfTokenID: %{public}d", ret);
    }
    return tokenIdEx;
}

int32_t MockToken::DeleteTestHapToken(AccessTokenID tokenID)
{
    uint64_t selfTokenId = GetSelfTokenID();
    if (MockToken::GetTokenByProcName(FOUNDATION_PROC_NAME) == selfTokenId) {
        return AccessTokenKit::DeleteToken(tokenID);
    }

    // Set sh token for self
    MockNativeToken mock(FOUNDATION_PROC_NAME);

    int32_t ret = AccessTokenKit::DeleteToken(tokenID);
    LOG_INFO("MockToken DeleteTestHapToken: %{public}d", ret);
    // Restore
    LOG_INFO("DeleteTestHapToken SetSelfTokenID: %{public}d", SetSelfTokenID(selfTokenId));
    return ret;
}
} // namespace DataShare
} // namespace OHOS