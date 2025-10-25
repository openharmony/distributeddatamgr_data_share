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

#define LOG_TAG "ishared_result_set_stub_test"

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

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
class IsharedResultSetStubTest : public testing::Test {
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
HWTEST_F(IsharedResultSetStubTest, CreateStubTest001, TestSize.Level0)
{
    LOG_INFO("IsharedResultSetStubTest CreateStubTest001::Start");
    std::shared_ptr<DataShareResultSet> resultset;
    MessageParcel parcel;
    ISharedResultSetStub stub(resultset);
    auto result = stub.CreateStub(resultset, parcel);
    EXPECT_EQ(result, nullptr);
    LOG_INFO("IsharedResultSetStubTest CreateStubTest001::End");
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
HWTEST_F(IsharedResultSetStubTest, CreateStubTestTest002, TestSize.Level0)
{
    LOG_INFO("IsharedResultSetStubTest CreateStubTestTest002::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    MessageParcel parcel;
    ISharedResultSetStub stub(resultset);
    auto result = stub.CreateStub(resultset, parcel);
    EXPECT_NE(result, nullptr);
    LOG_INFO("IsharedResultSetStubTest CreateStubTestTest002::End");
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
HWTEST_F(IsharedResultSetStubTest, OnRemoteRequestTest001, TestSize.Level0)
{
    LOG_INFO("IsharedResultSetStubTest OnRemoteRequestTest001::Start");
    std::shared_ptr<DataShareResultSet> resultset;
    ISharedResultSetStub stub(resultset);
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel parcel;
    MessageOption option;
    auto result = stub.OnRemoteRequest(code, data, parcel, option);
    EXPECT_EQ(result, INVALID_FD);
    LOG_INFO("IsharedResultSetStubTest OnRemoteRequestTest001::End");
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
HWTEST_F(IsharedResultSetStubTest, HandleGetRowCountRequestTest001, TestSize.Level0)
{
    LOG_INFO("IsharedResultSetStubTest HandleGetRowCountRequestTest001::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    MessageParcel data;
    MessageParcel reply;
    ISharedResultSetStub stub(resultset);
    auto result = stub.HandleGetRowCountRequest(data, reply);
    EXPECT_EQ(reply.ReadInt32(), E_ERROR);
    EXPECT_EQ(result, NO_ERROR);
    LOG_INFO("IsharedResultSetStubTest HandleGetRowCountRequestTest001::End");
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
HWTEST_F(IsharedResultSetStubTest, HandleGetRowCountRequestTest002, TestSize.Level0)
{
    LOG_INFO("IsharedResultSetStubTest HandleGetRowCountRequestTest002::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    resultset->bridge_ = std::make_shared<ResultSetBridgeTest>();
    MessageParcel data;
    MessageParcel reply;
    ISharedResultSetStub stub(resultset);
    auto result = stub.HandleGetRowCountRequest(data, reply);
    EXPECT_NE(reply.ReadInt32(), E_ERROR);
    EXPECT_EQ(result, NO_ERROR);
    LOG_INFO("IsharedResultSetStubTest HandleGetRowCountRequestTest002::End");
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
HWTEST_F(IsharedResultSetStubTest, HandleGetAllColumnNamesRequestTest001, TestSize.Level0)
{
    LOG_INFO("IsharedResultSetStubTest HandleGetAllColumnNamesRequestTest001::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    MessageParcel data;
    MessageParcel reply;
    ISharedResultSetStub stub(resultset);
    auto result = stub.HandleGetAllColumnNamesRequest(data, reply);
    EXPECT_EQ(reply.ReadInt32(), E_ERROR);
    EXPECT_EQ(result, NO_ERROR);
    LOG_INFO("IsharedResultSetStubTest HandleGetAllColumnNamesRequestTest001::End");
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
HWTEST_F(IsharedResultSetStubTest, HandleGetAllColumnNamesRequestTest002, TestSize.Level0)
{
    LOG_INFO("IsharedResultSetStubTest HandleGetAllColumnNamesRequestTest002::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    resultset->bridge_ = std::make_shared<ResultSetBridgeTest>();
    MessageParcel data;
    MessageParcel reply;
    ISharedResultSetStub stub(resultset);
    auto result = stub.HandleGetAllColumnNamesRequest(data, reply);
    EXPECT_NE(reply.ReadInt32(), E_ERROR);
    EXPECT_EQ(result, NO_ERROR);
    LOG_INFO("IsharedResultSetStubTest HandleGetAllColumnNamesRequestTest002::End");
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
HWTEST_F(IsharedResultSetStubTest, HandleOnGoRequestTest001, TestSize.Level0)
{
    LOG_INFO("IsharedResultSetStubTest HandleOnGoRequestTest001::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    MessageParcel data;
    MessageParcel reply;
    ISharedResultSetStub stub(resultset);
    auto result = stub.HandleOnGoRequest(data, reply);
    EXPECT_EQ(reply.ReadInt32(), -1);
    EXPECT_EQ(result, NO_ERROR);
    LOG_INFO("IsharedResultSetStubTest HandleOnGoRequestTest001::End");
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
HWTEST_F(IsharedResultSetStubTest, HandleOnGoRequestTest002, TestSize.Level0)
{
    LOG_INFO("IsharedResultSetStubTest HandleOnGoRequestTest002::Start");
    std::shared_ptr<DataShareResultSet> resultset = std::make_shared<DataShareResultSet>();
    resultset->bridge_ = std::make_shared<ResultSetBridgeTest>();
    MessageParcel data;
    MessageParcel reply;
    ISharedResultSetStub stub(resultset);
    auto result = stub.HandleOnGoRequest(data, reply);
    EXPECT_NE(reply.ReadInt32(), E_ERROR);
    EXPECT_EQ(result, NO_ERROR);
    LOG_INFO("IsharedResultSetStubTest HandleOnGoRequestTest002::End");
}

/**
 * @tc.name: AbnormalBranchTest_ResultSetStubNull_Test_001
 * @tc.desc: Verify ISharedResultSetStub behavior when input parameters are null
 * @tc.type: FUNC
 * @tc.precon:
    1. ISharedResultSetStub class is implemented, supporting null-parameter constructor
    2. CreateStub method of ISharedResultSetStub accepts DataShareResultSet pointer and MessageParcel
    3. MessageParcel class is implemented and can be initialized as a valid instance
 * @tc.step:
    1. Create ISharedResultSetStub instance with null constructor parameter
    2. Initialize a null std::shared_ptr<DataShareResultSet> (named result)
    3. Create a valid MessageParcel instance (named parcel)
    4. Call stub.CreateStub(result, parcel) and get the returned ISharedResultSet pointer
 * @tc.expect:
    1. The returned ISharedResultSet pointer from CreateStub is nullptr
 */
HWTEST_F(IsharedResultSetStubTest, ResultSetStubNull_Test_001, TestSize.Level0)
{
    LOG_INFO("IsharedResultSetStubTest ResultSetStubNull_Test_001::Start");
    ISharedResultSetStub stub(nullptr);
    std::shared_ptr<DataShareResultSet> result = nullptr;
    OHOS::MessageParcel parcel;
    sptr<ISharedResultSet> resultSet = stub.CreateStub(result, parcel);
    EXPECT_EQ(resultSet, nullptr);
    LOG_INFO("IsharedResultSetStubTest ResultSetStubNull_Test_001::End");
}
}
}