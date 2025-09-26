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

#define LOG_TAG "datashare_helper_test"

#include <gtest/gtest.h>
#include <unistd.h>

#include "datashare_helper.h"
#include "datashare_helper_impl.h"
#include "datashare_log.h"
#include "datashare_uri_utils.h"
#include "iservice_registry.h"
#include "ikvstore_data_service_mock.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;

constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;
std::string NON_SILENT_ACCESS_URI = "datashare:///com.acts.datasharetest";
std::string NON_SILENT_ACCESS_ERROR_URI = "datashare:///com.acts.test";
constexpr int DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID = 1301;

class DataShareHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void){};
    static void TearDownTestCase(void){};
    void SetUp(){};
    void TearDown(){};
};

void DataShareManagerImplHelper()
{
    auto helper = DataShareManagerImpl::GetInstance();
    helper->dataShareService_ = nullptr;
    auto manager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = manager->CheckSystemAbility(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID);
    sptr<MockDataShareKvServiceProxy> mockProxy = sptr<MockDataShareKvServiceProxy>
        (new MockDataShareKvServiceProxy(remoteObject));
    EXPECT_CALL(*mockProxy, GetFeatureInterface(testing::_))
        .WillOnce(testing::Return(nullptr));
    helper->dataMgrService_ = (sptr<DataShareKvServiceProxy>)mockProxy;
}

/**
 * @tc.name:
 * @tc.desc: test Creator function when options.isProxy_ = false and options.token_ = nullptr
 * @tc.type: FUNC
 * @tc.require:issueIC413F
 * @tc.precon: None
 * @tc.step:
    1.Create a DataShareHelper object what options.isProxy_ = false and options.token_ = nullptr
    2.call Creator function and check the result
 * @tc.experct: Creator failed and reutrn nullptr
 */
HWTEST_F(DataShareHelperTest, CreatorTest001, TestSize.Level0)
{
    LOG_INFO("DataShareHelperTest CreatorTest001::Start");
    std::string strUri = "testUri";
    CreateOptions options;
    options.isProxy_ = false;
    options.token_ = nullptr;
    std::string bundleName = "testBundle";
    int waitTime = 1;
    bool isSystem = true;
    auto result = DataShareHelper::Creator(strUri, options, bundleName, waitTime, isSystem);
    EXPECT_EQ(result, nullptr);
    LOG_INFO("DataShareHelperTest CreatorTest001::End");
}

/**
 * @tc.name:
 * @tc.desc: test Creator function when options.isProxy_ = true and options.token_ = nullptr
 * @tc.type: FUNC
 * @tc.require:issueIC413F
 * @tc.precon: None
 * @tc.step:
    1.Create a DataShareHelper object what options.isProxy_ = true and options.token_ = nullptr
    2.call Creator function and check the result
 * @tc.experct: Creator failed and reutrn nullptr
 */
HWTEST_F(DataShareHelperTest, CreatorTest002, TestSize.Level0)
{
    LOG_INFO("DataShareHelperTest CreatorTest002::Start");
    std::string strUri = "testUri";
    CreateOptions options;
    options.isProxy_ = true;
    options.token_ = nullptr;
    std::string bundleName = "testBundle";
    int waitTime = 1;
    bool isSystem = true;
    auto result = DataShareHelper::Creator(strUri, options, bundleName, waitTime, isSystem);
    EXPECT_EQ(result, nullptr);
    LOG_INFO("DataShareHelperTest CreatorTest002::End");
}

/**
 * @tc.name:
 * @tc.desc: test Creator function when options.isProxy_ = true and options.token_ != nullptr
 * @tc.type: FUNC
 * @tc.precon: None
 * @tc.step:
    1.Create a DataShareHelper object what options.isProxy_ = true and options.token_ != nullptr
    2.call Creator function and check the result
 * @tc.experct: Creator failed and not reutrn nullptr
 */
HWTEST_F(DataShareHelperTest, CreatorTest004, TestSize.Level0)
{
    LOG_INFO("DataShareHelperTest CreatorTest004::Start");
    CreateOptions options;
    options.isProxy_ = true;
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(saManager, nullptr);
    auto remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    EXPECT_NE(remoteObj, nullptr);
    options.token_ = remoteObj;
    auto result = DataShareHelper::Creator(NON_SILENT_ACCESS_ERROR_URI, options);
    EXPECT_EQ(result, nullptr);
    LOG_INFO("DataShareHelperTest CreatorTest004::End");
}

/**
 * @tc.name:
 * @tc.desc: test CreateExtHelper function when Uri contains "appIndex="
 * @tc.type: FUNC
 * @tc.require:issueIC413F
 * @tc.precon: None
 * @tc.step:
    1.Create a DataShareHelper object what Uri contains "appIndex="
    2.call CreateExtHelper function and check the result
 * @tc.experct: CreateExtHelper failed and reutrn nullptr
 */
HWTEST_F(DataShareHelperTest, CreateExtHelper001, TestSize.Level0)
{
    LOG_INFO("DataShareHelperTest CreateExtHelper001::Start");
    OHOS::Uri uri("datashareproxy://com.acts.ohos.data.datasharetest/test?appIndex=abcd");
    uri.query_ = ("appIndex=abcd");
    sptr<IRemoteObject> token = nullptr;
    int waitTime = 1000;
    bool isSystem = false;
    auto result = DataShareHelper::CreateExtHelper(uri, token, waitTime, isSystem);
    EXPECT_EQ(result, nullptr);
    LOG_INFO("DataShareHelperTest CreateExtHelper001::End");
}

/**
 * @tc.name: SetSilentSwitch001
 * @tc.desc: test SetSilentSwitch function when DataShareManagerImpl::GetServiceProxy() == nullptr
 * @tc.type: FUNC
 * @tc.require:issueIC413F
 * @tc.precon: None
 * @tc.step:
    1.Create a DataShareHelper object
    2.call SetSilentSwitch function when DataShareManagerImpl::GetServiceProxy() == nullptr
 * @tc.experct: SetSilentSwitch failed and reutrn DATA_SHARE_ERROR
 */
HWTEST_F(DataShareHelperTest, SetSilentSwitch001, TestSize.Level0)
{
    LOG_INFO("DataShareHelperTest SetSilentSwitch001::Start");
    DataShareManagerImplHelper();
    OHOS::Uri uri("datashareproxy://com.acts.ohos.data.datasharetest/test?appIndex=abcd");
    uri.query_ = ("appIndex=abcd");
    bool enable = false;
    bool isSystem = false;
    auto result = DataShareHelper::SetSilentSwitch(uri, enable, isSystem);
    EXPECT_EQ(result, DATA_SHARE_ERROR);
    LOG_INFO("DataShareHelperTest SetSilentSwitch001::End");
}

/**
 * @tc.name: GetSilentProxyStatus001
 * @tc.desc: test GetSilentProxyStatus function when DataShareManagerImpl::GetServiceProxy() == nullptr
 * @tc.type: FUNC
 * @tc.require:issueIC413F
 * @tc.precon: None
 * @tc.step:
    1.Create a DataShareHelper object
    2.call GetSilentProxyStatus function when DataShareManagerImpl::GetServiceProxy() == nullptr
 * @tc.experct: GetSilentProxyStatus failed and reutrn E_ERROR
 */
HWTEST_F(DataShareHelperTest, GetSilentProxyStatus001, TestSize.Level0)
{
    LOG_INFO("DataShareHelperTest GetSilentProxyStatus001::Start");
    DataShareManagerImplHelper();
    std::string uri = "datashareproxy://com.acts.ohos.data.datasharetest/test?appIndex=abcd";
    bool isSystem = false;
    auto result = DataShareHelper::GetSilentProxyStatus(uri, isSystem);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareHelperTest GetSilentProxyStatus001::End");
}

/**
 * @tc.name: CreateServiceHelper001
 * @tc.desc: test CreateExtHelper function when DataShareManagerImpl::GetServiceProxy() == nullptr
 * @tc.type: FUNC
 * @tc.require:issueIC413F
 * @tc.precon: None
 * @tc.step:
    1.Create a DataShareHelper object
    2.call CreateServiceHelper function when DataShareManagerImpl::GetServiceProxy() == nullptr
 * @tc.experct: CreateServiceHelper failed and reutrn nullptr
 */
HWTEST_F(DataShareHelperTest, CreateServiceHelper001, TestSize.Level0)
{
    LOG_INFO("DataShareHelperTest CreateServiceHelper001::Start");
    DataShareManagerImplHelper();
    std::string exuri = "testExuri";
    std::string bundleName = "bundleName";
    bool isSystem = false;
    auto result = DataShareHelper::CreateServiceHelper(exuri, bundleName, isSystem);
    EXPECT_EQ(result, nullptr);
    LOG_INFO("DataShareHelperTest CreateServiceHelper001::End");
}
} // namespace DataShare
} // namespace OHOS