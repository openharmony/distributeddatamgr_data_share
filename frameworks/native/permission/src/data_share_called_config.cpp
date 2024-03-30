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
#define LOG_TAG "DataShareCalledConfig"

#include "data_share_called_config.h"

#include <string>
#include <utility>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "bundle_mgr_helper.h"
#include "datashare_errno.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS::DataShare {
using namespace OHOS::AppExecFwk;
using namespace OHOS::Security::AccessToken;
DataShareCalledConfig::DataShareCalledConfig(const std::string &uri)
{
    providerInfo_.uri = uri;
}

int32_t DataShareCalledConfig::GetUserByToken(uint32_t tokenId)
{
    auto type = AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (type == TOKEN_NATIVE || type == TOKEN_SHELL) {
        return 0;
    }
    HapTokenInfo tokenInfo;
    auto result = AccessTokenKit::GetHapTokenInfo(tokenId, tokenInfo);
    if (result != RET_SUCCESS) {
        LOG_ERROR("Get user failed!token:0x%{public}x, result:%{public}d, uri:%{public}s",
            tokenId, result, DataShareStringUtils::Anonymous(providerInfo_.uri).c_str());
        return -1;
    }
    return tokenInfo.userID;
}

int DataShareCalledConfig::GetFromProxyData()
{
    auto [success, bundleInfo] = GetBundleInfoFromBMS();
    if (!success) {
        LOG_ERROR("Get bundleInfo failed! bundleName:%{public}s, userId:%{public}d, uri:%{public}s",
            providerInfo_.bundleName.c_str(), providerInfo_.currentUserId,
            DataShareStringUtils::Anonymous(providerInfo_.uri).c_str());
        return E_BUNDLE_NAME_NOT_EXIST;
    }
    std::string uriWithoutQuery = providerInfo_.uri;
    DataShareStringUtils::RemoveFromQuery(uriWithoutQuery);
    size_t schemePos = uriWithoutQuery.find(Constants::PARAM_URI_SEPARATOR);
    if (schemePos != uriWithoutQuery.npos) {
        uriWithoutQuery.replace(schemePos, Constants::PARAM_URI_SEPARATOR_LEN, Constants::URI_SEPARATOR);
    }
    for (auto &hapModuleInfo : bundleInfo.hapModuleInfos) {
        for (auto &data : hapModuleInfo.proxyDatas) {
            if (data.uri != uriWithoutQuery) {
                continue;
            }
            providerInfo_.readPermission = std::move(data.requiredReadPermission);
            providerInfo_.writePermission = std::move(data.requiredWritePermission);
            providerInfo_.moduleName = std::move(hapModuleInfo.moduleName);
            return E_OK;
        }
    }
    return E_URI_NOT_EXIST;
}

std::pair<int, DataShareCalledConfig::ProviderInfo> DataShareCalledConfig::GetProviderInfo(uint32_t tokenId)
{
    Uri uriTemp(providerInfo_.uri);
    auto isProxyData = PROXY_URI_SCHEMA == uriTemp.GetScheme();
    std::string bundleName = uriTemp.GetAuthority();
    if (!isProxyData) {
        std::vector<std::string> pathSegments;
        uriTemp.GetPathSegments(pathSegments);
        if (pathSegments.size() != 0) {
            bundleName = pathSegments[0];
        }
    }
    if (bundleName.empty()) {
        LOG_ERROR("BundleName not exist!, tokenId:0x%{public}x, uri:%{public}s",
            tokenId, DataShareStringUtils::Anonymous(providerInfo_.uri).c_str());
        return std::make_pair(E_BUNDLE_NAME_NOT_EXIST, DataShareCalledConfig::ProviderInfo{});
    }
    providerInfo_.bundleName = bundleName;
    providerInfo_.currentUserId = GetUserByToken(tokenId);
    auto ret = GetFromProxyData();
    if (ret != E_OK) {
        LOG_ERROR("Failed! isProxyData:%{public}d,ret:%{public}d,tokenId:0x%{public}x,uri:%{public}s",
            isProxyData, ret, tokenId, DataShareStringUtils::Anonymous(providerInfo_.uri).c_str());
    }
    return std::make_pair(ret, providerInfo_);
}

std::pair<bool, BundleInfo> DataShareCalledConfig::GetBundleInfoFromBMS()
{
    BundleInfo bundleInfo;
    auto bmsHelper = DelayedSingleton<BundleMgrHelper>::GetInstance();
    if (bmsHelper == nullptr) {
        LOG_ERROR("BmsHelper is nullptr!.uri: %{public}s",
            DataShareStringUtils::Anonymous(providerInfo_.uri).c_str());
        return std::make_pair(false, bundleInfo);
    }
    bool ret = bmsHelper->GetBundleInfo(providerInfo_.bundleName,
        BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO, bundleInfo, providerInfo_.currentUserId);
    if (!ret) {
        LOG_ERROR("Get BundleInfo failed! bundleName:%{public}s, userId:%{public}d,uri:%{public}s",
            providerInfo_.bundleName.c_str(), providerInfo_.currentUserId,
            DataShareStringUtils::Anonymous(providerInfo_.uri).c_str());
        return std::make_pair(false, bundleInfo);
    }
    return std::make_pair(true, bundleInfo);
}
} // namespace OHOS::DataShare