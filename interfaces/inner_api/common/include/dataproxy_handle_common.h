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

#ifndef DATASHARE_PROXY_COMMON_H
#define DATASHARE_PROXY_COMMON_H

#include "datashare_observer.h"

namespace OHOS {
namespace DataShare {
static constexpr int32_t URI_MAX_SIZE = 256;
static constexpr int32_t VALUE_MAX_SIZE = 4096;
static constexpr int32_t APPIDENTIFIER_MAX_SIZE = 128;
static constexpr int32_t URI_MAX_COUNT = 32;
static constexpr int32_t PROXY_DATA_MAX_COUNT = 32;
static constexpr int32_t ALLOW_LIST_MAX_COUNT = 256;
constexpr const char* ALLOW_ALL = "all";
constexpr const char* DATA_PROXY_SCHEMA = "datashareproxy://";

enum DataProxyErrorCode {
    SUCCESS = 0,
    URI_NOT_EXIST,
    NO_PERMISSION,
    OVER_LIMIT,
    INNER_ERROR,
};

/**
 * @brief DataProxy Value Type .
 */
enum DataProxyValueType : int32_t {
    /** DataProxy Value Types is int.*/
    VALUE_INT = 0,
    /** DataProxy Value Types is double.*/
    VALUE_DOUBLE,
    /** DataProxy Value Types is string.*/
    VALUE_STRING,
    /** DataProxy Value Types is bool.*/
    VALUE_BOOL,
};

using DataProxyValue = std::variant<int64_t, double, std::string, bool>;

enum DataProxyType {
    SHARED_CONFIG = 0,
};

struct DataProxyConfig {
    DataProxyType type_;
};

struct DataShareProxyData {
    /**
     * @brief Constructor.
     */
    DataShareProxyData() = default;

    /**
     * @brief Destructor.
     */
    ~DataShareProxyData() = default;
    
    DataShareProxyData(const std::string &uri, const DataProxyValue &value,
        const std::vector<std::string> &allowList = {}) : uri_(uri), value_(value), allowList_(allowList) {}

    std::string uri_;
    DataProxyValue value_;
    std::vector<std::string> allowList_;
    bool isAllowListExceed = false;
};

struct DataProxyResult {
    DataProxyResult() = default;
    DataProxyResult(const std::string &uri, const DataProxyErrorCode &result) : uri_(uri), result_(result) {}
    std::string uri_;
    DataProxyErrorCode result_;
};

struct DataProxyGetResult {
    DataProxyGetResult() = default;
    DataProxyGetResult(const std::string &uri, const DataProxyErrorCode &result,
        const DataProxyValue &value = {}, const std::vector<std::string> allowList = {})
        : uri_(uri), result_(result), value_(value), allowList_(allowList) {}
    std::string uri_;
    DataProxyErrorCode result_;
    DataProxyValue value_;
    std::vector<std::string> allowList_;
};

struct DataProxyChangeInfo {
    DataProxyChangeInfo() = default;
    DataProxyChangeInfo(const DataShareObserver::ChangeType &changeType,
        const std::string &uri, const DataProxyValue &value)
        : changeType_(changeType), uri_(uri), value_(value) {}
    DataShareObserver::ChangeType changeType_ = DataShareObserver::INVAILD;
    std::string uri_;
    DataProxyValue value_;
};
} // namespace DataShare
} // namespace OHOS
#endif // DATASHARE_PROXY_COMMON_H