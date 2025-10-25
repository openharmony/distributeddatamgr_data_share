/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#define LOG_TAG "datashare_result_set_test"

#include <gtest/gtest.h>
#include <unistd.h>

#include "datashare_errno.h"
#include "datashare_itypes_utils.h"
#include "datashare_log.h"
#include "datashare_abs_result_set.h"
#include "datashare_result_set.h"
#include "ikvstore_data_service.h"
#include "ipc_types.h"
#include "ishared_result_set_stub.h"
#include "itypes_util.h"
#include "message_parcel.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
class DatashareResultSetTest : public testing::Test {
public:
    static void SetUpTestCase(void){};
    static void TearDownTestCase(void){};
    void SetUp(){};
    void TearDown(){};
};

/**
 * @tc.name: GetDataTypeTest001
 * @tc.desc: Verify the behavior of the GetDataType function in DataShareResultSet when its 'sharedBlock_' member is
 *           set to nullptr, focusing on whether the function fails and returns the expected error code.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareResultSet objects and DataShare::DataType variables
       without initialization errors.
    2. The DataShareResultSet class initializes its 'sharedBlock_' member to nullptr by default when no explicit
       initialization is performed.
    3. The GetDataType function of DataShareResultSet accepts an integer (columnIndex) and a DataShare::DataType
       reference as parameters, and returns an integer error code (E_ERROR is predefined and accessible).
 * @tc.step:
    1. Create a DataShareResultSet object (dataShareResultSet); its 'sharedBlock_' is default-initialized to nullptr.
    2. Define an integer variable columnIndex and set it to 1 (valid column index value for testing).
    3. Define a DataShare::DataType variable dataType to store the output data type.
    4. Call the GetDataType function of dataShareResultSet, passing columnIndex and dataType as parameters.
    5. Check the integer return value of the GetDataType function.
 * @tc.expect:
    1. The GetDataType function execution fails and returns E_ERROR.
    2. The value of the DataShare::DataType variable dataType remains unmodified (no invalid data is written).
 */
HWTEST_F(DatashareResultSetTest, GetDataTypeTest001, TestSize.Level0)
{
    LOG_INFO("DatashareResultSetTest GetDataTypeTest001::Start");
    DataShareResultSet dataShareResultSet;
    int columnIndex = 1;
    DataShare::DataType dataType;
    auto result = dataShareResultSet.GetDataType(columnIndex, dataType);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DatashareResultSetTest GetDataTypeTest001::End");
}

/**
 * @tc.name: CheckStateTest001
 * @tc.desc: Verify the behavior of the CheckState function in DataShareResultSet when the input 'columnIndex' is
 *           greater than or equal to 0, focusing on whether the function fails and returns the expected error code.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareResultSet objects without initialization errors.
    2. The CheckState function of DataShareResultSet accepts an integer 'columnIndex' as a parameter and returns an
       integer error code (E_INVALID_COLUMN_INDEX is predefined and accessible).
    3. No additional initialization (e.g., setting valid column count) is performed on the DataShareResultSet object
       to trigger the column index error.
 * @tc.step:
    1. Create a DataShareResultSet object (dataShareResultSet) without additional initialization.
    2. Define an integer variable columnIndex and set it to 1 (a value >= 0 for testing).
    3. Call the CheckState function of dataShareResultSet, passing columnIndex as the parameter.
    4. Check the integer return value of the CheckState function.
 * @tc.expect:
    1. The CheckState function execution fails and returns E_INVALID_COLUMN_INDEX.
 */
HWTEST_F(DatashareResultSetTest, CheckStateTest001, TestSize.Level0)
{
    LOG_INFO("DatashareResultSetTest CheckStateTest001::Start");
    DataShareResultSet dataShareResultSet;
    int columnIndex = 1;
    auto result = dataShareResultSet.CheckState(columnIndex);
    EXPECT_EQ(result, E_INVALID_COLUMN_INDEX);
    LOG_INFO("DatashareResultSetTest CheckStateTest001::End");
}

/**
 * @tc.name: CheckStateTest002
 * @tc.desc: Verify the behavior of the CheckState function in DataShareResultSet when the input 'columnIndex' is
 *           less than 0, focusing on whether the function fails and returns the expected error code.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareResultSet objects without initialization errors.
    2. The CheckState function of DataShareResultSet accepts an integer 'columnIndex' as a parameter and returns an
       integer error code (E_INVALID_COLUMN_INDEX is predefined and accessible).
    3. Negative 'columnIndex' values are recognized as invalid by the CheckState function.
 * @tc.step:
    1. Create a DataShareResultSet object (dataShareResultSet) without additional initialization.
    2. Define an integer variable columnIndex and set it to -1 (a value < 0 for testing).
    3. Call the CheckState function of dataShareResultSet, passing columnIndex as the parameter.
    4. Check the integer return value of the CheckState function.
 * @tc.expect:
    1. The CheckState function execution fails and returns E_INVALID_COLUMN_INDEX.
 */
HWTEST_F(DatashareResultSetTest, CheckStateTest002, TestSize.Level0)
{
    LOG_INFO("DatashareResultSetTest CheckState002::Start");
    DataShareResultSet dataShareResultSet;
    int columnIndex = -1;
    auto result = dataShareResultSet.CheckState(columnIndex);
    EXPECT_EQ(result, E_INVALID_COLUMN_INDEX);
    LOG_INFO("DatashareResultSetTest CheckState002::End");
}

/**
 * @tc.name: MarshalTest001
 * @tc.desc: Verify the behavior of the Marshal function in DataShareResultSet when the input 'resultset' (shared
 *           pointer to DataShareResultSet) is set to nullptr, focusing on whether the function fails and returns
 *           false.
 * @tc.type: FUNC
 * @tc.require: issueICCAXH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareResultSet objects, std::shared_ptr<DataShareResultSet>,
       and MessageParcel objects without initialization errors.
    2. The Marshal function of DataShareResultSet accepts a std::shared_ptr<DataShareResultSet> and a MessageParcel
       reference as parameters, and returns a boolean (true for success, false for failure).
    3. A nullptr 'resultset' is recognized as an invalid input by the Marshal function.
 * @tc.step:
    1. Create a DataShareResultSet object (dataShareResultSet) without additional initialization.
    2. Create a std::shared_ptr<DataShareResultSet> variable (resultset) and explicitly set it to nullptr.
    3. Create a MessageParcel object (parcel) to store marshaled data.
    4. Call the Marshal function of dataShareResultSet, passing resultset and parcel as parameters.
    5. Check the boolean return value of the Marshal function.
 * @tc.expect:
    1. The Marshal function execution fails and returns false.
    2. The MessageParcel object parcel remains empty (no invalid data is written).
 */
HWTEST_F(DatashareResultSetTest, MarshalTest001, TestSize.Level0)
{
    LOG_INFO("DatashareResultSetTest MarshalTest001::Start");
    DataShareResultSet dataShareResultSet;
    std::shared_ptr<DataShareResultSet> resultset = nullptr;
    MessageParcel parcel;
    auto result = dataShareResultSet.Marshal(resultset, parcel);
    EXPECT_FALSE(result);
    LOG_INFO("DatashareResultSetTest MarshalTest001::End");
}

/**
 * @tc.name: UnmarshalTest001
 * @tc.desc: Verify the behavior of the Unmarshal function in DataShareResultSet when the input MessageParcel is
 *           uninitialized (contains no valid marshaled data), focusing on whether the function fails and returns
 *           nullptr.
 * @tc.type: FUNC
 * @tc.require: issueICCAXH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareResultSet objects and MessageParcel objects without
       initialization errors.
    2. The Unmarshal function of DataShareResultSet accepts a MessageParcel reference as a parameter and returns a
       std::shared_ptr<DataShareResultSet> (nullptr for failure).
    3. The input MessageParcel is uninitialized (no prior marshaled DataShareResultSet data) to trigger the failure.
 * @tc.step:
    1. Create a DataShareResultSet object (dataShareResultSet) without additional initialization.
    2. Create a MessageParcel object (parcel) without writing any valid marshaled data into it.
    3. Call the Unmarshal function of dataShareResultSet, passing parcel as the parameter.
    4. Check the std::shared_ptr<DataShareResultSet> return value of the Unmarshal function.
 * @tc.expect:
    1. The Unmarshal function execution fails and returns nullptr.
 */
HWTEST_F(DatashareResultSetTest, UnmarshalTest001, TestSize.Level0)
{
    LOG_INFO("DatashareResultSetTest UnmarshalTest001::Start");
    DataShareResultSet dataShareResultSet;
    MessageParcel parcel;
    auto result = dataShareResultSet.Unmarshal(parcel);
    EXPECT_EQ(result, nullptr);
    LOG_INFO("DatashareResultSetTest UnmarshalTest001::End");
}

/**
 * @tc.name: MarshallingTest001
 * @tc.desc: Verify the behavior of the Marshalling function in DataShareResultSet when its 'sharedBlock_' member is
 *           set to nullptr, focusing on whether the function fails and returns false.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareResultSet objects and MessageParcel objects without
       initialization errors.
    2. The DataShareResultSet class initializes its 'sharedBlock_' member to nullptr by default when no explicit
       initialization is performed.
    3. The Marshalling function of DataShareResultSet accepts a MessageParcel reference as a parameter and returns a
       boolean (true for success, false for failure); it depends on 'sharedBlock_' to complete marshaling.
 * @tc.step:
    1. Create a DataShareResultSet object (dataShareResultSet); its 'sharedBlock_' is default-initialized to nullptr.
    2. Create a MessageParcel object (parcel) to store marshaled data.
    3. Call the Marshalling function of dataShareResultSet, passing parcel as the parameter.
    4. Check the boolean return value of the Marshalling function.
 * @tc.expect:
    1. The Marshalling function execution fails and returns false.
    2. The MessageParcel object parcel remains empty (no invalid data is written).
 */
HWTEST_F(DatashareResultSetTest, MarshallingTest001, TestSize.Level0)
{
    LOG_INFO("DatashareResultSetTest MarshallingTest001::Start");
    DataShareResultSet dataShareResultSet;
    MessageParcel parcel;
    auto result = dataShareResultSet.Marshalling(parcel);
    EXPECT_FALSE(result);
    LOG_INFO("DatashareResultSetTest MarshallingTest001::End");
}

/**
 * @tc.name: CloseResulteSetTest001
 * @tc.desc: Verify the behavior of the Close function in DataShareResultSet, focusing on whether the function
 *           successfully sets all its pointer members (sharedBlock_, blockWriter_, bridge_) to nullptr and returns
 *           E_OK.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of DataShareResultSet objects without initialization errors.
    2. The DataShareResultSet class has pointer members: 'sharedBlock_', 'blockWriter_', and 'bridge_', which are
       accessible for null check after calling Close.
    3. The Close function of DataShareResultSet takes no parameters and returns an integer error code (E_OK is
       predefined and accessible).
 * @tc.step:
    1. Create a DataShareResultSet object (dataShareResultSet); its pointer members may be uninitialized or set to
       default values.
    2. Call the Close function of dataShareResultSet and record its integer return value.
    3. Check whether the 'sharedBlock_' member of dataShareResultSet is nullptr.
    4. Check whether the 'blockWriter_' member of dataShareResultSet is nullptr.
    5. Check whether the 'bridge_' member of dataShareResultSet is nullptr.
 * @tc.expect:
    1. The Close function execution succeeds and returns E_OK.
    2. The 'sharedBlock_' member of dataShareResultSet is set to nullptr.
    3. The 'blockWriter_' member of dataShareResultSet is set to nullptr.
    4. The 'bridge_' member of dataShareResultSet is set to nullptr.
 */
HWTEST_F(DatashareResultSetTest, CloseResulteSetTest001, TestSize.Level0)
{
    LOG_INFO("DatashareResultSetTest CloseResulteSetTest001::Start");
    DataShareResultSet dataShareResultSet;
    auto result = dataShareResultSet.Close();
    EXPECT_EQ(result, E_OK);
    ASSERT_EQ(dataShareResultSet.sharedBlock_, nullptr);
    ASSERT_EQ(dataShareResultSet.blockWriter_, nullptr);
    ASSERT_EQ(dataShareResultSet.bridge_, nullptr);
    LOG_INFO("DatashareResultSetTest CloseResulteSetTest001::End");
}
}
}