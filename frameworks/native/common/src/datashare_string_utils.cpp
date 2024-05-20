/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "datashare_string_utils.h"

namespace OHOS {
namespace DataShare {
constexpr int32_t END_SIZE = 10;
constexpr int32_t ANONYMOUS_SIZE = 12;
constexpr const char *DEFAULT_ANONYMOUS = "******";
std::string DataShareStringUtils::Anonymous(const std::string &name)
{
    if (name.length() <= END_SIZE) {
        return DEFAULT_ANONYMOUS;
    }

    return (DEFAULT_ANONYMOUS + name.substr(name.length() - END_SIZE, END_SIZE));
}

void DataShareStringUtils::RemoveFromQuery(std::string &uri)
{
    auto pos = uri.find_last_of('?');
    if (pos == std::string::npos) {
        return;
    }
    uri.resize(pos);
}

std::string DataShareStringUtils::Change(const std::string &name)
{
    if (name.length() <= ANONYMOUS_SIZE) {
        return name;
    }
    return DEFAULT_ANONYMOUS + name.substr(ANONYMOUS_SIZE);
}
DataShareStringUtils::DataShareStringUtils() {}
DataShareStringUtils::~DataShareStringUtils() {}
} // namespace DataShare
} // namespace OHOS