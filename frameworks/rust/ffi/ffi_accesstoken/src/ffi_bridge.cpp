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

// C++ bridge implementation for AccessToken FFI.

#include "ffi_accesstoken_bridge.h"
#include "wrapper.rs.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "tokenid_kit.h"
#include "hap_token_info.h"
#include "native_token_info.h"
#include "ipc_skeleton.h"

namespace OHOS::Security::AccessToken {

uint64_t get_calling_token_id()
{
    return static_cast<uint64_t>(IPCSkeleton::GetCallingTokenID());
}

uint64_t get_calling_process_token_id()
{
    // No separate process token API; return the same calling token ID.
    return static_cast<uint64_t>(IPCSkeleton::GetCallingTokenID());
}

int32_t verify_permission(uint64_t token_id, rust::Str permission_name)
{
    std::string perm(permission_name);
    AccessTokenID tokenId = static_cast<AccessTokenID>(token_id);
    return AccessTokenKit::VerifyAccessToken(tokenId, perm);
}

int32_t get_token_type(uint64_t token_id)
{
    AccessTokenID tokenId = static_cast<AccessTokenID>(token_id);
    return static_cast<int32_t>(AccessTokenKit::GetTokenType(tokenId));
}

rust::String get_bundle_name(uint64_t token_id)
{
    AccessTokenID tokenId = static_cast<AccessTokenID>(token_id);
    HapTokenInfo hapInfo;
    int ret = AccessTokenKit::GetHapTokenInfo(tokenId, hapInfo);
    if (ret != 0) {
        return rust::String("");
    }
    return rust::String(hapInfo.bundleName);
}

CallingInfo get_calling_info()
{
    CallingInfo info;
    info.token_id = get_calling_token_id();
    info.process_token_id = get_calling_process_token_id();
    return info;
}

FfiHapTokenInfo get_hap_token_info(uint64_t token_id)
{
    FfiHapTokenInfo result;
    AccessTokenID tokenId = static_cast<AccessTokenID>(token_id);
    HapTokenInfo hapInfo;
    int ret = AccessTokenKit::GetHapTokenInfo(tokenId, hapInfo);
    if (ret != 0) {
        result.bundle_name = rust::String("");
        result.inst_index = -1;
        result.dlp_type = -1;
        result.user_id = -1;
        return result;
    }
    result.bundle_name = rust::String(hapInfo.bundleName);
    result.inst_index = hapInfo.instIndex;
    result.dlp_type = static_cast<int32_t>(hapInfo.dlpType);
    result.user_id = hapInfo.userID;
    return result;
}

FfiNativeTokenInfo get_native_token_info(uint64_t token_id)
{
    FfiNativeTokenInfo result;
    AccessTokenID tokenId = static_cast<AccessTokenID>(token_id);
    NativeTokenInfo nativeInfo;
    int ret = AccessTokenKit::GetNativeTokenInfo(tokenId, nativeInfo);
    if (ret != 0) {
        result.process_name = rust::String("");
        result.apl = -1;
        return result;
    }
    result.process_name = rust::String(nativeInfo.processName);
    result.apl = static_cast<int32_t>(nativeInfo.apl);
    return result;
}

uint64_t get_hap_token_id(int32_t user_id, rust::Str bundle_name, int32_t app_index)
{
    std::string name(bundle_name);
    AccessTokenIDEx tokenIdEx = AccessTokenKit::GetHapTokenIDEx(user_id, name, app_index);
    return tokenIdEx.tokenIDEx;
}

} // namespace OHOS::Security::AccessToken
