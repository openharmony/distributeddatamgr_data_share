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
* @tc.desc: test GoTo function when GoToRow return E_ERROR
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a MockDataShareAbsResultSet object
    2.call GoTo function when GoToRow return E_ERROR and check the result
* @tc.experct: GoTo failed and return E_ERROR
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
* @tc.desc: test GoTo function
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a MockDataShareAbsResultSet object
    2.call GoTo function and check the result
* @tc.experct: GoTo success and return E_OK
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
* @tc.desc: test IsEnded function when GetRowCount return E_ERROR
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a MockDataShareAbsResultSet object
    2.call IsEnded function when GetRowCount return E_ERROR and check the result
* @tc.experct: IsEnded failed and return E_ERROR
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
* @tc.desc: test IsEnded function
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a MockDataShareAbsResultSet object
    2.call IsEnded function and check the result
* @tc.experct: IsEnded success and return E_OK
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
* @tc.desc: test GetColumnCount function when GetAllColumnNames return E_ERROR
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a MockDataShareAbsResultSet2 object
    2.call GetColumnCount function when GetAllColumnNames return E_ERROR and check the result
* @tc.experct: GetColumnCount failed and return E_ERROR
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
* @tc.desc: test GetColumnName function when GetColumnCount return E_ERROR
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a MockDataShareAbsResultSet object
    2.call GetColumnName function when GetColumnCount return E_ERROR and check the result
* @tc.experct: GetColumnName failed and return E_ERROR
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
* @tc.desc: test GetColumnName function when columnIndex >= 0
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a MockDataShareAbsResultSet object
    2.call GetColumnName function when columnIndex >= 0 and check the result
* @tc.experct: GetColumnName failed and return E_INVALID_COLUMN_INDEX
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
* @tc.desc: test GetColumnName function when columnIndex < 0
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a MockDataShareAbsResultSet object
    2.call GetColumnName function when columnIndex < 0 and check the result
* @tc.experct: GetColumnName failed and return E_INVALID_COLUMN_INDEX
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