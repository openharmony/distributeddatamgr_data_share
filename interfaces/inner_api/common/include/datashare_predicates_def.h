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


#ifndef DATASHARE_PREDICATES_DEF_H
#define DATASHARE_PREDICATES_DEF_H

#include <string>
#include <vector>
#include "datashare_predicates_object.h"
#include "datashare_predicates_objects.h"
namespace OHOS {
namespace DataShare {
typedef enum {
    INVALID_OPERATION = 0x0,
    EQUAL_TO,
    NOT_EQUAL_TO,
    GREATER_THAN,
    LESS_THAN,
    GREATER_THAN_OR_EQUAL_TO,
    LESS_THAN_OR_EQUAL_TO,
    AND,
    OR,
    IS_NULL,
    IS_NOT_NULL,
    SQL_IN,
    NOT_IN,
    LIKE,
    UNLIKE,
    ORDER_BY_ASC,
    ORDER_BY_DESC,
    LIMIT,
    OFFSET,
    BEGIN_WARP,
    END_WARP,
    BEGIN_WITH,
    END_WITH,
    IN_KEY,
    DISTINCT,
    GROUP_BY,
    INDEXED_BY,
    CONTAINS,
    GLOB,
    BETWEEN,
    NOTBETWEEN,
    KEY_PREFIX,
    LAST_TYPE
} OperationType;

typedef struct {
    OperationType operation;
    std::vector<DataSharePredicatesObject> singleParams;
    std::vector<DataSharePredicatesObjects> multiParams;
} OperationItem;

typedef enum {
    INVALID_MODE,
    QUERY_LANGUAGE,
    PREDICATES_METHOD
} SettingMode;
} // namespace DataShare
} // namespace OHOS

#endif
