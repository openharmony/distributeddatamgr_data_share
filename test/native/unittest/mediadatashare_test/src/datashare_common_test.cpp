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

#include <gtest/gtest.h>
#include <unistd.h>
#include <gtest/gtest.h>

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
class DataShareCommonTest : public testing::Test {
public:
     static void SetUpTestCase(void){};
     static void TearDownTestCase(void){};
     void SetUp(){};
     void TearDown(){};
};

class ResultSetBridgeTest : public OHOS::DataShare::ResultSetBridge {
public:
    int GetAllColumnNames(std::vector<std::string> &columnNames) override {
        return 0;
    }

    int GetRowCount(int32_t &count) override {
        return 0;
    }

    int OnGo(int32_t startRowIndex, int32_t targetRowIndex, Writer &writer) override {
        return 0;
    }
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
* @tc.name: CreateStubTestTest001
* @tc.desc: test CreateStub function when resuletset_ = nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and resuletset_ = nullptr
    2.call CreateStub function and check the result
* @tc.experct: CreateStub failed and reutrn nullptr
*/
HWTEST_F(DataShareCommonTest, CreateStubTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest CreateStubTest001::Start");
    std::shared_ptr<DataShareResultSet> resultset;
    MessageParcel parcel;
    ISharedResultSetStub stub(resultset);
    auto result = stub.CreateStub(resultset, parcel);
    EXPECT_EQ(result, nullptr);
    LOG_INFO("DataShareCommonTest CreateStubTest001::End");
}

/**
* @tc.name: CreateStubTestTest002
* @tc.desc: test CreateStub function when resuletset_ is not nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and resuletset_ is not nullptr
    2.call CreateStub function and check the result
* @tc.experct: CreateStub succees and not reutrn nullptr
*/
HWTEST_F(DataShareCommonTest, CreateStubTestTest002, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest CreateStubTestTest002::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    MessageParcel parcel;
    ISharedResultSetStub stub(resultset);
    auto result = stub.CreateStub(resultset, parcel);
    EXPECT_NE(result, nullptr);
    LOG_INFO("DataShareCommonTest CreateStubTestTest002::End");
}

/**
* @tc.name: OnRemoteRequestTest001
* @tc.desc: test OnRemoteRequest function when resuletset_ = nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and resuletset_ = nullptr
    2.call OnRemoteRequest function and check the result
* @tc.experct: OnRemoteRequest failed and reutrn INVALID_FD
*/
HWTEST_F(DataShareCommonTest, OnRemoteRequestTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest OnRemoteRequestTest001::Start");
    std::shared_ptr<DataShareResultSet> resultset;
    ISharedResultSetStub stub(resultset);
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel parcel;
    MessageOption option;
    auto result = stub.OnRemoteRequest(code, data, parcel, option);
    EXPECT_EQ(result, INVALID_FD);
    LOG_INFO("DataShareCommonTest OnRemoteRequestTest001::End");
}

/**
* @tc.name: HandleGetRowCountRequestTest001
* @tc.desc: test HandleGetRowCountRequest function when bridge = nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and bridge is nullptr
    2.call HandleGetRowCountRequest function and check the result
* @tc.experct: HandleGetRowCountRequest success and write E_ERROR into reply
*/
HWTEST_F(DataShareCommonTest, HandleGetRowCountRequestTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest HandleGetRowCountRequestTest001::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    MessageParcel data;
    MessageParcel reply;
    ISharedResultSetStub stub(resultset);
    auto result = stub.HandleGetRowCountRequest(data, reply);
    EXPECT_EQ(reply.ReadInt32(), E_ERROR);
    EXPECT_EQ(result, NO_ERROR);
    LOG_INFO("DataShareCommonTest HandleGetRowCountRequestTest001::End");
}

/**
* @tc.name: HandleGetRowCountRequestTest002
* @tc.desc: test HandleGetRowCountRequest function when bridge is not nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and bridge is not nullptr
    2.call HandleGetRowCountRequest function and check the result
* @tc.experct: HandleGetRowCountRequest success and write count into reply
*/
HWTEST_F(DataShareCommonTest, HandleGetRowCountRequestTest002, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest HandleGetRowCountRequestTest002::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    resultset->bridge_ = std::make_shared<ResultSetBridgeTest>();
    MessageParcel data;
    MessageParcel reply;
    ISharedResultSetStub stub(resultset);
    auto result = stub.HandleGetRowCountRequest(data, reply);
    EXPECT_NE(reply.ReadInt32(), E_ERROR);
    EXPECT_EQ(result, NO_ERROR);
    LOG_INFO("DataShareCommonTest HandleGetRowCountRequestTest002::End");
}

/**
* @tc.name: HandleGetAllColumnNamesRequestTest001
* @tc.desc: test HandleGetAllColumnNamesRequest function when bridge is nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and bridge is nullptr
    2.call HandleGetAllColumnNamesRequest function and check the result
* @tc.experct: HandleGetAllColumnNamesRequest success and write E_ERROR into reply
*/
HWTEST_F(DataShareCommonTest, HandleGetAllColumnNamesRequestTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest HandleGetAllColumnNamesRequestTest001::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    MessageParcel data;
    MessageParcel reply;
    ISharedResultSetStub stub(resultset);
    auto result = stub.HandleGetAllColumnNamesRequest(data, reply);
    EXPECT_EQ(reply.ReadInt32(), E_ERROR);
    EXPECT_EQ(result, NO_ERROR);
    LOG_INFO("DataShareCommonTest HandleGetAllColumnNamesRequestTest001::End");
}

/**
* @tc.name: HandleGetAllColumnNamesRequestTest002
* @tc.desc: test HandleGetAllColumnNamesRequest function when bridge is not nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and bridge is not nullptr
    2.call HandleGetAllColumnNamesRequest function and check the result
* @tc.experct: HandleGetAllColumnNamesRequest success and write count into reply
*/
HWTEST_F(DataShareCommonTest, HandleGetAllColumnNamesRequestTest002, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest HandleGetAllColumnNamesRequestTest002::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    resultset->bridge_ = std::make_shared<ResultSetBridgeTest>();
    MessageParcel data;
    MessageParcel reply;
    ISharedResultSetStub stub(resultset);
    auto result = stub.HandleGetAllColumnNamesRequest(data, reply);
    EXPECT_NE(reply.ReadInt32(), E_ERROR);
    EXPECT_EQ(result, NO_ERROR);
    LOG_INFO("DataShareCommonTest HandleGetAllColumnNamesRequestTest002::End");
}

/**
* @tc.name: HandleOnGoRequestTest001
* @tc.desc: test HandleOnGoRequest function when bridge is nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and bridge is nullptr
    2.call HandleOnGoRequest function and check the result
* @tc.experct: HandleOnGoRequest success and write -1 into reply
*/
HWTEST_F(DataShareCommonTest, HandleOnGoRequestTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest HandleOnGoRequestTest001::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    MessageParcel data;
    MessageParcel reply;
    ISharedResultSetStub stub(resultset);
    auto result = stub.HandleOnGoRequest(data, reply);
    EXPECT_EQ(reply.ReadInt32(), -1);
    EXPECT_EQ(result, NO_ERROR);
    LOG_INFO("DataShareCommonTest HandleOnGoRequestTest001::End");
}

/**
* @tc.name: HandleOnGoRequestTest002
* @tc.desc: test HandleOnGoRequest function when bridge is not nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and bridge is not nullptr
    2.call HandleOnGoRequest function and check the result
* @tc.experct: HandleOnGoRequest success and write count into reply
*/
HWTEST_F(DataShareCommonTest, HandleOnGoRequestTest002, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest HandleOnGoRequestTest002::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    resultset->bridge_ = std::make_shared<ResultSetBridgeTest>();
    MessageParcel data;
    MessageParcel reply;
    ISharedResultSetStub stub(resultset);
    auto result = stub.HandleOnGoRequest(data, reply);
    EXPECT_NE(reply.ReadInt32(), E_ERROR);
    EXPECT_EQ(result, NO_ERROR);
    LOG_INFO("DataShareCommonTest HandleOnGoRequestTest002::End");
}

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
HWTEST_F(DataShareCommonTest, GetDataTypeTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest GetDataTypeTest001::Start");
    DataShareResultSet dataShareResultSet;
    int columnIndex = 1;
    DataShare::DataType dataType;
    auto result = dataShareResultSet.GetDataType(columnIndex, dataType);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareCommonTest GetDataTypeTest001::End");
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
HWTEST_F(DataShareCommonTest, CheckStateTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest CheckStateTest001::Start");
    DataShareResultSet dataShareResultSet;
    int columnIndex = 1;
    auto result = dataShareResultSet.CheckState(columnIndex);
    EXPECT_EQ(result, E_INVALID_COLUMN_INDEX);
    LOG_INFO("DataShareCommonTest CheckStateTest001::End");
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
HWTEST_F(DataShareCommonTest, CheckStateTest002, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest CheckState002::Start");
    DataShareResultSet dataShareResultSet;
    int columnIndex = -1;
    auto result = dataShareResultSet.CheckState(columnIndex);
    EXPECT_EQ(result, E_INVALID_COLUMN_INDEX);
    LOG_INFO("DataShareCommonTest CheckState002::End");
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
HWTEST_F(DataShareCommonTest, MarshalTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest MarshalTest001::Start");
    DataShareResultSet dataShareResultSet;
    std::shared_ptr<DataShareResultSet> resultset = nullptr;
    MessageParcel parcel;
    auto result = dataShareResultSet.Marshal(resultset, parcel);
    EXPECT_FALSE(result);
    LOG_INFO("DataShareCommonTest MarshalTest001::End");
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
HWTEST_F(DataShareCommonTest, UnmarshalTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest UnmarshalTest001::Start");
    DataShareResultSet dataShareResultSet;
    MessageParcel parcel;
    auto result = dataShareResultSet.Unmarshal(parcel);
    EXPECT_EQ(result, nullptr);
    LOG_INFO("DataShareCommonTest UnmarshalTest001::End");
}

/**
* @tc.name: UnmarshallingTest001
* @tc.desc: test Unmarshalling function when parcel is nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a ITypesUtil object and parcel is nullptr
    2.call Unmarshalling function and check the result
* @tc.experct: Unmarshalling failed and return false
*/
HWTEST_F(DataShareCommonTest, UnmarshallingTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest UnmarshallingTest001::Start");
    ITypesUtil::Predicates predicates;
    MessageParcel parcel;
    auto result = ITypesUtil::Unmarshalling(predicates, parcel);
    EXPECT_FALSE(result);
    LOG_INFO("DataShareCommonTest UnmarshallingTest001::End");
}

/**
* @tc.name: UnmarshallingTest002
* @tc.desc: test Unmarshalling function when parcel is nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a ITypesUtil object and parcel is nullptr
    2.call Unmarshalling function and check the result
* @tc.experct: Unmarshalling failed and return false
*/
HWTEST_F(DataShareCommonTest, UnmarshallingTest002, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest UnmarshallingTest002::Start");
    OperationStatement operationStatement;
    MessageParcel parcel;
    auto result = ITypesUtil::Unmarshalling(operationStatement, parcel);
    EXPECT_FALSE(result);
    LOG_INFO("DataShareCommonTest UnmarshallingTest002::End");
}

/**
* @tc.name: UnmarshallingTest003
* @tc.desc: test Unmarshalling function when parcel is nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a ITypesUtil object and parcel is nullptr
    2.call Unmarshalling function and check the result
* @tc.experct: Unmarshalling failed and return false
*/
HWTEST_F(DataShareCommonTest, UnmarshallingTest003, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest UnmarshallingTest003::Start");
    ExecResult execResult;
    MessageParcel parcel;
    auto result = ITypesUtil::Unmarshalling(execResult, parcel);
    EXPECT_FALSE(result);
    LOG_INFO("DataShareCommonTest UnmarshallingTest003::End");
}

/**
* @tc.name: UnmarshallingTest004
* @tc.desc: test Unmarshalling function when parcel is nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a ITypesUtil object and parcel is nullptr
    2.call Unmarshalling function and check the result
* @tc.experct: Unmarshalling failed and return false
*/
HWTEST_F(DataShareCommonTest, UnmarshallingTest004, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest UnmarshallingTest004::Start");
    ExecResultSet execResultSet;
    MessageParcel parcel;
    auto result = ITypesUtil::Unmarshalling(execResultSet, parcel);
    EXPECT_FALSE(result);
    LOG_INFO("DataShareCommonTest UnmarshallingTest004::End");
}

/**
* @tc.name: MarshallingTest001
* @tc.desc: test Marshalling function when parcel is nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a ITypesUtil object and parcel is nullptr
    2.call Marshalling function and check the result
* @tc.experct: Marshalling success and return ture
*/
HWTEST_F(DataShareCommonTest, MarshallingTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest MarshallingTest001::Start");
    ITypesUtil::RdbChangeNode changeNode;
    MessageParcel parcel;
    auto result = ITypesUtil::Marshalling(changeNode, parcel);
    EXPECT_TRUE(result);
    LOG_INFO("DataShareCommonTest MarshallingTest001::End");
}

/**
* @tc.name: MarshallingTest002
* @tc.desc: test Marshalling function when sharedBlock_ is nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a DataShareResultSet object
    2.call Marshalling function when sharedBlock_ is nullptr and check the result
* @tc.experct: Marshalling failed and return false
*/
HWTEST_F(DataShareCommonTest, MarshallingTest002, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest MarshallingTest002::Start");
    DataShareResultSet dataShareResultSet;
    MessageParcel parcel;
    auto result = dataShareResultSet.Marshalling(parcel);
    EXPECT_FALSE(result);
    LOG_INFO("DataShareCommonTest MarshallingTest002::End");
}

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
HWTEST_F(DataShareCommonTest, GoToTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest GoToTest001::Start");
    MockDataShareAbsResultSet mockResultSet;
    int offset = 1;
    EXPECT_CALL(mockResultSet, GoToRow(testing::_))
        .WillOnce(testing::Return(E_ERROR));
    auto result = mockResultSet.GoTo(offset);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareCommonTest GoToTest001::End");
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
HWTEST_F(DataShareCommonTest, GoToTest002, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest GoToTest002::Start");
    DataShareAbsResultSet dataShareAbsResultSet;
    int offset = 1;
    auto result = dataShareAbsResultSet.GoTo(offset);
    EXPECT_EQ(result, E_OK);
    LOG_INFO("DataShareCommonTest GoToTest002::End");
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
HWTEST_F(DataShareCommonTest, IsEndedTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest IsEndedTest001::Start");
    MockDataShareAbsResultSet mockResultSet;
    EXPECT_CALL(mockResultSet, GetRowCount(testing::_))
        .WillOnce(testing::Return(E_ERROR));
    bool test = true;
    auto result = mockResultSet.IsEnded(test);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareCommonTest IsEndedTest001::End");
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
HWTEST_F(DataShareCommonTest, IsEndedTest002, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest IsEndedTest002::Start");
    DataShareAbsResultSet dataShareAbsResultSet;
    bool test = true;
    auto result = dataShareAbsResultSet.IsEnded(test);
    EXPECT_EQ(result, E_OK);
    LOG_INFO("DataShareCommonTest IsEndedTest002::End");
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
HWTEST_F(DataShareCommonTest, GetColumnCountTest001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest GetColumnCountTest001::Start");
    MockDataShareAbsResultSet2 mockResultSet;
    int offset = -1;
    mockResultSet.count_ = -1;
    EXPECT_CALL(mockResultSet, GetAllColumnNames(testing::_))
        .WillOnce(testing::Return(E_ERROR));
    auto result = mockResultSet.GetColumnCount(offset);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareCommonTest GetColumnCountTest001::End");
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
HWTEST_F(DataShareCommonTest, GetColumnName001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest GetColumnName001::Start");
    MockDataShareAbsResultSet mockResultSet;
    int columnIndex = 1;
    std::string columnName = "test";
    EXPECT_CALL(mockResultSet, GetColumnCount(testing::_))
        .WillOnce(testing::Return(E_ERROR));
    auto result = mockResultSet.GetColumnName(columnIndex, columnName);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareCommonTest GetColumnName001::End");
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
HWTEST_F(DataShareCommonTest, GetColumnName002, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest MarshallingTest002::Start");
    DataShareAbsResultSet dataShareAbsResultSet;
    int columnIndex = 1;
    std::string columnName = "test";
    auto result = dataShareAbsResultSet.GetColumnName(columnIndex, columnName);
    EXPECT_EQ(result, E_INVALID_COLUMN_INDEX);
    LOG_INFO("DataShareCommonTest MarshallingTest002::End");
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
HWTEST_F(DataShareCommonTest, GetColumnName003, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest MarshallingTest002::Start");
    DataShareAbsResultSet dataShareAbsResultSet;
    int columnIndex = -1;
    std::string columnName = "test";
    auto result = dataShareAbsResultSet.GetColumnName(columnIndex, columnName);
    EXPECT_EQ(result, E_INVALID_COLUMN_INDEX);
    LOG_INFO("DataShareCommonTest MarshallingTest002::End");
}

/**
* @tc.name: RegisterClientDeathObserver001
* @tc.desc: test RegisterClientDeathObserver function when observer = nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a MockDataShareAbsResultSet object when observer = nullptr
    2.call RegisterClientDeathObserver function and check the result
* @tc.experct: RegisterClientDeathObserver failed and return -1
*/
HWTEST_F(DataShareCommonTest, RegisterClientDeathObserver001, TestSize.Level0)
{
    LOG_INFO("DataShareCommonTest RegisterClientDeathObserver001::Start");
    sptr<IRemoteObject> observer = nullptr;
    std::string appId = "testAppid";
    DataShareKvServiceProxy dataShareKvServiceProxy(observer);
    auto result = dataShareKvServiceProxy.RegisterClientDeathObserver(appId, observer);
    EXPECT_EQ(result, -1);
    LOG_INFO("DataShareCommonTest RegisterClientDeathObserver001::End");
}
} // namespace DataShare
} // namespace OHOS