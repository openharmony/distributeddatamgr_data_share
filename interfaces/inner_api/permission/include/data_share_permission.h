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

#ifndef DATA_SHARE_PERMISSION_H
#define DATA_SHARE_PERMISSION_H

#include <string>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "uri.h"

namespace OHOS {
namespace DataShare {
class DataSharePermission {
using Uri = OHOS::Uri;
public:
    DataSharePermission() = default;
    ~DataSharePermission() = default;
    /**
     * @brief Verify if tokenId has access permission to uri.

     * @param tokenId Unique identification of application.
     * @param uri, Indicates the path of data to verify permission.
     * @param isRead, Obtain read permission for true and write permission for false.

     * @return Returns the error code.
     */
    static int VerifyPermission(Security::AccessToken::AccessTokenID tokenId, const Uri &uri, bool isRead);

    static std::pair<int, std::string> GetExtensionUriPermission(Uri &uri,
        int32_t user, bool isRead);

    static int CheckExtensionTrusts(uint32_t consumerToken, uint32_t providerToken);

    static void ReportExtensionFault(int32_t errCode, uint32_t tokenId,
        std::string &uri, std::string &bussinessType);

    static bool VerifyPermission(uint32_t tokenId, std::string &permission);

    static bool VerifyPermission(Uri &uri, uint32_t tokenId, std::string &permission, bool isExtension);

    static std::pair<int, std::string> GetSilentUriPermission(Uri &uri, int32_t user, bool isRead);

    static int32_t UriIsTrust(Uri &uri);

    static std::pair<int, std::string> GetUriPermission(Uri &uri, int32_t user, bool isRead, bool isExtension);

    static int32_t IsExtensionValid(uint32_t tokenId, uint32_t fullToken, int32_t user);
private:

    static constexpr const char *SCHEMA_DATASHARE = "datashare";
    static constexpr const char *SCHEMA_DATASHARE_PROXY = "datashareproxy";
    static constexpr const char *SCHEMA_PREFERENCE = "sharepreferences";
    static constexpr const char *SCHEMA_RDB = "rdb";
    static constexpr const char *SCHEMA_FILE = "file";

    static void ReportExcuteFault(int32_t errCode, std::string &consumer, std::string &provider);

    static int VerifyDataObsPermissionInner(Security::AccessToken::AccessTokenID tokenID,
        Uri &uri, bool isRead, bool &isTrust);
};
} // namespace DataShare
} // namespace OHOS
#endif // DATA_SHARE_PERMISSION_H