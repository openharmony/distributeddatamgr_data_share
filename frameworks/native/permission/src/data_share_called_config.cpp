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
#define LOG_TAG "data_share_called_config"

#include "data_share_called_config.h"

#include <string>
#include <utility>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "bundle_mgr_helper.h"
#include "datashare_errno.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"
#include "data_share_permission.h"
#include "hiview_datashare.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "ipc_skeleton.h"
#include "system_ability_definition.h"

namespace OHOS::DataShare {
using namespace OHOS::AppExecFwk;
using namespace OHOS::Security::AccessToken;
DataShareCalledConfig::DataShareCalledConfig(const std::string &uri)
{
    providerInfo_.uri = uri;
    Uri uriTemp(providerInfo_.uri);
    providerInfo_.scheme = uriTemp.GetScheme();
    auto isProxyData = PROXY_URI_SCHEME == providerInfo_.scheme;
    std::string bundleName = uriTemp.GetAuthority();
    if (!isProxyData) {
        std::vector<std::string> pathSegments;
        uriTemp.GetPathSegments(pathSegments);
        if (pathSegments.size() != 0) {
            bundleName = pathSegments[0];
        }
    }
    providerInfo_.bundleName = bundleName;
}

std::string DataShareCalledConfig::BundleName()
{
    return providerInfo_.bundleName;
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
        LOG_ERROR("Get user failed!token:0x%{public}x, result:%{public}d",
            tokenId, result);
        return -1;
    }
    return tokenInfo.userID;
}

int DataShareCalledConfig::GetFromProxyData()
{
    auto [success, bundleInfo] = GetBundleInfoFromBMS(providerInfo_.bundleName, providerInfo_.currentUserId);
    if (!success) {
        LOG_ERROR("Get bundleInfo failed! bundleName:%{public}s, userId:%{public}d, uri:%{public}s",
            providerInfo_.bundleName.c_str(), providerInfo_.currentUserId,
            DataShareStringUtils::Anonymous(providerInfo_.uri).c_str());
        return E_BUNDLE_NAME_NOT_EXIST;
    }
    std::string uriWithoutQuery = providerInfo_.uri;
    DataShareStringUtils::RemoveFromQuery(uriWithoutQuery);

    for (auto &hapModuleInfo : bundleInfo.hapModuleInfos) {
        for (auto &data : hapModuleInfo.proxyDatas) {
            if (data.uri.length() > uriWithoutQuery.length() ||
                uriWithoutQuery.compare(0, data.uri.length(), data.uri) != 0) {
                continue;
            }
            providerInfo_.readPermission = std::move(data.requiredReadPermission);
            providerInfo_.writePermission = std::move(data.requiredWritePermission);
            providerInfo_.moduleName = std::move(hapModuleInfo.moduleName);
            return E_OK;
        }
    }
    LOG_ERROR("E_URI_NOT_EXIST uriWithoutQuery %{public}s", uriWithoutQuery.c_str());
    return E_URI_NOT_EXIST;
}

std::pair<int, DataShareCalledConfig::ProviderInfo> DataShareCalledConfig::GetProviderInfo(int32_t user)
{
    if (providerInfo_.bundleName.empty()) {
        LOG_ERROR("BundleName not exist!, user:%{public}d, uri:%{public}s",
            user, DataShareStringUtils::Anonymous(providerInfo_.uri).c_str());
        return std::make_pair(E_BUNDLE_NAME_NOT_EXIST, DataShareCalledConfig::ProviderInfo{});
    }
    providerInfo_.currentUserId = user;
    auto ret = GetFromProxyData();
    if (ret != E_OK) {
        LOG_ERROR("GetFromProxyData Failed! ret:%{public}d,user:%{public}d,uri:%{public}s",
            ret, user, providerInfo_.uri.c_str());
    }
    return std::make_pair(ret, providerInfo_);
}

std::pair<bool, BundleInfo> DataShareCalledConfig::GetBundleInfoFromBMS(std::string bundleName, int32_t user)
{
    BundleInfo bundleInfo;
    auto bmsHelper = DelayedSingleton<BundleMgrHelper>::GetInstance();
    if (bmsHelper == nullptr) {
        LOG_ERROR("BmsHelper is nullptr!.uri: %{public}s",
            DataShareStringUtils::Anonymous(bundleName).c_str());
        return std::make_pair(false, bundleInfo);
    }

    if (user == 0) {
        user = Constants::ANY_USERID;
    }
    // because BMS and obs are in the same process.
    // set IPCSkeleton tokenid to this process's tokenid.
    // otherwise BMS may check permission failed.
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    bool ret = bmsHelper->GetBundleInfo(bundleName,
        BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO, bundleInfo, user);
    IPCSkeleton::SetCallingIdentity(identity);
    if (!ret) {
        LOG_ERROR("Get BundleInfo failed! bundleName:%{public}s, userId:%{public}d",
            bundleName.c_str(), user);
        return std::make_pair(false, bundleInfo);
    }
    return std::make_pair(true, bundleInfo);
}

std::pair<bool, ExtensionAbilityInfo> DataShareCalledConfig::GetExtensionInfoFromBMS(std::string &uri, int32_t user)
{
    ExtensionAbilityInfo info;
    auto bmsHelper = DelayedSingleton<BundleMgrHelper>::GetInstance();
    if (bmsHelper == nullptr) {
        LOG_ERROR("BmsHelper is nullptr!.uri: %{public}s",
            DataShareStringUtils::Anonymous(uri).c_str());
        return std::make_pair(false, info);
    }

    if (user == 0) {
        user = Constants::ANY_USERID;
    }
    // because BMS and obs are in the same process.
    // set IPCSkeleton tokenid to this process's tokenid.
    // otherwise BMS may check permission failed.
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    bool ret = bmsHelper->QueryExtensionAbilityInfoByUri(uri, user, info);
    IPCSkeleton::SetCallingIdentity(identity);
    if (!ret) {
        LOG_ERROR("QueryExtensionAbilityInfoByUri failed! uri:%{public}s, userId:%{public}d",
            uri.c_str(), user);
        return std::make_pair(false, info);
    }
    if (info.type != ExtensionAbilityType::DATASHARE) {
        LOG_ERROR("QueryExtensionAbilityInfoByUri type invalid! uri:%{public}s, userId:%{public}d, type:%{public}d",
            uri.c_str(), user, info.type);
        return std::make_pair(false, info);
    }
    return std::make_pair(true, info);
}
} // namespace OHOS::DataShare
