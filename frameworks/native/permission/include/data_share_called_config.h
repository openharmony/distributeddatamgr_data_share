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

#ifndef DATA_SHARE_CALLED_CONFIG_H
#define DATA_SHARE_CALLED_CONFIG_H

#include <cstdint>
#include <memory>
#include <string>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "bundle_info.h"
#include "bundle_mgr_helper.h"
#include "bundle_mgr_proxy.h"
#include "hap_token_info.h"
#include "refbase.h"
#include "tokenid_kit.h"
#include "uri.h"

namespace OHOS {
namespace DataShare {
using namespace OHOS::Security::AccessToken;
using namespace OHOS::AppExecFwk;
using BundleInfo = OHOS::AppExecFwk::BundleInfo;
using ExtensionAbility = OHOS::AppExecFwk::ExtensionAbilityInfo;
class DataShareCalledConfig {
public:

    explicit DataShareCalledConfig(const std::string &uri);
    ~DataShareCalledConfig() = default;

    struct ProviderInfo {
        std::string uri;
        std::string bundleName;
        std::string extensionName;
        std::string moduleName;
        std::string storeName;
        std::string tableName;
        std::string readPermission;
        std::string writePermission;
        int32_t currentUserId = -1;
    };
    std::pair<int, ProviderInfo> GetProviderInfo(bool isProxyData, uint32_t tokenId);
private:
    sptr<AppExecFwk::BundleMgrProxy> GetBundleMgrProxy();
    int GetFromProxyData();
    int32_t GetUserByToken(uint32_t tokenId);
    std::pair<bool, AppExecFwk::BundleInfo> GetBundleInfoFromBMS(int32_t userId);
    std::mutex mutex_;
    ProviderInfo providerInfo_;
    sptr<IRemoteObject> proxy_;
};
} // namespace DataShare
} // namespace OHOS
#endif // DATA_SHARE_CALLED_CONFIG_H