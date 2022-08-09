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
    LOG_DEBUG("%{public}s call field%{public}s", __func__, field.c_str());
    SetOperationList(EQUAL_TO, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::EqualTo End");
    return this;
}

/**
 * NotEqualTo
 */
DataSharePredicates *DataSharePredicates::NotEqualTo(const std::string &field, const DataSharePredicatesObject &value)
{
    LOG_DEBUG("%{public}s call field%{public}s", __func__, field.c_str());
    SetOperationList(NOT_EQUAL_TO, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::NotEqualTo End");
    return this;
}

/**
 * GreaterThan
 */
DataSharePredicates *DataSharePredicates::GreaterThan(const std::string &field, const DataSharePredicatesObject &value)
{
    LOG_DEBUG("%{public}s call field%{public}s", __func__, field.c_str());
    SetOperationList(GREATER_THAN, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::GreaterThan End");
    return this;
}

/**
 * LessThan
 */
DataSharePredicates *DataSharePredicates::LessThan(const std::string &field, const DataSharePredicatesObject &value)
{
    LOG_DEBUG("%{public}s call field%{public}s", __func__, field.c_str());
    SetOperationList(LESS_THAN, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::LessThan End");
    return this;
}

/**
 * GreaterThanOrEqualTo
 */
DataSharePredicates *DataSharePredicates::GreaterThanOrEqualTo(const std::string &field,
    const DataSharePredicatesObject &value)
{
    LOG_DEBUG("%{public}s call field%{public}s", __func__, field.c_str());
    SetOperationList(GREATER_THAN_OR_EQUAL_TO, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::GreaterThanOrEqualTo End");
    return this;
}

/**
 * LessThanOrEqualTo
 */
DataSharePredicates *DataSharePredicates::LessThanOrEqualTo(const std::string &field,
    const DataSharePredicatesObject &value)
{
    LOG_DEBUG("%{public}s call field%{public}s", __func__, field.c_str());
    SetOperationList(LESS_THAN_OR_EQUAL_TO, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::LessThanOrEqualTo End");
    return this;
}

/**
 * In
 */
DataSharePredicates *DataSharePredicates::In(const std::string &field, const DataSharePredicatesObject &value)
{
    LOG_DEBUG("DataSharePredicates::In Start field%{public}s", field.c_str());
    SetOperationList(IN, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::In End");
    return this;
}

/**
 * NotIn
 */
DataSharePredicates *DataSharePredicates::NotIn(const std::string &field, const DataSharePredicatesObject &value)
{
    LOG_DEBUG("DataSharePredicates::NotIn Start field%{public}s", field.c_str());
    SetOperationList(NOT_IN, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::NotIn End");
    return this;
}

/**
 * BeginWrap
 */
DataSharePredicates *DataSharePredicates::BeginWrap()
{
    LOG_DEBUG("DataSharePredicates::BeginWrap Start");
    SetOperationList(BEGIN_WARP, {}, {}, {}, ZERO_COUNT);
    LOG_DEBUG("DataSharePredicates::BeginWrap End");
    return this;
}

/**
 * EndWrap
 */
DataSharePredicates *DataSharePredicates::EndWrap()
{
    LOG_DEBUG("DataSharePredicates::EndWrap Start");
    SetOperationList(END_WARP, {}, {}, {}, ZERO_COUNT);
    LOG_DEBUG("DataSharePredicates::EndWrap End");
    return this;
}

/**
 * Or
 */
DataSharePredicates *DataSharePredicates::Or()
{
    LOG_DEBUG("DataSharePredicates::Or Start");
    SetOperationList(OR, {}, {}, {}, ZERO_COUNT);
    LOG_DEBUG("DataSharePredicates::Or End");
    return this;
}

/**
 * And
 */
DataSharePredicates *DataSharePredicates::And()
{
    LOG_DEBUG("DataSharePredicates::And Start");
    SetOperationList(AND, {}, {}, {}, ZERO_COUNT);
    LOG_DEBUG("DataSharePredicates::And End");
    return this;
}

/**
 * Contains
 */
DataSharePredicates *DataSharePredicates::Contains(const std::string &field, const std::string &value)
{
    LOG_DEBUG("DataSharePredicates::Contains Start field%{public}s,value%{public}s", field.c_str(), value.c_str());
    SetOperationList(CONTAINS, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::Contains End");
    return this;
}

/**
 * BeginsWith
 */
DataSharePredicates *DataSharePredicates::BeginsWith(const std::string &field, const std::string &value)
{
    LOG_DEBUG("DataSharePredicates::BeginsWith Start field%{public}s,value%{public}s", field.c_str(), value.c_str());
    SetOperationList(BEGIN_WITH, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::BeginsWith End");
    return this;
}

/**
 * EndsWith
 */
DataSharePredicates *DataSharePredicates::EndsWith(const std::string &field, const std::string &value)
{
    LOG_DEBUG("DataSharePredicates::EndsWith Start field%{public}s,value%{public}s", field.c_str(), value.c_str());
    SetOperationList(END_WITH, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::EndsWith End");
    return this;
}

/**
 * IsNull
 */
DataSharePredicates *DataSharePredicates::IsNull(const std::string &field)
{
    LOG_DEBUG("DataSharePredicates::IsNull Start field%{public}s", field.c_str());
    SetOperationList(IS_NULL, field, {}, {}, ONE_COUNT);
    LOG_DEBUG("DataSharePredicates::IsNull End");
    return this;
}

/**
 * IsNotNull
 */
DataSharePredicates *DataSharePredicates::IsNotNull(const std::string &field)
{
    LOG_DEBUG("DataSharePredicates::IsNotNull Start field%{public}s", field.c_str());
    SetOperationList(IS_NOT_NULL, field, {}, {}, ONE_COUNT);
    LOG_DEBUG("DataSharePredicates::IsNotNull End");
    return this;
}

/**
 * Like
 */
DataSharePredicates *DataSharePredicates::Like(const std::string &field, const std::string &value)
{
    LOG_DEBUG("DataSharePredicates::Like Start field%{public}s value%{public}s", field.c_str(), value.c_str());
    SetOperationList(LIKE, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::Like End");
    return this;
}

/**
 * UnLike
 */
DataSharePredicates *DataSharePredicates::Unlike(const std::string &field, const std::string &value)
{
    LOG_DEBUG("DataSharePredicates::Unlike Start field%{public}s value%{public}s", field.c_str(), value.c_str());
    SetOperationList(UNLIKE, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::Unlike End");
    return this;
}

/**
 * Glob
 */
DataSharePredicates *DataSharePredicates::Glob(const std::string &field, const std::string &value)
{
    LOG_DEBUG("DataSharePredicates::Glob Start field%{public}s value%{public}s", field.c_str(), value.c_str());
    SetOperationList(GLOB, field, value, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::Glob End");
    return this;
}

/**
 * Between
 */
DataSharePredicates *DataSharePredicates::Between(const std::string &field,
    const std::string &low, const std::string &high)
{
    LOG_DEBUG("DataSharePredicates::Between Start field%{public}s low%{public}s high%{public}s",
        field.c_str(), low.c_str(), high.c_str());
    SetOperationList(BETWEEN, field, low, high, THREE_COUNT);
    LOG_DEBUG("DataSharePredicates::Between End");
    return this;
}

/**
 * NotBetween
 */
DataSharePredicates *DataSharePredicates::NotBetween(const std::string &field,
    const std::string &low, const std::string &high)
{
    LOG_DEBUG("DataSharePredicates::NotBetween Start field%{public}s low%{public}s high%{public}s",
        field.c_str(), low.c_str(), high.c_str());
    SetOperationList(NOTBETWEEN, field, low, high, THREE_COUNT);
    LOG_DEBUG("DataSharePredicates::NotBetween End");
    return this;
}

/**
 * OrderByAsc
 */
DataSharePredicates *DataSharePredicates::OrderByAsc(const std::string &field)
{
    LOG_DEBUG("DataSharePredicates::OrderByAsc Start field%{public}s", field.c_str());
    SetOperationList(ORDER_BY_ASC, field, {}, {}, ONE_COUNT);
    LOG_DEBUG("DataSharePredicates::OrderByAsc End");
    return this;
}

/**
 * OrderByDesc
 */
DataSharePredicates *DataSharePredicates::OrderByDesc(const std::string &field)
{
    LOG_DEBUG("DataSharePredicates::OrderByDesc Start field%{public}s", field.c_str());
    SetOperationList(ORDER_BY_DESC, field, {}, {}, ONE_COUNT);
    LOG_DEBUG("DataSharePredicates::OrderByDesc End");
    return this;
}

/**
 * Distinct
 */
DataSharePredicates *DataSharePredicates::Distinct()
{
    LOG_DEBUG("DataSharePredicates::Distinct Start");
    SetOperationList(DISTINCT, {}, {}, {}, ZERO_COUNT);
    LOG_DEBUG("DataSharePredicates::Distinct End");
    return this;
}

/**
 * Limit
 */
DataSharePredicates *DataSharePredicates::Limit(const int number, const int offset)
{
    LOG_DEBUG("DataSharePredicates::Limit Start number : %{public}d, offset : %{public}d", number, offset);
    SetOperationList(LIMIT, number, offset, {}, TWO_COUNT);
    LOG_DEBUG("DataSharePredicates::Limit End");
    return this;
}

/**
 * GroupBy
 */
DataSharePredicates *DataSharePredicates::GroupBy(const std::vector<std::string> &fields)
{
    LOG_DEBUG("DataSharePredicates::GroupBy Start fields%{public}s", fields.at(0).c_str());
    SetOperationList(GROUP_BY, fields, {}, {}, ONE_COUNT);
    LOG_DEBUG("DataSharePredicates::GroupBy End");
    return this;
}

/**
 * IndexedBy
 */
DataSharePredicates *DataSharePredicates::IndexedBy(const std::string &indexName)
{
    LOG_DEBUG("DataSharePredicates::IndexedBy Start indexName%{public}s", indexName.c_str());
    SetOperationList(INDEXED_BY, indexName, {}, {}, ONE_COUNT);
    LOG_DEBUG("DataSharePredicates::IndexedBy End");
    return this;
}

/**
 * KeyPrefix
 */
DataSharePredicates *DataSharePredicates::KeyPrefix(const std::string &prefix)
{
    LOG_DEBUG("DataSharePredicates::KeyPrefix Start prefix%{public}s", prefix.c_str());
    SetOperationList(KEY_PREFIX, prefix, {}, {}, ONE_COUNT);
    LOG_DEBUG("DataSharePredicates::KeyPrefix End");
    return this;
}

/**
 * InKeys
 */
DataSharePredicates *DataSharePredicates::InKeys(const std::vector<std::string> &keys)
{
    LOG_DEBUG("DataSharePredicates::InKeys Start keys%{public}s", keys.at(0).c_str());
    SetOperationList(IN_KEY, keys, {}, {}, ONE_COUNT);
    LOG_DEBUG("DataSharePredicates::InKeys End");
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
    LOG_DEBUG("DataSharePredicates::SetOrder Start order%{public}s", order.c_str());
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
    const DataSharePredicatesObject &para2, const DataSharePredicatesObject &para3, ParameterCount parameterCount)
{
    LOG_DEBUG("DataSharePredicates::SetOperationList Start");
    OperationItem operationItem {};
    operationItem.operation = operationType;
    operationItem.para1 = para1;
    operationItem.para2 = para2;
    operationItem.para3 = para3;
    operationItem.parameterCount = parameterCount;
    operationList_.push_back(operationItem);
    if (settingMode_ != PREDICATES_METHOD) {
        ClearQueryLanguage();
        settingMode_ = PREDICATES_METHOD;
    }
    LOG_DEBUG("DataSharePredicates::SetOperationList END settingMode_%{public}d", settingMode_);
}
} // namespace DataShare
} // namespace OHOS