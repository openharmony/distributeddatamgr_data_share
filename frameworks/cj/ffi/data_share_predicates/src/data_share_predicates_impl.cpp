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

#include "data_share_predicates_impl.h"

#include "data_share_predicates_utils.h"

namespace OHOS {
namespace DataShare {
DataSharePredicatesImpl::DataSharePredicatesImpl()
{
    predicates_ = std::make_shared<DataSharePredicates>();
}

std::shared_ptr<DataSharePredicates> DataSharePredicatesImpl::GetPredicates()
{
    return this->predicates_;
}

void DataSharePredicatesImpl::EqualTo(const char *field, CValueType value)
{
    std::string cField = field;
    SingleValue::Type valueObject = parseValueType(value);
    predicates_->EqualTo(cField, valueObject);
}

void DataSharePredicatesImpl::And()
{
    predicates_->And();
}

void DataSharePredicatesImpl::OrderByAsc(const char *field)
{
    std::string cField = field;
    predicates_->OrderByAsc(cField);
}

void DataSharePredicatesImpl::OrderByDesc(const char *field)
{
    std::string cField = field;
    predicates_->OrderByDesc(cField);
}

void DataSharePredicatesImpl::Limit(const int32_t total, const int32_t offset)
{
    predicates_->Limit(static_cast<int>(total), static_cast<int>(offset));
}

void DataSharePredicatesImpl::In(const char *field, CValueType *values, int64_t valuesSize)
{
    std::string cField = field;
    auto valuesArray = parseValueTypeArray(values, valuesSize);
    predicates_->In(cField, valuesArray);
}

void DataSharePredicatesImpl::Or()
{
    predicates_->Or();
}

void DataSharePredicatesImpl::BeginWrap()
{
    predicates_->BeginWrap();
}

void DataSharePredicatesImpl::EndWrap()
{
    predicates_->EndWrap();
}
} // namespace DataShare
} // namespace OHOS