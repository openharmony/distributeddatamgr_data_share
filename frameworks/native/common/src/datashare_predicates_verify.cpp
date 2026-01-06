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
#define LOG_TAG "datashare_predicates_verify"
#include <regex>

#include "datashare_predicates_verify.h"

#include "datashare_errno.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
static const std::set<OperationType> SINGLE_2_PARAMS_PUBLIC_SET = { ORDER_BY_ASC, ORDER_BY_DESC };
static const std::set<OperationType> SINGLE_3_PARAMS_PUBLIC_SET = { EQUAL_TO };
static const std::set<OperationType> SINGLE_2_PARAMS_SYS_SET = { IS_NULL, IS_NOT_NULL, INDEXED_BY, KEY_PREFIX };
static const std::set<OperationType> SINGLE_3_PARAMS_SYS_SET = { NOT_EQUAL_TO, GREATER_THAN, LESS_THAN,
    GREATER_THAN_OR_EQUAL_TO, LESS_THAN_OR_EQUAL_TO, NOT_IN, LIKE, UNLIKE, BEGIN_WITH, END_WITH, CONTAINS, GLOB,
    BETWEEN, NOTBETWEEN };
static const std::set<OperationType> MULTI_2_PARAMS_SYS_SET = { IN_KEY, GROUP_BY };

// colName or (colName) or [colName] or "colName"
// allows leading/trailing spaces
static const std::regex COLNAME_OPTIONAL_BRACKETS(
    "^\\s*([a-zA-Z0-9_]+)\\s*$|"
    "^\\s*\\(([a-zA-Z0-9_]+)\\)\\s*$|"
    "^\\s*\\[([a-zA-Z0-9_]+)\\]\\s*$|"
    "^\\s*\"([a-zA-Z0-9_]+)\"\\s*$"
);

// tableName.colName or (tableName.colName) or [tableName.colName] or "tableName.colName"
// allows leading/trailing spaces
static const std::regex TABLENAME_DOT_COLNAME_OPTIONAL_BRACKETS(
    "^\\s*([a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+)\\s*$|"
    "^\\s*\\(([a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+)\\)\\s*$|"
    "^\\s*\\[([a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+)\\]\\s*$|"
    "^\\s*\"([a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+)\"\\s*$"
);

// $.colName or ($.colName) or [$.colName] or "$.colName"
// allows leading/trailing spaces
static const std::regex AMPERSAND_DOT_COLNAME_OPTIONAL_BRACKETS(
    "^\\s*(\\$\\.[a-zA-Z0-9_]+)\\s*$|"
    "^\\s*\\((\\$\\.[a-zA-Z0-9_]+)\\)\\s*$|"
    "^\\s*\\[(\\$\\.[a-zA-Z0-9_]+)\\]\\s*$|"
    "^\\s*\"(\\$\\.[a-zA-Z0-9_]+)\"\\s*$"
);

// store.table.colName or (store.table.colName) or [store.table.colName] or "store.table.colName"
// allows leading/trailing spaces
static const std::regex STORE_TABLE_COLNAME_OPTIONAL_BRACKETS(
    "^\\s*([a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+)\\s*$|"
    "^\\s*\\(([a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+)\\)\\s*$|"
    "^\\s*\\[([a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+)\\]\\s*$|"
    "^\\s*\"([a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+)\"\\s*$"
);

std::pair<int, int> DataSharePredicatesVerify::VerifyPredicates(const DataSharePredicates &predicates)
{
    const auto &operations = predicates.GetOperationList();
    for (const auto &oper : operations) {
        auto type = GetPredicatesVerifyType(oper.operation);
        int errCode = VerifyPredicatesByType(type, oper);
        if (errCode != E_OK) {
            int predicatesType = static_cast<int>(oper.operation);
            return std::make_pair(predicatesType, errCode);
        }
    }
    return std::make_pair(E_OK, E_OK);
}

DataSharePredicatesVerify::PredicatesVerifyType DataSharePredicatesVerify::GetPredicatesVerifyType(const int32_t type)
{
    // public verify type add log, system verify type add modify
    if (SINGLE_2_PARAMS_PUBLIC_SET.find(static_cast<OperationType>(type)) != SINGLE_2_PARAMS_PUBLIC_SET.end()) {
        return PredicatesVerifyType::SINGLE_2_PARAMS_PUBLIC;
    } else if (SINGLE_3_PARAMS_PUBLIC_SET.find(static_cast<OperationType>(type)) != SINGLE_3_PARAMS_PUBLIC_SET.end()) {
        return PredicatesVerifyType::SINGLE_3_PARAMS_PUBLIC;
    } else if (SINGLE_2_PARAMS_SYS_SET.find(static_cast<OperationType>(type)) != SINGLE_2_PARAMS_SYS_SET.end()) {
        return PredicatesVerifyType::SINGLE_2_PARAMS_SYS;
    } else if (SINGLE_3_PARAMS_SYS_SET.find(static_cast<OperationType>(type)) != SINGLE_3_PARAMS_SYS_SET.end()) {
        return PredicatesVerifyType::SINGLE_3_PARAMS_SYS;
    } else if (MULTI_2_PARAMS_SYS_SET.find(static_cast<OperationType>(type)) != MULTI_2_PARAMS_SYS_SET.end()) {
        return PredicatesVerifyType::MULTI_2_PARAMS_SYS;
    }
    // default type do not modify
    return PredicatesVerifyType::VERIFY_DEFAULT;
}

int DataSharePredicatesVerify::VerifyPredicatesByType(const PredicatesVerifyType &verifyType, const OperationItem &item)
{
    // VERIFY_DEFAULT and invalid params num do not need verify
    if (verifyType == PredicatesVerifyType::VERIFY_DEFAULT || !CheckParamNum(verifyType, item)) {
        LOG_WARN("predicates missing elements Verifytype:%{public}d, operation:%{public}d",
            static_cast<int>(verifyType), item.operation);
        return E_OK;
    }
    // public interfaces add hiview when field invalid
    // system interfaces need return error when field illegal
    if (verifyType == PredicatesVerifyType::SINGLE_2_PARAMS_PUBLIC ||
        verifyType == PredicatesVerifyType::SINGLE_3_PARAMS_PUBLIC) {
        if (!VerifyField(item.GetSingle(0))) {
            return E_FIELD_INVALID;
        }
    } else if (verifyType == PredicatesVerifyType::SINGLE_2_PARAMS_SYS ||
        verifyType == PredicatesVerifyType::SINGLE_3_PARAMS_SYS) {
        if (!VerifyField(item.GetSingle(0))) {
            return E_FIELD_ILLEGAL;
        }
    } else if (verifyType == PredicatesVerifyType::MULTI_2_PARAMS_SYS) {
        if (!VerifyFields(MutliValue(item.multiParams[0]))) {
            return E_FIELD_ILLEGAL;
        }
    }
    return E_OK;
}

bool DataSharePredicatesVerify::CheckParamNum(const PredicatesVerifyType &verifyType, const OperationItem &item)
{
    // singleParams and multiParams needs at last 1 parameter (field)
    if (verifyType == PredicatesVerifyType::VERIFY_DEFAULT) {
        return true;
    } else if (verifyType == PredicatesVerifyType::MULTI_2_PARAMS_SYS) {
        return (item.multiParams.size() > 0);
    }
    return (item.singleParams.size() > 0);
}

bool DataSharePredicatesVerify::VerifyField(const std::string &field)
{
    if (field.empty()) {
        LOG_WARN("field is empty");
        return true;
    }
    return (std::regex_match(field, COLNAME_OPTIONAL_BRACKETS) ||
        std::regex_match(field, TABLENAME_DOT_COLNAME_OPTIONAL_BRACKETS) ||
        std::regex_match(field, AMPERSAND_DOT_COLNAME_OPTIONAL_BRACKETS) ||
        std::regex_match(field, STORE_TABLE_COLNAME_OPTIONAL_BRACKETS));
}

bool DataSharePredicatesVerify::VerifyFields(const std::vector<std::string> &fields)
{
    for (const auto &field : fields) {
        if (!VerifyField(field)) {
            return false;
        }
    }
    return true;
}
} // namespace DataShare
} // namespace OHOS