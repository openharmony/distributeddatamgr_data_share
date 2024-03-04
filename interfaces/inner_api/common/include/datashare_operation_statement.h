/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef DATASHARE_OPERATION_STATEMENT_H
#define DATASHARE_OPERATION_STATEMENT_H

#include "datashare_predicates.h"
#include "datashare_values_bucket.h"

namespace OHOS {
namespace DataShare {
enum class Operation : int32_t {
    INSERT = 0,
    UPDATE,
    DELETE,
};

struct UpdateOperation {
    DataShareValuesBucket valuesBucket;
    DataSharePredicates predicates;
};
using UpdateOperations = std::map<std::string, std::vector<UpdateOperation>>;

struct BatchUpdateResult {
    std::string uri;
    std::vector<int> codes;
};

struct OperationStatement {
    Operation operationType;
    std::string uri;
    DataSharePredicates predicates;
    DataShareValuesBucket valuesBucket;
};

enum ExecErrorCode : int32_t {
    EXEC_SUCCESS = 0,
    EXEC_FAILED,
    EXEC_PARTIAL_SUCCESS,
};

struct ExecResult {
    Operation operationType;
    int code;
    std::string message;
};

struct ExecResultSet {
    ExecErrorCode errorCode;
    std::vector<ExecResult> results;
};
}
}
#endif // DATASHARE_OPERATION_STATEMENT_H
