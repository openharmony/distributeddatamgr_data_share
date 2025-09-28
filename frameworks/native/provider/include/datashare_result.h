/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef DATASHARE_RESULT_H
#define DATASHARE_RESULT_H

#include "datashare_result_set.h"
#include "datashare_operation_statement.h"
#include "datashare_business_error.h"

namespace OHOS {
namespace DataShare {
class JsResult;
struct AsyncContext {
    bool isNeedNotify_ = false;
};

struct AsyncPoint {
    std::shared_ptr<AsyncContext> context;
};

struct AsyncCallBackPoint {
    std::shared_ptr<JsResult> result;
};

class JsResult {
public:
    JsResult() = default;
    bool GetRecvReply() const
    {
        return isRecvReply_;
    }

    void GetResult(int &value)
    {
        value = callbackResultNumber_;
    }

    void GetResult(std::string &value)
    {
        std::lock_guard<std::mutex> lock(asyncLock_);
        value = callbackResultString_;
    }

    void GetResult(std::vector<std::string> &value)
    {
        std::lock_guard<std::mutex> lock(asyncLock_);
        value = callbackResultStringArr_;
    }

    void GetResult(std::vector<BatchUpdateResult> &results)
    {
        std::lock_guard<std::mutex> lock(asyncLock_);
        results = updateResults_;
    }

    void GetResultSet(std::shared_ptr<DataShareResultSet> &value)
    {
        std::lock_guard<std::mutex> lock(asyncLock_);
        value = callbackResultObject_;
    }

    void GetBusinessError(DatashareBusinessError &businessError)
    {
        std::lock_guard<std::mutex> lock(asyncLock_);
        businessError = businessError_;
    }

public:
    bool isRecvReply_ = false;
    int callbackResultNumber_ = -1;
    std::string callbackResultString_ = "";
    std::vector<std::string> callbackResultStringArr_ = {};
    std::mutex asyncLock_;
    std::shared_ptr<DataShareResultSet> callbackResultObject_ = nullptr;
    DatashareBusinessError businessError_;
    std::vector<BatchUpdateResult> updateResults_ = {};
};

} // namespace DataShare
} // namespace OHOS
#endif // DATASHARE_RESULT_H
