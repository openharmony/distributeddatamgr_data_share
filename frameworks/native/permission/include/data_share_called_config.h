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

#include "bundle_info.h"
#include "bundle_mgr_proxy.h"

namespace OHOS {
namespace DataShare {
class DataShareCalledConfig {
public:
    explicit DataShareCalledConfig(const std::string &uri);
    ~DataShareCalledConfig() = default;

    struct ProviderInfo {
        std::string uri;
        std::string bundleName;
        std::string moduleName;
        std::string readPermission;
        std::string writePermission;
        int32_t currentUserId = -1;
    };
    std::pair<int, ProviderInfo> GetProviderInfo(uint32_t tokenId);
private:
    int GetFromProxyData();
    int32_t GetUserByToken(uint32_t tokenId);
    std::pair<bool, OHOS::AppExecFwk::BundleInfo> GetBundleInfoFromBMS();
    ProviderInfo providerInfo_;
    static constexpr const char *PROXY_URI_SCHEMA = "datashareproxy";
};
} // namespace DataShare
} // namespace OHOS
#endif // DATA_SHARE_CALLED_CONFIG_H