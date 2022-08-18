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

#include "datashare_predicates.h"
#include "datashare_log.h"
#include "datashare_errno.h"
namespace OHOS {
namespace DataShare {
DataSharePredicates::DataSharePredicates()
{
}

DataSharePredicates::DataSharePredicates(const std::list<OperationItem> &operList)
    : operationList_(operList)
{
}

DataSharePredicates::~DataSharePredicates()
{
}

/**
 * EqualTo
 */
DataSharePredicates *DataSharePredicates::EqualTo(const std::string &field, const DataSharePredicatesObject &value)
{
    LOG_DEBUG("Start");
    SetOperationList(EQUAL_TO, field, value);
    return this;
}

/**
 * NotEqualTo
 */
DataSharePredicates *DataSharePredicates::NotEqualTo(const std::string &field, const DataSharePredicatesObject &value)
{
    LOG_DEBUG("Start");
    SetOperationList(NOT_EQUAL_TO, field, value);
    return this;
}

/**
 * GreaterThan
 */
DataSharePredicates *DataSharePredicates::GreaterThan(const std::string &field, const DataSharePredicatesObject &value)
{
    LOG_DEBUG("Start");
    SetOperationList(GREATER_THAN, field, value);
    return this;
}

/**
 * LessThan
 */
DataSharePredicates *DataSharePredicates::LessThan(const std::string &field, const DataSharePredicatesObject &value)
{
    LOG_DEBUG("Start");
    SetOperationList(LESS_THAN, field, value);
    return this;
}

/**
 * GreaterThanOrEqualTo
 */
DataSharePredicates *DataSharePredicates::GreaterThanOrEqualTo(const std::string &field,
    const DataSharePredicatesObject &value)
{
    LOG_DEBUG("Start");
    SetOperationList(GREATER_THAN_OR_EQUAL_TO, field, value);
    return this;
}

/**
 * LessThanOrEqualTo
 */
DataSharePredicates *DataSharePredicates::LessThanOrEqualTo(const std::string &field,
    const DataSharePredicatesObject &value)
{
    LOG_DEBUG("Start");
    SetOperationList(LESS_THAN_OR_EQUAL_TO, field, value);
    return this;
}

/**
 * In
 */
DataSharePredicates *DataSharePredicates::In(const std::string &field, const DataSharePredicatesObjects &value)
{
    LOG_DEBUG("Start");
    SetOperationList(SQL_IN, field, value);
    return this;
}

/**
 * NotIn
 */
DataSharePredicates *DataSharePredicates::NotIn(const std::string &field, const DataSharePredicatesObjects &value)
{
    LOG_DEBUG("Start");
    SetOperationList(NOT_IN, field, value);
    return this;
}

/**
 * BeginWrap
 */
DataSharePredicates *DataSharePredicates::BeginWrap()
{
    LOG_DEBUG("Start");
    SetOperationList(BEGIN_WARP);
    return this;
}

/**
 * EndWrap
 */
DataSharePredicates *DataSharePredicates::EndWrap()
{
    LOG_DEBUG("Start");
    SetOperationList(END_WARP);
    return this;
}

/**
 * Or
 */
DataSharePredicates *DataSharePredicates::Or()
{
    LOG_DEBUG("Start");
    SetOperationList(OR);
    return this;
}

/**
 * And
 */
DataSharePredicates *DataSharePredicates::And()
{
    LOG_DEBUG("Start");
    SetOperationList(AND);
    return this;
}

/**
 * Contains
 */
DataSharePredicates *DataSharePredicates::Contains(const std::string &field, const std::string &value)
{
    LOG_DEBUG("Start");
    SetOperationList(CONTAINS, field, value);
    return this;
}

/**
 * BeginsWith
 */
DataSharePredicates *DataSharePredicates::BeginsWith(const std::string &field, const std::string &value)
{
    LOG_DEBUG("Start");
    SetOperationList(BEGIN_WITH, field, value);
    return this;
}

/**
 * EndsWith
 */
DataSharePredicates *DataSharePredicates::EndsWith(const std::string &field, const std::string &value)
{
    LOG_DEBUG("Start");
    SetOperationList(END_WITH, field, value);
    return this;
}

/**
 * IsNull
 */
DataSharePredicates *DataSharePredicates::IsNull(const std::string &field)
{
    LOG_DEBUG("Start");
    SetOperationList(IS_NULL, field);
    return this;
}

/**
 * IsNotNull
 */
DataSharePredicates *DataSharePredicates::IsNotNull(const std::string &field)
{
    LOG_DEBUG("Start");
    SetOperationList(IS_NOT_NULL, field);
    return this;
}

/**
 * Like
 */
DataSharePredicates *DataSharePredicates::Like(const std::string &field, const std::string &value)
{
    LOG_DEBUG("Start");
    SetOperationList(LIKE, field, value);
    return this;
}

/**
 * UnLike
 */
DataSharePredicates *DataSharePredicates::Unlike(const std::string &field, const std::string &value)
{
    LOG_DEBUG("Start");
    SetOperationList(UNLIKE, field, value);
    return this;
}

/**
 * Glob
 */
DataSharePredicates *DataSharePredicates::Glob(const std::string &field, const std::string &value)
{
    LOG_DEBUG("Start");
    SetOperationList(GLOB, field, value);
    return this;
}

/**
 * Between
 */
DataSharePredicates *DataSharePredicates::Between(const std::string &field,
    const std::string &low, const std::string &high)
{
    LOG_DEBUG("Start");
    SetOperationList(BETWEEN, field, low, high);
    return this;
}

/**
 * NotBetween
 */
DataSharePredicates *DataSharePredicates::NotBetween(const std::string &field,
    const std::string &low, const std::string &high)
{
    LOG_DEBUG("Start");
    SetOperationList(NOTBETWEEN, field, low, high);
    return this;
}

/**
 * OrderByAsc
 */
DataSharePredicates *DataSharePredicates::OrderByAsc(const std::string &field)
{
    LOG_DEBUG("Start");
    SetOperationList(ORDER_BY_ASC, field);
    return this;
}

/**
 * OrderByDesc
 */
DataSharePredicates *DataSharePredicates::OrderByDesc(const std::string &field)
{
    LOG_DEBUG("Start");
    SetOperationList(ORDER_BY_DESC, field);
    return this;
}

/**
 * Distinct
 */
DataSharePredicates *DataSharePredicates::Distinct()
{
    LOG_DEBUG("Start");
    SetOperationList(DISTINCT);
    return this;
}

/**
 * Limit
 */
DataSharePredicates *DataSharePredicates::Limit(const int number, const int offset)
{
    LOG_DEBUG("Start");
    SetOperationList(LIMIT, number, offset);
    return this;
}

/**
 * GroupBy
 */
DataSharePredicates *DataSharePredicates::GroupBy(const std::vector<std::string> &fields)
{
    LOG_DEBUG("Start");
    SetOperationList(GROUP_BY, fields);
    return this;
}

/**
 * IndexedBy
 */
DataSharePredicates *DataSharePredicates::IndexedBy(const std::string &indexName)
{
    LOG_DEBUG("Start");
    SetOperationList(INDEXED_BY, indexName);
    return this;
}

/**
 * KeyPrefix
 */
DataSharePredicates *DataSharePredicates::KeyPrefix(const std::string &prefix)
{
    LOG_DEBUG("Start");
    SetOperationList(KEY_PREFIX, prefix);
    return this;
}

/**
 * InKeys
 */
DataSharePredicates *DataSharePredicates::InKeys(const std::vector<std::string> &keys)
{
    LOG_DEBUG("Start");
    SetOperationList(IN_KEY, keys);
    return this;
}

/**
 * GetOperationList
 */
const std::list<OperationItem>& DataSharePredicates::GetOperationList() const
{
    return operationList_;
}

/**
 * Get WhereClause
 */
std::string DataSharePredicates::GetWhereClause() const
{
    return whereClause_;
}

/**
 * Set WhereClause
 */
int DataSharePredicates::SetWhereClause(const std::string &whereClause)
{
    if ((settingMode_ != PREDICATES_METHOD) && (!whereClause.empty())) {
        this->whereClause_ = whereClause;
        settingMode_ = QUERY_LANGUAGE;
        return E_OK;
    }
    return E_ERROR;
}

/**
 * Get WhereArgs
 */
std::vector<std::string> DataSharePredicates::GetWhereArgs() const
{
    return whereArgs_;
}

/**
 * Get WhereArgs
 */
int DataSharePredicates::SetWhereArgs(const std::vector<std::string> &whereArgs)
{
    if ((settingMode_ != PREDICATES_METHOD) && (!whereArgs.empty())) {
        if (!whereArgs.empty()) {
            this->whereArgs_ = whereArgs;
            settingMode_ = QUERY_LANGUAGE;
            return E_OK;
        }
    }
    return E_ERROR;
}

/**
 * Get Order
 */
std::string DataSharePredicates::GetOrder() const
{
    return order_;
}

/**
 * Set Order
 */
int DataSharePredicates::SetOrder(const std::string &order)
{
    LOG_DEBUG("Start");
    if ((settingMode_ != PREDICATES_METHOD) && (!order.empty())) {
        this->order_ = order;
        settingMode_ = QUERY_LANGUAGE;
        return E_OK;
    }
    return E_ERROR;
}

/**
 * Clear Query Language
 */
void DataSharePredicates::ClearQueryLanguage()
{
    whereClause_ = "";
    whereArgs_ = {};
    order_ = "";
}

/**
 * Set Setting Mode
 */
void DataSharePredicates::SetSettingMode(const SettingMode &settingMode)
{
    settingMode_ = settingMode;
}

/**
 * Get Setting Mode
 */
SettingMode DataSharePredicates::GetSettingMode() const
{
    return settingMode_;
}

/**
 * SetOperationList
 */
void DataSharePredicates::SetOperationList(OperationType operationType, const DataSharePredicatesObject &para1,
    const DataSharePredicatesObject &para2, const DataSharePredicatesObject &para3)
{
    LOG_DEBUG("Start");
    OperationItem operationItem {};
    operationItem.operation = operationType;
    operationItem.singleParams.push_back(para1);
    operationItem.singleParams.push_back(para2);
    operationItem.singleParams.push_back(para3);
    operationList_.push_back(operationItem);
    if (settingMode_ != PREDICATES_METHOD) {
        ClearQueryLanguage();
        settingMode_ = PREDICATES_METHOD;
    }
}

void DataSharePredicates::SetOperationList(OperationType operationType, const DataSharePredicatesObjects &param)
{
    LOG_DEBUG("Start");
    OperationItem operationItem {};
    operationItem.operation = operationType;
    operationItem.multiParams.push_back(param);
    operationList_.push_back(operationItem);
    if (settingMode_ != PREDICATES_METHOD) {
        ClearQueryLanguage();
        settingMode_ = PREDICATES_METHOD;
    }
}

void DataSharePredicates::SetOperationList(OperationType operationType, const DataSharePredicatesObject &param1,
    const DataSharePredicatesObjects &param2)
{
    LOG_DEBUG("Start");
    OperationItem operationItem {};
    operationItem.operation = operationType;
    operationItem.singleParams.push_back(param1);
    operationItem.multiParams.push_back(param2);
    operationList_.push_back(operationItem);
    if (settingMode_ != PREDICATES_METHOD) {
        ClearQueryLanguage();
        settingMode_ = PREDICATES_METHOD;
    }
}
} // namespace DataShare
} // namespace OHOS