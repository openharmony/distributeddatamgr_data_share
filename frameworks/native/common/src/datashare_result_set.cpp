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

#define LOG_TAG "datashare_result_set"

#include "datashare_result_set.h"

#include <securec.h>
#include <sstream>

#include "adaptor.h"
#include "datashare_block_writer_impl.h"
#include "datashare_errno.h"
#include "datashare_log.h"
#include "parcel.h"
#include "shared_block.h"
#include "string_ex.h"
#include "ishared_result_set.h"

namespace OHOS {
namespace DataShare {
namespace {
// The default position of the cursor
static const int INITIAL_POS = -1;
static const size_t DEFAULT_SHARE_BLOCK_SIZE = 2 * 1024 * 1024;
} // namespace
int DataShareResultSet::blockId_ = 0;
DataShareResultSet::DataShareResultSet()
{
}

DataShareResultSet::DataShareResultSet(std::shared_ptr<ResultSetBridge> &bridge)
    : bridge_(bridge)
{
    std::string name = "DataShare" + std::to_string(blockId_++);
    blockWriter_ = std::make_shared<DataShareBlockWriterImpl>(name, DEFAULT_SHARE_BLOCK_SIZE);
    if (blockWriter_ == nullptr) {
        return;
    }
    sharedBlock_ = blockWriter_->GetBlock();
}

DataShareResultSet::~DataShareResultSet()
{
    Close();
}

int DataShareResultSet::GetAllColumnNames(std::vector<std::string> &columnNames)
{
    auto bridge = GetBridge();
    if (bridge == nullptr) {
        LOG_ERROR("bridge_ is null!");
        return E_ERROR;
    }
    return bridge->GetAllColumnNames(columnNames);
}

int DataShareResultSet::GetRowCount(int &count)
{
    auto bridge = GetBridge();
    if (bridge == nullptr) {
        LOG_ERROR("bridge_ is null!");
        return E_ERROR;
    }
    return bridge->GetRowCount(count);
}

bool DataShareResultSet::OnGo(int startRowIndex, int targetRowIndex, int *cachedIndex)
{
    auto block = GetBlock();
    auto bridge = GetBridge();
    if (bridge == nullptr || blockWriter_ == nullptr || block == nullptr) {
        LOG_ERROR("bridge_ or blockWriter_ or sharedBlock_ is null!");
        return false;
    }
    std::vector<std::string> columnNames;
    GetAllColumnNames(columnNames);
    block->Clear();
    block->SetColumnNum(columnNames.size());
    int result = bridge->OnGo(startRowIndex, targetRowIndex, *blockWriter_);
    if (cachedIndex != nullptr) {
        *cachedIndex = result;
    }
    if (result < 0) {
        return false;
    }
    return true;
}

void DataShareResultSet::FillBlock(int startRowIndex, AppDataFwk::SharedBlock *block)
{
    return;
}

/**
 * Get current bridge
 */
std::shared_ptr<ResultSetBridge> DataShareResultSet::GetBridge()
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return bridge_;
}

/**
 * Get current shared block
 */
std::shared_ptr<AppDataFwk::SharedBlock> DataShareResultSet::GetBlock()
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return sharedBlock_;
}

int DataShareResultSet::GetDataType(int columnIndex, DataType &dataType)
{
    auto block = GetBlock();
    if (block == nullptr) {
        LOG_ERROR("sharedBlock is null!");
        return E_ERROR;
    }
    int rowCount = 0;
    GetRowCount(rowCount);
    AppDataFwk::SharedBlock::CellUnit *cellUnit =
        block->GetCellUnit(static_cast<uint32_t>(rowPos_) - startRowPos_, static_cast<uint32_t>(columnIndex));
    if (!cellUnit) {
        return E_ERROR;
    }
    dataType = (DataType)cellUnit->type;
    return E_OK;
}

int DataShareResultSet::GoToRow(int position)
{
    auto block = GetBlock();
    if (block == nullptr) {
        LOG_ERROR("sharedBlock is null!");
        return E_ERROR;
    }
    int rowCnt = 0;
    GetRowCount(rowCnt);
    if (position >= rowCnt) {
        rowPos_ = rowCnt;
        return E_ERROR;
    }
    if (position < 0) {
        rowPos_ = INITIAL_POS;
        return E_ERROR;
    }
    if (position == rowPos_) {
        return E_OK;
    }
    bool result = true;
    if (position > endRowPos_ || position < startRowPos_) {
        int endPos = -1;
        result = OnGo(position, rowCnt - 1, &endPos);
        if (result) {
            startRowPos_ = position;
            endRowPos_ = endPos;
        }
    }

    if (!result) {
        rowPos_ = INITIAL_POS;
        startRowPos_ = INITIAL_POS;
        endRowPos_ = INITIAL_POS;
        return E_ERROR;
    } else {
        rowPos_ = position;
        return E_OK;
    }
}

int DataShareResultSet::GetBlob(int columnIndex, std::vector<uint8_t> &value)
{
    auto block = GetBlock();
    if (block == nullptr) {
        LOG_ERROR("sharedBlock is null!");
        return E_ERROR;
    }
    int errorCode = CheckState(columnIndex);
    if (errorCode != E_OK) {
        return errorCode;
    }

    AppDataFwk::SharedBlock::CellUnit *cellUnit = block->GetCellUnit(rowPos_ - startRowPos_, columnIndex);
    if (!cellUnit) {
        return E_ERROR;
    }

    value.resize(0);
    int type = cellUnit->type;
    if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_BLOB
        || type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_STRING) {
        size_t size;
        const auto *blob = static_cast<const uint8_t *>(block->GetCellUnitValueBlob(cellUnit, &size));
        if (size == 0 || blob == nullptr) {
            LOG_WARN("blob data is empty!");
        } else {
            value.resize(size);
            value.assign(blob, blob + size);
        }
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_INTEGER) {
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL) {
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_FLOAT) {
        return E_OK;
    } else {
        LOG_ERROR("AppDataFwk::SharedBlock::nothing !");
        return E_INVALID_OBJECT_TYPE;
    }
}

int DataShareResultSet::GetString(int columnIndex, std::string &value)
{
    auto block = GetBlock();
    if (block == nullptr) {
        LOG_ERROR("sharedBlock is null!");
        return E_ERROR;
    }
    AppDataFwk::SharedBlock::CellUnit *cellUnit = block->GetCellUnit(rowPos_ - startRowPos_, columnIndex);
    if (!cellUnit) {
        return E_ERROR;
    }
    int type = cellUnit->type;
    if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_STRING) {
        size_t sizeIncludingNull;
        value = std::string(block->GetCellUnitValueString(cellUnit, &sizeIncludingNull));
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL) {
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_INTEGER) {
        int64_t tempValue = cellUnit->cell.longValue;
        value = std::to_string(tempValue);
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_FLOAT) {
        double tempValue = cellUnit->cell.doubleValue;
        std::ostringstream os;
        if (os << tempValue) {
            value = os.str();
        }
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_BLOB) {
        return E_ERROR;
    } else {
        LOG_ERROR("GetString is failed!");
        return E_ERROR;
    }
}

int DataShareResultSet::GetInt(int columnIndex, int &value)
{
    auto block = GetBlock();
    if (block == nullptr) {
        LOG_ERROR("sharedBlock is null!");
        return E_ERROR;
    }
    AppDataFwk::SharedBlock::CellUnit *cellUnit = block->GetCellUnit(rowPos_ - startRowPos_, columnIndex);
    if (!cellUnit) {
        return E_ERROR;
    }
    value = (int)cellUnit->cell.longValue;
    return E_OK;
}

int DataShareResultSet::GetLong(int columnIndex, int64_t &value)
{
    auto block = GetBlock();
    if (block == nullptr) {
        LOG_ERROR("sharedBlock is null!");
        return E_ERROR;
    }
    AppDataFwk::SharedBlock::CellUnit *cellUnit = block->GetCellUnit(rowPos_ - startRowPos_, columnIndex);
    if (!cellUnit) {
        return E_ERROR;
    }

    int type = cellUnit->type;

    if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_INTEGER) {
        value = cellUnit->cell.longValue;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_STRING) {
        size_t sizeIncludingNull;
        const char *tempValue = block->GetCellUnitValueString(cellUnit, &sizeIncludingNull);
        value = ((sizeIncludingNull > 1) && (tempValue != nullptr)) ? long(strtoll(tempValue, nullptr, 0)) : 0L;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_FLOAT) {
        value = (int64_t)cellUnit->cell.doubleValue;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL) {
        value = 0L;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_BLOB) {
        value = 0L;
        return E_OK;
    } else {
        LOG_ERROR("Nothing !");
        return E_INVALID_OBJECT_TYPE;
    }
}

int DataShareResultSet::GetDouble(int columnIndex, double &value)
{
    auto block = GetBlock();
    if (block == nullptr) {
        LOG_ERROR("sharedBlock is null!");
        return E_ERROR;
    }
    int errorCode = CheckState(columnIndex);
    if (errorCode != E_OK) {
        return errorCode;
    }
    AppDataFwk::SharedBlock::CellUnit *cellUnit = block->GetCellUnit(rowPos_ - startRowPos_, columnIndex);
    if (!cellUnit) {
        return E_ERROR;
    }
    int type = cellUnit->type;
    if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_FLOAT) {
        value = cellUnit->cell.doubleValue;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_STRING) {
        size_t sizeIncludingNull;
        const char *tempValue = block->GetCellUnitValueString(cellUnit, &sizeIncludingNull);
        value = ((sizeIncludingNull > 1) && (tempValue != nullptr)) ? strtod(tempValue, nullptr) : 0.0;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_INTEGER) {
        value = static_cast<double>(cellUnit->cell.longValue);
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL) {
        value = 0.0;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_BLOB) {
        value = 0.0;
        return E_OK;
    } else {
        LOG_ERROR("AppDataFwk::SharedBlock::nothing !");
        value = 0.0;
        return E_INVALID_OBJECT_TYPE;
    }
}

int DataShareResultSet::IsColumnNull(int columnIndex, bool &isNull)
{
    auto block = GetBlock();
    if (block == nullptr) {
        LOG_ERROR("sharedBlock is null!");
        return E_ERROR;
    }
    int errorCode = CheckState(columnIndex);
    if (errorCode != E_OK) {
        return errorCode;
    }
    AppDataFwk::SharedBlock::CellUnit *cellUnit = block->GetCellUnit(rowPos_ - startRowPos_, columnIndex);
    if (!cellUnit) {
        return E_ERROR;
    }
    if (cellUnit->type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL) {
        isNull = true;
        return E_OK;
    }
    isNull = false;
    return E_OK;
}

int DataShareResultSet::Close()
{
    DISTRIBUTED_DATA_HITRACE(std::string(__FUNCTION__));
    DataShareAbsResultSet::Close();
    ClosedBlockAndBridge();
    return E_OK;
}

/**
 * Allocates a new shared block to an {@link DataShareResultSet}
 */
void DataShareResultSet::SetBlock(AppDataFwk::SharedBlock *block)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (sharedBlock_ != nullptr) {
        if (sharedBlock_.get() != block) {
            sharedBlock_ = std::shared_ptr<AppDataFwk::SharedBlock>(block);
        }
    } else {
        if (block != nullptr) {
            sharedBlock_ = std::shared_ptr<AppDataFwk::SharedBlock>(block);
        }
    }
}

/**
 * Checks whether an {@code DataShareResultSet} object contains shared blocks
 */
bool DataShareResultSet::HasBlock()
{
    return GetBlock() != nullptr;
}

/**
 * Closes a shared block that is not empty in this {@code DataShareResultSet} object
 */
void DataShareResultSet::ClosedBlockAndBridge()
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    sharedBlock_ = nullptr;
    bridge_ = nullptr;
}

void DataShareResultSet::Finalize()
{
    Close();
}

/**
 * Check current status
 */
int DataShareResultSet::CheckState(int columnIndex)
{
    int cnt = 0;
    GetColumnCount(cnt);
    if (columnIndex >= cnt || columnIndex < 0) {
        return E_INVALID_COLUMN_INDEX;
    }
    if (rowPos_ == INITIAL_POS) {
        return E_INVALID_STATEMENT;
    }
    return E_OK;
}

bool DataShareResultSet::Marshalling(MessageParcel &parcel)
{
    auto block = GetBlock();
    if (block == nullptr) {
        LOG_ERROR("sharedBlock is null.");
        return false;
    }
    return block->WriteMessageParcel(parcel);
}

bool DataShareResultSet::Unmarshalling(MessageParcel &parcel)
{
    auto block = GetBlock();
    if (block != nullptr) {
        LOG_ERROR("sharedBlock is not null.");
        return false;
    }
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    int result = AppDataFwk::SharedBlock::ReadMessageParcel(parcel, sharedBlock);
    SetBlock(sharedBlock);
    if (result < 0) {
        LOG_ERROR("create from parcel error is %{public}d.", result);
    }
    return true;
}

bool DataShareResultSet::Marshal(const std::shared_ptr<DataShareResultSet> resultSet, MessageParcel &parcel)
{
    if (resultSet == nullptr || !DataShare::ISharedResultSet::WriteToParcel(resultSet, parcel)) {
        return false;
    }
    return true;
}

std::shared_ptr<DataShareResultSet> DataShareResultSet::Unmarshal(MessageParcel &parcel)
{
    return DataShare::ISharedResultSet::ReadFromParcel(parcel);
}
}  // namespace DataShare
}  // namespace OHOS
