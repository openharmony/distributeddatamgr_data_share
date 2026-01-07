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

#define LOG_TAG "ANIDatashare"

#include "ani_base_context.h"
#include "datashare_ani.h"
#include "datashare_business_error.h"
#include "datashare_errno.h"
#include "datashare_log.h"
#include "datashare_predicates.h"
#include "datashare_result.h"
#include "datashare_value_object.h"
#include "datashare_values_bucket.h"
#include "ikvstore_data_service.h"
#include "idata_share_service.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "js_proxy.h"
#include "system_ability_definition.h"
#include "tokenid_kit.h"
#include "wrapper.rs.h"
#include <map>
#define UNIMPL_RET_CODE 0

namespace OHOS {
using namespace DataShare;
using namespace DistributedShare::DataShare;
namespace DataShareAni {

std::mutex listMutex_{};
static std::map<std::string, std::list<sptr<ANIDataShareObserver>>> observerMap_;

static std::vector<std::string> convert_rust_vec_to_cpp_vector(const rust::Vec<rust::String>& rust_vec)
{
    std::vector<std::string> cpp_vector;
    for (const auto& rust_str : rust_vec) {
        cpp_vector.push_back(std::string(rust_str));
    }
    return cpp_vector;
}

static std::vector<uint8_t> convert_rust_vec_to_cpp_vector(const rust::Vec<uint8_t>& rust_vec)
{
    std::vector<uint8_t> cpp_vector;
    for (const auto& rust_data : rust_vec) {
        cpp_vector.push_back(rust_data);
    }
    return cpp_vector;
}

static rust::Vec<int32_t> convert_cpp_vector_to_rust_vec(const std::vector<int>& cpp_vec)
{
    rust::Vec<int32_t> rust_vec;
    for (const auto &cpp_data : cpp_vec) {
        rust_vec.push_back(cpp_data);
    }
    return rust_vec;
}

static rust::Vec<uint8_t> convert_cpp_vector_to_rust_vec(const std::vector<uint8_t>& cpp_vec)
{
    rust::Vec<uint8_t> rust_vec;
    for (const auto &cpp_data : cpp_vec) {
        rust_vec.push_back(cpp_data);
    }
    return rust_vec;
}

// @ohos.data.DataShareResultSet.d.ets
rust::Vec<rust::string> GetColumnNames(int64_t resultSetPtr)
{
    std::vector<std::string> columnNames;
    rust::Vec<rust::String> rust_vec;
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSet == nullptr || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return rust_vec;
    }
    int errCode = resultSet->resultSetPtr_->GetAllColumnNames(columnNames);
    if (errCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errCode);
    }
    for (const auto &column_names_data : columnNames) {
        rust_vec.push_back(rust::String(column_names_data));
    }
    return rust_vec;
}

int32_t GetColumnCount(int64_t resultSetPtr)
{
    int32_t count = -1;
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSet == nullptr || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return count;
    }
    int errCode = resultSet->resultSetPtr_->GetColumnCount(count);
    if (errCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errCode);
    }
    return count;
}

int32_t GetRowCount(int64_t resultSetPtr)
{
    int32_t count = -1;
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return count;
    }
    resultSet->resultSetPtr_->GetRowCount(count);
    return count;
}

bool GetIsClosed(int64_t resultSetPtr)
{
    bool result = false;
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSet == nullptr || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return result;
    }
    result = resultSet->resultSetPtr_->IsClosed();
    return result;
}

bool GoToFirstRow(int64_t resultSetPtr)
{
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return false;
    }
    int errCode = resultSet->resultSetPtr_->GoToFirstRow();
    if (errCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errCode);
        return false;
    }
    return true;
}

bool GoToLastRow(int64_t resultSetPtr)
{
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return false;
    }
    int errCode = resultSet->resultSetPtr_->GoToLastRow();
    if (errCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errCode);
        return false;
    }
    return true;
}

bool GoToNextRow(int64_t resultSetPtr)
{
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return false;
    }
    int errCode = resultSet->resultSetPtr_->GoToNextRow();
    if (errCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errCode);
        return false;
    }
    return true;
}

bool GoToPreviousRow(int64_t resultSetPtr)
{
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return false;
    }
    int errCode = resultSet->resultSetPtr_->GoToPreviousRow();
    if (errCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errCode);
        return false;
    }
    return true;
}

bool GoTo(int64_t resultSetPtr, int32_t offset)
{
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return false;
    }
    int errCode = resultSet->resultSetPtr_->GoTo(offset);
    if (errCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errCode);
        return false;
    }
    return true;
}

bool GoToRow(int64_t resultSetPtr, int32_t position)
{
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return false;
    }
    int errCode = resultSet->resultSetPtr_->GoToRow(position);
    if (errCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errCode);
        return false;
    }
    return true;
}

rust::Vec<uint8_t> GetBlob(int64_t resultSetPtr, int32_t columnIndex)
{
    std::vector<uint8_t> blob;
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return rust::Vec<uint8_t>{};
    }
    int errorCode = resultSet->resultSetPtr_->GetBlob(columnIndex, blob);
    if (errorCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errorCode);
        return rust::Vec<uint8_t>{};
    }
    return convert_cpp_vector_to_rust_vec(blob);
}

rust::String GetString(int64_t resultSetPtr, int columnIndex)
{
    std::string strValue;
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
    } else {
        resultSet->resultSetPtr_->GetString(columnIndex, strValue);
    }
    return rust::String(strValue);
}

int64_t GetLong(int64_t resultSetPtr, int columnIndex)
{
    int64_t value = -1;
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return value;
    }
    int errorCode = resultSet->resultSetPtr_->GetLong(columnIndex, value);
    if (errorCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errorCode);
    }
    return value;
}

double GetDouble(int64_t resultSetPtr, int columnIndex)
{
    double value = -1.0;
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return value;
    }
    int errorCode = resultSet->resultSetPtr_->GetDouble(columnIndex, value);
    if (errorCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errorCode);
    }
    return value;
}

void Close(int64_t resultSetPtr)
{
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return;
    }
    int errCode = resultSet->resultSetPtr_->Close();
    if (errCode != E_OK) {
        LOG_ERROR("failed code: %{public}d", errCode);
    }
    resultSet->resultSetPtr_ = nullptr;
}

int GetColumnIndex(int64_t resultSetPtr, rust::String columnName)
{
    std::string name = std::string(columnName);
    int32_t columnIndex = -1;
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return columnIndex;
    }
    int errorCode = resultSet->resultSetPtr_->GetColumnIndex(name, columnIndex);
    if (errorCode != E_OK) {
        LOG_ERROR("failed code:%{public}d columnIndex:%{public}d", errorCode, columnIndex);
    }
    return columnIndex;
}

rust::String GetColumnName(int64_t resultSetPtr, int columnIndex)
{
    std::string strValue;
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return rust::String(strValue);
    }
    int errorCode = resultSet->resultSetPtr_->GetColumnName(columnIndex, strValue);
    if (errorCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errorCode);
    }
    return rust::String(strValue);
}

int32_t GetDataType(int64_t resultSetPtr, int columnIndex)
{
    DataType dataType = DataType::TYPE_NULL;
    auto resultSet = reinterpret_cast<ResultSetHolder*>(resultSetPtr);
    if (resultSetPtr == 0 || resultSet->resultSetPtr_ == nullptr) {
        LOG_ERROR("resultSet is null.");
        return static_cast<int32_t>(DataType::TYPE_NULL);
    }
    int errorCode = resultSet->resultSetPtr_->GetDataType(columnIndex, dataType);
    if (errorCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errorCode);
        return static_cast<int32_t>(DataType::TYPE_NULL);
    }
    return static_cast<int32_t>(dataType);
}

// @ohos.data.dataSharePredicates.d.ets
int64_t DataSharePredicatesNew()
{
    return reinterpret_cast<long long>(new DataSharePredicates);
}

void DataSharePredicatesClean(int64_t predicatesPtr)
{
    delete reinterpret_cast<DataSharePredicates*>(predicatesPtr);
}

void DataSharePredicatesEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    std::string strFiled = std::string(field);
    EnumType type = value_type_get_type(value);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(value);
            predicates->EqualTo(strFiled, std::string(str));
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(value);
            predicates->EqualTo(strFiled, data);
            break;
        }
        case EnumType::BooleanType: {
            bool data = value_type_get_bool(value);
            predicates->EqualTo(strFiled, data);
            break;
        }
        default: {
            LOG_ERROR("Invalid argument! Wrong argument Type");
            break;
        }
    }
}

void DataSharePredicatesNotEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    std::string strFiled = std::string(field);
    EnumType type = value_type_get_type(value);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(value);
            predicates->NotEqualTo(strFiled, std::string(str));
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(value);
            predicates->NotEqualTo(strFiled, data);
            break;
        }
        case EnumType::BooleanType: {
            bool data = value_type_get_bool(value);
            predicates->NotEqualTo(strFiled, data);
            break;
        }
        default: {
            LOG_ERROR("Invalid argument! Wrong argument Type");
            break;
        }
    }
}

void DataSharePredicatesBeginWrap(int64_t predicatesPtr)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->BeginWrap());
}

void DataSharePredicatesEndWrap(int64_t predicatesPtr)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->EndWrap());
}

void DataSharePredicatesOr(int64_t predicatesPtr)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->Or());
}

void DataSharePredicatesAnd(int64_t predicatesPtr)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->And());
}

void DataSharePredicatesContains(int64_t predicatesPtr, rust::String field, rust::String value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->Contains(std::string(field), std::string(value)));
}

void DataSharePredicatesBeginsWith(int64_t predicatesPtr, rust::String field, rust::String value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->BeginsWith(std::string(field), std::string(value)));
}

void DataSharePredicatesEndsWith(int64_t predicatesPtr, rust::String field, rust::String value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->EndsWith(std::string(field), std::string(value)));
}

void DataSharePredicatesIsNull(int64_t predicatesPtr, rust::String field)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->IsNull(std::string(field)));
}

void DataSharePredicatesIsNotNull(int64_t predicatesPtr, rust::String field)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->IsNotNull(std::string(field)));
}

void DataSharePredicatesLike(int64_t predicatesPtr, rust::String field, rust::String value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->Like(std::string(field), std::string(value)));
}

void DataSharePredicatesUnlike(int64_t predicatesPtr, rust::String field, rust::String value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->Unlike(std::string(field), std::string(value)));
}

void DataSharePredicatesGlob(int64_t predicatesPtr, rust::String field, rust::String value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->Glob(std::string(field), std::string(value)));
}

void GetValueType(const ValueType& valueType, std::string& typeStr)
{
    EnumType type = value_type_get_type(valueType);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(valueType);
            typeStr = std::string(str);
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(valueType);
            typeStr = std::to_string(data);
            break;
        }
        case EnumType::BooleanType: {
            bool data = value_type_get_bool(valueType);
            typeStr = std::to_string(data);
            break;
        }
        case EnumType::I64Type: {
            int64_t data = value_type_get_i64(valueType);
            typeStr = std::to_string(data);
            break;
        }
        default: {
            LOG_ERROR("Invalid argument! Wrong argument Type");
            break;
        }
    }
}

void DataSharePredicatesBetween(int64_t predicatesPtr, rust::String field, const ValueType& low,
    const ValueType& high)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    std::string strLow;
    std::string strHigh;
    GetValueType(low, strLow);
    GetValueType(high, strHigh);
    (void)(*predicates->Between(std::string(field), strLow, strHigh));
}

void DataSharePredicatesNotBetween(int64_t predicatesPtr, rust::String field, const ValueType& low,
    const ValueType& high)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    std::string strLow;
    std::string strHigh;
    GetValueType(low, strLow);
    GetValueType(high, strHigh);
    (void)(*predicates->NotBetween(std::string(field), strLow, strHigh));
}

void DataSharePredicatesGreaterThan(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    EnumType type = value_type_get_type(value);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(value);
            (void)(*predicates->GreaterThan(std::string(field), std::string(str)));
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(value);
            (void)(*predicates->GreaterThan(std::string(field), data));
            break;
        }
        default: {
            LOG_ERROR("Invalid argument! Wrong argument Type");
            break;
        }
    }
}

void DataSharePredicatesGreaterThanOrEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    EnumType type = value_type_get_type(value);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(value);
            (void)(*predicates->GreaterThanOrEqualTo(std::string(field), std::string(str)));
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(value);
            (void)(*predicates->GreaterThanOrEqualTo(std::string(field), data));
            break;
        }
        default: {
            LOG_ERROR("Invalid argument! Wrong argument Type");
            break;
        }
    }
}

void DataSharePredicatesLessThanOrEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    EnumType type = value_type_get_type(value);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(value);
            (void)(*predicates->LessThanOrEqualTo(std::string(field), std::string(str)));
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(value);
            (void)(*predicates->LessThanOrEqualTo(std::string(field), data));
            break;
        }
        default: {
            LOG_ERROR("Invalid argument! Wrong argument Type");
            break;
        }
    }
}

void DataSharePredicatesLessThan(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    EnumType type = value_type_get_type(value);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(value);
            (void)(*predicates->LessThan(std::string(field), std::string(str)));
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(value);
            (void)(*predicates->LessThan(std::string(field), data));
            break;
        }
        default: {
            LOG_ERROR("Invalid argument! Wrong argument Type");
            break;
        }
    }
}

void DataSharePredicatesOrderByAsc(int64_t predicatesPtr, rust::String field)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->OrderByAsc(std::string(field)));
}

void DataSharePredicatesOrderByDesc(int64_t predicatesPtr, rust::String field)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->OrderByDesc(std::string(field)));
}

void DataSharePredicatesDistinct(int64_t predicatesPtr)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->Distinct());
}

void DataSharePredicatesLimit(int64_t predicatesPtr, int total, int offset)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->Limit(total, offset));
}

void DataSharePredicatesGroupBy(int64_t predicatesPtr, rust::Vec<rust::String> field)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    std::vector<std::string> strings;
    for (const auto &str : field) {
        strings.push_back(std::string(str));
    }
    (void)(*predicates->GroupBy(strings));
}

void DataSharePredicatesIndexedBy(int64_t predicatesPtr, rust::String field)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->IndexedBy(std::string(field)));
}

void DataSharePredicatesIn(int64_t predicatesPtr, rust::String field,  rust::Vec<ValueType> value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    std::vector<std::string> values;
    for (const ValueType& v : value) {
        EnumType type = value_type_get_type(v);
        switch (type) {
            case EnumType::StringType: {
                rust::String str = value_type_get_string(v);
                values.push_back(std::string(str));
                break;
            }
            case EnumType::F64Type: {
                double data = value_type_get_f64(v);
                values.push_back(std::to_string(data));
                break;
            }
            case EnumType::BooleanType: {
                bool data = value_type_get_bool(v);
                values.push_back(std::to_string(data));
                break;
            }
            default: {
                LOG_ERROR("Invalid argument! Wrong argument Type");
                break;
            }
        }
    }
    (void)(*predicates->In(std::string(field), values));
}

void DataSharePredicatesNotIn(int64_t predicatesPtr, rust::String field, rust::Vec<ValueType> value)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    std::vector<std::string> values;
    for (const ValueType& v : value) {
        EnumType type = value_type_get_type(v);
        switch (type) {
            case EnumType::StringType: {
                rust::String str = value_type_get_string(v);
                values.push_back(std::string(str));
                break;
            }
            case EnumType::F64Type: {
                double data = value_type_get_f64(v);
                values.push_back(std::to_string(data));
                break;
            }
            case EnumType::BooleanType: {
                bool data = value_type_get_bool(v);
                values.push_back(std::to_string(data));
                break;
            }
            default: {
                LOG_ERROR("Invalid argument! Wrong argument Type");
                break;
            }
        }
    }
    (void)(*predicates->NotIn(std::string(field), values));
}

void DataSharePredicatesPrefixKey(int64_t predicatesPtr, rust::String prefix)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    (void)(*predicates->KeyPrefix(std::string(prefix)));
}

void DataSharePredicatesInKeys(int64_t predicatesPtr, rust::Vec<rust::String> keys)
{
    auto predicates = reinterpret_cast<DataSharePredicates*>(predicatesPtr);
    if (predicates == nullptr) {
        LOG_ERROR("predicates is null.");
        return;
    }
    auto strings = convert_rust_vec_to_cpp_vector(keys);
    (void)(*predicates->InKeys(strings));
}

// @ohos.data.dataShare.d.ets
I64ResultWrap DataShareNativeCreate(int64_t context, rust::String strUri,
    bool optionIsUndefined, bool isProxy, int waitTime)
{
    uint64_t tokenId = IPCSkeleton::GetSelfTokenID();
    if (!Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(tokenId)) {
        return I64ResultWrap{0, EXCEPTION_SYSTEMAPP_CHECK};
    }
    
    if (context == 0) {
        LOG_ERROR("context is nullptr, create dataShareHelper failed.");
        return I64ResultWrap{0, EXCEPTION_PARAMETER_CHECK};
    }
    std::string stdStrUri = std::string(strUri);
    std::shared_ptr<AbilityRuntime::Context> weakContext =
        reinterpret_cast<std::weak_ptr<AbilityRuntime::Context>*>(context)->lock();
    if (weakContext == nullptr) {
        LOG_ERROR("weakContext is nullptr, create dataShareHelper failed.");
        return I64ResultWrap{0, EXCEPTION_PARAMETER_CHECK};
    }
    std::shared_ptr<DataShareHelper> dataShareHelper;
    if (optionIsUndefined) {
        dataShareHelper = DataShareHelper::Creator(weakContext->GetToken(), stdStrUri, "", waitTime, true);
    } else {
        CreateOptions options = {
            isProxy,
            weakContext->GetToken(),
            Uri(stdStrUri).GetScheme() == "datashareproxy",
            waitTime
        };
        dataShareHelper = DataShareHelper::Creator(stdStrUri, options, "", waitTime, true);
    }
    if (dataShareHelper == nullptr) {
        LOG_ERROR("create dataShareHelper failed.");
        return I64ResultWrap{0, EXCEPTION_HELPER_UNINITIALIZED};
    }
    auto helperHolder = new SharedPtrHolder(dataShareHelper);
    return I64ResultWrap{reinterpret_cast<long long>(helperHolder), E_OK};
}

void DataShareNativeClean(int64_t dataShareHelperPtr)
{
    delete reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
}

int DataShareNativeEnableSilentProxy(int64_t context, rust::String strUri)
{
    if (context == 0) {
        LOG_ERROR("DataShareNativeEnableSilentProxy context is null");
        return EXCEPTION_PARAMETER_CHECK;
    }
    std::string stdStrUri = std::string(strUri);
    Uri uri(stdStrUri);
    int res = DataShareHelper::SetSilentSwitch(uri, true, true);
    if (res == E_NOT_SYSTEM_APP) {
        return EXCEPTION_SYSTEMAPP_CHECK;
    }
    return E_OK;
}

int DataShareNativeDisableSilentProxy(int64_t context, rust::String strUri)
{
    if (context == 0) {
        LOG_ERROR("DataShareNativeDisableSilentProxy context is null");
        return EXCEPTION_PARAMETER_CHECK;
    }
    std::string stdStrUri = std::string(strUri);
    Uri uri(stdStrUri);
    int res = DataShareHelper::SetSilentSwitch(uri, false, true);
    if (res == E_NOT_SYSTEM_APP) {
        return EXCEPTION_SYSTEMAPP_CHECK;
    }
    return E_OK;
}

I64ResultWrap DataShareNativeQuery(int64_t dataShareHelperPtr, rust::String strUri,
    int64_t dataSharePredicatesPtr, rust::Vec<rust::String> columns)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("datashareHelper query failed, helper is nullptr.");
        return I64ResultWrap{0, EXCEPTION_HELPER_CLOSED};
    }
    std::string stdStrUri = std::string(strUri);
    std::vector<std::string> std_vector = convert_rust_vec_to_cpp_vector(columns);
    DatashareBusinessError businessError;
    Uri uri(stdStrUri);
    std::shared_ptr<DataShareResultSet> resultSet = helperHolder->datashareHelper_->Query(uri,
        *reinterpret_cast<DataSharePredicates*>(dataSharePredicatesPtr), std_vector, &businessError);
    if (resultSet == nullptr) {
        LOG_ERROR("datashareHelper query failed, resultSet is nullptr.");
        return I64ResultWrap{0, EXCEPTION_INNER};
    }

    if (businessError.GetCode() != 0) {
        LOG_ERROR("datashareHelper query falied, errorCode: %{public}d.", businessError.GetCode());
        return I64ResultWrap{0, EXCEPTION_INNER};
    }
    auto resultSetHolder = new ResultSetHolder(resultSet);
    return I64ResultWrap{reinterpret_cast<long long>(resultSetHolder), E_OK};
}

void GetValuesBucketWrap(const rust::Vec<ValuesBucketKvItem> &bucket, DataShareValuesBucket &valuesBucket)
{
    for (const ValuesBucketKvItem& cpp_bucket : bucket) {
        rust::String bucket_key = value_bucket_get_key(cpp_bucket);
        std::string key = std::string(bucket_key);
        EnumType bucket_type = value_bucket_get_vtype(cpp_bucket);
        switch (bucket_type) {
            case EnumType::StringType: {
                rust::String str = value_bucket_get_string(cpp_bucket);
                std::string stdStr = std::string(str);
                valuesBucket.Put(key, stdStr);
                break;
            }
            case EnumType::F64Type: {
                double data = value_bucket_get_f64(cpp_bucket);
                valuesBucket.Put(key, data);
                break;
            }
            case EnumType::BooleanType: {
                bool data = value_bucket_get_bool(cpp_bucket);
                valuesBucket.Put(key, data);
                break;
            }
            case EnumType::Uint8ArrayType: {
                rust::Vec<uint8_t> data = value_bucket_get_uint8array(cpp_bucket);
                std::vector<uint8_t> std_vector = convert_rust_vec_to_cpp_vector(data);
                valuesBucket.Put(key, std_vector);
                break;
            }
            case EnumType::NullType: {
                valuesBucket.Put(key, {});
                break;
            }
            default: {
                break;
            }
        }
    }
}

I32ResultWrap DataShareNativeUpdate(int64_t dataShareHelperPtr, rust::String strUri,
    int64_t dataSharePredicatesPtr, rust::Vec<ValuesBucketKvItem> bucket)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("datashareHelper update failed, helper is nullptr.");
        return I32ResultWrap{0, EXCEPTION_HELPER_CLOSED};
    }
    std::string stdStrUri = std::string(strUri);
    DataShareValuesBucket valuesBucket;
    GetValuesBucketWrap(bucket, valuesBucket);
    
    Uri uri(stdStrUri);
    int resultNumber = helperHolder->datashareHelper_->Update(uri,
        *reinterpret_cast<DataSharePredicates*>(dataSharePredicatesPtr), valuesBucket);
    if (resultNumber < 0) {
        LOG_ERROR("datashareHelper update failed, resultNumber: %{public}d.", resultNumber);
        return I32ResultWrap{resultNumber, EXCEPTION_INNER};
    }
    return I32ResultWrap{resultNumber, E_OK};
}

int DataShareNativePublish(int64_t dataShareHelperPtr, rust::Vec<PublishedItem> data,
    rust::String bundleName, VersionWrap version, PublishSretParam& sret)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("datashareHelper publish failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    std::string stdBundleName = std::string(bundleName);
    Data PublishData;
    for (PublishedItem& item : data) {
        rust::String key = published_item_get_key(item);
        std::string strKey = std::string(key);
        rust::String subscriberId = published_item_get_subscriber_id(item);
        std::string strSubscriberId = std::string(published_item_get_subscriber_id(item));
        int64_t llSubscriberId = atoll(strSubscriberId.c_str());
        EnumType data_type = published_item_get_data_type(item);
        switch (data_type) {
            case EnumType::StringType: {
                rust::String data_str = published_item_get_data_string(item);
                std::string strData = std::string(data_str);
                PublishData.datas_.emplace_back(strKey, llSubscriberId, strData);
                break;
            }
            case EnumType::ArrayBufferType: {
                rust::Vec<uint8_t> data_arraybuffer = published_item_get_data_arraybuffer(item);
                std::vector<uint8_t> std_vec;
                for (const auto &dataItem : data_arraybuffer) {
                    std_vec.push_back(dataItem);
                }
                PublishData.datas_.emplace_back(strKey, llSubscriberId, std_vec);
                break;
            }
            default: {
                break;
            }
        }
    }

    std::vector<OperationResult> results = helperHolder->datashareHelper_->Publish(PublishData, stdBundleName);
    for (const auto &result : results) {
        if (result.errCode_ == E_BUNDLE_NAME_NOT_EXIST) {
            return EXCEPTION_DATA_AREA_NOT_EXIST;
        }
        publish_sret_push(sret, rust::String(result.key_), result.errCode_);
    }
    return E_OK;
}

int DataShareNativeGetPublishedData(int64_t dataShareHelperPtr, rust::String bundleName,
    GetPublishedDataSretParam& sret)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("datashareHelper getPublish failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    std::string stdBundleName = std::string(bundleName);
    int errorCode = 0;
    Data publishData = helperHolder->datashareHelper_->GetPublishedData(stdBundleName, errorCode);
    if (errorCode == E_BUNDLE_NAME_NOT_EXIST) {
        return EXCEPTION_DATA_AREA_NOT_EXIST;
    }
    for (const auto &data : publishData.datas_) {
        DataShare::PublishedDataItem::DataType dataItem = data.GetData();
        if (dataItem.index() == 0) {
            std::vector std_vec = std::get<std::vector<uint8_t>>(dataItem);
            rust::Vec<uint8_t> rust_vec;
            for (const auto &u8data : std_vec) {
                rust_vec.push_back(u8data);
            }
            published_data_sret_push_array(sret,
                rust::String(data.key_), rust::String(std::to_string(data.subscriberId_)), rust_vec);
        } else {
            std::string strData = std::get<std::string>(dataItem);
            published_data_sret_push_str(sret,
                rust::String(data.key_), rust::String(std::to_string(data.subscriberId_)), rust::String(strData));
        }
    }
    return E_OK;
}

int DataShareNativeAddTemplate(int64_t dataShareHelperPtr, rust::String uri,
    rust::String subscriberId, const Template& temp)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("AddTemplate failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    rust::String scheduler = template_get_scheduler(temp);
    rust::String update = template_get_update(temp);
    rust::Vec<TemplatePredicatesKvItem> predicates = template_get_predicates(temp);
    std::vector<PredicateTemplateNode> stdPredicates;
    for (TemplatePredicatesKvItem& kv : predicates) {
        rust::String key = template_predicates_get_key(kv);
        rust::String value = template_predicates_get_value(kv);
        stdPredicates.emplace_back(std::string(key), std::string(value));
    }

    std::string tplUri = std::string(uri);
    int64_t llSubscriberId = atoll(std::string(subscriberId).c_str());
    DataShare::Template tpl(std::string(update), stdPredicates, std::string(scheduler));
    auto result = helperHolder->datashareHelper_->AddQueryTemplate(tplUri, llSubscriberId, tpl);
    if (result == E_URI_NOT_EXIST || result == E_BUNDLE_NAME_NOT_EXIST) {
        LOG_ERROR("AddTemplate failed, result: %{public}d.", result);
        return EXCEPTION_URI_NOT_EXIST;
    }
    return E_OK;
}

int DataShareNativeDelTemplate(int64_t dataShareHelperPtr, rust::String uri, rust::String subscriberId)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("DelTemplate update failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    std::string tplUri = std::string(uri);
    int64_t llSubscriberId = atoll(std::string(subscriberId).c_str());
    auto result = helperHolder->datashareHelper_->DelQueryTemplate(tplUri, llSubscriberId);
    if (result == E_URI_NOT_EXIST || result == E_BUNDLE_NAME_NOT_EXIST) {
        LOG_ERROR("DelTemplate failed, result: %{public}d.", result);
        return EXCEPTION_URI_NOT_EXIST;
    }
    return E_OK;
}

I32ResultWrap DataShareNativeInsert(int64_t dataShareHelperPtr, rust::String strUri,
    rust::Vec<ValuesBucketKvItem> bucket)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("datashareHelper insert failed, helper is nullptr.");
        return I32ResultWrap{0, EXCEPTION_HELPER_CLOSED};
    }
    std::string stdStrUri = std::string(strUri);
    DataShareValuesBucket valuesBucket;
    GetValuesBucketWrap(bucket, valuesBucket);

    Uri uri(std::string(strUri).c_str());
    int resultNumber = 0;
    resultNumber = helperHolder->datashareHelper_->Insert(uri, valuesBucket);
    if (resultNumber < 0) {
        LOG_ERROR("datashareHelper insert failed, resultNumber: %{public}d.", resultNumber);
        return I32ResultWrap{resultNumber, EXCEPTION_INNER};
    }
    return I32ResultWrap{resultNumber, E_OK};
}

I32ResultWrap DataShareNativeBatchInsert(int64_t dataShareHelperPtr, rust::String strUri,
    rust::Vec<ValuesBucketWrap> buckets)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("datashareHelper batchInsert failed, helper is nullptr.");
        return I32ResultWrap{0, EXCEPTION_HELPER_CLOSED};
    }
    std::vector<DataShareValuesBucket> valuesBuckets;
    for (ValuesBucketWrap& wrapBuckets : buckets) {
        rust::Vec<ValuesBucketKvItem> const &bucket = values_bucket_wrap_inner(wrapBuckets);
        DataShareValuesBucket valuesBucket;
        GetValuesBucketWrap(bucket, valuesBucket);
        valuesBuckets.push_back(valuesBucket);
    }

    Uri uri(std::string(strUri).c_str());
    int resultNumber = 0;
    resultNumber = helperHolder->datashareHelper_->BatchInsert(uri, valuesBuckets);
    if (resultNumber < 0) {
        LOG_ERROR("datashareHelper batchInsert failed, resultNumber: %{public}d.", resultNumber);
        return I32ResultWrap{resultNumber, EXCEPTION_INNER};
    }
    return I32ResultWrap{resultNumber, E_OK};
}

std::map<std::string, std::vector<UpdateOperation>> DataShareNativeBatchUpdateGetOperations(
    const rust::Vec<rust::String>& vec_key,
    const rust::Vec<int64_t>& vec_predicates,
    const rust::Vec<ValuesBucketWrap>& vec_buckets,
    const rust::Vec<int64_t>& vec_steps)
{
    std::map<std::string, std::vector<UpdateOperation>> operations;
    int sum = 0;
    for (size_t i = 0; i < vec_key.size(); ++i) {
        std::vector<UpdateOperation> op;
        for (int j = 0; j < vec_steps[i]; ++j) {
            DataSharePredicates* predicates = reinterpret_cast<DataSharePredicates*>(vec_predicates[sum + j]);
            rust::Vec<ValuesBucketKvItem> const &bucket = values_bucket_wrap_inner(vec_buckets[sum + j]);
            DataShareValuesBucket valuesBucket;
            std::string key = std::string(value_bucket_get_key(bucket[j]));
            EnumType bucket_type = value_bucket_get_vtype(bucket[j]);
            switch (bucket_type) {
                case EnumType::StringType: {
                    valuesBucket.Put(key, std::string(value_bucket_get_string(bucket[j])));
                    break;
                }
                case EnumType::F64Type: {
                    valuesBucket.Put(key, (double)value_bucket_get_f64(bucket[j]));
                    break;
                }
                case EnumType::BooleanType: {
                    valuesBucket.Put(key, (bool)value_bucket_get_bool(bucket[j]));
                    break;
                }
                case EnumType::I64Type: {
                    valuesBucket.Put(key, (int64_t)value_bucket_get_i64(bucket[j]));
                    break;
                }
                case EnumType::Uint8ArrayType: {
                    rust::Vec<uint8_t> data = value_bucket_get_uint8array(bucket[j]);
                    std::vector<uint8_t> std_vec = convert_rust_vec_to_cpp_vector(data);
                    valuesBucket.Put(key, std_vec);
                    break;
                }
                case EnumType::NullType: {
                    valuesBucket.Put(key, {});
                    break;
                }
                default: {
                    break;
                }
            }
            op.push_back(UpdateOperation{valuesBucket, *predicates});
        }
        sum += vec_steps[i];
        operations[std::string(vec_key[i])] = op;
    }
    return operations;
}

void DataShareNativeBatchUpdate(int64_t dataShareHelperPtr, const DataShareBatchUpdateParamIn& param_in,
    DataShareBatchUpdateParamOut& param_out)
{
    rust::Vec<rust::String> vec_key;
    rust::Vec<int64_t> vec_predicates;
    rust::Vec<ValuesBucketWrap> vec_bucket;
    rust::Vec<int64_t> vec_step;
    data_share_batch_update_param_in_get_value(param_in, vec_key, vec_predicates, vec_bucket, vec_step);
    if (vec_bucket.size() != vec_predicates.size()) {
        LOG_ERROR("vec_bucket size is not equal to vec_predicates size");
        return;
    }
    if (vec_key.size() != vec_step.size()) {
        LOG_ERROR("vec_key size is not equal to vec_step size");
        return;
    }
    auto operations = DataShareNativeBatchUpdateGetOperations(vec_key, vec_predicates, vec_bucket, vec_step);
    std::vector<BatchUpdateResult> results;
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    auto ret = helperHolder->datashareHelper_->BatchUpdate(operations, results);
    if (ret < 0) {
        data_share_batch_update_param_out_error_code(param_out, EXCEPTION_INNER);
        LOG_ERROR("BatchUpdate failed, ret: %{public}d", ret);
        return;
    }
    for (const auto &result : results) {
        data_share_batch_update_param_out_push(param_out, rust::String(result.uri),
            convert_cpp_vector_to_rust_vec(result.codes));
    }
}

StringResultWrap DataShareNativeNormalizeUri(int64_t dataShareHelperPtr, rust::String strUri)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr || strUri.empty()) {
        LOG_ERROR("datashareHelper normalizeUri failed, helper is nullptr.");
        return StringResultWrap{"", EXCEPTION_HELPER_CLOSED};
    }
    Uri uri(std::string(strUri).c_str());
    Uri normalizedUri = helperHolder->datashareHelper_->NormalizeUri(uri);
    return StringResultWrap{normalizedUri.ToString(), 0};
}

StringResultWrap DataShareNativeDeNormalizeUri(int64_t dataShareHelperPtr, rust::String strUri)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr || strUri.empty()) {
        LOG_ERROR("datashareHelper deNormalizeUri failed, helper is nullptr.");
        return StringResultWrap{"", EXCEPTION_HELPER_CLOSED};
    }
    Uri uri(std::string(strUri).c_str());
    Uri denormalizedUri = helperHolder->datashareHelper_->DenormalizeUri(uri);
    return StringResultWrap{denormalizedUri.ToString(), 0};
}

int DataShareNativeNotifyChange(int64_t dataShareHelperPtr, rust::String strUri)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("datashareHelper notifyChange failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    if (strUri.empty()) {
        LOG_ERROR("Parameter error.Mandatory parameters are left unspecified.");
        return EXCEPTION_PARAMETER_CHECK;
    }
    Uri uri(std::string(strUri).c_str());
    helperHolder->datashareHelper_->NotifyChange(uri);
    return E_OK;
}
void ParseValueBucket(rust::Vec<ValuesBucketKvItem> const &bucket,
                      std::map<std::string, std::variant<std::monostate, int64_t, double,
                      std::string, bool, std::vector<uint8_t>>>& valuesBucket)
{
    for (const ValuesBucketKvItem& cpp_bucket : bucket) {
        std::string key = std::string(value_bucket_get_key(cpp_bucket));
        EnumType bucket_type = value_bucket_get_vtype(cpp_bucket);
        switch (bucket_type) {
            case EnumType::StringType: {
                valuesBucket.insert(std::make_pair(key, std::string(value_bucket_get_string(cpp_bucket))));
                break;
            }
            case EnumType::F64Type: {
                valuesBucket.insert(std::make_pair(key, (double)value_bucket_get_f64(cpp_bucket)));
                break;
            }
            case EnumType::BooleanType: {
                valuesBucket.insert(std::make_pair(key, (bool)value_bucket_get_bool(cpp_bucket)));
                break;
            }
            case EnumType::I64Type: {
                valuesBucket.insert(std::make_pair(key, (int64_t)value_bucket_get_i64(cpp_bucket)));
                break;
            }
            case EnumType::Uint8ArrayType: {
                rust::Vec<uint8_t> data = value_bucket_get_uint8array(cpp_bucket);
                valuesBucket.insert(std::make_pair(key, convert_rust_vec_to_cpp_vector(data)));
                break;
            }
            case EnumType::NullType: {
                valuesBucket.insert(std::make_pair(key, std::monostate{}));
                break;
            }
            default: {
                valuesBucket.insert(std::make_pair(key, std::monostate{}));
                break;
            }
        }
    }
}

void rust_change_info_to_cpp_observer_change_info(const int32_t changeType,
    const rust::Vec<ValuesBucketWrap>& buckets, DataShareObserver::ChangeInfo& info)
{
    info.changeType_ = (DataShareObserver::ChangeType)changeType;
    for (const ValuesBucketWrap& wrapBuckets : buckets) {
        rust::Vec<ValuesBucketKvItem> const &bucket = values_bucket_wrap_inner(wrapBuckets);
        std::map<std::string,
            std::variant<std::monostate, int64_t, double, std::string, bool, std::vector<uint8_t>>> valuesBucket;
        ParseValueBucket(bucket, valuesBucket);
        info.valueBuckets_.push_back(valuesBucket);
    }
}

int DataShareNativeNotifyChangeInfo(int64_t dataShareHelperPtr, int32_t changeType,
    rust::String strUri, rust::Vec<ValuesBucketWrap> buckets)
{
    std::string stdStrUri = std::string(strUri);
    if (stdStrUri.empty() || buckets.empty()) {
        LOG_ERROR("stdStrUri or buckets is empty!");
        return EXCEPTION_PARAMETER_CHECK;
    }
    DataShareObserver::ChangeInfo info;
    info.uris_.push_back(Uri(stdStrUri));
    rust_change_info_to_cpp_observer_change_info(changeType, buckets, info);
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("datashareHelper notifyChange failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    helperHolder->datashareHelper_->NotifyChangeExt(info, true);
    return E_OK;
}

I32ResultWrap DataShareNativeDelete(int64_t dataShareHelperPtr, rust::String strUri, int64_t dataSharePredicatesPtr)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("datashareHelper delete failed, helper is nullptr.");
        return I32ResultWrap{0, EXCEPTION_HELPER_CLOSED};
    }
    int resultNumber = 0;
    Uri uri(std::string(strUri).c_str());
    resultNumber = helperHolder->datashareHelper_->Delete(uri,
        *reinterpret_cast<DataSharePredicates*>(dataSharePredicatesPtr));
    if (resultNumber < 0) {
        LOG_ERROR("datashareHelper delete failed, resultNumber: %{public}d.", resultNumber);
        return I32ResultWrap{resultNumber, EXCEPTION_INNER};
    }
    return I32ResultWrap{resultNumber, E_OK};
}

int DataShareNativeClose(int64_t dataShareHelperPtr)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        return E_OK;
    }
    if (!helperHolder->datashareHelper_->Release()) {
        LOG_ERROR("datashareHelper close failed, inner error.");
        return EXCEPTION_INNER;
    }
    helperHolder->datashareHelper_ = nullptr;
    return E_OK;
}

int ANIRegisterObserver(const std::string &strUri, long long dataShareHelperPtr,
    rust::Box<DataShareCallback> &callback, bool isNotifyDetails)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("ANIRegisterObserver failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    std::lock_guard<std::mutex> lck(listMutex_);
    observerMap_.try_emplace(strUri);
    auto &list = observerMap_.find(strUri)->second;
    for (const auto &item : list) {
        if (callback_is_equal(*callback, *(item->observer_->GetCallback()))) {
            LOG_ERROR("observer has already subscribed.");
            return E_OK;
        }
    }

    auto innerObserver = std::make_shared<ANIInnerDataShareObserver>(std::move(callback));
    sptr<ANIDataShareObserver> observer(new (std::nothrow) ANIDataShareObserver(innerObserver));
    if (observer == nullptr) {
        LOG_ERROR("observer is nullptr.");
        return E_OK;
    }
    Uri uri(strUri);
    if (!isNotifyDetails) {
        helperHolder->datashareHelper_->RegisterObserver(uri, observer);
    } else {
        helperHolder->datashareHelper_->RegisterObserverExt(uri,
            std::shared_ptr<DataShareObserver>(observer.GetRefPtr(), [holder = observer](const auto*) {}), false);
    }
    list.push_back(observer);
    return E_OK;
}

int ANIUnRegisterObserver(const std::string &strUri, long long dataShareHelperPtr,
    rust::Box<DataShareCallback> &callback, bool isNotifyDetails)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("ANIUnRegisterObserver failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    std::lock_guard<std::mutex> lck(listMutex_);
    if (observerMap_.find(strUri) == observerMap_.end()) {
        LOG_ERROR("ANIUnRegisterObserver failed, uri does not exist.");
        return E_OK;
    }
    auto &list = observerMap_.find(strUri)->second;
    auto it = list.begin();
    Uri uri(strUri);
    while (it != list.end()) {
        if (!callback_is_equal(*callback, *((*it)->observer_->GetCallback()))) {
            ++it;
            continue;
        }
        if (!isNotifyDetails) {
            helperHolder->datashareHelper_->UnregisterObserver(uri, *it);
        } else {
            helperHolder->datashareHelper_->UnregisterObserverExt(uri,
                std::shared_ptr<DataShareObserver>((*it).GetRefPtr(), [holder = *it](const auto*) {}));
        }
        it = list.erase(it);
        break;
    }
    if (list.empty()) {
        observerMap_.erase(strUri);
    }
    return E_OK;
}

int ANIUnRegisterObserver(const std::string &strUri, long long dataShareHelperPtr, bool isNotifyDetails)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("ANIUnRegisterObserver failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    std::lock_guard<std::mutex> lck(listMutex_);
    if (observerMap_.find(strUri) == observerMap_.end()) {
        LOG_ERROR("ANIUnRegisterObserver failed, uri does not exist.");
        return E_OK;
    }
    auto &list = observerMap_.find(strUri)->second;
    auto it = list.begin();
    Uri uri(strUri);
    while (it != list.end()) {
        if (!isNotifyDetails) {
            helperHolder->datashareHelper_->UnregisterObserver(uri, *it);
        } else {
            helperHolder->datashareHelper_->UnregisterObserverExt(uri,
                std::shared_ptr<DataShareObserver>((*it).GetRefPtr(), [holder = *it](const auto*) {}));
        }
        it = list.erase(it);
    }
    observerMap_.erase(strUri);
    return E_OK;
}

int DataShareNativeOn(PtrWrap ptrWrap, rust::String strUri)
{
    return ANIRegisterObserver(std::string(strUri), ptrWrap.dataShareHelperPtr, ptrWrap.callback);
}

int DataShareNativeOnChangeinfo(PtrWrap ptrWrap, int32_t arktype, rust::String strUri)
{
    return ANIRegisterObserver(std::string(strUri), ptrWrap.dataShareHelperPtr, ptrWrap.callback, true);
}

int DataShareNativeOnRdbDataChange(PtrWrap ptrWrap, rust::Vec<rust::String> uris,
    const TemplateId& templateId, PublishSretParam& sret)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(ptrWrap.dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("OnRdbDataChange failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    std::vector<OperationResult> results;
    std::vector<std::string> stdUris;
    for (const auto &uri : uris) {
        stdUris.push_back(std::string(uri));
    }
    DataShare::TemplateId tplId;
    tplId.subscriberId_ = atoll(std::string(template_id_get_subscriber_id(templateId)).c_str());
    tplId.bundleName_ = std::string(template_id_get_bundle_name_of_owner(templateId));
    helperHolder->jsRdbObsManager_ =
        std::make_shared<AniRdbSubscriberManager>(helperHolder->datashareHelper_);
    if (helperHolder->jsRdbObsManager_ == nullptr) {
        LOG_ERROR("OnRdbDataChange failed, jsRdbObsManager is nullptr");
        return E_OK;
    }
    results = helperHolder->jsRdbObsManager_->AddObservers(ptrWrap.callback, stdUris, tplId);
    for (const auto &result : results) {
        publish_sret_push(sret, rust::String(result.key_), result.errCode_);
    }
    return E_OK;
}

int DataShareNativeOnPublishedDataChange(PtrWrap ptrWrap, rust::Vec<rust::String> uris,
    rust::String subscriberId, PublishSretParam& sret)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(ptrWrap.dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("OnPublishedDataChange failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    std::vector<OperationResult> results;
    std::vector<std::string> stdUris;
    for (const auto &uri : uris) {
        stdUris.push_back(std::string(uri));
    }

    int64_t innerSubscriberId = atoll(std::string(subscriberId).c_str());
    helperHolder->jsPublishedObsManager_ =
        std::make_shared<AniPublishedSubscriberManager>(helperHolder->datashareHelper_);
    if (helperHolder->jsPublishedObsManager_ == nullptr) {
        LOG_ERROR("OnPublishedDataChange failed, jsPublishedObsManager is nullptr");
        return E_OK;
    }
    results = helperHolder->jsPublishedObsManager_->AddObservers(ptrWrap.callback, stdUris, innerSubscriberId);
    for (const auto &result : results) {
        publish_sret_push(sret, rust::String(result.key_), result.errCode_);
    }
    return E_OK;
}

int DataShareNativeOff(PtrWrap ptrWrap, rust::String strUri)
{
    return ANIUnRegisterObserver(std::string(strUri), ptrWrap.dataShareHelperPtr, ptrWrap.callback, false);
}

int DataShareNativeOffNone(int64_t dataShareHelperPtr, rust::String strUri)
{
    return ANIUnRegisterObserver(std::string(strUri), dataShareHelperPtr, false);
}

int DataShareNativeOffChangeinfo(PtrWrap ptrWrap, int32_t arktype, rust::String strUri)
{
    return ANIUnRegisterObserver(std::string(strUri), ptrWrap.dataShareHelperPtr, ptrWrap.callback, true);
}

int DataShareNativeOffChangeinfoNone(int64_t dataShareHelperPtr, int32_t arktype, rust::String strUri)
{
    return ANIUnRegisterObserver(std::string(strUri), dataShareHelperPtr, true);
}

int DataShareNativeOffRdbDataChange(PtrWrap ptrWrap, rust::Vec<rust::String> uris, const TemplateId& templateId,
    PublishSretParam& sret)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(ptrWrap.dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("OffRdbDataChange failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    std::vector<OperationResult> results;
    std::vector<std::string> stdUris;
    for (const auto &uri : uris) {
        stdUris.push_back(std::string(uri));
    }
    DataShare::TemplateId tplId;
    tplId.subscriberId_ = atoll(std::string(template_id_get_subscriber_id(templateId)).c_str());
    tplId.bundleName_ = std::string(template_id_get_bundle_name_of_owner(templateId));
    if (helperHolder->jsRdbObsManager_ == nullptr) {
        LOG_ERROR("OffRdbDataChange failed, jsRdbObsManager is nullptr");
        return E_OK;
    }
    results = helperHolder->jsRdbObsManager_->DelObservers(ptrWrap.callback, stdUris, tplId);
    for (const auto &result : results) {
        publish_sret_push(sret, rust::String(result.key_), result.errCode_);
    }
    return E_OK;
}

int DataShareNativeOffRdbDataChangeNone(int64_t dataShareHelperPtr, rust::Vec<rust::String> uris,
    const TemplateId& templateId, PublishSretParam& sret)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("OffRdbDataChange failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    std::vector<OperationResult> results;
    std::vector<std::string> stdUris;
    for (const auto &uri : uris) {
        stdUris.push_back(std::string(uri));
    }
    DataShare::TemplateId tplId;
    tplId.subscriberId_ = atoll(std::string(template_id_get_subscriber_id(templateId)).c_str());
    tplId.bundleName_ = std::string(template_id_get_bundle_name_of_owner(templateId));
    if (helperHolder->jsRdbObsManager_ == nullptr) {
        LOG_ERROR("OffRdbDataChange failed, jsRdbObsManager is nullptr");
        return E_OK;
    }
    results = helperHolder->jsRdbObsManager_->DelObservers(stdUris, tplId);
    for (const auto &result : results) {
        publish_sret_push(sret, rust::String(result.key_), result.errCode_);
    }
    return E_OK;
}

int DataShareNativeOffPublishedDataChange(PtrWrap ptrWrap, rust::Vec<rust::String> uris, rust::String subscriberId,
    PublishSretParam& sret)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(ptrWrap.dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("OffPublishedDataChange failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    std::vector<OperationResult> results;
    std::vector<std::string> stdUris;
    for (const auto &uri : uris) {
        stdUris.push_back(std::string(uri));
    }
    int64_t innerSubscriberId = atoll(std::string(subscriberId).c_str());
    if (helperHolder->jsPublishedObsManager_ == nullptr) {
        LOG_ERROR("OffPublishedDataChange failed, jsPublishedObsManager is nullptr");
        return E_OK;
    }
    results = helperHolder->jsPublishedObsManager_->DelObservers(ptrWrap.callback, stdUris, innerSubscriberId);
    for (const auto &result : results) {
        publish_sret_push(sret, rust::String(result.key_), result.errCode_);
    }
    return E_OK;
}

int DataShareNativeOffPublishedDataChangeNone(int64_t dataShareHelperPtr, rust::Vec<rust::String> uris,
    rust::String subscriberId, PublishSretParam& sret)
{
    auto helperHolder = reinterpret_cast<SharedPtrHolder *>(dataShareHelperPtr);
    if (helperHolder == nullptr || helperHolder->datashareHelper_ == nullptr) {
        LOG_ERROR("OffPublishedDataChange failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    std::vector<OperationResult> results;
    std::vector<std::string> stdUris;
    for (const auto &uri : uris) {
        stdUris.push_back(std::string(uri));
    }
    int64_t innerSubscriberId = atoll(std::string(subscriberId).c_str());
    if (helperHolder->jsPublishedObsManager_ == nullptr) {
        LOG_ERROR("OffPublishedDataChange failed, jsPublishedObsManager is nullptr");
        return E_OK;
    }
    results = helperHolder->jsPublishedObsManager_->DelObservers(stdUris, innerSubscriberId);
    for (const auto &result : results) {
        publish_sret_push(sret, rust::String(result.key_), result.errCode_);
    }
    return E_OK;
}

void DataShareNativeExtensionCallbackInt(double errorCode, rust::string errorMsg, int32_t data, int64_t nativePtr)
{
    AsyncCallBackPoint* point = reinterpret_cast<AsyncCallBackPoint*>(nativePtr);
    if (point == nullptr) {
        LOG_ERROR("AsyncCallBackPoint is nullptr.");
        return;
    }
    auto resultWrap = point->result;
    if (resultWrap == nullptr) {
        LOG_ERROR("ResultWrap is nullptr.");
        return;
    }
    resultWrap->callbackResultNumber_ = data;
    DatashareBusinessError businessError;
    businessError.SetCode((int)errorCode);
    businessError.SetMessage(std::string(errorMsg));
    resultWrap->businessError_= businessError;
    resultWrap->isRecvReply_ = true;
}

void DataShareNativeExtensionCallbackObject(double errorCode, rust::string errorMsg, int64_t ptr, int64_t nativePtr)
{
    AsyncCallBackPoint* point = reinterpret_cast<AsyncCallBackPoint*>(nativePtr);
    if (point == nullptr) {
        LOG_ERROR("AsyncCallBackPoint is nullptr.");
        return;
    }
    auto resultWrap = point->result;
    if (resultWrap == nullptr) {
        LOG_ERROR("ResultWrap is nullptr.");
        return;
    }

    JSProxy::JSCreator<ResultSetBridge> *proxy = reinterpret_cast<JSProxy::JSCreator<ResultSetBridge> *>(ptr);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr.");
        return;
    }
    std::shared_ptr<ResultSetBridge> resultPtr = proxy->Create();
    resultWrap->callbackResultObject_ = std::make_shared<DataShareResultSet>(resultPtr);
    DatashareBusinessError businessError;
    businessError.SetCode((int)errorCode);
    businessError.SetMessage(std::string(errorMsg));
    resultWrap->businessError_= businessError;
    resultWrap->isRecvReply_ = true;
}

void DataShareNativeExtensionCallbackVoid(double errorCode, rust::string errorMsg, int64_t nativePtr)
{
    AsyncPoint *point = reinterpret_cast<AsyncPoint *>(nativePtr);
    if (point == nullptr) {
        LOG_ERROR("AsyncPoint is nullptr.");
        return;
    }
    if (point->context == nullptr) {
        LOG_ERROR("AsyncContext is nullptr.");
        return;
    }
    if (point->context->isNeedNotify_) {
        auto manager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (manager == nullptr) {
            LOG_ERROR("Get system ability manager failed!");
            return;
        }
        auto remoteObject = manager->CheckSystemAbility(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID);
        if (remoteObject == nullptr) {
            LOG_ERROR("CheckSystemAbility failed!");
            return;
        }
        auto serviceProxy = std::make_shared<DataShareKvServiceProxy>(remoteObject);
        if (serviceProxy == nullptr) {
            LOG_ERROR("Create service proxy failed!");
            return;
        }
        auto remote = serviceProxy->GetFeatureInterface("data_share");
        if (remote == nullptr) {
            LOG_ERROR("Get DataShare service failed!");
            return;
        }
        MessageParcel data;
        MessageParcel reply;
        MessageOption option(MessageOption::TF_ASYNC);
        if (!data.WriteInterfaceToken(DataShare::IDataShareService::GetDescriptor())) {
            LOG_ERROR("Write descriptor failed!");
            return;
        }
        remote->SendRequest(
            static_cast<uint32_t>(DataShareServiceInterfaceCode::DATA_SHARE_SERVICE_CMD_NOTIFY), data, reply, option);
    }
}

void DataShareNativeExtensionCallbackString(double errorCode, rust::String errorMsg,
                                            rust::String value, int64_t nativePtr)
{
    AsyncCallBackPoint* point = reinterpret_cast<AsyncCallBackPoint*>(nativePtr);
    if (point == nullptr) {
        LOG_ERROR("AsyncCallBackPoint is nullptr.");
        return;
    }
    auto jsResult = point->result;
    if (jsResult == nullptr) {
        LOG_ERROR("JsResult is nullptr.");
        return;
    }
    jsResult->callbackResultString_ = std::string(value);
    DatashareBusinessError businessError;
    businessError.SetCode((int)errorCode);
    businessError.SetMessage(std::string(errorMsg));
    jsResult->businessError_= businessError;
    jsResult->isRecvReply_ = true;
}

void DataShareNativeExtensionCallbackBatchUpdate(double errorCode, rust::String errorMsg,
    const ExtensionBatchUpdateParamIn& param_in, int64_t nativePtr)
{
    AsyncCallBackPoint* point = reinterpret_cast<AsyncCallBackPoint*>(nativePtr);
    if (point == nullptr) {
        LOG_ERROR("AsyncCallBackPoint is nullptr.");
        return;
    }
    auto jsResult = point->result;
    if (jsResult == nullptr) {
        LOG_ERROR("JsResult is nullptr.");
        return;
    }
    rust::Vec<rust::String> in_key;
    rust::Vec<int32_t> in_value;
    rust::Vec<int32_t> in_steps;
    extension_batch_update_param_in_get_value(param_in, in_key, in_value, in_steps);
    if (in_key.size() != in_steps.size()) {
        LOG_ERROR("key size not equal step size.");
        return;
    }
    int sum = 0;
    for (size_t i = 0; i < in_key.size(); ++i) {
        BatchUpdateResult bur;
        bur.uri = std::string(in_key[i]);
        for (int j = 0; j < in_steps[i]; ++j) {
            bur.codes.push_back(in_value[sum + j]);
        }
        sum += in_steps[i];
        jsResult->updateResults_.push_back(bur);
    }
    DatashareBusinessError businessError;
    businessError.SetCode((int)errorCode);
    businessError.SetMessage(std::string(errorMsg));
    jsResult->businessError_= businessError;
    jsResult->callbackResultNumber_ = E_OK;
    jsResult->isRecvReply_ = true;
}

int ValidateUrisForDataProxy(rust::Vec<rust::String> uris)
{
    if (uris.empty()) {
        LOG_ERROR("ValidateUrisForDataProxy failed, uris is empty.");
        return EXCEPTION_PARAMETER_CHECK;
    }
    if (uris.size() > URI_MAX_COUNT) {
        LOG_ERROR("the size of uris %{public}zu is over limit", uris.size());
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    auto curis = convert_rust_vec_to_cpp_vector(uris);
    for (const auto &uri : curis) {
        if (uri.size() > URI_MAX_SIZE) {
            LOG_ERROR("the size of uri %{public}s is over limit", uri.c_str());
            return EXCEPTION_PROXY_PARAMETER_CHECK;
        }
    }
    return E_OK;
}

int ValidateDataShareNativePublishParameters(rust::Vec<AniProxyData> proxydata)
{
    std::vector<DataShareProxyData> vec_proxyData;
    for (const auto &item : proxydata) {
        EnumType type = ani_proxy_data_get_enum_type(item);
        DataShareProxyData dspd;
        dspd.uri_ = std::string(ani_proxy_data_get_uri(item));
        if (type == EnumType::StringType) {
            std::string valStr = std::string(ani_proxy_data_get_value_string(item));
            if (valStr.size() > VALUE_MAX_SIZE) {
                LOG_ERROR("ProxyData's value is over limit, uri: %{public}s", dspd.uri_.c_str());
                return EXCEPTION_PROXY_PARAMETER_CHECK;
            }
        }
        if (dspd.uri_.size() > URI_MAX_SIZE) {
            LOG_ERROR("the size of uri %{public}s is over limit", dspd.uri_.c_str());
            return EXCEPTION_PROXY_PARAMETER_CHECK;
        }
        vec_proxyData.push_back(dspd);
    }
    if (vec_proxyData.size() > PROXY_DATA_MAX_COUNT || vec_proxyData.empty()) {
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    return E_OK;
}
} // namespace DataShareAni
} // namespace OHOS
