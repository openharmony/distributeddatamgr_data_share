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
// count the func call and check if the funcCount exceeds the threshold
bool DataShareCallReporter::Count(const std::string &funcName, const std::string &uri)
{
    int overCount = 0;
    int64_t firstCallTime = 0;
    // isOverThreshold means that the call count of the funcName over threshold, true means exceeded
    // if exceeds the threshold, the current 30s time window will return 'true'
    // and the funcCount will be reseted in the next 30s time window
    bool isOverThreshold = false;

    UpdateCallCounts(funcName, overCount, firstCallTime, isOverThreshold);

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
    // error log for over threshold and only print once
    if (isOverThreshold) {
        callCounts_.Compute(funcName, [funcName, now, uri](auto &key, CallInfo &callInfo) {
            if (!callInfo.logPrintFlag) {
                int64_t thresholdStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    callInfo.startTime.time_since_epoch()).count();

                callInfo.logPrintFlag = true;
                LOG_ERROR("Over threshold, func: %{public}s, first:%{public}" PRIi64 "ms, now:%{public}" PRIi64
                    "ms, uri:%{public}s", funcName.c_str(), thresholdStartTime, now,
                    DataShareStringUtils::Anonymous(uri).c_str());
            }
            return true;
        });
    }
    return isOverThreshold;
}

void DataShareCallReporter::UpdateCallCounts(const std::string &funcName, int &overCount, int64_t &firstCallTime,
    bool &isOverThreshold)
{
    callCounts_.Compute(funcName, [&overCount, &firstCallTime, &isOverThreshold](auto &key, CallInfo &callInfo) {
        int callCount = callInfo.count;
        // totalCallCount is the call count of funcName in 30s
        int totalCallCount = callInfo.totalCount;
        std::chrono::system_clock::time_point nowTimeStamp = std::chrono::system_clock::now();
        int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
            nowTimeStamp.time_since_epoch()).count();
        if (callCount == 0) {
            callInfo.firstTime = nowTimeStamp;
        }
        if (++callCount % RESET_COUNT_THRESHOLD == 0) {
            int64_t first = std::chrono::duration_cast<std::chrono::milliseconds>(
                callInfo.firstTime.time_since_epoch()).count();
            ++overCount;
            firstCallTime = first;
            if (now - first <= TIME_THRESHOLD.count()) {
                ++overCount;
            }
            callCount = 0;
        }
        callInfo.count = callCount;

        // update access control count
        if (totalCallCount == 0) {
            callInfo.startTime = nowTimeStamp;
        }
        int64_t thresholdStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            callInfo.startTime.time_since_epoch()).count();
        // reset callInfo when time >= 30s
        if (now - thresholdStartTime >= TIME_THRESHOLD.count()) {
            callInfo.startTime = nowTimeStamp;
            callInfo.totalCount = 0;
            callInfo.logPrintFlag = false;
            totalCallCount = 0;
        }
        // isOverThreshold return true when callCount >= 3000 in 30s
        if (++totalCallCount >= ACCESS_COUNT_THRESHOLD) {
            isOverThreshold = true;
        }
        callInfo.totalCount = totalCallCount;
        return true;
    });
}
} // namespace DataShare
} // namespace OHOS