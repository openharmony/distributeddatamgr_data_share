/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
std::atomic_int g_callbackTimes = 0;

class ProxyHandleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ProxyHandleTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    int sleepTime = 1;
    sleep(sleepTime);

    HapInfoParams info = { .userID = 100,
        .bundleName = "com.acts.datasharetest",
        .instIndex = 0,
        .appIDDesc = "com.acts.datasharetest",
        .isSystemApp = true };
    HapPolicyParams policy = { .apl = APL_SYSTEM_BASIC,
        .domain = "test.domain",
        .permList = { { .permissionName = "ohos.permission.GET_BUNDLE_INFO",
            .bundleName = "com.acts.datasharetest",
            .grantMode = 1,
            .availableLevel = APL_SYSTEM_BASIC,
            .label = "label",
            .labelId = 1,
            .description = "com.acts.datasharetest",
            .descriptionId = 1 } },
        .permStateList = { { .permissionName = "ohos.permission.GET_BUNDLE_INFO",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 } } } };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenIDEx(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId.tokenIDEx);

    LOG_INFO("SetUpTestCase end");
}

void ProxyHandleTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenID(100, "com.acts.datasharetest", 0);
    AccessTokenKit::DeleteToken(tokenId);
}

void ProxyHandleTest::SetUp(void)
{
}

void ProxyHandleTest::TearDown(void)
{
}

/**
 * @tc.name: ProxyHandleTest_Publish_Test_001
 * @tc.desc: DataProxyHandle publish string data success
 * @tc.type: FUNC
 */
HWTEST_F(ProxyHandleTest, ProxyHandleTest_Publish_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyHandleTest_Publish_Test_001::Start");
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

    LOG_INFO("ProxyHandleTest_Publish_Test_001::End");
}

/**
 * @tc.name: ProxyHandleTest_Publish_Test_002
 * @tc.desc: DataProxyHandle publish int data success
 * @tc.type: FUNC
 */
HWTEST_F(ProxyHandleTest, ProxyHandleTest_Publish_Test_002, TestSize.Level0)
{
    LOG_INFO("ProxyHandleTest_Publish_Test_002::Start");
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

    LOG_INFO("ProxyHandleTest_Publish_Test_002::End");
}

/**
 * @tc.name: ProxyHandleTest_Publish_Test_003
 * @tc.desc: DataProxyHandle publish double data success
 * @tc.type: FUNC
 */
HWTEST_F(ProxyHandleTest, ProxyHandleTest_Publish_Test_003, TestSize.Level0)
{
    LOG_INFO("ProxyHandleTest_Publish_Test_003::Start");
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

    LOG_INFO("ProxyHandleTest_Publish_Test_003::End");
}

/**
 * @tc.name: ProxyHandleTest_Publish_Test_004
 * @tc.desc: DataProxyHandle publish bool data success
 * @tc.type: FUNC
 */
HWTEST_F(ProxyHandleTest, ProxyHandleTest_Publish_Test_004, TestSize.Level0)
{
    LOG_INFO("ProxyHandleTest_Publish_Test_004::Start");
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

    LOG_INFO("ProxyHandleTest_Publish_Test_004::End");
}
 
/**
 * @tc.name: ProxyHandleTest_Publish_Test_005
 * @tc.desc: DataProxyHandle publish data of other bundle name,
 * failed because of no permission.
 * @tc.type: FUNC
 */
HWTEST_F(ProxyHandleTest, ProxyHandleTest_Publish_Test_005, TestSize.Level0)
{
    LOG_INFO("ProxyHandleTest_Publish_Test_005::Start");
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

    LOG_INFO("ProxyHandleTest_Publish_Test_005::End");
}

/**
 * @tc.name: ProxyHandleTest_Delete_Test_001
 * @tc.desc: DataProxyHandle delete data success
 * @tc.type: FUNC
 */
HWTEST_F(ProxyHandleTest, ProxyHandleTest_Delete_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyHandleTest_Delete_Test_001::Start");
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

    LOG_INFO("ProxyHandleTest_Delete_Test_001::End");
}

/**
 * @tc.name: ProxyHandleTest_Delete_Test_002
 * @tc.desc: DataProxyHandle delete data of other bundle name,
 * failed because of no permission.
 * @tc.type: FUNC
 */
HWTEST_F(ProxyHandleTest, ProxyHandleTest_Delete_Test_002, TestSize.Level0)
{
    LOG_INFO("ProxyHandleTest_Delete_Test_002::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    std::vector<std::string> uris;
    uris.push_back(TEST_URI2);

    // delete data of other bundle name, failed because of no permission.
    auto ret4 = handle->DeleteProxyData(uris, proxyConfig);
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::NO_PERMISSION);

    LOG_INFO("ProxyHandleTest_Delete_Test_002::End");
}

/**
 * @tc.name: ProxyHandleTest_Delete_Test_003
 * @tc.desc: DataProxyHandle delete unpublished data failed
 * @tc.type: FUNC
 */
HWTEST_F(ProxyHandleTest, ProxyHandleTest_Delete_Test_003, TestSize.Level0)
{
    LOG_INFO("ProxyHandleTest_Delete_Test_003::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    std::vector<std::string> uris;
    uris.push_back(TEST_UNUSED_URI);

    // delete unpublished data failed
    auto ret4 = handle->DeleteProxyData(uris, proxyConfig);
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::URI_NOT_EXIST);

    LOG_INFO("ProxyHandleTest_Delete_Test_003::End");
}

/**
 * @tc.name: ProxyHandleTest_Get_Test_001
 * @tc.desc: DataProxyHandle get unpublished data failed
 * @tc.type: FUNC
 */
HWTEST_F(ProxyHandleTest, ProxyHandleTest_Get_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyHandleTest_Get_Test_001::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    std::vector<std::string> uris;
    uris.push_back(TEST_UNUSED_URI);

    // delete unpublished data failed
    auto ret4 = handle->GetProxyData(uris, proxyConfig);
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::URI_NOT_EXIST);

    LOG_INFO("ProxyHandleTest_Get_Test_001::End");
}

/**
 * @tc.name: ProxyHandleTest_Subscribe_Test_001
 * @tc.desc: DataProxyHandle subscribe data and callback success
 * @tc.type: FUNC
 */
HWTEST_F(ProxyHandleTest, ProxyHandleTest_Subscribe_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyHandleTest_Subscribe_Test_001::Start");
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
    auto ret4 = handle->SubscribeProxyData(uris, [](const std::vector<DataProxyChangeInfo> &changeNode) {
        LOG_INFO("ProxyHandleTest_Subscribe_Test_001::CallBack success");
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
    LOG_INFO("ProxyHandleTest_Subscribe_Test_001::End");
}

/**
 * @tc.name: ProxyHandleTest_Subscribe_Test_002
 * @tc.desc: DataProxyHandle subscribe unpublished data failed
 * @tc.type: FUNC
 */
HWTEST_F(ProxyHandleTest, ProxyHandleTest_Subscribe_Test_002, TestSize.Level0)
{
    LOG_INFO("ProxyHandleTest_Subscribe_Test_002::Start");
    auto [ret, handle] = DataShare::DataProxyHandle::Create();
    EXPECT_EQ(ret, E_OK);

    DataProxyConfig proxyConfig;
    proxyConfig.type_ = DataProxyType::SHARED_CONFIG;

    std::vector<std::string> uris;
    uris.push_back(TEST_UNUSED_URI);
    // subscribe data
    g_callbackTimes = 0;
    auto ret4 = handle->SubscribeProxyData(uris, [](const std::vector<DataProxyChangeInfo> &changeNode) {
        LOG_INFO("ProxyHandleTest_Subscribe_Test_002::CallBack success");
        g_callbackTimes++;
        EXPECT_EQ(changeNode.size(), 1);
    });
    EXPECT_EQ(ret4[0].result_, DataProxyErrorCode::URI_NOT_EXIST);

    LOG_INFO("ProxyHandleTest_Subscribe_Test_002::End");
}

/**
 * @tc.name: ProxyHandleTest_Unsubscribe_Test_001
 * @tc.desc: DataProxyHandle unsubscribe success
 * @tc.type: FUNC
 */
HWTEST_F(ProxyHandleTest, ProxyHandleTest_Unsubscribe_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyHandleTest_Unsubscribe_Test_001::Start");
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
    auto ret4 = handle->SubscribeProxyData(uris, [](const std::vector<DataProxyChangeInfo> &changeNode) {
        LOG_INFO("ProxyHandleTest_Unsubscribe_Test_001::CallBack success");
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
    LOG_INFO("ProxyHandleTest_Unsubscribe_Test_001::End");
}

/**
 * @tc.name: ProxyHandleTest_Unsubscribe_Test_002
 * @tc.desc: DataProxyHandle unsubscribe all uris success
 * @tc.type: FUNC
 */
HWTEST_F(ProxyHandleTest, ProxyHandleTest_Unsubscribe_Test_002, TestSize.Level0)
{
    LOG_INFO("ProxyHandleTest_Unsubscribe_Test_002::Start");
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
    auto ret4 = handle->SubscribeProxyData(uris, [](const std::vector<DataProxyChangeInfo> &changeNode) {
        LOG_INFO("ProxyHandleTest_Unsubscribe_Test_002::CallBack success");
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
    LOG_INFO("ProxyHandleTest_Unsubscribe_Test_002::End");
}
} // namespace DataShare
} // namespace OHOS