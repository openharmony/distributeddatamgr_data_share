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
#ifndef DATASHARE_URI_UTILS_H
#define DATASHARE_URI_UTILS_H

#include <map>
#include <vector>
#include <string>

namespace OHOS::DataShare {
class DataShareURIUtils {
public:
    static std::string FormatUri(const std::string &uri);
    static std::map<std::string, std::string> GetQueryParams(const std::string& uri);
    static std::pair<bool, uint32_t> Strtoul(const std::string &str);
    static std::pair<bool, int32_t> GetUserFromUri(const std::string &uri);

private:
    static constexpr const char USER_PARAM[] = "user";
    static constexpr const int BASE_TEN = 10;
};

} // namespace OHOS::DataShare
#endif // DATASHARE_URI_UTILS_H