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
#define LOG_TAG "URIUtils"

#include "datashare_uri_utils.h"

#include <string>

#include "log_print.h"
#include "string_ex.h"
#include "uri.h"

namespace OHOS::DataShare {
std::string DataShareURIUtils::FormatUri(const std::string &uri)
{
    auto pos = uri.find_last_of('?');
    if (pos == std::string::npos) {
        return uri;
    }

    return uri.substr(0, pos);
}

std::pair<bool, uint32_t> DataShareURIUtils::Strtoul(const std::string &str)
{
    unsigned long data = 0;
    if (str.empty()) {
        return std::make_pair(false, data);
    }
    char* end = nullptr;
    errno = 0;
    data = strtoul(str.c_str(), &end, BASE_TEN);
    if (errno == ERANGE || end == nullptr || end == str || *end != '\0') {
        return std::make_pair(false, data);
    }
    return std::make_pair(true, data);
}

std::map<std::string, std::string> DataShareURIUtils::GetQueryParams(const std::string& uri)
{
    size_t queryStartPos = uri.find('?');
    if (queryStartPos == std::string::npos) {
        return {};
    }
    std::map<std::string, std::string> params;
    std::string queryParams = uri.substr(queryStartPos + 1);
    size_t startPos = 0;
    while (startPos < queryParams.size()) {
        size_t delimiterIndex = queryParams.find('&', startPos);
        if (delimiterIndex == std::string::npos) {
            delimiterIndex = queryParams.size();
        }
        size_t equalIndex = queryParams.find('=', startPos);
        if (equalIndex == std::string::npos || equalIndex > delimiterIndex) {
            startPos = delimiterIndex + 1;
            continue;
        }
        std::string key = queryParams.substr(startPos, equalIndex - startPos);
        std::string value = queryParams.substr(equalIndex + 1, delimiterIndex - equalIndex - 1);
        params[key] = value;
        startPos = delimiterIndex + 1;
    }
    return params;
}

std::pair<bool, int32_t> DataShareURIUtils::GetUserFromUri(const std::string &uri)
{
    auto queryParams = GetQueryParams(uri);
    if (queryParams[USER_PARAM].empty()) {
        // -1 is placeholder for visit provider's user
        return std::make_pair(true, -1);
    }
    auto [success, data] = Strtoul(queryParams[USER_PARAM]);
    if (!success) {
        return std::make_pair(false, -1);
    }
    if (data < 0 || data > INT32_MAX) {
        return std::make_pair(false, -1);
    }
    return std::make_pair(true, data);
}
} // namespace OHOS::DataShare