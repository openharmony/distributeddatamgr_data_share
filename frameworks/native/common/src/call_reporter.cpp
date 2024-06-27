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
    auto callCount = 0;
    auto iter = callCounts.Find(funcName);
    if (iter.first) {
        callCount = iter.second;
    }
    if (callCount == 0) {
        callFirstTime.Insert(funcName, std::chrono::system_clock::now());
    }
    if (++callCount % RESET_COUNT_THRESHOLD == 0) {
        auto iterTime = callFirstTime.Find(funcName);
        if (!iterTime.first) {
            callFirstTime.Insert(funcName, std::chrono::system_clock::now());
            return;
        }
        int64_t first = std::chrono::duration_cast<std::chrono::milliseconds>(
            iterTime.second.time_since_epoch()).count();
        int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        LOG_WARN("Call the threshold, func: %{public}s, first:%{public}" PRIi64 "ms, now:%{public}" PRIi64
            "ms, uri:%{public}s", funcName.c_str(), first, now, DataShareStringUtils::Anonymous(uri).c_str());
        if (now - first <= TIME_THRESHOLD.count()) {
            LOG_WARN("Call too frequently, func: %{public}s, first:%{public}" PRIi64 "ms, now:%{public}" PRIi64
                "ms, uri:%{public}s", funcName.c_str(), first, now, DataShareStringUtils::Anonymous(uri).c_str());
        }
        callCount = 0;
        callFirstTime.Erase(funcName);
    }
    callCounts.Insert(funcName, callCount);
}
} // namespace DataShare
} // namespace OHOS