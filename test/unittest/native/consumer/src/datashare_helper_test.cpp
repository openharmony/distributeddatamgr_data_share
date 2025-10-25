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
 * @tc.name: CreatorTest001
 * @tc.desc: Test the Creator function of DataShareHelper when CreateOptions.isProxy_ is false and CreateOptions.token_
 *           is nullptr, verifying if the function fails and returns nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment supports instantiation of CreateOptions and calling the static DataShareHelper::Creator
       method.
    2. Predefined parameters (test URI, bundle name, wait time, isSystem flag) are valid and accessible.
    3. The DataShareHelper::Creator method accepts the input parameters (string URI, CreateOptions, bundle name, int
       waitTime, bool isSystem).
 * @tc.step:
    1. Define a test string URI ("testUri"), a test bundle name ("testBundle"), set waitTime to 1, and isSystem to true.
    2. Create a CreateOptions instance, set its isProxy_ member to false and token_ member to nullptr.
    3. Call DataShareHelper::Creator with the test URI, created CreateOptions, bundle name, waitTime, and isSystem
       as parameters.
    4. Check the return value of the Creator function to verify if it is nullptr.
 * @tc.expect:
    1. The DataShareHelper::Creator function fails and returns nullptr.
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
 * @tc.name: CreatorTest002
 * @tc.desc: Test the Creator function of DataShareHelper when CreateOptions.isProxy_ is true and CreateOptions.token_
 *           is nullptr, verifying if the function fails and returns nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment supports instantiation of CreateOptions and calling the static DataShareHelper::Creator
       method.
    2. Predefined parameters (test URI, bundle name, wait time, isSystem flag) are valid and accessible.
    3. The DataShareHelper::Creator method accepts the input parameters (string URI, CreateOptions, bundle name, int
       waitTime, bool isSystem).
 * @tc.step:
    1. Define a test string URI ("testUri"), a test bundle name ("testBundle"), set waitTime to 1, and isSystem to true.
    2. Create a CreateOptions instance, set its isProxy_ member to true and token_ member to nullptr.
    3. Call DataShareHelper::Creator with the test URI, created CreateOptions, bundle name, waitTime, and isSystem
       as parameters.
    4. Check the return value of the Creator function to verify if it is nullptr.
 * @tc.expect:
    1. The DataShareHelper::Creator function fails and returns nullptr.
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
 * @tc.name: CreatorTest003
 * @tc.desc: Test the Creator function of the DataShareHelper class under the condition that CreateOptions.isProxy_ is
 *           set to false and CreateOptions.token_ is not a nullptr, verifying whether the Creator function can
 *           successfully create a valid DataShareHelper instance.
 * @tc.type: FUNC
 * @tc.require: issueICDSBD
 * @tc.precon:
    1. The SystemAbilityManagerClient::GetInstance() method can successfully obtain a valid SystemAbilityManager
       instance (non-nullptr).
    2. The predefined constant STORAGE_MANAGER_MANAGER_ID is a valid system ability ID, and the corresponding system
       ability is registered and accessible.
    3. Calling SystemAbilityManager::GetSystemAbility(STORAGE_MANAGER_MANAGER_ID) can return a non-nullptr
       IRemoteObject instance.
    4. The CreateOptions structure can be normally initialized, and its member variables (isProxy_, token_) support
       assignment operations.
    5. The predefined constant NON_SILENT_ACCESS_URI is a valid URI string and can be used as a parameter for
       DataShareHelper::Creator.
    6. The DataShareHelper::Creator static method is callable and supports parameters of type std::string (URI) and
       CreateOptions.
 * @tc.step:
    1. Initialize a CreateOptions object named options, and set options.isProxy_ to false.
    2. Call SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager() to obtain a SystemAbilityManager
       instance named saManager, and verify saManager is not a nullptr.
    3. Call saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID) to obtain an IRemoteObject instance named
       remoteObj, and verify remoteObj is not a nullptr; assign remoteObj to options.token_.
    4. Call the static method DataShareHelper::Creator, passing NON_SILENT_ACCESS_URI (as the target URI) and the
       configured options as parameters, and save the return value as a DataShareHelper pointer.
    5. Check the return value (DataShareHelper pointer) of the Creator function.
 * @tc.expect:
    1. The DataShareHelper::Creator function is called successfully without throwing exceptions or runtime errors.
    2. The DataShareHelper pointer returned by the Creator function is not a nullptr, indicating a valid instance is
       created.
 */
HWTEST_F(DataShareHelperTest, CreatorTest003, TestSize.Level0)
{
    LOG_INFO("DataShareHelperTest CreatorTest003::Start");
    CreateOptions options;
    options.isProxy_ = false;

auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
EXPECT_NE(saManager, nullptr);
auto remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
EXPECT_NE(remoteObj, nullptr);
options.token_ = remoteObj;
auto result = DataShareHelper::Creator(NON_SILENT_ACCESS_URI, options);
EXPECT_NE(result, nullptr);
LOG_INFO("DataShareHelperTest CreatorTest003::End");
}

/**
 * @tc.name: CreatorTest004
 * @tc.desc: Test the Creator function of DataShareHelper when CreateOptions.isProxy_ is true and CreateOptions.token_
 *           is not nullptr, verifying if the function fails and returns nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment supports SystemAbilityManagerClient::GetInstance() to obtain a valid SystemAbilityManager
       instance.
    2. The SystemAbilityManager can successfully get the system ability (STORAGE_MANAGER_MANAGER_ID) and return a
       non-null IRemoteObject.
    3. The DataShareHelper::Creator method accepts the input parameters (string URI and CreateOptions).
    4. The NON_SILENT_ACCESS_ERROR_URI constant is predefined and accessible.
 * @tc.step:
    1. Create a CreateOptions instance and set its isProxy_ member to true.
    2. Get the SystemAbilityManager via SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager(), then get
       the IRemoteObject for STORAGE_MANAGER_MANAGER_ID and assign it to CreateOptions.token_.
    3. Call DataShareHelper::Creator with NON_SILENT_ACCESS_ERROR_URI and the created CreateOptions as parameters.
    4. Check the return value of the Creator function to verify if it is nullptr.
 * @tc.expect:
    1. The DataShareHelper::Creator function fails and returns nullptr.
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
 * @tc.name: CreateExtHelper001
 * @tc.desc: Test the CreateExtHelper function of DataShareHelper when the input Uri contains the "appIndex=" query
 *           parameter, verifying if the function fails and returns nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment supports instantiation of Uri and setting its query_ member explicitly.
    2. The DataShareHelper::CreateExtHelper method accepts the input parameters (Uri, IRemoteObject*, int waitTime,
       bool isSystem).
    3. Predefined parameters (waitTime, isSystem flag) are valid for the method call.
 * @tc.step:
    1. Create an Uri instance with the string "datashareproxy://com.acts.ohos.data.datasharetest/test?appIndex=abcd".
    2. Set the query_ member of the Uri instance explicitly to "appIndex=abcd" to ensure the parameter exists.
    3. Set IRemoteObject* token to nullptr, waitTime to 1000, and isSystem to false.
    4. Call DataShareHelper::CreateExtHelper with the created Uri, token, waitTime, and isSystem as parameters.
    5. Check the return value of the CreateExtHelper function to verify if it is nullptr.
 * @tc.expect:
    1. The DataShareHelper::CreateExtHelper function fails and returns nullptr.
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
 * @tc.desc: Test the SetSilentSwitch function of DataShareHelper when DataShareManagerImpl::GetServiceProxy()
 *           returns nullptr, verifying if the function fails and returns DATA_SHARE_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The DataShareManagerImplHelper() function is available to configure DataShareManagerImpl such that
       GetServiceProxy() returns nullptr.
    2. The test environment supports instantiation of Uri and setting its query_ member.
    3. The DataShareHelper::SetSilentSwitch method accepts the input parameters (Uri, bool enable, bool isSystem).
    4. The DATA_SHARE_ERROR constant is predefined and accessible as the expected error code.
 * @tc.step:
    1. Call DataShareManagerImplHelper() to ensure DataShareManagerImpl::GetServiceProxy() returns nullptr.
    2. Create an Uri instance with "datashareproxy://com.acts.ohos.data.datasharetest/test?appIndex=abcd" and set
       its query_ to "appIndex=abcd".
    3. Set the enable flag to false and isSystem to false.
    4. Call DataShareHelper::SetSilentSwitch with the created Uri, enable, and isSystem as parameters.
    5. Check the return value of the SetSilentSwitch function to verify if it equals DATA_SHARE_ERROR.
 * @tc.expect:
    1. The DataShareHelper::SetSilentSwitch function fails and returns DATA_SHARE_ERROR.
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
 * @tc.desc: Test the GetSilentProxyStatus function of DataShareHelper when DataShareManagerImpl::GetServiceProxy()
 *           returns nullptr, verifying if the function fails and returns E_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The DataShareManagerImplHelper() function is available to configure DataShareManagerImpl such that
       GetServiceProxy() returns nullptr.
    2. The test environment supports using a string URI as input for the GetSilentProxyStatus method.
    3. The DataShareHelper::GetSilentProxyStatus method accepts the input parameters (string URI, bool isSystem).
    4. The E_ERROR constant is predefined and accessible as the expected error code.
 * @tc.step:
    1. Call DataShareManagerImplHelper() to ensure DataShareManagerImpl::GetServiceProxy() returns nullptr.
    2. Define a test string "datashareproxy://com.acts.ohos.data.datasharetest/test?appIndex=abcd".
    3. Set the isSystem flag to false.
    4. Call DataShareHelper::GetSilentProxyStatus with the test URI and isSystem as parameters.
    5. Check the return value of the GetSilentProxyStatus function to verify if it equals E_ERROR.
 * @tc.expect:
    1. The DataShareHelper::GetSilentProxyStatus function fails and returns E_ERROR.
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
 * @tc.desc: Test the CreateServiceHelper function of DataShareHelper when DataShareManagerImpl::GetServiceProxy()
 *           returns nullptr, verifying if the function fails and returns nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The DataShareManagerImplHelper() function is available to configure DataShareManagerImpl such that
       GetServiceProxy() returns nullptr.
    2. The DataShareHelper::CreateServiceHelper method accepts the input parameters (string URI, string bundleName,
       bool isSystem).
    3. Predefined parameters (test URI, bundle name, isSystem flag) are valid for the method call.
 * @tc.step:
    1. Call DataShareManagerImplHelper() to ensure DataShareManagerImpl::GetServiceProxy() returns nullptr.
    2. Define a test string URI ("testExuri") and a test bundle name ("bundleName").
    3. Set the isSystem flag to false.
    4. Call DataShareHelper::CreateServiceHelper with the test URI, bundle name, and isSystem as parameters.
    5. Check the return value of the CreateServiceHelper function to verify if it is nullptr.
 * @tc.expect:
    1. The DataShareHelper::CreateServiceHelper function fails and returns nullptr.
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