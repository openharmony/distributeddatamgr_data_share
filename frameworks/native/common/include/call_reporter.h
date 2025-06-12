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
        // total count for silent access threshold
        int totalCount = 0;
        std::chrono::system_clock::time_point firstTime;
        // start time of ervery 30s
        std::chrono::system_clock::time_point startTime;
        // print err log only once by using flag
        bool logPrintFlag = false;
    };
    bool Count(const std::string &funcName, const std::string &uri);
private:
    ConcurrentMap<std::string, CallInfo> callCounts_;
    static constexpr int RESET_COUNT_THRESHOLD = 100;
    static constexpr int ACCESS_COUNT_THRESHOLD = 3000; // silent access threshold
    static constexpr std::chrono::milliseconds TIME_THRESHOLD = std::chrono::milliseconds(30000);
    void UpdateCallCounts(const std::string &funcName, int &overCount, int64_t &firstCallTime, bool &isOverThreshold);
};
} // namespace DataShare
} // namespace OHOS
#endif