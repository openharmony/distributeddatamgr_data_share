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

#define LOG_TAG "datashare_abs_result_set_test"

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
#include "gmock/gmock.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
class DataShareAbsResultSetTest : public testing::Test {
public:
    static void SetUpTestCase(void){};
    static void TearDownTestCase(void){};
    void SetUp(){};
    void TearDown(){};
};

class MockDataShareAbsResultSet : public DataShareAbsResultSet {
public:
    MOCK_METHOD1(GoToRow, int(int));
    MOCK_METHOD1(GetRowCount, int(int &));
    MOCK_METHOD1(GetAllColumnNames, int(std::vector<std::string> &));
    MOCK_METHOD1(GetColumnCount, int(int &));
};

class MockDataShareAbsResultSet2 : public DataShareAbsResultSet {
public:
    MOCK_METHOD1(GetAllColumnNames, int(std::vector<std::string> &));
};

/**
 * @tc.name: GoToTest001
 * @tc.desc: Verify the behavior of the GoTo function in DataShareAbsResultSet when the internal GoToRow method
 *           returns E_ERROR, ensuring GoTo propagates the error correctly.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of MockDataShareAbsResultSet (a mock implementation of
       DataShareAbsResultSet).
    2. MockDataShareAbsResultSet allows setting expectations for the GoToRow method (e.g., forcing it to return
       E_ERROR).
    3. The predefined error code E_ERROR is valid and accessible in the test environment.
    4. The GoTo function accepts an integer offset parameter and returns an integer error code.
 * @tc.step:
    1. Create an instance of MockDataShareAbsResultSet named mockResultSet.
    2. Set an expectation on mockResultSet: when GoToRow is called (with any parameter), it returns E_ERROR.
    3. Define an integer offset with value 1, then call the GoTo function of mockResultSet with this offset.
    4. Record the return value of the GoTo function and compare it with E_ERROR.
 * @tc.expect:
    1. The GoTo function execution fails and returns E_ERROR.
 */
HWTEST_F(DataShareAbsResultSetTest, GoToTest001, TestSize.Level0)
{
    LOG_INFO("DataShareAbsResultSetTest GoToTest001::Start");
    MockDataShareAbsResultSet mockResultSet;
    int offset = 1;
    EXPECT_CALL(mockResultSet, GoToRow(testing::_))
        .WillOnce(testing::Return(E_ERROR));
    auto result = mockResultSet.GoTo(offset);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareAbsResultSetTest GoToTest001::End");
}

/**
 * @tc.name: GoToTest002
 * @tc.desc: Verify the normal behavior of the GoTo function in the base DataShareAbsResultSet class (without mock),
 *           ensuring it executes successfully and returns E_OK.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of the base DataShareAbsResultSet class (no mock) without
       initialization errors.
    2. The predefined success code E_OK is valid and accessible in the test environment.
    3. The GoTo function accepts an integer offset parameter and returns an integer error code.
 * @tc.step:
    1. Create an instance of DataShareAbsResultSet named dataShareAbsResultSet.
    2. Define an integer offset with value 1, then call the GoTo function of dataShareAbsResultSet with this offset.
    3. Record the return value of the GoTo function and compare it with E_OK.
 * @tc.expect:
    1. The GoTo function executes successfully and returns E_OK.
 */
HWTEST_F(DataShareAbsResultSetTest, GoToTest002, TestSize.Level0)
{
    LOG_INFO("DataShareAbsResultSetTest GoToTest002::Start");
    DataShareAbsResultSet dataShareAbsResultSet;
    int offset = 1;
    auto result = dataShareAbsResultSet.GoTo(offset);
    EXPECT_EQ(result, E_OK);
    LOG_INFO("DataShareAbsResultSetTest GoToTest002::End");
}

/**
 * @tc.name: IsEndedTest001
 * @tc.desc: Verify the behavior of the IsEnded function in DataShareAbsResultSet when the internal GetRowCount method
 *           returns E_ERROR, ensuring IsEnded propagates the error correctly.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of MockDataShareAbsResultSet.
    2. MockDataShareAbsResultSet allows setting expectations for the GetRowCount method (e.g., forcing it to return
       E_ERROR).
    3. The IsEnded function accepts a non-const boolean reference parameter (to store the "ended" state) and returns an
       integer error code.
    4. The predefined error code E_ERROR is valid and accessible.
 * @tc.step:
    1. Create an instance of MockDataShareAbsResultSet named mockResultSet.
    2. Set an expectation on mockResultSet: when GetRowCount is called (with any integer reference parameter), it
       returns E_ERROR.
    3. Define a boolean variable test (initialized to true), then call the IsEnded function of mockResultSet, passing
       test as the parameter.
    4. Record the return value of the IsEnded function and compare it with E_ERROR.
 * @tc.expect:
    1. The IsEnded function execution fails and returns E_ERROR.
 */
HWTEST_F(DataShareAbsResultSetTest, IsEndedTest001, TestSize.Level0)
{
    LOG_INFO("DataShareAbsResultSetTest IsEndedTest001::Start");
    MockDataShareAbsResultSet mockResultSet;
    EXPECT_CALL(mockResultSet, GetRowCount(testing::_))
        .WillOnce(testing::Return(E_ERROR));
    bool test = true;
    auto result = mockResultSet.IsEnded(test);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareAbsResultSetTest IsEndedTest001::End");
}

/**
 * @tc.name: IsEndedTest002
 * @tc.desc: Verify the normal behavior of the IsEnded function in the base DataShareAbsResultSet class (without mock),
 *           ensuring it executes successfully and returns E_OK.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of the base DataShareAbsResultSet class without initialization
       errors.
    2. The predefined success code E_OK is valid and accessible.
    3. The IsEnded function accepts a non-const boolean reference parameter and returns an integer error code.
 * @tc.step:
    1. Create an instance of DataShareAbsResultSet named dataShareAbsResultSet.
    2. Define a boolean variable test (initialized to true), then call the IsEnded function of dataShareAbsResultSet,
       passing test.
    3. Record the return value of the IsEnded function and compare it with E_OK.
 * @tc.expect:
    1. The IsEnded function executes successfully and returns E_OK.
 */
HWTEST_F(DataShareAbsResultSetTest, IsEndedTest002, TestSize.Level0)
{
    LOG_INFO("DataShareAbsResultSetTest IsEndedTest002::Start");
    DataShareAbsResultSet dataShareAbsResultSet;
    bool test = true;
    auto result = dataShareAbsResultSet.IsEnded(test);
    EXPECT_EQ(result, E_OK);
    LOG_INFO("DataShareAbsResultSetTest IsEndedTest002::End");
}

/**
 * @tc.name: GetColumnCountTest001
 * @tc.desc: Verify the behavior of the GetColumnCount function in MockDataShareAbsResultSet2 when the internal
 *           GetAllColumnNames method returns E_ERROR, ensuring GetColumnCount propagates the error correctly.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of MockDataShareAbsResultSet2 (a mock subclass of
       DataShareAbsResultSet).
    2. MockDataShareAbsResultSet2 allows setting expectations for the GetAllColumnNames method (e.g., forcing it to
       return E_ERROR).
    3. MockDataShareAbsResultSet2 has a public member variable count_ that can be explicitly assigned (e.g., to -1).
    4. The GetColumnCount function accepts an integer reference parameter (to store the count) and returns an integer
       error code.
    5. The predefined error code E_ERROR is valid and accessible.
 * @tc.step:
    1. Create an instance of MockDataShareAbsResultSet2 named mockResultSet.
    2. Assign the count_ member of mockResultSet to -1.
    3. Set an expectation on mockResultSet: when GetAllColumnNames is called (with any vector<string> reference
       parameter), it returns E_ERROR.
    4. Define an integer offset (initialized to -1), then call the GetColumnCount function of mockResultSet, passing
       offset.
    5. Record the return value of GetColumnCount and compare it with E_ERROR.
 * @tc.expect:
    1. The GetColumnCount function execution fails and returns E_ERROR.
 */
HWTEST_F(DataShareAbsResultSetTest, GetColumnCountTest001, TestSize.Level0)
{
    LOG_INFO("DataShareAbsResultSetTest GetColumnCountTest001::Start");
    MockDataShareAbsResultSet2 mockResultSet;
    int offset = -1;
    mockResultSet.count_ = -1;
    EXPECT_CALL(mockResultSet, GetAllColumnNames(testing::_))
        .WillOnce(testing::Return(E_ERROR));
    auto result = mockResultSet.GetColumnCount(offset);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareAbsResultSetTest GetColumnCountTest001::End");
}

/**
 * @tc.name: GetColumnName001
 * @tc.desc: Verify the behavior of the GetColumnName function in DataShareAbsResultSet when the internal
 *           GetColumnCount method returns E_ERROR, ensuring GetColumnName propagates the error correctly.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of MockDataShareAbsResultSet.
    2. MockDataShareAbsResultSet allows setting expectations for the GetColumnCount method (e.g., forcing it to return
       E_ERROR).
    3. The GetColumnName function accepts an integer columnIndex (for the target column) and a string reference (to
       store the name),
       and returns an integer error code.
    4. The predefined error code E_ERROR is valid and accessible.
    5. The test uses a valid columnIndex (e.g., 1) and an empty string (for columnName) as input parameters.
 * @tc.step:
    1. Create an instance of MockDataShareAbsResultSet named mockResultSet.
    2. Set an expectation on mockResultSet: when GetColumnCount is called (with any integer reference parameter), it
       returns E_ERROR.
    3. Define an integer columnIndex (value 1) and a string columnName (initialized to "test").
    4. Call the GetColumnName function of mockResultSet, passing columnIndex and columnName as parameters.
    5. Record the return value of GetColumnName and compare it with E_ERROR.
 * @tc.expect:
    1. The GetColumnName function execution fails and returns E_ERROR.
 */
HWTEST_F(DataShareAbsResultSetTest, GetColumnName001, TestSize.Level0)
{
    LOG_INFO("DataShareAbsResultSetTest GetColumnName001::Start");
    MockDataShareAbsResultSet mockResultSet;
    int columnIndex = 1;
    std::string columnName = "test";
    EXPECT_CALL(mockResultSet, GetColumnCount(testing::_))
        .WillOnce(testing::Return(E_ERROR));
    auto result = mockResultSet.GetColumnName(columnIndex, columnName);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareAbsResultSetTest GetColumnName001::End");
}

/**
 * @tc.name: GetColumnName002
 * @tc.desc: Verify the behavior of the GetColumnName function in the base DataShareAbsResultSet class when the
 *           input columnIndex is ≥ 0 (but invalid, as no columns exist), ensuring it returns E_INVALID_COLUMN_INDEX.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of the base DataShareAbsResultSet class without initialization
       errors.
    2. The base DataShareAbsResultSet has no predefined columns (so any columnIndex ≥ 0 is invalid).
    3. The GetColumnName function accepts an integer columnIndex and a string reference, and returns an integer error
       code.
    4. The predefined error code E_INVALID_COLUMN_INDEX is valid and accessible.
    5. The test uses a columnIndex of 1 (≥ 0, invalid) and a string columnName (initialized to "test") as input.
 * @tc.step:
    1. Create an instance of DataShareAbsResultSet named dataShareAbsResultSet.
    2. Define an integer columnIndex (value 1) and a string columnName (initialized to "test").
    3. Call the GetColumnName function of dataShareAbsResultSet, passing columnIndex and columnName as parameters.
    4. Record the return value of GetColumnName and compare it with E_INVALID_COLUMN_INDEX.
 * @tc.expect:
    1. The GetColumnName function execution fails and returns E_INVALID_COLUMN_INDEX.
 */
HWTEST_F(DataShareAbsResultSetTest, GetColumnName002, TestSize.Level0)
{
    LOG_INFO("DataShareAbsResultSetTest MarshallingTest002::Start");
    DataShareAbsResultSet dataShareAbsResultSet;
    int columnIndex = 1;
    std::string columnName = "test";
    auto result = dataShareAbsResultSet.GetColumnName(columnIndex, columnName);
    EXPECT_EQ(result, E_INVALID_COLUMN_INDEX);
    LOG_INFO("DataShareAbsResultSetTest MarshallingTest002::End");
}

/**
 * @tc.name: GetColumnName003
 * @tc.desc: Verify the behavior of the GetColumnName function in the base DataShareAbsResultSet class when the
 *           input columnIndex is < 0 (invalid), ensuring it returns E_INVALID_COLUMN_INDEX.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of the base DataShareAbsResultSet class without initialization
       errors.
    2. The GetColumnName function accepts an integer columnIndex and a string reference, and returns an integer
       error code.
    3. The predefined error code E_INVALID_COLUMN_INDEX is valid and accessible (for invalid column indices).
    4. The test uses a columnIndex of -1 (< 0, explicitly invalid) and a string columnName (initialized to "test") as
       input.
 * @tc.step:
    1. Create an instance of DataShareAbsResultSet named dataShareAbsResultSet.
    2. Define an integer columnIndex (value -1) and a string columnName (initialized to "test").
    3. Call the GetColumnName function of dataShareAbsResultSet, passing columnIndex and columnName as parameters.
    4. Record the return value of GetColumnName and compare it with E_INVALID_COLUMN_INDEX.
 * @tc.expect:
    1. The GetColumnName function execution fails and returns E_INVALID_COLUMN_INDEX.
 */
HWTEST_F(DataShareAbsResultSetTest, GetColumnName003, TestSize.Level0)
{
    LOG_INFO("DataShareAbsResultSetTest MarshallingTest002::Start");
    DataShareAbsResultSet dataShareAbsResultSet;
    int columnIndex = -1;
    std::string columnName = "test";
    auto result = dataShareAbsResultSet.GetColumnName(columnIndex, columnName);
    EXPECT_EQ(result, E_INVALID_COLUMN_INDEX);
    LOG_INFO("DataShareAbsResultSetTest MarshallingTest002::End");
}
}
}