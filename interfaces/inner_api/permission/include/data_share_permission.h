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
     * @brief Verify if tokenId has access perimission to uri.

     * @param tokenId Unique identification of application.
     * @param uri, Indicates the path of data to verify perimission.
     * @param isRead, Obtain read permission for true and write permission for false.

     * @return Returns the error code.
     */
    static int VerifyPermission(Security::AccessToken::AccessTokenID tokenId, const Uri &uri, bool isRead);
};
} // namespace DataShare
} // namespace OHOS
#endif // DATA_SHARE_PERMISSION_H