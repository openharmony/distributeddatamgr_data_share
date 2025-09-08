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
* @tc.desc: test GetDataType function when sharedBlock_ is nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a DataShareResultSet object and sharedBlock_ is nullptr
    2.call GetDataType function and check the result
* @tc.experct: GetDataType failed and return E_ERROR
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
* @tc.desc: test CheckState function when columnIndex >= 0
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a DataShareResultSet object
    2.call CheckState function when columnIndex >= 0 and check the result
* @tc.experct: CheckState failed and return E_INVALID_COLUMN_INDEX
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
* @tc.desc: test CheckState function when columnIndex < 0
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a DataShareResultSet object
    2.call CheckState function when columnIndex < 0 and check the result
* @tc.experct: CheckState failed and return E_INVALID_COLUMN_INDEX
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
* @tc.desc: test Marshal function when resultset = nullptr
* @tc.type: FUNC
* @tc.require: issueICCAXH
* @tc.precon: None
* @tc.step:
    1.Creat a DataShareResultSet object when resultset = nullptr
    2.call Marshal function and check the result
* @tc.experct: Marshal failed and return false
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
* @tc.desc: test Unmarshal function
* @tc.type: FUNC
* @tc.require: issueICCAXH
* @tc.precon: None
* @tc.step:
    1.Creat a DataShareResultSet object
    2.call Unmarshal function and check the result
* @tc.experct: Unmarshal failed and return nullptr
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
* @tc.desc: test Marshalling function when sharedBlock_ is nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a DataShareResultSet object
    2.call Marshalling function when sharedBlock_ is nullptr and check the result
* @tc.experct: Marshalling failed and return false
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
}
}