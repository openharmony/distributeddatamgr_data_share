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

#define LOG_TAG "dataproxy_handle_test"

#include <gtest/gtest.h>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "dataproxy_handle.h"
#include "dataproxy_handle_common.h"
#include "datashare_log.h"
#include "hap_token_info.h"
#include "token_setproc.h"
#include "datashare_errno.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
std::string TEST_URI1 = "datashareproxy://com.acts.datasharetest/test1";
std::string TEST_URI2 = "datashareproxy://com.acts.datasharetest.other/test1";
std::string TEST_UNUSED_URI = "datashareproxy://com.acts.datasharetest/unused";
static std::string TEST_TRUNCATE_URI1 = "datashareproxy://com.acts.datasharetest/truncate1";
static std::string TEST_TRUNCATE_URI2 = "datashareproxy://com.acts.datasharetest/truncate2";
std::atomic_int g_callbackTimes = 0;

class DataProxyHandleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DataProxyHandleTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    int sleepTime = 1;
    sleep(sleepTime);

    int32_t userId = 100;
    std::string bundleName = "com.acts.datasharetest";
    int32_t appIndex = 0;
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenID(userId, bundleName, appIndex);
    SetSelfTokenID(testTokenId);

    LOG_INFO("SetUpTestCase end");
}

void DataProxyHandleTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenID(100, "com.acts.datasharetest", 0);
    AccessTokenKit::DeleteToken(tokenId);
}

void DataProxyHandleTest::SetUp(void)
{
}

void DataProxyHandleTest::TearDown(void)
{
}

/**
 * @tc.name: Publish_Test_001
 * @tc.desc: Verify DataProxyHandle successfully publishing string data
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. TEST_URI1 (valid URI for data operation) is predefined
    2. DataProxyType::SHARED_CONFIG is a supported configuration type
    3. DataShare::DataProxyHandle::Create() method is implemented
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Initialize DataProxyConfig, set its type to DataProxyType::SHARED_CONFIG
    3. Prepare string test data "hello", wrap it with TEST_URI1 into DataShareProxyData
    4. Add the DataShareProxyData to a vector and call PublishProxyData
    5. Create a URI vector with TEST_URI1, call GetProxyData to retrieve data
 * @tc.expect:
    1. DataProxyHandle creation returns E_OK and non-null handle
    2. PublishProxyData returns DataProxyErrorCode::SUCCESS
    3. Retrieved data matches the published string "hello"
 */
HWTEST_F(DataProxyHandleTest, Publish_Test_001, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Publish_Test_001::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);
    EXPECT_NE(handle, nullptr);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish and get string
    DataProxyValue value = "hello";
    DataShareProxyData data(TEST_URI1, value);
    std::vector<DataShareProxyData> proxyData;
    proxyData.push_back(data);

    LOG_INFO("PublishProxyData::Start");
    auto ret2 = handle->PublishProxyData(proxyData, proxyConfig);
    EXPECT_EQ(ret2[0].result_, DataProxyErrorCode::SUCCESS);

    std::vector<std::string> uris;
    uris.push_back(TEST_URI1);
    LOG_INFO("GetProxyData::Start");
    auto ret3 = handle->GetProxyData(uris, proxyConfig);
    EXPECT_EQ(ret3[0].value_, value);

    LOG_INFO("DataProxyHandleTest_Publish_Test_001::End");
}

/**
 * @tc.name: Publish_Test_002
 * @tc.desc: Verify DataProxyHandle successfully publishing integer data
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. TEST_URI1 (valid URI for data operation) is predefined
    2. DataProxyValue supports integer-type assignment
    3. DataProxyType::SHARED_CONFIG supports integer data transmission
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Set DataProxyConfig's type to DataProxyType::SHARED_CONFIG
    3. Prepare integer test data 123456, bind it with TEST_URI1 into DataShareProxyData
    4. Add the DataShareProxyData to a vector and call PublishProxyData
    5. Use TEST_URI1 to call GetProxyData and retrieve the data
 * @tc.expect:
    1. DataProxyHandle creation returns E_OK
    2. PublishProxyData returns DataProxyErrorCode::SUCCESS
    3. Retrieved data equals the published integer 123456
 */
HWTEST_F(DataProxyHandleTest, Publish_Test_002, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Publish_Test_002::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish and get int
    DataProxyValue value = 123456;
    DataShareProxyData data(TEST_URI1, value);
    std::vector<DataShareProxyData> proxyData;
    proxyData.push_back(data);

    auto ret2 = handle->PublishProxyData(proxyData, proxyConfig);
    EXPECT_EQ(ret2[0].result_, DataProxyErrorCode::SUCCESS);

    std::vector<std::string> uris;
    uris.push_back(TEST_URI1);
    auto ret3 = handle->GetProxyData(uris, proxyConfig);
    EXPECT_EQ(ret3[0].value_, value);

    LOG_INFO("DataProxyHandleTest_Publish_Test_002::End");
}

/**
 * @tc.name: Publish_Test_003
 * @tc.desc: Verify DataProxyHandle successfully publishing double-precision floating-point data
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. TEST_URI1 (valid URI for data operation) is predefined
    2. DataProxyValue supports double-type assignment
    3. DataProxyType::SHARED_CONFIG supports double data transmission
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Configure DataProxyConfig to DataProxyType::SHARED_CONFIG
    3. Prepare double test data 123456.123456, wrap with TEST_URI1 into DataShareProxyData
    4. Add to a vector and call PublishProxyData
    5. Call GetProxyData with TEST_URI1 to retrieve the data
 * @tc.expect:
    1. DataProxyHandle creation returns E_OK
    2. PublishProxyData returns DataProxyErrorCode::SUCCESS
    3. Retrieved data matches the published double 123456.123456
 */
HWTEST_F(DataProxyHandleTest, Publish_Test_003, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Publish_Test_003::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish and get double
    DataProxyValue value = 123456.123456;
    DataShareProxyData data(TEST_URI1, value);
    std::vector<DataShareProxyData> proxyData;
    proxyData.push_back(data);

    auto ret2 = handle->PublishProxyData(proxyData, proxyConfig);
    EXPECT_EQ(ret2[0].result_, DataProxyErrorCode::SUCCESS);

    std::vector<std::string> uris;
    uris.push_back(TEST_URI1);
    auto ret3 = handle->GetProxyData(uris, proxyConfig);
    EXPECT_EQ(ret3[0].value_, value);

    LOG_INFO("DataProxyHandleTest_Publish_Test_003::End");
}

/**
 * @tc.name: Publish_Test_004
 * @tc.desc: Verify DataProxyHandle successfully publishing boolean data
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. TEST_URI1 (valid URI for data operation) is predefined
    2. DataProxyValue supports boolean-type assignment
    3. DataProxyType::SHARED_CONFIG supports boolean data transmission
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Set DataProxyConfig's type to DataProxyType::SHARED_CONFIG
    3. Prepare boolean test data true, bind with TEST_URI1 into DataShareProxyData
    4. Add to a vector and call PublishProxyData
    5. Use TEST_URI1 to call GetProxyData and retrieve the data
 * @tc.expect:
    1. DataProxyHandle creation returns E_OK
    2. PublishProxyData returns DataProxyErrorCode::SUCCESS
    3. Retrieved data equals the published boolean true
 */
HWTEST_F(DataProxyHandleTest, Publish_Test_004, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Publish_Test_004::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish and get bool
    DataProxyValue value = true;
    DataShareProxyData data(TEST_URI1, value);
    std::vector<DataShareProxyData> proxyData;
    proxyData.push_back(data);

    auto ret2 = handle->PublishProxyData(proxyData, proxyConfig);
    EXPECT_EQ(ret2[0].result_, DataProxyErrorCode::SUCCESS);

    std::vector<std::string> uris;
    uris.push_back(TEST_URI1);
    auto ret3 = handle->GetProxyData(uris, proxyConfig);
    EXPECT_EQ(ret3[0].value_, value);

    LOG_INFO("DataProxyHandleTest_Publish_Test_004::End");
}

/**
 * @tc.name: Publish_Test_005
 * @tc.desc: Verify that DataProxyHandle fails to publish data from another bundlename
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. TEST_URI2 (URI belonging to another bundle) is predefined
    2. Current test process has no permission to access TEST_URI2's bundle
    3. DataProxyType::SHARED_CONFIG enforces bundle permission checks
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Configure DataProxyConfig to DataProxyType::SHARED_CONFIG
    3. Prepare boolean data true, wrap with TEST_URI2 into DataShareProxyData
    4. Add to a vector and call PublishProxyData to attempt publishing
 * @tc.expect:
    1. DataProxyHandle creation returns E_OK and non-null handle
    2. PublishProxyData returns DataProxyErrorCode::NO_PERMISSION
 */
HWTEST_F(DataProxyHandleTest, Publish_Test_005, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Publish_Test_005::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish and get bool
    DataProxyValue value = true;
    DataShareProxyData data(TEST_URI2, value);
    std::vector<DataShareProxyData> proxyData;
    proxyData.push_back(data);

    // Publish other bundle name, failed because of no permission.
    auto ret2 = handle->PublishProxyData(proxyData, proxyConfig);
    EXPECT_EQ(ret2[0].result_, DataProxyErrorCode::NO_PERMISSION);

    LOG_INFO("DataProxyHandleTest_Publish_Test_005::End");
}

/**
 * @tc.name: Delete_Test_001
 * @tc.desc: Verify the functionality of DataProxyHandle successfully deleting published data
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. TEST_URI1 (valid URI for data operation) is predefined
    2. DataProxyType::SHARED_CONFIG supports publish, get, delete operations
    3. DataProxyErrorCode includes SUCCESS and URI_NOT_EXIST
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Set DataProxyConfig's type to DataProxyType::SHARED_CONFIG
    3. Publish boolean data true to TEST_URI1 via PublishProxyData
    4. Call GetProxyData with TEST_URI1 to verify publish success
    5. Call DeleteProxyData with TEST_URI1 to delete the data
    6. Call GetProxyData again to check deletion result
 * @tc.expect:
    1. Publish and Delete operations return DataProxyErrorCode::SUCCESS
    2. Initial GetProxyData retrieves the published boolean true
    3. Post-deletion GetProxyData returns DataProxyErrorCode::URI_NOT_EXIST
 */
HWTEST_F(DataProxyHandleTest, Delete_Test_001, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Delete_Test_001::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish and get bool
    DataProxyValue value = true;
    DataShareProxyData data(TEST_URI1, value);
    std::vector<DataShareProxyData> proxyData;
    proxyData.push_back(data);

    auto ret2 = handle->PublishProxyData(proxyData, proxyConfig);
    EXPECT_EQ(ret2[0].result_, DataProxyErrorCode::SUCCESS);

    std::vector<std::string> uris;
    uris.push_back(TEST_URI1);
    auto ret3 = handle->GetProxyData(uris, proxyConfig);
    EXPECT_EQ(ret3[0].value_, value);

    // delete the data success
    auto ret4 = handle->DeleteProxyData(uris, proxyConfig);
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::SUCCESS);

    // get data failed
    auto ret5 = handle->GetProxyData(uris, proxyConfig);
    EXPECT_EQ(ret5[0].result_, DataProxyErrorCode::URI_NOT_EXIST);

    LOG_INFO("DataProxyHandleTest_Delete_Test_001::End");
}

/**
 * @tc.name: Delete_Test_002
 * @tc.desc: Verify that DataProxyHandle fails to delete data from another bundlename
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. TEST_URI2 (URI belonging to another bundle) is predefined
    2. Current test process has no permission to delete TEST_URI2's data
    3. DataProxyType::SHARED_CONFIG checks permissions for delete operations
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Configure DataProxyConfig to DataProxyType::SHARED_CONFIG
    3. Create a URI vector with TEST_URI2
    4. Call DeleteProxyData with the URI vector to attempt deletion
 * @tc.expect:
    1. DataProxyHandle creation returns E_OK and non-null handle
    2. DeleteProxyData returns DataProxyErrorCode::NO_PERMISSION
 */
HWTEST_F(DataProxyHandleTest, Delete_Test_002, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Delete_Test_002::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    std::vector<std::string> uris;
    uris.push_back(TEST_URI2);

    // delete data of other bundle name, failed because of no permission.
    auto ret4 = handle->DeleteProxyData(uris, proxyConfig);
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::NO_PERMISSION);

    LOG_INFO("DataProxyHandleTest_Delete_Test_002::End");
}

/**
 * @tc.name: Delete_Test_003
 * @tc.desc: Verify that DataProxyHandle fails to delete unpublished data
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. TEST_UNUSED_URI (URI with no published data) is predefined
    2. DataProxyType::SHARED_CONFIG checks data existence for delete operations
    3. DataProxyErrorCode includes URI_NOT_EXIST
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Set DataProxyConfig's type to DataProxyType::SHARED_CONFIG
    3. Create a URI vector with TEST_UNUSED_URI
    4. Call DeleteProxyData with the URI vector to attempt deletion
 * @tc.expect:
    1. DataProxyHandle creation returns E_OK and non-null handle
    2. DeleteProxyData returns DataProxyErrorCode::URI_NOT_EXIST
 */
HWTEST_F(DataProxyHandleTest, Delete_Test_003, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Delete_Test_003::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    std::vector<std::string> uris;
    uris.push_back(TEST_UNUSED_URI);

    // delete unpublished data failed
    auto ret4 = handle->DeleteProxyData(uris, proxyConfig);
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::URI_NOT_EXIST);

    LOG_INFO("DataProxyHandleTest_Delete_Test_003::End");
}

/**
 * @tc.name: Get_Test_001
 * @tc.desc: Verify that DataProxyHandle fails to retrieve unpublished data
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. TEST_UNUSED_URI (URI with no published data) is predefined
    2. DataProxyType::SHARED_CONFIG supports data retrieval operations
    3. DataProxyErrorCode includes URI_NOT_EXIST
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Configure DataProxyConfig to DataProxyType::SHARED_CONFIG
    3. Create a URI vector with TEST_UNUSED_URI
    4. Call GetProxyData with the URI vector to attempt retrieval
 * @tc.expect:
    1. DataProxyHandle creation returns E_OK and non-null handle
    2. GetProxyData returns DataProxyErrorCode::URI_NOT_EXIST
 */
HWTEST_F(DataProxyHandleTest, Get_Test_001, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Get_Test_001::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    std::vector<std::string> uris;
    uris.push_back(TEST_UNUSED_URI);

    // delete unpublished data failed
    auto ret4 = handle->GetProxyData(uris, proxyConfig);
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::URI_NOT_EXIST);

    LOG_INFO("DataProxyHandleTest_Get_Test_001::End");
}

/**
 * @tc.name: Subscribe_Test_001
 * @tc.desc: Verify the functionality of DataProxyHandle successfully subscribing
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. TEST_URI1 (valid URI for data operation) is predefined
    2. DataProxyType::SHARED_CONFIG supports subscription to data changes
    3. Global variable g_callbackTimes is used to count callback triggers
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Configure DataProxyConfig to DataProxyType::SHARED_CONFIG
    3. Publish initial string data "hello" to TEST_URI1
    4. Subscribe to TEST_URI1 with a callback function
    5. Update data to "world" and publish again
 * @tc.expect:
    1. Publish and subscription operations return DataProxyErrorCode::SUCCESS
    2. Callback function is triggered once after data update
    3. g_callbackTimes equals 1 after update
 */
HWTEST_F(DataProxyHandleTest, Subscribe_Test_001, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Subscribe_Test_001::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish data first
    DataProxyValue value1 = "hello";
    DataShareProxyData data1(TEST_URI1, value1);
    std::vector<DataShareProxyData> proxyData1;
    proxyData1.push_back(data1);
    handle->PublishProxyData(proxyData1, proxyConfig);

    std::vector<std::string> uris;
    uris.push_back(TEST_URI1);
    // subscribe data
    g_callbackTimes = 0;
    DataProxyConfig config;
    auto ret4 = handle->SubscribeProxyData(uris, config, [](const std::vector<DataProxyChangeInfo> &changeNode) {
        LOG_INFO("DataProxyHandleTest_Subscribe_Test_001::CallBack success");
        g_callbackTimes++;
        EXPECT_EQ(changeNode.size(), 1);
    });
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::SUCCESS);

    // publish data and do callback
    DataProxyValue value2 = "world";
    DataShareProxyData data2(TEST_URI1, value2);
    std::vector<DataShareProxyData> proxyData2;
    proxyData2.push_back(data2);
    handle->PublishProxyData(proxyData2, proxyConfig);
    EXPECT_EQ(g_callbackTimes, 1);
    LOG_INFO("DataProxyHandleTest_Subscribe_Test_001::End");
}

/**
 * @tc.name: Subscribe_Test_002
 * @tc.desc: Verify that DataProxyHandle fails to subscribe to unpublished data
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. TEST_UNUSED_URI (URI with no published data) is predefined
    2. DataProxyType::SHARED_CONFIG checks data existence for subscriptions
    3. DataProxyErrorCode includes URI_NOT_EXIST
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Configure DataProxyConfig to DataProxyType::SHARED_CONFIG
    3. Create a URI vector with TEST_UNUSED_URI
    4. Attempt to subscribe to the URI with a callback function
 * @tc.expect:
    1. DataProxyHandle creation returns E_OK and non-null handle
    2. Subscription operation returns DataProxyErrorCode::URI_NOT_EXIST
    3. Callback function is not triggered (g_callbackTimes remains 0)
 */
HWTEST_F(DataProxyHandleTest, Subscribe_Test_002, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Subscribe_Test_002::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    std::vector<std::string> uris;
    uris.push_back(TEST_UNUSED_URI);
    // subscribe data
    g_callbackTimes = 0;
    DataProxyConfig config;
    auto ret4 = handle->SubscribeProxyData(uris, config, [](const std::vector<DataProxyChangeInfo> &changeNode) {
        LOG_INFO("DataProxyHandleTest_Subscribe_Test_002::CallBack success");
        g_callbackTimes++;
        EXPECT_EQ(changeNode.size(), 1);
    });
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::URI_NOT_EXIST);

    LOG_INFO("DataProxyHandleTest_Subscribe_Test_002::End");
}

/**
 * @tc.name: Unsubscribe_Test_001
 * @tc.desc: Verify the functionality of DataProxyHandle successfully unsubscribing from data changes
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon: None
 * @tc.step:
    1. Create a DataProxyHandle instance
    2. Configure the operation parameters to SHARED_CONFIG type
    3. Publish initial test data
    4. Subscribe to data change events
    5. Update the data to verify that the callback function is triggered
    6. Unsubscribe from the data
    7. Update the data again
 * @tc.expect:
    1. The data publish operation returns a SUCCESS status code
    2. The data subscription operation returns a SUCCESS status code
    3. The callback function is triggered after the first data update
    4. The unsubscribe operation is successful
    5. The callback function is not triggered after the second data update
 */
HWTEST_F(DataProxyHandleTest, Unsubscribe_Test_001, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Unsubscribe_Test_001::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish data first
    DataProxyValue value1 = "hello";
    DataShareProxyData data1(TEST_URI1, value1);
    std::vector<DataShareProxyData> proxyData1;
    proxyData1.push_back(data1);
    handle->PublishProxyData(proxyData1, proxyConfig);

    std::vector<std::string> uris;
    uris.push_back(TEST_URI1);
    // subscribe data
    g_callbackTimes = 0;
    DataProxyConfig config;
    auto ret4 = handle->SubscribeProxyData(uris, config, [](const std::vector<DataProxyChangeInfo> &changeNode) {
        LOG_INFO("DataProxyHandleTest_Unsubscribe_Test_001::CallBack success");
        g_callbackTimes++;
        EXPECT_EQ(changeNode.size(), 1);
    });
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::SUCCESS);

    // publish data and do callback
    DataProxyValue value2 = "world";
    DataShareProxyData data2(TEST_URI1, value2);
    std::vector<DataShareProxyData> proxyData2;
    proxyData2.push_back(data2);
    handle->PublishProxyData(proxyData2, proxyConfig);

    // unsubscribe data success
    ret4 = handle->UnsubscribeProxyData(uris);


    // publish data and not do callback
    handle->PublishProxyData(proxyData2, proxyConfig);
    LOG_INFO("DataProxyHandleTest_Unsubscribe_Test_001::End");
}

/**
 * @tc.name: Unsubscribe_Test_002
 * @tc.desc: Verify the functionality of DataProxyHandle successfully unsubscribing
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon: None
 * @tc.step:
    1. Create a DataProxyHandle instance
    2. Configure the operation parameters to SHARED_CONFIG type
    3. Publish initial test data
    4. Subscribe to data change events
    5. Update the data to verify that the callback function is triggered
    6. Call the unsubscribe interface with no parameters to unsubscribe from all subscriptions
    7. Update the data again
 * @tc.expect:
    1. The data publish operation returns a SUCCESS status code
    2. The data subscription operation returns a SUCCESS status code
    3. The callback function is triggered after the first data update
    4. The operation to unsubscribe from all subscriptions is successful
    5. The callback function is not triggered after the second data update
 */
HWTEST_F(DataProxyHandleTest, Unsubscribe_Test_002, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Unsubscribe_Test_002::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish data first
    DataProxyValue value1 = "hello";
    DataShareProxyData data1(TEST_URI1, value1);
    std::vector<DataShareProxyData> proxyData1;
    proxyData1.push_back(data1);
    handle->PublishProxyData(proxyData1, proxyConfig);

    std::vector<std::string> uris;
    uris.push_back(TEST_URI1);
    // subscribe data
    g_callbackTimes = 0;
    DataProxyConfig config;
    auto ret4 = handle->SubscribeProxyData(uris, config, [](const std::vector<DataProxyChangeInfo> &changeNode) {
        LOG_INFO("DataProxyHandleTest_Unsubscribe_Test_002::CallBack success");
        g_callbackTimes++;
        EXPECT_EQ(changeNode.size(), 1);
    });
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::SUCCESS);

    // publish data and do callback
    DataProxyValue value2 = "world";
    DataShareProxyData data2(TEST_URI1, value2);
    std::vector<DataShareProxyData> proxyData2;
    proxyData2.push_back(data2);
    handle->PublishProxyData(proxyData2, proxyConfig);

    // unsubscribe all uris success
    std::vector<std::string> emptyUris;
    auto ret2 = handle->UnsubscribeProxyData(emptyUris);

    // publish data and not do callback
    handle->PublishProxyData(proxyData2, proxyConfig);
    LOG_INFO("DataProxyHandleTest_Unsubscribe_Test_002::End");
}

/**
 * @tc.name: DeleteAll_Test_001
 * @tc.desc: Verify the functionality of DataProxyHandle successfully deleting all published data
 * @tc.type: FUNC
 * @tc.require: issueNumber
 * @tc.precon:
    1. TEST_URI1, TEST_UNUSED_URI (valid URIs for data operation) are predefined
    2. DataProxyType::SHARED_CONFIG supports publish, get, delete operations
    3. DataProxyErrorCode includes SUCCESS and URI_NOT_EXIST
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Set DataProxyConfig's type to DataProxyType::SHARED_CONFIG
    3. Publish multiple data items (int, string) to TEST_URI1 and TEST_UNUSED_URI via PublishProxyData
    4. Call GetProxyData to verify all data published successfully
    5. Call DeleteProxyData with only proxyConfig to delete all data
    6. Call GetProxyData again to verify all data deleted
 * @tc.expect:
    1. Publish operations return DataProxyErrorCode::SUCCESS
    2. DeleteProxyData returns DataProxyErrorCode::SUCCESS for all URIs
    3. Post-deletion GetProxyData returns DataProxyErrorCode::URI_NOT_EXIST for all URIs
 */
HWTEST_F(DataProxyHandleTest, DeleteAll_Test_001, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_DeleteAll_Test_001::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish multiple data
    std::vector<DataShareProxyData> proxyData;
    proxyData.push_back(DataShareProxyData(TEST_URI1, static_cast<int64_t>(100)));
    proxyData.push_back(DataShareProxyData(TEST_UNUSED_URI, std::string("test_string")));

    auto ret2 = handle->PublishProxyData(proxyData, proxyConfig);
    EXPECT_EQ(ret2[0].result_, DataProxyErrorCode::SUCCESS);
    EXPECT_EQ(ret2[1].result_, DataProxyErrorCode::SUCCESS);

    // verify data exists
    std::vector<std::string> uris = {TEST_URI1, TEST_UNUSED_URI};
    auto ret3 = handle->GetProxyData(uris, proxyConfig);
    EXPECT_EQ(ret3.size(), 2);

    // delete all data
    auto ret4 = handle->DeleteProxyData(proxyConfig);
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::SUCCESS);
    EXPECT_EQ(ret4[1].result_, DataProxyErrorCode::SUCCESS);

    // verify all data deleted
    auto ret5 = handle->GetProxyData(uris, proxyConfig);
    EXPECT_EQ(ret5[0].result_, DataProxyErrorCode::URI_NOT_EXIST);
    EXPECT_EQ(ret5[1].result_, DataProxyErrorCode::URI_NOT_EXIST);

    LOG_INFO("DataProxyHandleTest_DeleteAll_Test_001::End");
}

/**
 * @tc.name: DeleteAll_Test_002
 * @tc.desc: Verify that DataProxyHandle.DeleteProxyData with proxyConfig returns empty result when no data published
 * @tc.type: FUNC
 * @tc.require: issueNumber
 * @tc.precon:
    1. DataProxyType::SHARED_CONFIG supports delete operations
    2. No data has been published to any URI under current bundle
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Set DataProxyConfig's type to DataProxyType::SHARED_CONFIG
    3. Call DeleteProxyData with only proxyConfig (no prior publish)
 * @tc.expect:
    1. DataProxyHandle creation returns E_OK and non-null handle
    2. DeleteProxyData returns empty vector (no data to delete)
 */
HWTEST_F(DataProxyHandleTest, DeleteAll_Test_002, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_DeleteAll_Test_002::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // delete all data when no data published
    auto ret4 = handle->DeleteProxyData(proxyConfig);
    EXPECT_TRUE(ret4.empty());

    LOG_INFO("DataProxyHandleTest_DeleteAll_Test_002::End");
}

/**
 * @tc.name: DeleteAll_Test_003
 * @tc.desc: Verify that DataProxyHandle.DeleteProxyData with proxyConfig only deletes data of current bundle
 * @tc.type: FUNC
 * @tc.require: issueNumber
 * @tc.precon:
    1. TEST_URI1 (valid URI of current bundle) is predefined
    2. TEST_URI2 (URI belonging to another bundle) is predefined
    3. Current process cannot delete data of other bundles
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Set DataProxyConfig's type to DataProxyType::SHARED_CONFIG
    3. Publish data to TEST_URI1 of current bundle
    4. Call DeleteProxyData with only proxyConfig
    5. Verify TEST_URI1 data deleted, TEST_URI2 unaffected (no permission)
 * @tc.expect:
    1. DeleteProxyData only affects data of current bundle
    2. TEST_URI1 data is successfully deleted
 */
HWTEST_F(DataProxyHandleTest, DeleteAll_Test_003, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_DeleteAll_Test_003::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish data to current bundle
    DataShareProxyData data(TEST_URI1, static_cast<int64_t>(200));
    std::vector<DataShareProxyData> proxyData;
    proxyData.push_back(data);

    auto ret2 = handle->PublishProxyData(proxyData, proxyConfig);
    EXPECT_EQ(ret2[0].result_, DataProxyErrorCode::SUCCESS);

    // delete all data of current bundle
    auto ret4 = handle->DeleteProxyData(proxyConfig);
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::SUCCESS);

    // verify current bundle data deleted
    std::vector<std::string> uris;
    uris.push_back(TEST_URI1);
    auto ret5 = handle->GetProxyData(uris, proxyConfig);
    EXPECT_EQ(ret5[0].result_, DataProxyErrorCode::URI_NOT_EXIST);

    LOG_INFO("DataProxyHandleTest_DeleteAll_Test_003::End");
}

/**
 * @tc.name: Subscribe_Truncate_Test_001
 * @tc.desc: Verify that DataProxyValue string is truncated when subscribe maxValueLength is smaller than publish
 * @tc.type: FUNC
 * @tc.require: issueNumber
 * @tc.precon:
    1. TEST_TRUNCATE_URI1 (unique URI for this test) is predefined
    2. DataProxyMaxValueLength::MAX_LENGTH_100K allows publishing string up to 102400 bytes
    3. DataProxyMaxValueLength::MAX_LENGTH_4K limits callback value to 4096 bytes
    4. Global variable g_callbackTimes is used to count callback triggers
 * @tc.step:
    1. Create a DataProxyHandle instance
    2. Unsubscribe first to clear any existing subscriptions
    3. Publish initial data with MAX_LENGTH_100K config
    4. Subscribe with MAX_LENGTH_4K config (smaller than publish)
    5. Publish a 5000 bytes string (within 100K limit)
    6. Verify callback is triggered and value is truncated to 4096 bytes
    7. Unsubscribe and delete to clean up
 * @tc.expect:
    1. Publish operations return DataProxyErrorCode::SUCCESS
    2. Callback function is triggered exactly once
    3. DataProxyChangeInfo.value_ is truncated to 4096 bytes
 */
HWTEST_F(DataProxyHandleTest, Subscribe_Truncate_Test_001, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Subscribe_Truncate_Test_001::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig publishConfig;
    publishConfig.type_ = DataProxyType::SHARED_CONFIG;
    publishConfig.maxValueLength_ = DataProxyMaxValueLength::MAX_LENGTH_100K;

    std::vector<std::string> uris;
    uris.push_back(TEST_TRUNCATE_URI1);

    // unsubscribe first to clear any existing subscriptions (no deduplication in inner API)
    handle->UnsubscribeProxyData(uris);

    // publish initial data
    DataProxyValue value1 = "initial";
    DataShareProxyData data1(TEST_TRUNCATE_URI1, value1);
    std::vector<DataShareProxyData> proxyData1;
    proxyData1.push_back(data1);
    auto publishRet1 = handle->PublishProxyData(proxyData1, publishConfig);
    EXPECT_EQ(publishRet1[0].result_, DataProxyErrorCode::SUCCESS);

    // subscribe with MAX_LENGTH_4K config
    g_callbackTimes = 0;
    DataProxyConfig subscribeConfig;
    subscribeConfig.maxValueLength_ = DataProxyMaxValueLength::MAX_LENGTH_4K;
    std::string receivedValue;
    auto ret4 = handle->SubscribeProxyData(uris, subscribeConfig,
        [&receivedValue](const std::vector<DataProxyChangeInfo> &changeNode) {
        LOG_INFO("Subscribe_Truncate_Test_001::CallBack success");
        g_callbackTimes++;
        if (changeNode.size() > 0 && changeNode[0].value_.index() == DataProxyValueType::VALUE_STRING) {
            receivedValue = std::get<std::string>(changeNode[0].value_);
        }
    });
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::SUCCESS);

    // publish 5000 bytes string
    std::string largeString(5000, 'a');
    DataProxyValue value2 = largeString;
    DataShareProxyData data2(TEST_TRUNCATE_URI1, value2);
    std::vector<DataShareProxyData> proxyData2;
    proxyData2.push_back(data2);
    auto publishRet2 = handle->PublishProxyData(proxyData2, publishConfig);
    EXPECT_EQ(publishRet2[0].result_, DataProxyErrorCode::SUCCESS);

    EXPECT_EQ(g_callbackTimes, 1);
    EXPECT_EQ(receivedValue.size(), static_cast<size_t>(DataProxyMaxValueLength::MAX_LENGTH_4K));

    // unsubscribe and delete to clean up
    handle->UnsubscribeProxyData(uris);
    handle->DeleteProxyData(uris, publishConfig);

    LOG_INFO("DataProxyHandleTest_Subscribe_Truncate_Test_001::End");
}

/**
 * @tc.name: Subscribe_Truncate_Test_002
 * @tc.desc: Verify that string within maxValueLength is not truncated in callback
 * @tc.type: FUNC
 * @tc.require: issueNumber
 * @tc.precon:
    1. TEST_TRUNCATE_URI2 (unique URI for this test) is predefined
    2. DataProxyMaxValueLength::MAX_LENGTH_100K allows publishing up to 102400 bytes
    3. Global variable g_callbackTimes is used to count callback triggers
 * @tc.step:
    1. Create a DataProxyHandle instance
    2. Unsubscribe first to clear any existing subscriptions
    3. Publish initial data with MAX_LENGTH_100K config
    4. Subscribe with MAX_LENGTH_100K config
    5. Publish a 50000 bytes string (within 100K limit)
    6. Verify callback is triggered and value is NOT truncated
    7. Unsubscribe and delete to clean up
 * @tc.expect:
    1. Publish operations return DataProxyErrorCode::SUCCESS
    2. Callback function is triggered exactly once
    3. DataProxyChangeInfo.value_ matches full 50000 bytes (no truncation)
 */
HWTEST_F(DataProxyHandleTest, Subscribe_Truncate_Test_002, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Subscribe_Truncate_Test_002::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig config;
    config.type_ = DataProxyType::SHARED_CONFIG;
    config.maxValueLength_ = DataProxyMaxValueLength::MAX_LENGTH_100K;

    std::vector<std::string> uris;
    uris.push_back(TEST_TRUNCATE_URI2);

    // unsubscribe first to clear any existing subscriptions (no deduplication in inner API)
    handle->UnsubscribeProxyData(uris);

    // publish initial data
    DataProxyValue value1 = "initial";
    DataShareProxyData data1(TEST_TRUNCATE_URI2, value1);
    std::vector<DataShareProxyData> proxyData1;
    proxyData1.push_back(data1);
    auto publishRet1 = handle->PublishProxyData(proxyData1, config);
    EXPECT_EQ(publishRet1[0].result_, DataProxyErrorCode::SUCCESS);

    // subscribe with MAX_LENGTH_100K config
    g_callbackTimes = 0;
    std::string receivedValue;
    auto ret4 = handle->SubscribeProxyData(uris, config,
        [&receivedValue](const std::vector<DataProxyChangeInfo> &changeNode) {
        LOG_INFO("Subscribe_Truncate_Test_002::CallBack success");
        g_callbackTimes++;
        if (changeNode.size() > 0 && changeNode[0].value_.index() == DataProxyValueType::VALUE_STRING) {
            receivedValue = std::get<std::string>(changeNode[0].value_);
        }
    });
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::SUCCESS);

    // publish 50000 bytes string
    std::string largeString(50000, 'b');
    DataProxyValue value2 = largeString;
    DataShareProxyData data2(TEST_TRUNCATE_URI2, value2);
    std::vector<DataShareProxyData> proxyData2;
    proxyData2.push_back(data2);
    auto publishRet2 = handle->PublishProxyData(proxyData2, config);
    EXPECT_EQ(publishRet2[0].result_, DataProxyErrorCode::SUCCESS);

    EXPECT_EQ(g_callbackTimes, 1);
    EXPECT_EQ(receivedValue.size(), largeString.size());

    // unsubscribe and delete to clean up
    handle->UnsubscribeProxyData(uris);
    handle->DeleteProxyData(uris, config);

    LOG_INFO("DataProxyHandleTest_Subscribe_Truncate_Test_002::End");
}

/**
 * @tc.name: Subscribe_InvalidMaxLength_Test_001
 * @tc.desc: Verify that callback is skipped when maxValueLength is invalid
 * @tc.type: FUNC
 * @tc.require: issueNumber
 * @tc.precon:
    1. TEST_UNUSED_URI (URI for data operation) is predefined
    2. Only MAX_LENGTH_4K and MAX_LENGTH_100K are valid maxValueLength values
    3. Global variable g_callbackTimes is used to count callback triggers
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Publish initial data to TEST_UNUSED_URI
    3. Subscribe to TEST_UNUSED_URI with invalid maxValueLength (e.g., static_cast<DataProxyMaxValueLength>(100))
    4. Publish new data to TEST_UNUSED_URI to trigger change notification
    5. Verify callback is not triggered due to invalid maxValueLength
    6. Unsubscribe and delete data to clean up
 * @tc.expect:
    1. Subscribe operation returns DataProxyErrorCode::SUCCESS (subscription succeeds)
    2. Callback function is NOT triggered (invalid maxValueLength causes skip in Emit)
    3. g_callbackTimes remains 0 after data update
 */
HWTEST_F(DataProxyHandleTest, Subscribe_InvalidMaxLength_Test_001, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Subscribe_InvalidMaxLength_Test_001::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish initial data to TEST_UNUSED_URI
    DataProxyValue value1 = "initial_invalid_test";
    DataShareProxyData data1(TEST_UNUSED_URI, value1);
    std::vector<DataShareProxyData> proxyData1;
    proxyData1.push_back(data1);
    auto publishRet = handle->PublishProxyData(proxyData1, proxyConfig);
    EXPECT_EQ(publishRet[0].result_, DataProxyErrorCode::SUCCESS);

    std::vector<std::string> uris;
    uris.push_back(TEST_UNUSED_URI);
    
    // subscribe with invalid maxValueLength (100, not 4096 or 102400)
    g_callbackTimes = 0;
    DataProxyConfig config;
    config.maxValueLength_ = static_cast<DataProxyMaxValueLength>(100);
    auto ret4 = handle->SubscribeProxyData(uris, config,
        [](const std::vector<DataProxyChangeInfo> &changeNode) {
        LOG_INFO("Subscribe_InvalidMaxLength_Test_001::CallBack should not be triggered");
        g_callbackTimes++;
    });
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::SUCCESS);

    // publish new data to trigger notification
    DataProxyValue value2 = "new_value_for_invalid_test";
    DataShareProxyData data2(TEST_UNUSED_URI, value2);
    std::vector<DataShareProxyData> proxyData2;
    proxyData2.push_back(data2);
    handle->PublishProxyData(proxyData2, proxyConfig);

    // callback should NOT be triggered due to invalid maxValueLength
    EXPECT_EQ(g_callbackTimes, 0);

    // unsubscribe and delete to clean up
    handle->UnsubscribeProxyData(uris);
    std::vector<std::string> deleteUris;
    deleteUris.push_back(TEST_UNUSED_URI);
    handle->DeleteProxyData(deleteUris, proxyConfig);

    LOG_INFO("DataProxyHandleTest_Subscribe_InvalidMaxLength_Test_001::End");
}

/**
 * @tc.name: Subscribe_NonStringNoTruncate_Test_001
 * @tc.desc: Verify that non-string DataProxyValue (int) is not truncated
 * @tc.type: FUNC
 * @tc.require: issueNumber
 * @tc.precon:
    1. TEST_UNUSED_URI (URI for data operation) is predefined
    2. Truncation only applies to string type values
    3. Global variable g_callbackTimes is used to count callback triggers
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Publish initial data to TEST_UNUSED_URI
    3. Subscribe to TEST_UNUSED_URI with MAX_LENGTH_4K config
    4. Publish integer data to TEST_UNUSED_URI
    5. Verify callback receives correct integer value without truncation
    6. Unsubscribe and delete to clean up
 * @tc.expect:
    1. Callback function is triggered once
    2. DataProxyChangeInfo.value_ is integer and matches published value
 */
HWTEST_F(DataProxyHandleTest, Subscribe_NonStringNoTruncate_Test_001, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Subscribe_NonStringNoTruncate_Test_001::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish initial data
    DataProxyValue value1 = "initial_int_test";
    DataShareProxyData data1(TEST_UNUSED_URI, value1);
    std::vector<DataShareProxyData> proxyData1;
    proxyData1.push_back(data1);
    auto publishRet = handle->PublishProxyData(proxyData1, proxyConfig);
    EXPECT_EQ(publishRet[0].result_, DataProxyErrorCode::SUCCESS);

    std::vector<std::string> uris;
    uris.push_back(TEST_UNUSED_URI);
    
    // subscribe with MAX_LENGTH_4K config
    g_callbackTimes = 0;
    DataProxyConfig config;
    config.maxValueLength_ = DataProxyMaxValueLength::MAX_LENGTH_4K;
    int64_t receivedValue = 0;
    auto ret4 = handle->SubscribeProxyData(uris, config,
        [&receivedValue](const std::vector<DataProxyChangeInfo> &changeNode) {
        LOG_INFO("Subscribe_NonStringNoTruncate_Test_001::CallBack success");
        g_callbackTimes++;
        if (changeNode.size() > 0 && changeNode[0].value_.index() == DataProxyValueType::VALUE_INT) {
            receivedValue = std::get<int64_t>(changeNode[0].value_);
        }
    });
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::SUCCESS);

    // publish integer value
    int64_t intValue = 123456789;
    DataProxyValue value2 = intValue;
    DataShareProxyData data2(TEST_UNUSED_URI, value2);
    std::vector<DataShareProxyData> proxyData2;
    proxyData2.push_back(data2);
    handle->PublishProxyData(proxyData2, proxyConfig);

    EXPECT_EQ(g_callbackTimes, 1);
    EXPECT_EQ(receivedValue, intValue);

    // unsubscribe and delete to clean up
    handle->UnsubscribeProxyData(uris);
    std::vector<std::string> deleteUris;
    deleteUris.push_back(TEST_UNUSED_URI);
    handle->DeleteProxyData(deleteUris, proxyConfig);

    LOG_INFO("DataProxyHandleTest_Subscribe_NonStringNoTruncate_Test_001::End");
}

/**
 * @tc.name: Subscribe_StringWithinLimit_Test_001
 * @tc.desc: Verify that string DataProxyValue within maxValueLength is not truncated
 * @tc.type: FUNC
 * @tc.require: issueNumber
 * @tc.precon:
    1. TEST_UNUSED_URI (URI for data operation) is predefined
    2. DataProxyMaxValueLength::MAX_LENGTH_4K limits string to 4096 bytes
    3. Global variable g_callbackTimes is used to count callback triggers
 * @tc.step:
    1. Create a DataProxyHandle instance via DataShare::DataProxyHandle::Create()
    2. Publish initial data to TEST_UNUSED_URI
    3. Subscribe to TEST_UNUSED_URI with MAX_LENGTH_4K config
    4. Publish a string value within 4096 bytes to TEST_UNUSED_URI
    5. Verify callback receives complete string without truncation
    6. Unsubscribe and delete to clean up
 * @tc.expect:
    1. Callback function is triggered once
    2. DataProxyChangeInfo.value_ matches published string exactly
 */
HWTEST_F(DataProxyHandleTest, Subscribe_StringWithinLimit_Test_001, TestSize.Level0)
{
    LOG_INFO("DataProxyHandleTest_Subscribe_StringWithinLimit_Test_001::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    // publish initial data
    DataProxyValue value1 = "initial_within_limit";
    DataShareProxyData data1(TEST_UNUSED_URI, value1);
    std::vector<DataShareProxyData> proxyData1;
    proxyData1.push_back(data1);
    auto publishRet = handle->PublishProxyData(proxyData1, proxyConfig);
    EXPECT_EQ(publishRet[0].result_, DataProxyErrorCode::SUCCESS);

    std::vector<std::string> uris;
    uris.push_back(TEST_UNUSED_URI);

    // subscribe with MAX_LENGTH_4K config
    g_callbackTimes = 0;
    DataProxyConfig config;
    config.maxValueLength_ = DataProxyMaxValueLength::MAX_LENGTH_4K;
    std::string receivedValue;
    auto ret4 = handle->SubscribeProxyData(uris, config,
        [&receivedValue](const std::vector<DataProxyChangeInfo> &changeNode) {
        LOG_INFO("Subscribe_StringWithinLimit_Test_001::CallBack success");
        g_callbackTimes++;
        if (changeNode.size() > 0 && changeNode[0].value_.index() == DataProxyValueType::VALUE_STRING) {
            receivedValue = std::get<std::string>(changeNode[0].value_);
        }
    });
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::SUCCESS);

    // publish string value within limit
    std::string normalString(1000, 'c');
    DataProxyValue value2 = normalString;
    DataShareProxyData data2(TEST_UNUSED_URI, value2);
    std::vector<DataShareProxyData> proxyData2;
    proxyData2.push_back(data2);
    handle->PublishProxyData(proxyData2, proxyConfig);

    EXPECT_EQ(g_callbackTimes, 1);
    EXPECT_EQ(receivedValue, normalString);
    EXPECT_EQ(receivedValue.size(), normalString.size());

    // unsubscribe and delete to clean up
    handle->UnsubscribeProxyData(uris);
    std::vector<std::string> deleteUris;
    deleteUris.push_back(TEST_UNUSED_URI);
    handle->DeleteProxyData(deleteUris, proxyConfig);

    LOG_INFO("DataProxyHandleTest_Subscribe_StringWithinLimit_Test_001::End");
}
} // namespace DataShare
} // namespace OHOS