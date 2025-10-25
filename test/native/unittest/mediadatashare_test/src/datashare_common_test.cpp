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

#define LOG_TAG "datashare_common_test"

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
    int GetAllColumnNames(std::vector<std::string> &columnNames) override
    {
        return 0;
    }

    int GetRowCount(int32_t &count) override
    {
        return 0;
    }

    int OnGo(int32_t startRowIndex, int32_t targetRowIndex, Writer &writer) override
    {
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
 * @tc.desc: Test the CreateStub function of ISharedResultSetStub when its internal 'resultset_' member is set to
 *           nullptr, verifying whether the function fails and returns the expected value.
 * @tc.type: FUNC
 * @tc.require: issueIC7OBM
 * @tc.precon:
    1. The test environment supports instantiation of ISharedResultSetStub, std::shared_ptr<DataShareResultSet>, and
       MessageParcel objects without initialization errors.
    2. The ISharedResultSetStub constructor accepts a std::shared_ptr<DataShareResultSet> parameter to initialize
       'resultset_'.
    3. The CreateStub method of ISharedResultSetStub takes 'resultset' and 'parcel' as parameters and returns a
       pointer.
 * @tc.step:
    1. Create an empty std::shared_ptr<DataShareResultSet> (i.e., 'resultset' = nullptr) to initialize the
       'resultset_' member.
    2. Instantiate an ISharedResultSetStub object using the nullptr 'resultset' created in Step 1.
    3. Create a MessageParcel object to pass as a parameter to the CreateStub function.
    4. Call the CreateStub method of the ISharedResultSetStub object, passing the 'resultset' and 'parcel', then
       check the return value.
 * @tc.expect:
    1. The CreateStub function executes without unexpected exceptions or crashes.
    2. The CreateStub function fails and returns nullptr.
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
 * @tc.desc: Test the CreateStub function of ISharedResultSetStub when its internal 'resultset_' member is not nullptr,
 *           verifying whether the function succeeds and returns the expected value.
 * @tc.type: FUNC
 * @tc.require: issueIC7OBM
 * @tc.precon:
    1. The test environment supports std::make_shared to create a valid std::shared_ptr<DataShareResultSet> instance.
    2. The ISharedResultSetStub constructor can initialize 'resultset_' with a non-nullptr DataShareResultSet pointer.
    3. The CreateStub method of ISharedResultSetStub can process non-nullptr 'resultset' and return a valid pointer.
 * @tc.step:
    1. Use std::make_shared<DataShareResultSet>() to create a non-nullptr std::shared_ptr<DataShareResultSet>.
    2. Instantiate an ISharedResultSetStub object using the non-nullptr 'resultset' from Step 1.
    3. Create a MessageParcel object to use as a parameter for the CreateStub function.
    4. Call the CreateStub method with 'resultset' and 'parcel', then check the return value.
 * @tc.expect:
    1. The CreateStub function executes without unexpected exceptions or crashes.
    2. The CreateStub function succeeds and returns a non-nullptr value.
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
 * @tc.desc: Test the OnRemoteRequest function of ISharedResultSetStub when its internal 'resultset_' member is set
 *           to nullptr, verifying whether the function fails and returns INVALID_FD.
 * @tc.type: FUNC
 * @tc.require: issueIC7OBM
 * @tc.precon:
    1. The test environment supports instantiation of MessageParcel, MessageOption, and ISharedResultSetStub objects.
    2. The INVALID_FD constant is predefined and accessible (used to judge the failure result).
    3. The OnRemoteRequest method of ISharedResultSetStub accepts parameters: uint32_t code, MessageParcel& data,
       MessageParcel& reply, MessageOption& option.
 * @tc.step:
    1. Create an empty std::shared_ptr<DataShareResultSet> (nullptr) and use it to instantiate an ISharedResultSetStub
       object.
    2. Define a uint32_t variable 'code' (initialized to 0) and create empty MessageParcel objects 'data' and 'reply'.
    3. Create a MessageOption object 'option' with default settings.
    4. Call the OnRemoteRequest method of the ISharedResultSetStub object, passing 'code', 'data', 'reply', and
       'option', then check the return value.
 * @tc.expect:
    1. The OnRemoteRequest function executes without unexpected exceptions or crashes.
    2. The OnRemoteRequest function fails and returns INVALID_FD.
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
 * @tc.desc: Test the HandleGetRowCountRequest function of ISharedResultSetStub when the 'bridge_' member of
 *           DataShareResultSet is nullptr, verifying whether the function writes E_ERROR to the reply and
 *           returns NO_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC7OBM
 * @tc.precon:
    1. The test environment supports creating std::shared_ptr<DataShareResultSet> (with 'bridge_' defaulting
       to nullptr) and MessageParcel objects.
    2. The E_ERROR and NO_ERROR constants are predefined and accessible (for result judgment).
    3. The HandleGetRowCountRequest method of ISharedResultSetStub accepts 'data' (input) and 'reply' (output)
       MessageParcel parameters.
 * @tc.step:
    1. Use std::make_shared<DataShareResultSet>() to create a DataShareResultSet instance (its 'bridge_' is
       nullptr by default).
    2. Instantiate an ISharedResultSetStub object using the DataShareResultSet pointer from Step 1.
    3. Create empty MessageParcel objects 'data' (input) and 'reply' (output) for the function call.
    4. Call the HandleGetRowCountRequest method with 'data' and 'reply', then check the method's return value.
    5. Read an int32_t value from the 'reply' parcel to verify its content.
 * @tc.expect:
    1. The HandleGetRowCountRequest method returns NO_ERROR (indicating the function executed successfully).
    2. The int32_t value read from the 'reply' parcel is E_ERROR.
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
 * @tc.desc: Test the HandleGetRowCountRequest function of ISharedResultSetStub when the 'bridge_' member of
 *           DataShareResultSet is not nullptr, verifying whether the function writes a valid row count to the reply
 *           and returns NO_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC7OBM
 * @tc.precon:
    1. The ResultSetBridgeTest class can be instantiated via std::make_shared to create a valid 'bridge_' instance.
    2. The 'bridge_' member of DataShareResultSet is accessible and can be assigned a non-nullptr ResultSetBridgeTest
       pointer.
    3. The HandleGetRowCountRequest method can interact with a valid 'bridge_' to get the row count and write it to
       'reply'.
 * @tc.step:
    1. Create a std::shared_ptr<DataShareResultSet> (named 'resultset') and a std::shared_ptr<ResultSetBridgeTest>.
    2. Assign 'bridge' to the 'bridge_' member of 'resultset' (resultset->bridge_ = bridge).
    3. Instantiate an ISharedResultSetStub object using 'resultset', then create empty 'data' and 'reply'
       MessageParcel objects.
    4. Call HandleGetRowCountRequest with 'data' and 'reply', then check the method's return value.
    5. Read an int32_t value from 'reply' to verify it is not E_ERROR.
 * @tc.expect:
    1. The HandleGetRowCountRequest method returns NO_ERROR (function execution success).
    2. The int32_t value read from 'reply' is not E_ERROR (indicating a valid row count is written).
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
 * @tc.desc: Test the HandleGetAllColumnNamesRequest function of ISharedResultSetStub when the 'bridge_' member of
 *           DataShareResultSet is nullptr, verifying whether the function writes E_ERROR to the reply and returns
 *           NO_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC7OBM
 * @tc.precon:
    1. The test environment supports creating DataShareResultSet (with 'bridge_' = nullptr) and ISharedResultSetStub
       instances.
    2. The HandleGetAllColumnNamesRequest method accepts 'data' and 'reply' MessageParcel parameters and returns an
       error code.
    3. The E_ERROR and NO_ERROR constants are correctly defined for result judgment.
 * @tc.step:
    1. Use std::make_shared<DataShareResultSet>() to create a 'resultset' (its 'bridge_' is nullptr).
    2. Instantiate an ISharedResultSetStub object with 'resultset', then create empty 'data' and 'reply' MessageParcel
       objects.
    3. Call the HandleGetAllColumnNamesRequest method with 'data' and 'reply', then check the method's return value.
    4. Read an int32_t value from the 'reply' parcel to verify its content.
 * @tc.expect:
    1. The HandleGetAllColumnNamesRequest method returns NO_ERROR (function executed successfully).
    2. The int32_t value read from 'reply' is E_ERROR.
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
 * @tc.desc: Test the HandleGetAllColumnNamesRequest function of ISharedResultSetStub when the 'bridge_' member
 *           of DataShareResultSet is not nullptr, verifying whether the function writes a valid count to the reply
 *           and returns NO_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC7OBM
 * @tc.precon:
    1. The ResultSetBridgeTest class supports instantiation via std::make_shared to create a valid 'bridge_' instance.
    2. The 'bridge_' member of DataShareResultSet can be assigned a non-nullptr ResultSetBridgeTest pointer.
    3. The HandleGetAllColumnNamesRequest method can interact with a valid 'bridge_' to get column names and write a
       valid count to 'reply'.
 * @tc.step:
    1. Create a 'resultset' (std::shared_ptr<DataShareResultSet>) and a 'bridge'.
    2. Set 'resultset->bridge_' = 'bridge' to ensure 'bridge_' is not nullptr.
    3. Instantiate an ISharedResultSetStub with 'resultset', then create empty 'data' and 'reply' MessageParcel
       objects.
    4. Call HandleGetAllColumnNamesRequest with 'data' and 'reply', then check the method's return value.
    5. Read an int32_t value from 'reply' to verify it is not E_ERROR.
 * @tc.expect:
    1. The HandleGetAllColumnNamesRequest method returns NO_ERROR (function execution success).
    2. The int32_t value read from 'reply' is not E_ERROR (indicating a valid count is written).
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
 * @tc.desc: Test the HandleOnGoRequest function of ISharedResultSetStub when the 'bridge_' member of
 *           DataShareResultSet is nullptr, verifying whether the function writes -1 to the reply and returns NO_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC7OBM
 * @tc.precon:
    1. The test environment supports creating DataShareResultSet (with 'bridge_' = nullptr) and ISharedResultSetStub
       instances.
    2. The HandleOnGoRequest method accepts 'data' and 'reply' MessageParcel parameters and returns an error code.
    3. The NO_ERROR constant is correctly defined for judging function execution success.
 * @tc.step:
    1. Create a 'resultset' (std::shared_ptr<DataShareResultSet>) with 'bridge_' defaulting to nullptr.
    2. Instantiate an ISharedResultSetStub object using 'resultset', then create empty 'data' and 'reply' MessageParcel
       objects.
    3. Call the HandleOnGoRequest method with 'data' and 'reply', then check the method's return value.
    4. Read an int32_t value from the 'reply' parcel to verify its content.
 * @tc.expect:
    1. The HandleOnGoRequest method returns NO_ERROR (function executed successfully).
    2. The int32_t value read from 'reply' is -1.
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
 * @tc.desc: Test the HandleOnGoRequest function of ISharedResultSetStub when the 'bridge_' member of
 *           DataShareResultSet is not nullptr, verifying whether the function writes a valid count to the reply
 *           and returns NO_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC7OBM
 * @tc.precon:
    1. The ResultSetBridgeTest class can be instantiated via std::make_shared to create a valid 'bridge_' instance.
    2. The 'bridge_' member of DataShareResultSet is accessible and can be assigned a non-nullptr 'bridge'.
    3. The HandleOnGoRequest method can interact with a valid 'bridge_' to get a valid result and write it to 'reply'.
 * @tc.step:
    1. Create a 'resultset' (std::shared_ptr<DataShareResultSet>) and a 'bridge'.
    2. Assign 'bridge' to 'resultset->bridge_' to ensure 'bridge_' is not nullptr.
    3. Instantiate an ISharedResultSetStub with 'resultset', then create empty 'data' and 'reply' MessageParcel
       objects.
    4. Call HandleOnGoRequest with 'data' and 'reply', then check the method's return value.
    5. Read an int32_t value from 'reply' to verify it is not E_ERROR.
 * @tc.expect:
    1. The HandleOnGoRequest method returns NO_ERROR (function execution success).
    2. The int32_t value read from 'reply' is not E_ERROR (indicating a valid count is written).
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
 * @tc.desc: Test the GetDataType function of DataShareResultSet when its internal 'sharedBlock_' member is set to
 *           nullptr, verifying whether the function fails and returns E_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareResultSet objects.
    2. The GetDataType method of DataShareResultSet accepts 'columnIndex' (int) and 'dataType' (DataShare::DataType&)
       as parameters.
    3. The E_ERROR constant is correctly defined for judging function failure.
 * @tc.step:
    1. Instantiate a DataShareResultSet object (its 'sharedBlock_' is nullptr by default).
    2. Define an int variable 'columnIndex' (initialized to 1) and a DataShare::DataType variable 'dataType' (for
       result storage).
    3. Call the GetDataType method of the DataShareResultSet object, passing 'columnIndex' and 'dataType' as
       parameters.
    4. Check the return value of the GetDataType method.
 * @tc.expect:
    1. The GetDataType function executes without unexpected exceptions or crashes.
    2. The GetDataType function fails and returns E_ERROR.
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
 * @tc.desc: Test the CheckState function of DataShareResultSet when the input 'columnIndex' is greater than or equal
 *           to 0, verifying whether the function fails and returns E_INVALID_COLUMN_INDEX.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareResultSet objects without additional initialization.
    2. The CheckState method of DataShareResultSet accepts an 'columnIndex' (int) parameter and returns an error code.
    3. The E_INVALID_COLUMN_INDEX constant is correctly defined for judging invalid column index.
 * @tc.step:
    1. Instantiate a DataShareResultSet object.
    2. Define an int variable 'columnIndex' and initialize it to 1 (≥ 0).
    3. Call the CheckState method of the DataShareResultSet object, passing 'columnIndex' as the parameter.
    4. Check the return value of the CheckState method.
 * @tc.expect:
    1. The CheckState function executes without unexpected exceptions or crashes.
    2. The CheckState function fails and returns E_INVALID_COLUMN_INDEX.
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
 * @tc.desc: Test the CheckState function of DataShareResultSet when the input 'columnIndex' is less than 0,
 *           verifying whether the function fails and returns E_INVALID_COLUMN_INDEX.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareResultSet objects without additional configuration.
    2. The CheckState method of DataShareResultSet accepts an 'columnIndex' (int) parameter and returns an error code.
    3. The E_INVALID_COLUMN_INDEX constant is correctly defined for judging invalid column index.
 * @tc.step:
    1. Instantiate a DataShareResultSet object.
    2. Define an int variable 'columnIndex' and initialize it to -1 (< 0).
    3. Call the CheckState method of the DataShareResultSet object, passing 'columnIndex' as the parameter.
    4. Check the return value of the CheckState method.
 * @tc.expect:
    1. The CheckState function executes without unexpected exceptions or crashes.
    2. The CheckState function fails and returns E_INVALID_COLUMN_INDEX.
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
 * @tc.desc: Test the Marshal function of DataShareResultSet when the input 'resultset' parameter is set to nullptr,
 *           verifying whether the function fails and returns false.
 * @tc.type: FUNC
 * @tc.require: issueICCAXH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareResultSet and MessageParcel objects.
    2. The Marshal method of DataShareResultSet accepts a std::shared_ptr<DataShareResultSet> ('resultset') and a
       MessageParcel ('parcel') as parameters, returning a bool.
    3. A nullptr std::shared_ptr<DataShareResultSet> is recognized as an invalid input by the Marshal method.
 * @tc.step:
    1. Instantiate a DataShareResultSet object (the caller of the Marshal method).
    2. Create a nullptr std::shared_ptr<DataShareResultSet> (named 'resultset') and an empty MessageParcel object.
    3. Call the Marshal method of the DataShareResultSet object, passing 'resultset' and 'parcel' as parameters.
    4. Check the boolean return value of the Marshal method.
 * @tc.expect:
    1. The Marshal function executes without unexpected exceptions or crashes.
    2. The Marshal function fails and returns false.
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
 * @tc.desc: Test the Unmarshal function of DataShareResultSet with an empty MessageParcel, verifying whether the
 *           function fails and returns nullptr.
 * @tc.type: FUNC
 * @tc.require: issueICCAXH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareResultSet and empty MessageParcel objects.
    2. The Unmarshal method of DataShareResultSet accepts a MessageParcel parameter and returns a.
    3. An empty MessageParcel (with no marshaled data) is recognized as invalid input by the Unmarshal method.
 * @tc.step:
    1. Instantiate a DataShareResultSet object (the caller of the Unmarshal method).
    2. Create an empty MessageParcel object (no marshaled DataShareResultSet data inside).
    3. Call the Unmarshal method of the DataShareResultSet object, passing the empty MessageParcel as the parameter.
    4. Check the return value (std::shared_ptr<DataShareResultSet>) of the Unmarshal method.
 * @tc.expect:
    1. The Unmarshal function executes without unexpected exceptions or crashes.
    2. The Unmarshal function fails and returns nullptr.
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
 * @tc.desc: Test the static Unmarshalling function of ITypesUtil for ITypesUtil::Predicates when the input 'parcel'
 *           is empty, verifying whether the function fails and returns false.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of ITypesUtil::Predicates and empty MessageParcel objects.
    2. The static Unmarshalling method of ITypesUtil accepts an ITypesUtil::Predicates& ('predicates') and a
       MessageParcel& ('parcel') as parameters, returning a bool.
    3. An empty MessageParcel (with no marshaled Predicates data) is invalid for the Unmarshalling method.
 * @tc.step:
    1. Define an ITypesUtil::Predicates variable 'predicates' (for storing unmarshaled data).
    2. Create an empty MessageParcel object 'parcel' (no marshaled Predicates data inside).
    3. Call the static ITypesUtil::Unmarshalling method, passing 'predicates' and 'parcel' as parameters.
    4. Check the boolean return value of the Unmarshalling method.
 * @tc.expect:
    1. The Unmarshalling function executes without unexpected exceptions or crashes.
    2. The Unmarshalling function fails and returns false.
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
 * @tc.desc: Test the static Unmarshalling function of ITypesUtil for OperationStatement when the input 'parcel'
 *           is empty, verifying whether the function fails and returns false.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of OperationStatement and empty MessageParcel objects.
    2. The static Unmarshalling method of ITypesUtil accepts an OperationStatement& ('operationStatement') and a
       MessageParcel& ('parcel') as parameters, returning a bool.
    3. An empty MessageParcel (with no marshaled OperationStatement data) is invalid for the Unmarshalling method.
 * @tc.step:
    1. Define an OperationStatement variable 'operationStatement' (for storing unmarshaled data).
    2. Create an empty MessageParcel object 'parcel' (no marshaled OperationStatement data inside).
    3. Call the static ITypesUtil::Unmarshalling method, passing 'operationStatement' and 'parcel' as parameters.
    4. Check the boolean return value of the Unmarshalling method.
 * @tc.expect:
    1. The Unmarshalling function executes without unexpected exceptions or crashes.
    2. The Unmarshalling function fails and returns false.
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
 * @tc.desc: Test the Unmarshalling function of ITypesUtil when the input MessageParcel is set to nullptr,
 *           verifying the function's error handling for invalid parcel input.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of ExecResult objects without initialization errors.
    2. The ITypesUtil class provides an Unmarshalling method that accepts ExecResult and MessageParcel parameters.
    3. The test environment allows explicitly setting MessageParcel to nullptr to simulate invalid input.
 * @tc.step:
    1. Create an ExecResult object to store the unmarshalled data.
    2. Explicitly set a MessageParcel pointer to nullptr (invalid input).
    3. Call ITypesUtil::Unmarshalling, passing the ExecResult object and the nullptr MessageParcel.
    4. Check the boolean return value of the Unmarshalling function.
 * @tc.expect:
    1. The ITypesUtil::Unmarshalling function returns false, indicating unmarshalling failed due to nullptr parcel.
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
 * @tc.desc: Test the Unmarshalling function of ITypesUtil when the input MessageParcel is set to nullptr,
 *           using ExecResultSet as the target object to verify consistent error handling.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of ExecResultSet objects without initialization errors.
    2. The ITypesUtil class provides an Unmarshalling method compatible with ExecResultSet and MessageParcel.
    3. MessageParcel can be explicitly set to nullptr to simulate invalid input in the test environment.
 * @tc.step:
    1. Create an ExecResultSet object to store the unmarshalled data.
    2. Set a MessageParcel pointer to nullptr to simulate invalid input.
    3. Invoke ITypesUtil::Unmarshalling with the ExecResultSet object and the nullptr MessageParcel.
    4. Verify the boolean return value of the Unmarshalling function.
 * @tc.expect:
    1. The ITypesUtil::Unmarshalling function returns false, confirming unmarshalling failure for nullptr parcel.
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
 * @tc.desc: Test the Marshalling function of ITypesUtil when the input MessageParcel is set to nullptr,
 *           verifying that the function succeeds as expected for RdbChangeNode objects.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of ITypesUtil::RdbChangeNode objects without errors.
    2. The ITypesUtil class provides a Marshalling method that accepts RdbChangeNode and MessageParcel parameters.
    3. The test environment allows setting MessageParcel to nullptr and executing the Marshalling function safely.
 * @tc.step:
    1. Create an ITypesUtil::RdbChangeNode object to be marshalled.
    2. Explicitly set a MessageParcel pointer to nullptr.
    3. Call ITypesUtil::Marshalling, passing the RdbChangeNode object and the nullptr MessageParcel.
    4. Check the boolean return value of the Marshalling function.
 * @tc.expect:
    1. The ITypesUtil::Marshalling function returns true, indicating successful marshalling even with nullptr parcel.
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
 * @tc.desc: Test the Marshalling method of DataShareResultSet when its internal 'sharedBlock_' member is set to
 *           nullptr, verifying the method's error handling for invalid internal state.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareResultSet objects without initialization errors.
    2. The 'sharedBlock_' member of DataShareResultSet is accessible and can be explicitly set to nullptr.
    3. The DataShareResultSet class provides a public Marshalling method that accepts a MessageParcel parameter.
 * @tc.step:
    1. Create a DataShareResultSet object.
    2. Explicitly set the 'sharedBlock_' member of the DataShareResultSet object to nullptr.
    3. Create a valid MessageParcel object for marshalling.
    4. Call the Marshalling method of the DataShareResultSet object, passing the valid MessageParcel.
    5. Check the boolean return value of the Marshalling method.
 * @tc.expect:
    1. The DataShareResultSet::Marshalling method returns false, indicating marshalling failed due to nullptr
       sharedBlock_.
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
 * @tc.desc: Test the GoTo function of MockDataShareAbsResultSet when its internal GoToRow function returns E_ERROR,
 *           verifying the GoTo function's error propagation for invalid row navigation.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports the MockDataShareAbsResultSet class and allows setting expected return values
       for its GoToRow method (via testing frameworks like GMock).
    2. The GoTo function of MockDataShareAbsResultSet accepts an int offset parameter and returns an error code.
    3. The E_ERROR constant is predefined and accessible in the test environment.
 * @tc.step:
    1. Create a MockDataShareAbsResultSet object.
    2. Use the testing framework to set an expectation: when GoToRow is called (with any parameter), it returns
       E_ERROR.
    3. Define an int offset (e.g., 1) to pass to the GoTo function.
    4. Call the GoTo function of the MockDataShareAbsResultSet object with the defined offset.
    5. Check the error code returned by the GoTo function.
 * @tc.expect:
    1. The GoTo function returns E_ERROR, propagating the error from the underlying GoToRow function.
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
 * @tc.desc: Test the normal execution of the GoTo function of DataShareAbsResultSet, verifying that it succeeds
 *           and returns the expected success code (E_OK) under valid conditions.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareAbsResultSet objects without initialization errors.
    2. The GoTo function of DataShareAbsResultSet accepts an int offset parameter and returns an error code.
    3. The E_OK constant is predefined and indicates successful execution in the test environment.
 * @tc.step:
    1. Create a DataShareAbsResultSet object.
    2. Define a valid int offset (e.g., 1) for row navigation.
    3. Call the GoTo function of the DataShareAbsResultSet object with the defined offset.
    4. Check the error code returned by the GoTo function.
 * @tc.expect:
    1. The GoTo function returns E_OK, indicating successful row navigation under valid conditions.
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
 * @tc.desc: Test the IsEnded function of MockDataShareAbsResultSet when its internal GetRowCount function returns
 *           E_ERROR, verifying the IsEnded function's error handling for invalid row count retrieval.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports MockDataShareAbsResultSet and allows setting expected return values for its
       GetRowCount method (via testing frameworks like GMock).
    2. The IsEnded function of MockDataShareAbsResultSet accepts a bool reference (to store the "ended" state) and
       returns an error code.
    3. The E_ERROR constant is predefined and accessible in the test environment.
 * @tc.step:
    1. Create a MockDataShareAbsResultSet object.
    2. Use the testing framework to set an expectation: when GetRowCount is called (with any int reference parameter),
       it returns E_ERROR.
    3. Declare a bool variable (e.g., test) to store the result of the IsEnded function.
    4. Call the IsEnded function of the MockDataShareAbsResultSet object, passing the bool variable.
    5. Check the error code returned by the IsEnded function.
 * @tc.expect:
    1. The IsEnded function returns E_ERROR, propagating the error from the underlying GetRowCount function.
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
 * @tc.desc: Test the normal execution of the IsEnded function of DataShareAbsResultSet, verifying that it succeeds
 *           and returns the expected success code (E_OK) under valid conditions.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareAbsResultSet objects without initialization errors.
    2. The IsEnded function of DataShareAbsResultSet accepts a bool reference (for "ended" state storage) and
       returns an error code (e.g., E_OK).
    3. The E_OK constant is predefined and indicates successful execution in the test environment.
 * @tc.step:
    1. Create a DataShareAbsResultSet object.
    2. Declare a bool variable (e.g., test) to store the "ended" state from the IsEnded function.
    3. Call the IsEnded function of the DataShareAbsResultSet object, passing the bool variable.
    4. Check the error code returned by the IsEnded function.
 * @tc.expect:
    1. The IsEnded function returns E_OK, indicating successful retrieval of the "ended" state under valid conditions.
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
 * @tc.desc: Test the GetColumnCount function of MockDataShareAbsResultSet2 when its internal GetAllColumnNames
 *           function returns E_ERROR, verifying error propagation for invalid column name retrieval.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports MockDataShareAbsResultSet2 and allows setting expected return values for its
       GetAllColumnNames method (via testing frameworks like GMock).
    2. The GetColumnCount function of MockDataShareAbsResultSet2 accepts an int reference (to store column count) and
       returns an error code; the 'count_' member is accessible and can be set (e.g., to -1).
    3. The E_ERROR constant is predefined and accessible in the test environment.
 * @tc.step:
    1. Create a MockDataShareAbsResultSet2 object and set its 'count_' member to -1.
    2. Use the testing framework to set an expectation: when GetAllColumnNames is called (with any vector<string>
       reference parameter), it returns E_ERROR.
    3. Declare an int variable (e.g., offset) initialized to -1 to store the column count.
    4. Call the GetColumnCount function of the MockDataShareAbsResultSet2 object, passing the int variable.
    5. Check the error code returned by the GetColumnCount function.
 * @tc.expect:
    1. The GetColumnCount function returns E_ERROR, propagating the error from the underlying GetAllColumnNames
       function.
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
 * @tc.desc: Test the GetColumnName function of MockDataShareAbsResultSet when its internal GetColumnCount function
 *           returns E_ERROR, verifying error handling for invalid column count retrieval.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports MockDataShareAbsResultSet and allows setting expected return values for its
       GetColumnCount method (via testing frameworks like GMock).
    2. The GetColumnName function of MockDataShareAbsResultSet accepts an int (column index) and a string reference
       (to store column name), and returns an error code.
    3. The E_ERROR constant is predefined and accessible in the test environment.
 * @tc.step:
    1. Create a MockDataShareAbsResultSet object.
    2. Use the testing framework to set an expectation: when GetColumnCount is called (with any int reference
       parameter), it returns E_ERROR.
    3. Define an int columnIndex (e.g., 1) and a string columnName (e.g., "test") to pass to GetColumnName.
    4. Call the GetColumnName function of the MockDataShareAbsResultSet object with columnIndex and columnName.
    5. Check the error code returned by the GetColumnName function.
 * @tc.expect:
    1. The GetColumnName function returns E_ERROR, propagating the error from the underlying GetColumnCount function.
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
 * @tc.desc: Test the GetColumnName function of DataShareAbsResultSet when the input columnIndex is greater than or
 *           equal to 0, verifying the function's handling of invalid column index (even for non-negative values).
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareAbsResultSet objects without initialization errors.
    2. The GetColumnName function of DataShareAbsResultSet accepts an int (column index) and a string reference
       (column name storage), and returns an error code.
    3. The E_INVALID_COLUMN_INDEX constant is predefined and indicates an invalid column index in the test environment.
 * @tc.step:
    1. Create a DataShareAbsResultSet object.
    2. Define an int columnIndex (e.g., 1, ≥ 0) and a string columnName (e.g., "test") to pass to GetColumnName.
    3. Call the GetColumnName function of the DataShareAbsResultSet object with columnIndex and columnName.
    4. Check the error code returned by the GetColumnName function.
 * @tc.expect:
    1. The GetColumnName function returns E_INVALID_COLUMN_INDEX, indicating failure due to invalid column index.
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
 * @tc.desc: Test the GetColumnName function of DataShareAbsResultSet when the input columnIndex is less than 0,
 *           verifying the function's error handling for negative (invalid) column index.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareAbsResultSet objects without initialization errors.
    2. The GetColumnName function of DataShareAbsResultSet accepts an int (column index) and a string reference
       (column name storage), and returns an error code.
    3. The E_INVALID_COLUMN_INDEX constant is predefined and indicates an invalid column index in the test environment.
 * @tc.step:
    1. Create a DataShareAbsResultSet object.
    2. Define an int columnIndex (e.g., -1, < 0) and a string columnName (e.g., "test") to pass to GetColumnName.
    3. Call the GetColumnName function of the DataShareAbsResultSet object with columnIndex and columnName.
    4. Check the error code returned by the GetColumnName function.
 * @tc.expect:
    1. The GetColumnName function returns E_INVALID_COLUMN_INDEX, indicating failure due to negative column index.
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
 * @tc.desc: Test the RegisterClientDeathObserver function of DataShareKvServiceProxy when the input observer
 *           (IRemoteObject) is set to nullptr, verifying the function's error handling for invalid observer input.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareKvServiceProxy objects (with IRemoteObject parameters)
       without initialization errors.
    2. The RegisterClientDeathObserver function of DataShareKvServiceProxy accepts a string (appId) and an
       sptr<IRemoteObject> (observer), and returns an int error code.
    3. The test environment allows setting sptr<IRemoteObject> to nullptr to simulate invalid observer input.
 * @tc.step:
    1. Set an sptr<IRemoteObject> observer to nullptr (invalid input).
    2. Define a string appId (e.g., "testAppid") for the client.
    3. Create a DataShareKvServiceProxy object, passing the nullptr observer to its constructor.
    4. Call the RegisterClientDeathObserver function of the proxy, passing appId and the nullptr observer.
    5. Check the int error code returned by the RegisterClientDeathObserver function.
 * @tc.expect:
    1. The RegisterClientDeathObserver function returns -1, indicating registration failed due to nullptr observer.
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