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

#ifndef DATASHARE_PREDICATES_H
#define DATASHARE_PREDICATES_H

#include <string>

#include "datashare_abs_predicates.h"
#include "datashare_errno.h"
#include "datashare_predicates_object.h"
#include "datashare_predicates_objects.h"

namespace OHOS {
namespace DataShare {
class DataSharePredicates : public DataShareAbsPredicates {
public:
    DataSharePredicates()
    {
    }
    explicit DataSharePredicates(const std::list<OperationItem> &operList) : operationList_(operList)
    {
    }
    ~DataSharePredicates()
    {
    }
    DataSharePredicates *EqualTo(const std::string &field, const DataSharePredicatesObject &value)
    {
        SetOperationList(EQUAL_TO, field, value);
        return this;
    }
    DataSharePredicates *NotEqualTo(const std::string &field, const DataSharePredicatesObject &value)
    {
        SetOperationList(NOT_EQUAL_TO, field, value);
        return this;
    }
    DataSharePredicates *GreaterThan(const std::string &field, const DataSharePredicatesObject &value)
    {
        SetOperationList(GREATER_THAN, field, value);
        return this;
    }
    DataSharePredicates *LessThan(const std::string &field, const DataSharePredicatesObject &value)
    {
        SetOperationList(LESS_THAN, field, value);
        return this;
    }
    DataSharePredicates *GreaterThanOrEqualTo(const std::string &field, const DataSharePredicatesObject &value)
    {
        SetOperationList(GREATER_THAN_OR_EQUAL_TO, field, value);
        return this;
    }
    DataSharePredicates *LessThanOrEqualTo(const std::string &field, const DataSharePredicatesObject &value)
    {
        SetOperationList(LESS_THAN_OR_EQUAL_TO, field, value);
        return this;
    }
    DataSharePredicates *In(const std::string &field, const DataSharePredicatesObjects &values)
    {
        SetOperationList(SQL_IN, field, values);
        return this;
    }
    DataSharePredicates *NotIn(const std::string &field, const DataSharePredicatesObjects &values)
    {
        SetOperationList(NOT_IN, field, values);
        return this;
    }
    DataSharePredicates *BeginWrap()
    {
        SetOperationList(BEGIN_WARP);
        return this;
    }
    DataSharePredicates *EndWrap()
    {
        SetOperationList(END_WARP);
        return this;
    }
    DataSharePredicates *Or()
    {
        SetOperationList(OR);
        return this;
    }
    DataSharePredicates *And()
    {
        SetOperationList(AND);
        return this;
    }
    DataSharePredicates *Contains(const std::string &field, const std::string &value)
    {
        SetOperationList(CONTAINS, field, value);
        return this;
    }
    DataSharePredicates *BeginsWith(const std::string &field, const std::string &value)
    {
        SetOperationList(BEGIN_WITH, field, value);
        return this;
    }
    DataSharePredicates *EndsWith(const std::string &field, const std::string &value)
    {
        SetOperationList(END_WITH, field, value);
        return this;
    }
    DataSharePredicates *IsNull(const std::string &field)
    {
        SetOperationList(IS_NULL, field);
        return this;
    }
    DataSharePredicates *IsNotNull(const std::string &field)
    {
        SetOperationList(IS_NOT_NULL, field);
        return this;
    }
    DataSharePredicates *Like(const std::string &field, const std::string &value)
    {
        SetOperationList(LIKE, field, value);
        return this;
    }
    DataSharePredicates *Unlike(const std::string &field, const std::string &value)
    {
        SetOperationList(UNLIKE, field, value);
        return this;
    }
    DataSharePredicates *Glob(const std::string &field, const std::string &value)
    {
        SetOperationList(GLOB, field, value);
        return this;
    }
    DataSharePredicates *Between(const std::string &field, const std::string &low, const std::string &high)
    {
        SetOperationList(BETWEEN, field, low, high);
        return this;
    }
    DataSharePredicates *NotBetween(const std::string &field, const std::string &low, const std::string &high)
    {
        SetOperationList(NOTBETWEEN, field, low, high);
        return this;
    }
    DataSharePredicates *OrderByAsc(const std::string &field)
    {
        SetOperationList(ORDER_BY_ASC, field);
        return this;
    }
    DataSharePredicates *OrderByDesc(const std::string &field)
    {
        SetOperationList(ORDER_BY_DESC, field);
        return this;
    }
    DataSharePredicates *Distinct()
    {
        SetOperationList(DISTINCT);
        return this;
    }
    DataSharePredicates *Limit(const int number, const int offset)
    {
        SetOperationList(LIMIT, number, offset);
        return this;
    }
    DataSharePredicates *GroupBy(const std::vector<std::string> &fields)
    {
        SetOperationList(GROUP_BY, fields);
        return this;
    }
    DataSharePredicates *IndexedBy(const std::string &indexName)
    {
        SetOperationList(INDEXED_BY, indexName);
        return this;
    }
    DataSharePredicates *KeyPrefix(const std::string &prefix)
    {
        SetOperationList(KEY_PREFIX, prefix);
        return this;
    }
    DataSharePredicates *InKeys(const std::vector<std::string> &keys)
    {
        SetOperationList(IN_KEY, keys);
        return this;
    }
    const std::list<OperationItem> &GetOperationList() const
    {
        return operationList_;
    }
    std::string GetWhereClause() const
    {
        return whereClause_;
    }
    int SetWhereClause(const std::string &whereClause)
    {
        if ((settingMode_ != PREDICATES_METHOD) && (!whereClause.empty())) {
            this->whereClause_ = whereClause;
            settingMode_ = QUERY_LANGUAGE;
            return E_OK;
        }
        return E_ERROR;
    }
    std::vector<std::string> GetWhereArgs() const
    {
        return whereArgs_;
    }
    int SetWhereArgs(const std::vector<std::string> &whereArgs)
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
    std::string GetOrder() const
    {
        return order_;
    }
    int SetOrder(const std::string &order)
    {
        if ((settingMode_ != PREDICATES_METHOD) && (!order.empty())) {
            this->order_ = order;
            settingMode_ = QUERY_LANGUAGE;
            return E_OK;
        }
        return E_ERROR;
    }
    SettingMode GetSettingMode() const
    {
        return settingMode_;
    }
    void SetSettingMode(const SettingMode &settingMode)
    {
        settingMode_ = settingMode;
    }

private:
    void SetOperationList(OperationType operationType, const DataSharePredicatesObjects &param)
    {
        OperationItem operationItem {};
        operationItem.operation = operationType;
        operationItem.multiParams.push_back(param);
        operationList_.push_back(operationItem);
        if (settingMode_ != PREDICATES_METHOD) {
            ClearQueryLanguage();
            settingMode_ = PREDICATES_METHOD;
        }
    }
    void SetOperationList(
        OperationType operationType, const DataSharePredicatesObject &param1, const DataSharePredicatesObjects &param2)
    {
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
    void SetOperationList(OperationType operationType, const DataSharePredicatesObject &para1 = {},
        const DataSharePredicatesObject &para2 = {}, const DataSharePredicatesObject &para3 = {})
    {
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
    void ClearQueryLanguage()
    {
        whereClause_ = "";
        whereArgs_ = {};
        order_ = "";
    }
    std::list<OperationItem> operationList_;
    std::string whereClause_;
    std::vector<std::string> whereArgs_;
    std::string order_;
    SettingMode settingMode_ = {};
};
} // namespace DataShare
} // namespace OHOS
#endif