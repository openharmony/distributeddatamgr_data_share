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
* @tc.desc: test CreateStub function when resuletset_ = nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and resuletset_ = nullptr
    2.call CreateStub function and check the result
* @tc.experct: CreateStub failed and reutrn nullptr
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
* @tc.desc: test CreateStub function when resuletset_ is not nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and resuletset_ is not nullptr
    2.call CreateStub function and check the result
* @tc.experct: CreateStub succees and not reutrn nullptr
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
* @tc.desc: test OnRemoteRequest function when resuletset_ = nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and resuletset_ = nullptr
    2.call OnRemoteRequest function and check the result
* @tc.experct: OnRemoteRequest failed and reutrn INVALID_FD
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
* @tc.desc: test HandleGetRowCountRequest function when bridge = nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and bridge is nullptr
    2.call HandleGetRowCountRequest function and check the result
* @tc.experct: HandleGetRowCountRequest success and write E_ERROR into reply
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
* @tc.desc: test HandleGetRowCountRequest function when bridge is not nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and bridge is not nullptr
    2.call HandleGetRowCountRequest function and check the result
* @tc.experct: HandleGetRowCountRequest success and write count into reply
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
* @tc.desc: test HandleGetAllColumnNamesRequest function when bridge is nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and bridge is nullptr
    2.call HandleGetAllColumnNamesRequest function and check the result
* @tc.experct: HandleGetAllColumnNamesRequest success and write E_ERROR into reply
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
* @tc.desc: test HandleGetAllColumnNamesRequest function when bridge is not nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and bridge is not nullptr
    2.call HandleGetAllColumnNamesRequest function and check the result
* @tc.experct: HandleGetAllColumnNamesRequest success and write count into reply
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
* @tc.desc: test HandleOnGoRequest function when bridge is nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and bridge is nullptr
    2.call HandleOnGoRequest function and check the result
* @tc.experct: HandleOnGoRequest success and write -1 into reply
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
* @tc.desc: test HandleOnGoRequest function when bridge is not nullptr
* @tc.type: FUNC
* @tc.require: issueIC7OBM
* @tc.precon: None
* @tc.step:
    1.Creat a ISharedResultSetStub object and bridge is not nullptr
    2.call HandleOnGoRequest function and check the result
* @tc.experct: HandleOnGoRequest success and write count into reply
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
 * @tc.name: IsharedResultSetStubTest_ResultSetStubNull_Test_001
 * @tc.desc: Verify ISharedResultSetStub behavior when input parameters are null
 * @tc.type: FUNC
 * @tc.precon: None
 * @tc.step:
    1. Create ISharedResultSetStub instance with null parameter
    2. Call CreateStub method with null result and valid parcel
    3. Check returned ISharedResultSet pointer
 * @tc.expect:
    1. CreateStub returns nullptr
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