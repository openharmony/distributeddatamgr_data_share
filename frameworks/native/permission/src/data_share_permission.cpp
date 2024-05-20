/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#define LOG_TAG "DataSharePermission"

#include "data_share_permission.h"

#include <string>

#include "data_share_called_config.h"
#include "datashare_errno.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"

namespace OHOS {
namespace DataShare {
using namespace AppExecFwk;
int DataSharePermission::VerifyPermission(Security::AccessToken::AccessTokenID tokenID, const Uri &uri, bool isRead)
{
    if (uri.ToString().empty()) {
        LOG_ERROR("Uri empty, tokenId:0x%{public}x", tokenID);
        return ERR_INVALID_VALUE;
    }
    DataShareCalledConfig calledConfig(uri.ToString());
    auto [errCode, providerInfo] = calledConfig.GetProviderInfo(tokenID);
    if (errCode != E_OK) {
        LOG_ERROR("ProviderInfo failed! token:0x%{public}x, errCode:%{public}d,uri:%{public}s", tokenID,
            errCode, DataShareStringUtils::Anonymous(uri.ToString()).c_str());
        return errCode;
    }
    auto permission = isRead ? providerInfo.readPermission : providerInfo.writePermission;
    if (permission.empty()) {
        LOG_ERROR("Reject, tokenId:0x%{public}x, uri:%{public}s", tokenID,
            DataShareStringUtils::Anonymous(providerInfo.uri).c_str());
        return ERR_PERMISSION_DENIED;
    }
    int status =
        Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenID, permission);
    if (status != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        LOG_ERROR("Permission denied! token:0x%{public}x,permission:%{public}s,uri:%{public}s",
            tokenID, permission.c_str(), DataShareStringUtils::Anonymous(providerInfo.uri).c_str());
        return ERR_PERMISSION_DENIED;
    }
    return E_OK;
}
} // namespace DataShare
} // namespace OHOS