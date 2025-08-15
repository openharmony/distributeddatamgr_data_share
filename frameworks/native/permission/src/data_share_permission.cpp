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
#include "datashare_uri_utils.h"
#define LOG_TAG "DataSharePermission"

#include "data_share_permission.h"

#include <string>

#include "access_token.h"
#include "bundle_mgr_helper.h"
#include "data_share_called_config.h"
#include "datashare_errno.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"
#include "data_share_config.h"
#include "hiview_datashare.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace DataShare {
using namespace AppExecFwk;
static constexpr const char *NO_PERMISSION = "noPermission";
int DataSharePermission::VerifyPermission(Security::AccessToken::AccessTokenID tokenID, const Uri &uri, bool isRead)
{
    if (uri.ToString().empty()) {
        LOG_ERROR("Uri empty, tokenId:0x%{public}x", tokenID);
        return ERR_INVALID_VALUE;
    }
    DataShareCalledConfig calledConfig(uri.ToString());
    int32_t user = DataShareCalledConfig::GetUserByToken(tokenID);
    auto [errCode, providerInfo] = calledConfig.GetProviderInfo(user);
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

bool IsInUriTrusts(Uri &uri)
{
    auto config = ConfigFactory::GetInstance().GetDataShareConfig();
    if (config == nullptr) {
        LOG_ERROR("GetDataShareConfig null");
        return false;
    }
    std::string uriStr = uri.ToString();
    for (std::string& item : config->uriTrusts) {
        if (item.length() > uriStr.length() ||
            uriStr.compare(0, item.length(), item) != 0) {
            continue;
        }
        return true;
    }
    return false;
}

bool CheckAppIdentifier(std::string &name, std::string &appIdentifier)
{
    auto [isSuccess, bundleInfo]  = DataShareCalledConfig::GetBundleInfoFromBMS(name, 0);
    if (!isSuccess) {
        return false;
    }
    if (bundleInfo.signatureInfo.appIdentifier == appIdentifier) {
        return true;
    }
    return false;
}

bool IsInExtensionTrusts(std::string& consumer, std::string& provider)
{
    auto config = ConfigFactory::GetInstance().GetDataShareConfig();
    if (config == nullptr) {
        LOG_ERROR("GetDataShareConfig is null");
        return false;
    }
    std::string appIdentifier;
    for (DataShareConfig::ConsumerProvider& item : config->extensionObsTrusts) {
        if (item.provider.name != provider) {
            continue;
        }
        if (!CheckAppIdentifier(item.provider.name, item.provider.appIdentifier)) {
            return false;
        }
        for (auto item : item.consumer) {
            if (item.name == consumer) {
                appIdentifier = item.appIdentifier;
                break;
            }
        }
    }
    if (consumer == provider) {
        return true;
    }
    if (CheckAppIdentifier(consumer, appIdentifier)) {
        return true;
    }
    return false;
}

int32_t DataSharePermission::UriIsTrust(Uri &uri)
{
    std::string schema = uri.GetScheme();
    if (schema == SCHEMA_PREFERENCE ||  schema == SCHEMA_RDB || schema == SCHEMA_FILE) {
        return E_OK;
    }
    if (IsInUriTrusts(uri)) {
        return E_OK;
    }
    if (!schema.empty() && schema != SCHEMA_DATASHARE && schema != SCHEMA_DATASHARE_PROXY) {
        LOG_ERROR("invalid uri %{public}s, schema %{public}s",
            uri.ToString().c_str(), uri.GetScheme().c_str());
        return E_DATASHARE_INVALID_URI;
    }
    return E_ERROR;
}

std::pair<int, std::string> DataSharePermission::GetExtensionUriPermission(Uri &uri,
    int32_t user, bool isRead)
{
    std::string uriStr = uri.ToString();
    std::string permission;
    auto [isSuccess, extensionInfo] = DataShareCalledConfig::GetExtensionInfoFromBMS(uriStr, user);
    if (isSuccess) {
        permission = isRead ? extensionInfo.readPermission : extensionInfo.writePermission;
        LOG_ERROR("GetExtensionUriPermission uri %{public}s %{public}s %{public}s",
            extensionInfo.uri.c_str(), extensionInfo.readPermission.c_str(), extensionInfo.writePermission.c_str());
        return std::make_pair(E_OK, permission);
    }
    return std::make_pair(E_URI_NOT_EXIST, "");
}

std::pair<int, std::string> DataSharePermission::GetUriPermission(Uri &uri, int32_t user, bool isRead, bool isExtension)
{
    std::string uriStr = uri.ToString();
    if (uriStr.empty()) {
        return std::make_pair(E_EMPTY_URI, "");
    }
    int32_t ret = UriIsTrust(uri);
    if (ret == E_OK) {
        return std::make_pair(E_OK, NO_PERMISSION);
    } else if (ret == E_DATASHARE_INVALID_URI) {
        return std::make_pair(E_DATASHARE_INVALID_URI, "");
    }
    std::string permission;
    if (isExtension) {
        std::tie(ret, permission) = GetExtensionUriPermission(uri, user, isRead);
    } else {
        std::tie(ret, permission) = GetSilentUriPermission(uri, user, isRead);
    }
    if (ret == E_OK) {
        return std::make_pair(E_OK, permission);
    }
    return std::make_pair(ret, "");
}

void DataSharePermission::ReportExcuteFault(int32_t errCode,  std::string &consumer, std::string &provider)
{
    DataShareFaultInfo faultInfo = {HiViewFaultAdapter::trustsFailed, consumer, provider,
        "", "", errCode, ""};
    HiViewFaultAdapter::ReportDataFault(faultInfo);
}

void DataSharePermission::ReportExtensionFault(int32_t errCode, uint32_t tokenId,
    std::string &uri, std::string &bussinessType)
{
    auto [bundleName, ret] = HiViewFaultAdapter::GetCallingName(tokenId);
    DataShareFaultInfo faultInfo = {HiViewFaultAdapter::extensionFailed, bundleName, "",
        "", bussinessType, errCode, uri};
    HiViewFaultAdapter::ReportDataFault(faultInfo);
}

std::pair<int, std::string> DataSharePermission::GetSilentUriPermission(Uri &uri, int32_t user, bool isRead)
{
    std::string uriStr = uri.ToString();
    DataShareCalledConfig calledConfig(uriStr);

    auto [errCode, providerInfo] = calledConfig.GetProviderInfo(user);
    if (errCode != E_OK) {
        LOG_ERROR("ProviderInfo failed! user:%{public}d, errCode:%{public}d,uri:%{public}s", user,
            errCode, uri.ToString().c_str());
        return std::make_pair(errCode, "");
    }
    auto permission = isRead ? providerInfo.readPermission : providerInfo.writePermission;
    return std::make_pair(E_OK, permission);
}

int DataSharePermission::CheckExtensionTrusts(uint32_t consumerToken, uint32_t providerToken)
{
    auto [consumer, ret1] = HiViewFaultAdapter::GetCallingName(consumerToken);
    auto [provider, ret2] = HiViewFaultAdapter::GetCallingName(providerToken);
    if (ret1 != E_OK || ret2 != E_OK) {
        LOG_ERROR("GetCallingName failed, consumer %{public}d %{public}s provider %{public}d "
            "%{public}s", consumerToken, consumer.c_str(), providerToken, provider.c_str());
        return E_GET_CALLER_NAME_FAILED;
    }
    if (IsInExtensionTrusts(consumer, provider)) {
        return E_OK;
    }
    LOG_ERROR("CheckExtensionTrusts failed, consumer %{public}d %{public}s provider %{public}d "
        "%{public}s", consumerToken, consumer.c_str(), providerToken, provider.c_str());
    ReportExcuteFault(E_NOT_IN_TRUSTS, consumer, provider);
    return E_NOT_IN_TRUSTS;
}

bool DataSharePermission::VerifyPermission(uint32_t tokenID, std::string &permission)
{
    if (permission.empty() || permission == NO_PERMISSION) {
        return true;
    }
    int status = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenID, permission);
    if (status != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        return false;
    }
    return true;
}

bool DataSharePermission::VerifyPermission(Uri &uri, uint32_t tokenID, std::string &permission, bool isExtension)
{
    if (permission == NO_PERMISSION) {
        return true;
    }

    std::string uriStr = uri.ToString();
    DataShareCalledConfig calledConfig(uriStr);
    std::string providerName = calledConfig.BundleName();

    auto [callingName, ret] = HiViewFaultAdapter::GetCallingName(tokenID);
    if (ret != E_OK) {
        LOG_WARN("GetCallingName failed, ret %{public}d", ret);
    }
    if (permission.empty() && isExtension) {
        return true;
    }

    if (permission.empty() && !isExtension) {
        LOG_INFO("Permission empty! token: %{public}d", tokenID);
        Security::AccessToken::HapTokenInfo tokenInfo;
        auto result = Security::AccessToken::AccessTokenKit::GetHapTokenInfo(tokenID, tokenInfo);
        if (result == Security::AccessToken::RET_SUCCESS && tokenInfo.bundleName == providerName) {
            return true;
        }
        LOG_ERROR("Permission denied! uri %{public}s callingName %{public}s token %{public}d",
            uri.ToString().c_str(), providerName.c_str(), tokenID);
        return false;
    }
    
    int status = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenID, permission);
    if (status != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        return false;
    }
    return true;
}

int32_t DataSharePermission::IsExtensionValid(uint32_t tokenId, uint32_t fullToken, int32_t user)
{
    Security::AccessToken::ATokenTypeEnum tokenType =
        Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenType != Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        return E_NOT_HAP;
    }
    if (Security::AccessToken::AccessTokenKit::IsSystemAppByFullTokenID(fullToken)) {
        return E_OK;
    }
    auto [bundleName, ret] = HiViewFaultAdapter::GetCallingName(tokenId);
    if (ret != 0) {
        return ret;
    }

    auto [success, bundleInfo] = DataShareCalledConfig::GetBundleInfoFromBMS(bundleName, user);
    if (!success) {
        LOG_ERROR("Get bundleInfo failed! bundleName:%{public}s, userId:%{public}d",
            bundleName.c_str(), user);
        return E_GET_BUNDLEINFO_FAILED;
    }
    for (auto &item : bundleInfo.extensionInfos) {
        if (item.type == AppExecFwk::ExtensionAbilityType::DATASHARE) {
            return E_OK;
        }
    }
    return E_NOT_DATASHARE_EXTENSION;
}

} // namespace DataShare
} // namespace OHOS