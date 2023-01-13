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
#define LOG_TAG "DataShareAbsResultSet"
#include "datashare_abs_result_set.h"

#include <algorithm>
#include <vector>

#include "adaptor.h"
#include "datashare_errno.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
DataShareAbsResultSet::DataShareAbsResultSet() : rowPos_(INIT_POS), count_(-1), isClosed_(false)
{}

DataShareAbsResultSet::~DataShareAbsResultSet() {}

int DataShareAbsResultSet::GetRowCount(int &count)
{
    return E_OK;
}

int DataShareAbsResultSet::GetAllColumnNames(std::vector<std::string> &columnNames)
{
    return E_OK;
}

int DataShareAbsResultSet::GetBlob(int columnIndex, std::vector<uint8_t> &blob)
{
    return E_OK;
}

int DataShareAbsResultSet::GetString(int columnIndex, std::string &value)
{
    return E_OK;
}

int DataShareAbsResultSet::GetInt(int columnIndex, int &value)
{
    return E_OK;
}

int DataShareAbsResultSet::GetLong(int columnIndex, int64_t &value)
{
    return E_OK;
}

int DataShareAbsResultSet::GetDouble(int columnIndex, double &value)
{
    return E_OK;
}

int DataShareAbsResultSet::IsColumnNull(int columnIndex, bool &isNull)
{
    return E_OK;
}

int DataShareAbsResultSet::GoToRow(int position)
{
    return E_OK;
}

int DataShareAbsResultSet::GetDataType(int columnIndex, DataType &dataType)
{
    return E_OK;
}

int DataShareAbsResultSet::GetRowIndex(int &position) const
{
    position = rowPos_;
    return E_OK;
}

int DataShareAbsResultSet::GoTo(int offset)
{
    int ret = GoToRow(rowPos_ + offset);
    if (ret != E_OK) {
        LOG_ERROR("return ret is wrong!");
    }
    return ret;
}

int DataShareAbsResultSet::GoToFirstRow()
{
    DISTRIBUTED_DATA_HITRACE(std::string(__FUNCTION__));
    int ret = GoToRow(0);
    if (ret != E_OK) {
        LOG_ERROR("return ret is wrong!");
    }
    return ret;
}

int DataShareAbsResultSet::GoToLastRow()
{
    int rowCnt = 0;
    int ret = GetRowCount(rowCnt);
    if (ret != E_OK) {
        LOG_ERROR("return GetRowCount ret is wrong!");
        return ret;
    }

    ret = GoToRow(rowCnt - 1);
    if (ret != E_OK) {
        LOG_ERROR("return GoToRow ret is wrong!");
    }
    return ret;
}

int DataShareAbsResultSet::GoToNextRow()
{
    int ret = GoToRow(rowPos_ + 1);
    if (ret != E_OK) {
        LOG_ERROR("return GoToRow ret is wrong!");
    }
    return ret;
}

int DataShareAbsResultSet::GoToPreviousRow()
{
    int ret = GoToRow(rowPos_ - 1);
    if (ret != E_OK) {
        LOG_ERROR("return GoToRow ret is wrong!");
    }
    return ret;
}

int DataShareAbsResultSet::IsAtFirstRow(bool &result) const
{
    result = (rowPos_ == 0);
    return E_OK;
}

int DataShareAbsResultSet::IsAtLastRow(bool &result)
{
    int rowCnt = 0;
    int ret = GetRowCount(rowCnt);
    if (ret != E_OK) {
        LOG_ERROR("return GetRowCount ret is wrong!");
        return ret;
    }
    result = (rowPos_ == (rowCnt - 1));
    return E_OK;
}

int DataShareAbsResultSet::IsStarted(bool &result) const
{
    result = (rowPos_ != INIT_POS);
    return E_OK;
}

int DataShareAbsResultSet::IsEnded(bool &result)
{
    int rowCnt = 0;
    int ret =  GetRowCount(rowCnt);
    if (ret != E_OK) {
        LOG_ERROR("return GetRowCount ret is wrong!");
        return ret;
    }
    result = (rowCnt == 0) ? true : (rowPos_ == rowCnt);
    return E_OK;
}

int DataShareAbsResultSet::GetColumnCount(int &count)
{
    if (count_ == -1) {
        std::vector<std::string> columnNames;
        int ret = GetAllColumnNames(columnNames);
        if (ret != E_OK) {
            LOG_ERROR("return GetAllColumnNames ret is wrong!");
            return ret;
        }
        count_ = static_cast<int>(columnNames.size());
    }
    count = count_;
    return E_OK;
}

int DataShareAbsResultSet::GetColumnIndex(const std::string &columnName, int &columnIndex)
{
    if (indexCache_.find(columnName) != indexCache_.end()) {
        columnIndex = indexCache_[columnName];
        return E_OK;
    }
    auto periodIndex = columnName.rfind('.');
    std::string columnNameLower = columnName;
    if (periodIndex != std::string::npos) {
        columnNameLower = columnNameLower.substr(periodIndex + 1);
    }
    transform(columnNameLower.begin(), columnNameLower.end(), columnNameLower.begin(), ::tolower);
    std::vector<std::string> columnNames;
    int ret = GetAllColumnNames(columnNames);
    if (ret != E_OK) {
        LOG_ERROR("return GetAllColumnNames ret is wrong!");
        return ret;
    }

    columnIndex = 0;
    for (const auto& name : columnNames) {
        std::string lowerName = name;
        transform(name.begin(), name.end(), lowerName.begin(), ::tolower);
        if (lowerName == columnNameLower) {
            indexCache_[columnName] = columnIndex;
            return E_OK;
        }
        columnIndex++;
    }
    columnIndex = -1;
    return E_ERROR;
}

int DataShareAbsResultSet::GetColumnName(int columnIndex, std::string &columnName)
{
    int rowCnt = 0;
    int ret = GetColumnCount(rowCnt);
    if (ret != E_OK) {
        LOG_ERROR("return GetColumnCount ret is wrong!");
        return ret;
    }
    if (columnIndex >= rowCnt || columnIndex < 0) {
        return E_INVALID_COLUMN_INDEX;
    }
    std::vector<std::string> columnNames;
    GetAllColumnNames(columnNames);
    columnName = columnNames[columnIndex];
    return E_OK;
}

bool DataShareAbsResultSet::IsClosed() const
{
    return isClosed_;
}

int DataShareAbsResultSet::Close()
{
    isClosed_ = true;
    return E_OK;
}
} // namespace DataShare
} // namespace OHOS