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

#ifndef DATASHARE_CALL_REPORTER_H
#define DATASHARE_CALL_REPORTER_H

#include <concurrent_map.h>
#include <string>

namespace OHOS {
namespace DataShare {
class DataShareCallReporter {
public:
    DataShareCallReporter() = default;
    struct CallInfo {
        int count = 0;
        std::chrono::system_clock::time_point firstTime;
    };
    void Count(const std::string &funcName, const std::string &uri);
private:
    ConcurrentMap<std::string, CallInfo> callCounts;
    static constexpr int RESET_COUNT_THRESHOLD = 100;
    static constexpr std::chrono::milliseconds TIME_THRESHOLD = std::chrono::milliseconds(30000);
};
} // namespace DataShare
} // namespace OHOS
#endif