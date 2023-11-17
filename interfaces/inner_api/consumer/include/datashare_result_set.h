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

#ifndef DATASHARE_RESULT_SET_H
#define DATASHARE_RESULT_SET_H

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "datashare_abs_result_set.h"
#include "datashare_errno.h"
#include "datashare_shared_result_set.h"
#include "message_parcel.h"
#include "result_set_bridge.h"

namespace OHOS {
namespace DataShare {
class DataShareBlockWriterImpl;

/**
 * This module provides data resultsets.
 */
class DataShareResultSet : public DataShareAbsResultSet, public DataShareSharedResultSet {
public:
    DataShareResultSet();
    explicit DataShareResultSet(std::shared_ptr<ResultSetBridge> &bridge);
    virtual ~DataShareResultSet();

    /**
     * @brief Get the data whose value type is blob from the database according to the columnIndex.
     *
     * @param columnIndex the zero-based index of the target column.
     * @param glob Indicates the value of glob type data to put.
     *
     * @return Return the value of the requested column as a byte array.
     */
    int GetBlob(int columnIndex, std::vector<uint8_t> &blob) override;

    /**
     * @brief Get the data whose value type is string from the database according to the columnIndex.
     *
     * @param columnIndex the zero-based index of the target column.
     * @param value Indicates the value of columnIndex data to put or update.
     *
     * @return Return the value of the requested column as a String.
     */
    int GetString(int columnIndex, std::string &value) override;

    /**
     * @brief Get the data whose value type is int from the database according to the columnIndex.
     *
     * @param columnIndex the zero-based index of the target column.
     * @param value Indicates the value of columnIndex data to put or update.
     *
     * @return Return the value of the requested column as a int.
     */
    int GetInt(int columnIndex, int &value) override;

    /**
     * @brief Get the data whose value type is long from the database according to the columnIndex.
     *
     * @param columnIndex the zero-based index of the target column.
     * @param value Indicates the value of columnIndex data to put or update.
     *
     * @return Return the value of the requested column as a long.
     */
    int GetLong(int columnIndex, int64_t &value) override;

    /**
     * @brief Get the data whose value type is double from the database according to the columnIndex.
     *
     * @param columnIndex the zero-based index of the target column.
     * @param value Indicates the value of columnIndex data to put or update.
     *
     * @return Return the value of the requested column as a double.
     */
    int GetDouble(int columnIndex, double &value) override;

    /**
     * @brief Get the data whose value type is isNull from the database according to the columnIndex.
     *
     * @param columnIndex the zero-based index of the target column.
     * @param isNull Indicates the value of glob type data to put.
     *
     * @return Return whether the column value is null.
     */
    int IsColumnNull(int columnIndex, bool &isNull) override;

    /**
     * @brief data type of the given column's value.
     *
     * @param columnIndex the zero-based index of the target column.
     * @param Place the obtained data type.
     *
     * @return Return column value type.
     */
    int GetDataType(int columnIndex, DataType &dataType) override;

    /**
     * @brief Go to row based on position.
     *
     * @param position the zero-based position to move to.
     *
     * @return Return whether the requested move succeeded.
     */
    int GoToRow(int position) override;

    /**
     * @brief Returns a string array holding the names of all of the columns in the result set.
     *
     * @return Return the names of the columns contains in this query result.
     */
    int GetAllColumnNames(std::vector<std::string> &columnNames) override;

    /**
     * @return Return the numbers of rows in the result set.
     */
    int GetRowCount(int &count) override;

    /**
     * Obtains a block from the SharedResultSet.
     */
    AppDataFwk::SharedBlock *GetBlock() const override;

    /**
     * Called when the position of the result set changes.
     */
    bool OnGo(int startRowIndex, int targetRowIndex, int *cachedIndex = nullptr) override;

    /**
     * Adds the data of a SharedResultSet to a SharedBlock.
     */
    void FillBlock(int startRowIndex, AppDataFwk::SharedBlock *block) override;

    /**
     * SetBlock.
     */
    virtual void SetBlock(AppDataFwk::SharedBlock *block);

    /**
     * Closes the result set, releasing all of its resources and making it completely invalid.
     */
    int Close() override;

    /**
     * Checks whether an DataShareResultSet object contains shared blocks.
     */
    bool HasBlock() const;

protected:
    int CheckState(int columnIndex);
    void ClosedBlock();
    virtual void Finalize();

    friend class ISharedResultSetStub;
    friend class ISharedResultSetProxy;
    bool Unmarshalling(MessageParcel &parcel);
    bool Marshalling(MessageParcel &parcel);

private:
    static int blockId_;
    // The actual position of the first row of data in the shareblock
    int startRowPos_ = -1;
    // The actual position of the last row of data in the shareblock
    int endRowPos_ = -1;
    // The SharedBlock owned by this DataShareResultSet
    AppDataFwk::SharedBlock *sharedBlock_  = nullptr;
    std::shared_ptr<DataShareBlockWriterImpl> blockWriter_ = nullptr;
    std::shared_ptr<ResultSetBridge> bridge_ = nullptr;
};
} // namespace DataShare
} // namespace OHOS

#endif // DATASHARE_RESULT_SET_H