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

#include "call_reporter.h"

#include <cinttypes>
#include "datashare_log.h"
#include "datashare_string_utils.h"

namespace OHOS {
namespace DataShare {
void DataShareCallReporter::Count(const std::string &funcName, const std::string &uri)
{
    int overCount = 0;
    int64_t firstCallTime = 0;
    callCounts.Compute(funcName, [&overCount, &firstCallTime](auto &key, CallInfo &callInfo) {
        auto callCount = callInfo.count;
        if (callCount == 0) {
            callInfo.firstTime = std::chrono::system_clock::now();
        }
        if (++callCount % RESET_COUNT_THRESHOLD == 0) {
            int64_t first = std::chrono::duration_cast<std::chrono::milliseconds>(
                callInfo.firstTime.time_since_epoch()).count();
            int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            ++overCount;
            firstCallTime = first;
            if (now - first <= TIME_THRESHOLD.count()) {
                ++overCount;
            }
            callCount = 0;
        }
        callInfo.count = callCount;
        return true;
    });
    int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    if (overCount > 0) {
        LOG_WARN("Call the threshold, func: %{public}s, first:%{public}" PRIi64 "ms, now:%{public}" PRIi64
            "ms, uri:%{public}s", funcName.c_str(), firstCallTime, now, DataShareStringUtils::Anonymous(uri).c_str());
    }
    if (overCount > 1) {
        LOG_WARN("Call too frequently, func: %{public}s, first:%{public}" PRIi64 "ms, now:%{public}" PRIi64
            "ms, uri:%{public}s", funcName.c_str(), firstCallTime, now, DataShareStringUtils::Anonymous(uri).c_str());
    }
}
} // namespace DataShare
} // namespace OHOS