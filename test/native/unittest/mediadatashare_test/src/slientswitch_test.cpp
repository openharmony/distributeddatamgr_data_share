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

#define LOG_TAG "slientswitch_test"

#include <gtest/gtest.h>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "token_setproc.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;
std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";
std::string SLIENT_ACCESS_URI = "datashare:///com.acts.datasharetest/entry/DB00/TBL00?Proxy=true";
std::string DATA_SHARE_PROXY_URI = "datashare:///com.acts.ohos.data.datasharetest/test";
constexpr int SUBSCRIBER_ID = 1000;
std::string TBL_STU_NAME = "name";
std::string TBL_STU_AGE = "age";
int32_t ERROR = -1;
std::shared_ptr<DataShare::DataShareHelper> g_slientAccessHelper;

class SlientSwitchTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int32_t systemAbilityId,
    const std::string &silentProxyUri, const std::string &providerUri = "")
{
    LOG_INFO("CreateDataShareHelper start");
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        LOG_ERROR("GetSystemAbilityManager get samgr failed.");
        return nullptr;
    }
    auto remoteObj = saManager->GetSystemAbility(systemAbilityId);
    if (remoteObj == nullptr) {
        LOG_ERROR("GetSystemAbility service failed.");
        return nullptr;
    }
    return DataShare::DataShareHelper::Creator(remoteObj, silentProxyUri, providerUri);
}

void SlientSwitchTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    auto dataShareHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, DATA_SHARE_URI);
    ASSERT_TRUE(dataShareHelper != nullptr);
    int sleepTime = 3;
    sleep(sleepTime);

    HapInfoParams info = {
        .userID = 100,
        .bundleName = "com.acts.datasharetest",
        .instIndex = 0,
        .appIDDesc = "com.acts.datasharetest"
    };
    HapPolicyParams policy = {
        .apl = APL_NORMAL,
        .domain = "test.domain",
        .permList = {
            {
                .permissionName = "ohos.permission.test",
                .bundleName = "com.acts.datasharetest",
                .grantMode = 1,
                .availableLevel = APL_NORMAL,
                .label = "label",
                .labelId = 1,
                .description = "com.acts.datasharetest",
                .descriptionId = 1
            }
        },
        .permStateList = {
            {
                .permissionName = "ohos.permission.test",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 }
            }
        }
    };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenID(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId);

    g_slientAccessHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(g_slientAccessHelper != nullptr);
    LOG_INFO("SetUpTestCase end");
}

void SlientSwitchTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenID(100, "com.acts.datasharetest", 0);
    AccessTokenKit::DeleteToken(tokenId);
    g_slientAccessHelper = nullptr;
}

void SlientSwitchTest::SetUp(void) {}
void SlientSwitchTest::TearDown(void) {}

/**
 * @tc.name: SlientSwitch_SetSilentSwitch_Test_001
 * @tc.desc: Test the behavior of DataShareHelper::SetSilentSwitch when enabling the silent switch with a valid
 *           access URI (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The predefined constant SLIENT_ACCESS_URI is a valid Uri string and accessible.
    2. The DataShareHelper::SetSilentSwitch method accepts a Uri and a bool (enable/disable) as parameters and
       returns an integer result code.
    3. The E_OK constant is a valid return code indicating success.
 * @tc.step:
    1. Create a Uri object using the SLIENT_ACCESS_URI constant.
    2. Call DataShareHelper::SetSilentSwitch with the created Uri and true (to enable the silent switch).
    3. Check the integer return value of the SetSilentSwitch method.
 * @tc.expect:
    1. The SetSilentSwitch operation succeeds, returning E_OK.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SetSilentSwitch_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_001::End");
}

/**
 * @tc.name: SlientSwitch_SetSilentSwitch_Test_002
 * @tc.desc: Test the behavior of DataShareHelper::SetSilentSwitch when disabling the silent switch with a valid
 *           access URI (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The predefined constant SLIENT_ACCESS_URI is a valid Uri string and accessible.
    2. The DataShareHelper::SetSilentSwitch method accepts a Uri and a bool (enable/disable) as parameters and
       returns an integer result code.
    3. The E_OK constant is a valid return code indicating success.
 * @tc.step:
    1. Create a Uri object using the SLIENT_ACCESS_URI constant.
    2. Call DataShareHelper::SetSilentSwitch with the created Uri and false (to disable the silent switch).
    3. Check the integer return value of the SetSilentSwitch method.
 * @tc.expect:
    1. The SetSilentSwitch operation succeeds, returning E_OK.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SetSilentSwitch_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_002::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_002::End");
}

/**
 * @tc.name: SlientSwitch_SetSilentSwitch_Test_003
 * @tc.desc: Test the behavior of DataShareHelper::SetSilentSwitch when disabling the silent switch with an empty URI.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. An empty string ("") is a valid input for creating a Uri object.
    2. The DataShareHelper::SetSilentSwitch method accepts a Uri and a bool (enable/disable) as parameters and
       returns an integer result code.
    3. The E_OK constant is a valid return code indicating success.
 * @tc.step:
    1. Create an empty Uri object (using an empty string).
    2. Call DataShareHelper::SetSilentSwitch with the empty Uri and false (to disable the silent switch).
    3. Check the integer return value of the SetSilentSwitch method.
 * @tc.expect:
    1. The SetSilentSwitch operation succeeds, returning E_OK.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SetSilentSwitch_Test_003, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_003::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_003::End");
}

/**
 * @tc.name: SlientSwitch_SetSilentSwitch_Test_004
 * @tc.desc: Test the behavior of DataShareHelper::SetSilentSwitch when enabling the silent switch with an empty URI.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. An empty string ("") is a valid input for creating a Uri object.
    2. The DataShareHelper::SetSilentSwitch method accepts a Uri and a bool (enable/disable) as parameters and
       returns an integer result code.
    3. The E_OK constant is a valid return code indicating success.
 * @tc.step:
    1. Create an empty Uri object (using an empty string).
    2. Call DataShareHelper::SetSilentSwitch with the empty Uri and true (to enable the silent switch).
    3. Check the integer return value of the SetSilentSwitch method.
 * @tc.expect:
    1. The SetSilentSwitch operation succeeds, returning E_OK.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SetSilentSwitch_Test_004, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_004::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_004::End");
}

/**
 * @tc.name: SlientSwitch_SwitchDisable_Insert_Test_001
 * @tc.desc: Test data insertion via the Insert method when the silent switch is disabled using a valid URI
 *           (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The predefined constant SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid
       DataShareHelper instance.
    2. The DataShareValuesBucket class supports the Put method to add key-value pairs (e.g., TBL_STU_NAME,
       TBL_STU_AGE).
    3. The DataShareHelper::Insert method accepts a Uri and DataShareValuesBucket, returning an integer result
       (positive for success, <=0 for failure).
 * @tc.step:
    1. Create a Uri object using the SLIENT_ACCESS_URI constant.
    2. Disable the silent switch by calling DataShareHelper::SetSilentSwitch with the Uri and false; verify it returns
       E_OK.
    3. Prepare a DataShareValuesBucket: add "lisi" for TBL_STU_NAME and 25 for TBL_STU_AGE using Put.
    4. Call the Insert method of g_slientAccessHelper with the Uri and prepared values bucket.
    5. Check if the return value of Insert is <= 0.
 * @tc.expect:
    1. The Insert operation fails, with a return value <= 0.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Insert_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Insert_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), false);
    LOG_INFO("SlientSwitch_SwitchDisable_Insert_Test_001::End");
}

/**
 * @tc.name: SlientSwitch_SwitchDisable_Insert_Test_002
 * @tc.desc: Test data insertion via the Insert method when the silent switch is disabled using an empty URI,
 *           with insertion attempted on a valid URI (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The predefined constant SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid
       DataShareHelper instance.
    2. DataShareValuesBucket supports the Put method; DataShareHelper::Insert returns an integer result
       (positive for success, <=0 for failure).
    3. An empty Uri (created with "") is a valid input for SetSilentSwitch.
 * @tc.step:
    1. Create an empty Uri object; disable the silent switch by calling DataShareHelper::SetSilentSwitch with it
       and false; verify it returns E_OK.
    2. Prepare a DataShareValuesBucket: add "wangwu" for TBL_STU_NAME and 25 for TBL_STU_AGE using Put.
    3. Create a new Uri object using SLIENT_ACCESS_URI.
    4. Call the Insert method of g_slientAccessHelper with the new Uri and prepared values bucket.
    5. Check if the return value of Insert is <= 0.
 * @tc.expect:
    1. The Insert operation fails, with a return value <= 0.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Insert_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Insert_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "wangwu";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    uri = Uri(SLIENT_ACCESS_URI);
    retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), false);
    LOG_INFO("SlientSwitch_SwitchDisable_Insert_Test_002::End");
}

/**
 * @tc.name: SlientSwitch_SwitchEnable_Insert_Test_001
 * @tc.desc: Test data insertion via the Insert method when the silent switch is enabled using a valid URI
 *           (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The predefined constant SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid
       DataShareHelper instance.
    2. The DataShareValuesBucket class supports the Put method to add key-value pairs (e.g., TBL_STU_NAME,
       TBL_STU_AGE).
    3. The DataShareHelper::Insert method accepts a Uri and DataShareValuesBucket, returning an integer result
       (positive for success, <=0 for failure).
 * @tc.step:
    1. Create a Uri object using the SLIENT_ACCESS_URI constant.
    2. Enable the silent switch by calling DataShareHelper::SetSilentSwitch with the Uri and true; verify it returns
       E_OK.
    3. Prepare a DataShareValuesBucket: add "lisi" for TBL_STU_NAME and 25 for TBL_STU_AGE using Put.
    4. Call the Insert method of g_slientAccessHelper with the Uri and prepared values bucket.
    5. Check if the return value of Insert is > 0.
 * @tc.expect:
    1. The Insert operation succeeds, with a return value > 0.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Insert_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Insert_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SlientSwitch_SwitchEnable_Insert_Test_001::End");
}

/**
 * @tc.name: SlientSwitch_SwitchEnable_Insert_Test_002
 * @tc.desc: Test data insertion via the Insert method when the silent switch is enabled using an empty URI,
 *           with insertion attempted on a valid URI (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The predefined constant SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid
       DataShareHelper instance.
    2. DataShareValuesBucket supports the Put method; DataShareHelper::Insert returns an integer result
       (positive for success, <=0 for failure).
    3. An empty Uri (created with "") is a valid input for SetSilentSwitch.
 * @tc.step:
    1. Create an empty Uri object; enable the silent switch by calling DataShareHelper::SetSilentSwitch with it
       and true; verify it returns E_OK.
    2. Prepare a DataShareValuesBucket: add "wangwu" for TBL_STU_NAME and 25 for TBL_STU_AGE using Put.
    3. Create a new Uri object using SLIENT_ACCESS_URI.
    4. Call the Insert method of g_slientAccessHelper with the new Uri and prepared values bucket.
    5. Check if the return value of Insert is > 0.
 * @tc.expect:
    1. The Insert operation succeeds, with a return value > 0.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Insert_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Insert_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "wangwu";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    uri = Uri(SLIENT_ACCESS_URI);
    retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SlientSwitch_SwitchEnable_Insert_Test_002::End");
}

/**
 * @tc.name: SlientSwitch_SwitchDisable_Update_Test_001
 * @tc.desc: Test data update via the Update method when the silent switch is disabled using a valid URI
 *           (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The predefined constant SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid
       DataShareHelper instance for silent access.
    2. DataShareValuesBucket supports the Put method to add update data (e.g., TBL_STU_AGE = 50).
    3. DataSharePredicates supports the SetWhereClause method to define filter conditions (e.g., TBL_STU_NAME =
       "lisi").
    4. DataShareHelper::Update returns an integer result (positive for success, <= 0 for failure) when passed
       a Uri, DataSharePredicates, and DataShareValuesBucket.
 * @tc.step:
    1. Create a Uri object using the SLIENT_ACCESS_URI constant.
    2. Disable the silent switch by calling DataShareHelper::SetSilentSwitch with the Uri and false; verify
       the return value is E_OK.
    3. Prepare a DataShareValuesBucket: use Put to add TBL_STU_AGE with the value 50.
    4. Create a DataSharePredicates object, then call SetWhereClause to set the condition: TBL_STU_NAME + " = 'lisi'".
    5. Call the Update method of g_slientAccessHelper with the Uri, predicates, and values bucket.
    6. Check if the return value of Update is <= 0.
 * @tc.expect:
    1. The Update operation fails, with a return value <= 0.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Update_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Update_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    int value = 50;
    valuesBucket.Put(TBL_STU_AGE, value);
    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'lisi'";
    predicates.SetWhereClause(selections);
    retVal = helper->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((retVal > 0), false);
    LOG_INFO("SlientSwitch_SwitchDisable_Update_Test_001::End");
}

/**
 * @tc.name: SlientSwitch_SwitchDisable_Update_Test_002
 * @tc.desc: Test data update via the Update method when the silent switch is disabled using an empty URI,
 *           with the update attempted on a valid URI (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. An empty string ("") is valid for creating a Uri object and passing to SetSilentSwitch.
    2. Predefined SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid DataShareHelper instance.
    3. DataShareValuesBucket supports Put for update data; DataSharePredicates supports SetWhereClause for filters.
    4. DataShareHelper::Update returns an integer (positive for success, <= 0 for failure).
 * @tc.step:
    1. Create an empty Uri object; call DataShareHelper::SetSilentSwitch with this Uri and false to disable the switch,
       then verify the return value is E_OK.
    2. Prepare a DataShareValuesBucket: use Put to add TBL_STU_AGE with the value 50.
    3. Create a DataSharePredicates object, call SetWhereClause to set the condition: TBL_STU_NAME + " = 'wangwu'".
    4. Create a new Uri object using the SLIENT_ACCESS_URI constant.
    5. Call the Update method of g_slientAccessHelper with the new Uri, predicates, and values bucket.
    6. Check if the return value of Update is <= 0.
 * @tc.expect:
    1. The Update operation fails, with a return value <= 0.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Update_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Update_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    int value = 50;
    valuesBucket.Put(TBL_STU_AGE, value);
    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'wangwu'";
    predicates.SetWhereClause(selections);
    uri = Uri(SLIENT_ACCESS_URI);
    retVal = helper->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((retVal > 0), false);
    LOG_INFO("SlientSwitch_SwitchDisable_Update_Test_002::End");
}

/**
 * @tc.name: SlientSwitch_SwitchEnable_Update_Test_001
 * @tc.desc: Test data update via the Update method when the silent switch is enabled using a valid URI
 *           (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. Predefined SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid DataShareHelper instance.
    2. DataShareValuesBucket supports Put to add update data (e.g., TBL_STU_AGE = 50); DataSharePredicates supports
       SetWhereClause to define filter conditions (e.g., TBL_STU_NAME = "lisi").
    3. DataShareHelper::Update returns an integer (positive for success, <= 0 for failure).
 * @tc.step:
    1. Create a Uri object using the SLIENT_ACCESS_URI constant.
    2. Enable the silent switch by calling DataShareHelper::SetSilentSwitch with the Uri and true; verify the
       return value is E_OK.
    3. Prepare a DataShareValuesBucket: use Put to add TBL_STU_AGE with the value 50.
    4. Create a DataSharePredicates object, call SetWhereClause to set the condition: TBL_STU_NAME + " = 'lisi'".
    5. Call the Update method of g_slientAccessHelper with the Uri, predicates, and values bucket.
    6. Check if the return value of Update is > 0.
 * @tc.expect:
    1. The Update operation succeeds, with a return value > 0.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Update_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Update_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    int value = 50;
    valuesBucket.Put(TBL_STU_AGE, value);
    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'lisi'";
    predicates.SetWhereClause(selections);
    retVal = helper->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SlientSwitch_SwitchEnable_Update_Test_001::End");
}

/**
 * @tc.name: SlientSwitch_SwitchEnable_Update_Test_002
 * @tc.desc: Test data update via the Update method when the silent switch is enabled using an empty URI,
 *           with the update attempted on a valid URI (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. An empty string ("") is valid for creating a Uri object and passing to SetSilentSwitch.
    2. Predefined SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid DataShareHelper instance.
    3. DataShareValuesBucket supports Put for update data; DataSharePredicates supports SetWhereClause for filters.
    4. DataShareHelper::Update returns an integer (positive for success, <= 0 for failure).
 * @tc.step:
    1. Create an empty Uri object; call DataShareHelper::SetSilentSwitch with this Uri and true to enable the switch,
       then verify the return value is E_OK.
    2. Prepare a DataShareValuesBucket: use Put to add TBL_STU_AGE with the value 50.
    3. Create a DataSharePredicates object, call SetWhereClause to set the condition: TBL_STU_NAME + " = 'wangwu'".
    4. Create a new Uri object using the SLIENT_ACCESS_URI constant.
    5. Call the Update method of g_slientAccessHelper with the new Uri, predicates, and values bucket.
    6. Check if the return value of Update is > 0.
 * @tc.expect:
    1. The Update operation succeeds, with a return value > 0.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Update_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Update_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    int value = 50;
    valuesBucket.Put(TBL_STU_AGE, value);
    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'wangwu'";
    predicates.SetWhereClause(selections);
    uri = Uri(SLIENT_ACCESS_URI);
    retVal = helper->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SlientSwitch_SwitchEnable_Update_Test_002::End");
}

/**
 * @tc.name: SlientSwitch_SwitchDisable_Query_Test_001
 * @tc.desc: Test data query via the Query method when the silent switch is disabled using a valid URI
 *           (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. Predefined SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid DataShareHelper instance.
    2. DataSharePredicates supports the EqualTo method to define filter conditions (e.g., TBL_STU_NAME = "lisi").
    3. DataShareHelper::Query returns a shared_ptr<ResultSet> (nullptr indicates query failure) when passed a Uri,
       DataSharePredicates, and empty column list.
 * @tc.step:
    1. Create a Uri object using the SLIENT_ACCESS_URI constant.
    2. Disable the silent switch by calling DataShareHelper::SetSilentSwitch with the Uri and false; verify the
       return value is E_OK.
    3. Create a DataSharePredicates object, call EqualTo to set the condition: TBL_STU_NAME = "lisi".
    4. Initialize an empty vector<string> for query columns.
    5. Call the Query method of g_slientAccessHelper with the Uri, predicates, and empty column list.
    6. Check if the returned ResultSet is nullptr.
 * @tc.expect:
    1. The Query operation fails, with the returned ResultSet being nullptr.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Query_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Query_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "lisi");
    vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    ASSERT_TRUE(resultSet == nullptr);
    LOG_INFO("SlientSwitch_SwitchDisable_Query_Test_001::End");
}

/**
 * @tc.name: SlientSwitch_SwitchDisable_Query_Test_002
 * @tc.desc: Test data query via the Query method when the silent switch is disabled using an empty URI,
 *           with the query attempted on a valid URI (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. An empty string ("") is valid for creating a Uri object and passing to SetSilentSwitch.
    2. Predefined SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid DataShareHelper instance.
    3. DataSharePredicates supports EqualTo for filters; DataShareHelper::Query returns nullptr for failed queries.
 * @tc.step:
    1. Create an empty Uri object; call DataShareHelper::SetSilentSwitch with this Uri and false to disable the switch,
       then verify the return value is E_OK.
    2. Create a DataSharePredicates object, call EqualTo to set the condition: TBL_STU_NAME = "wangwu".
    3. Initialize an empty vector<string> for query columns.
    4. Create a new Uri object using the SLIENT_ACCESS_URI constant.
    5. Call the Query method of g_slientAccessHelper with the new Uri, predicates, and empty column list.
    6. Check if the returned ResultSet is nullptr.
 * @tc.expect:
    1. The Query operation fails, with the returned ResultSet being nullptr.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Query_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Query_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "wangwu");
    vector<string> columns;
    uri = Uri(SLIENT_ACCESS_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    ASSERT_TRUE(resultSet == nullptr);
    LOG_INFO("SlientSwitch_SwitchDisable_Query_Test_002::End");
}

/**
 * @tc.name: SlientSwitch_SwitchEnable_Query_Test_001
 * @tc.desc: Test data query via the Query method when the silent switch is enabled using a valid URI
 *           (SLIENT_ACCESS_URI), verifying the returned record count is 1.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. Predefined SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid DataShareHelper instance.
    2. DataSharePredicates supports EqualTo to filter for TBL_STU_NAME = "lisi"; the target record exists in the data
       source.
    3. ResultSet supports the GetRowCount method to retrieve the number of matched records; DataShareHelper::Query
       returns a non-null ResultSet for successful queries.
 * @tc.step:
    1. Create a Uri object using the SLIENT_ACCESS_URI constant.
    2. Enable the silent switch by calling DataShareHelper::SetSilentSwitch with the Uri and true; verify the
       return value is E_OK.
    3. Create a DataSharePredicates object, call EqualTo to set the condition: TBL_STU_NAME = "lisi".
    4. Initialize an empty vector<string> for query columns.
    5. Call the Query method of g_slientAccessHelper with the Uri, predicates, and empty column list.
    6. If the ResultSet is non-null, call GetRowCount to get the number of records; check the count.
 * @tc.expect:
    1. The Query operation succeeds, with a non-null ResultSet.
    2. The number of records returned by GetRowCount is 1.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Query_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Query_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "lisi");
    vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("SlientSwitch_SwitchEnable_Query_Test_001::End");
}

/**
 * @tc.name: SlientSwitch_SwitchEnable_Query_Test_002
 * @tc.desc: Test data query via the Query method when the silent switch is enabled using an empty URI,
 *           with the query attempted on a valid URI (SLIENT_ACCESS_URI), verifying the returned record count is 1.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. An empty string ("") is valid for creating a Uri object and passing to SetSilentSwitch.
    2. Predefined SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid DataShareHelper instance.
    3. DataSharePredicates supports EqualTo for TBL_STU_NAME = "wangwu"; the target record exists.
    4. ResultSet supports GetRowCount; Query returns non-null for successful queries.
 * @tc.step:
    1. Create an empty Uri object; call DataShareHelper::SetSilentSwitch with this Uri and true to enable the switch,
       then verify the return value is E_OK.
    2. Create a DataSharePredicates object, call EqualTo to set the condition: TBL_STU_NAME = "wangwu".
    3. Initialize an empty vector<string> for query columns.
    4. Create a new Uri object using the SLIENT_ACCESS_URI constant.
    5. Call the Query method of g_slientAccessHelper with the new Uri, predicates, and empty column list.
    6. If the ResultSet is non-null, call GetRowCount to get the record count; check the count.
 * @tc.expect:
    1. The Query operation succeeds, with a non-null ResultSet.
    2. The number of records returned by GetRowCount is 1.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Query_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Query_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "wangwu");
    vector<string> columns;
    uri = Uri(SLIENT_ACCESS_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("SlientSwitch_SwitchEnable_Query_Test_002::End");
}

/**
 * @tc.name: SlientSwitch_SwitchDisable_Delete_Test_001
 * @tc.desc: Test data deletion via the Delete method when the silent switch is disabled using a valid URI
 *           (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The predefined constant SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid
       DataShareHelper instance for silent access.
    2. DataSharePredicates supports the SetWhereClause method to define filter conditions (e.g., TBL_STU_NAME =
       "lisi").
    3. DataShareHelper::Delete returns an integer result (positive for success, <= 0 for failure) when passed
       a Uri and DataSharePredicates.
 * @tc.step:
    1. Create a Uri object using the SLIENT_ACCESS_URI constant.
    2. Disable the silent switch by calling DataShareHelper::SetSilentSwitch with the Uri and false; verify
       the return value is E_OK.
    3. Create a DataSharePredicates object, then call SetWhereClause to set the condition: TBL_STU_NAME + " = 'lisi'".
    4. Call the Delete method of g_slientAccessHelper with the Uri and predicates.
    5. Check if the return value of Delete is <= 0.
 * @tc.expect:
    1. The Delete operation fails, with a return value <= 0.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Delete_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Delete_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'lisi'";
    deletePredicates.SetWhereClause(selections);
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal > 0), false);
    LOG_INFO("SlientSwitch_SwitchDisable_Delete_Test_001::End");
}

/**
 * @tc.name: SlientSwitch_SwitchDisable_Delete_Test_002
 * @tc.desc: Test data deletion via the Delete method when the silent switch is disabled using an empty URI,
 *           with the deletion attempted on a valid URI (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. An empty string ("") is valid for creating a Uri object and passing to DataShareHelper::SetSilentSwitch.
    2. Predefined SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid DataShareHelper instance.
    3. DataSharePredicates supports SetWhereClause for filter conditions; DataShareHelper::Delete returns an integer
       (positive for success, <= 0 for failure).
 * @tc.step:
    1. Create an empty Uri object; call DataShareHelper::SetSilentSwitch with this Uri and false to disable the switch,
       then verify the return value is E_OK.
    2. Create a DataSharePredicates object, call SetWhereClause to set the condition: TBL_STU_NAME + " = 'wangwu'".
    3. Create a new Uri object using the SLIENT_ACCESS_URI constant.
    4. Call the Delete method of g_slientAccessHelper with the new Uri and predicates.
    5. Check if the return value of Delete is <= 0.
 * @tc.expect:
    1. The Delete operation fails, with a return value <= 0.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Delete_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Delete_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'wangwu'";
    deletePredicates.SetWhereClause(selections);
    uri = Uri(SLIENT_ACCESS_URI);
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal > 0), false);
    LOG_INFO("SlientSwitch_SwitchDisable_Delete_Test_002::End");
}

/**
 * @tc.name: SlientSwitch_SwitchEnable_Delete_Test_001
 * @tc.desc: Test data deletion via the Delete method when the silent switch is enabled using a valid URI
 *           (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. Predefined SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid DataShareHelper instance.
    2. DataSharePredicates supports SetWhereClause to define filter conditions (e.g., TBL_STU_NAME = "lisi"); the
       target record exists in the data source.
    3. DataShareHelper::Delete returns an integer (positive for success, <= 0 for failure).
 * @tc.step:
    1. Create a Uri object using the SLIENT_ACCESS_URI constant.
    2. Enable the silent switch by calling DataShareHelper::SetSilentSwitch with the Uri and true; verify the
       return value is E_OK.
    3. Create a DataSharePredicates object, call SetWhereClause to set the condition: TBL_STU_NAME + " = 'lisi'".
    4. Call the Delete method of g_slientAccessHelper with the Uri and predicates.
    5. Check if the return value of Delete is > 0.
 * @tc.expect:
    1. The Delete operation succeeds, with a return value > 0.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Delete_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Delete_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'lisi'";
    deletePredicates.SetWhereClause(selections);
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SlientSwitch_SwitchEnable_Delete_Test_001::End");
}

/**
 * @tc.name: SlientSwitch_SwitchEnable_Delete_Test_002
 * @tc.desc: Test data deletion via the Delete method when the silent switch is enabled using an empty URI,
 *           with the deletion attempted on a valid URI (SLIENT_ACCESS_URI).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. An empty string ("") is valid for creating a Uri object and passing to SetSilentSwitch.
    2. Predefined SLIENT_ACCESS_URI is a valid Uri string; g_slientAccessHelper is a valid DataShareHelper instance.
    3. DataSharePredicates supports SetWhereClause for filters; the target record (TBL_STU_NAME = "wangwu") exists.
    4. DataShareHelper::Delete returns an integer (positive for success, <= 0 for failure).
 * @tc.step:
    1. Create an empty Uri object; call DataShareHelper::SetSilentSwitch with this Uri and true to enable the switch,
       then verify the return value is E_OK.
    2. Create a DataSharePredicates object, call SetWhereClause to set the condition: TBL_STU_NAME + " = 'wangwu'".
    3. Create a new Uri object using the SLIENT_ACCESS_URI constant.
    4. Call the Delete method of g_slientAccessHelper with the new Uri and predicates.
    5. Check if the return value of Delete is > 0.
 * @tc.expect:
    1. The Delete operation succeeds, with a return value > 0.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Delete_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Delete_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'wangwu'";
    deletePredicates.SetWhereClause(selections);
    uri = Uri(SLIENT_ACCESS_URI);
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SlientSwitch_SwitchEnable_Delete_Test_002::End");
}

/**
 * @tc.name: SlientSwitch_SwitchDisable_CreateHelper_Test_001
 * @tc.desc: Test the creation of DataShareHelper instances when the silent switch is disabled: verify the first
 *           creation succeeds, the second fails, and re-enabling the switch works.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The CreateDataShareHelper function accepts STORAGE_MANAGER_MANAGER_ID and SLIENT_ACCESS_URI as parameters,
       returning a non-null DataShareHelper on success and nullptr on failure.
    2. Predefined constants (STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, E_OK) are valid and accessible.
    3. DataShareHelper::SetSilentSwitch accepts an empty Uri and returns E_OK for successful switch state changes.
 * @tc.step:
    1. Call CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID and SLIENT_ACCESS_URI to create the first helper;
       verify it is non-null.
    2. Create an empty Uri object; call DataShareHelper::SetSilentSwitch with this Uri and false to disable the switch,
       then verify the return value is E_OK.
    3. Call CreateDataShareHelper again with the same parameters as step 1 to create the second helper; check if it is
       nullptr.
    4. Call DataShareHelper::SetSilentSwitch with the empty Uri and true to re-enable the switch; verify the return
       value is E_OK.
 * @tc.expect:
    1. The first DataShareHelper instance is non-null (creation succeeds).
    2. The second DataShareHelper instance is nullptr (creation fails).
    3. Re-enabling the silent switch returns E_OK.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_CreateHelper_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_CreateHelper_Test_001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);

    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(helper == nullptr);
    retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);
    LOG_INFO("SlientSwitch_SwitchDisable_CreateHelper_Test_001::End");
}

Template GetTemplate()
{
    PredicateTemplateNode node1("p1", "select name0 as name from TBL00");
    PredicateTemplateNode node2("p2", "select name1 as name from TBL00");
    std::vector<PredicateTemplateNode> nodes;
    nodes.emplace_back(node1);
    nodes.emplace_back(node2);
    Template tpl(nodes, "select name1 as name from TBL00");
    return tpl;
}

/**
 * @tc.name: SlientSwitch_SwitchDisable_CreateHelper_Test_002
 * @tc.desc: Test DataShareHelper creation and the AddQueryTemplate method when the silent switch is disabled:
 *           verify helper creation succeeds, AddQueryTemplate fails, and re-enabling the switch works.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. CreateDataShareHelper accepts STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, and DATA_SHARE_URI as parameters,
       returning a non-null DataShareHelper on success.
    2. The GetTemplate() function returns a valid Template object; AddQueryTemplate accepts DATA_SHARE_PROXY_URI,
       SUBSCRIBER_ID, and Template, returning ERROR (-1) on failure.
    3. Predefined constants (DATA_SHARE_URI, DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, ERROR, E_OK) are valid.
 * @tc.step:
    1. Create an empty Uri object; call DataShareHelper::SetSilentSwitch with this Uri and false to disable the switch,
       then verify the return value is E_OK.
    2. Call CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, and DATA_SHARE_URI to create a
       helper;
       verify it is non-null.
    3. Call GetTemplate() to obtain a valid Template object.
    4. Call the helper's AddQueryTemplate method with DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, and the Template; check if
       it returns ERROR.
    5. Call DataShareHelper::SetSilentSwitch with the empty Uri and true to re-enable the switch; verify the return
       value is E_OK.
 * @tc.expect:
    1. The DataShareHelper instance is non-null (creation succeeds).
    2. The AddQueryTemplate method returns ERROR (-1) (operation fails).
    3. Re-enabling the silent switch returns E_OK.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_CreateHelper_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_CreateHelper_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, DATA_SHARE_URI);
    ASSERT_TRUE(helper != nullptr);

    auto tpl = GetTemplate();
    auto result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, ERROR);
    retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);
    LOG_INFO("SlientSwitch_SwitchDisable_CreateHelper_Test_002::End");
}

/**
 * @tc.name: SlientSwitch_SwitchEnable_CreateHelper_Test_001
 * @tc.desc: Test DataShareHelper creation (without provider URI) and AddQueryTemplate when the silent switch is
 *           enabled: verify helper creation succeeds, AddQueryTemplate returns E_BUNDLE_NAME_NOT_EXIST.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. CreateDataShareHelper accepts STORAGE_MANAGER_MANAGER_ID and SLIENT_ACCESS_URI (without provider URI) as
       parameters, returning a non-null DataShareHelper on success.
    2. The GetTemplate() function returns a valid Template object; AddQueryTemplate accepts DATA_SHARE_PROXY_URI,
       SUBSCRIBER_ID, and Template, returning E_BUNDLE_NAME_NOT_EXIST under specific conditions.
    3. Predefined constants (E_OK, E_BUNDLE_NAME_NOT_EXIST) are valid; SetSilentSwitch works with empty Uri.
 * @tc.step:
    1. Create an empty Uri object; call DataShareHelper::SetSilentSwitch with this Uri and true to enable the switch,
       then verify the return value is E_OK.
    2. Call CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID and SLIENT_ACCESS_URI (no provider URI) to create a
       helper; verify it is non-null.
    3. Call GetTemplate() to obtain a valid Template object.
    4. Call the helper's AddQueryTemplate method with DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, and the Template; check the
       return value.
 * @tc.expect:
    1. The DataShareHelper instance is non-null (creation succeeds).
    2. The AddQueryTemplate method returns E_BUNDLE_NAME_NOT_EXIST.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_CreateHelper_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_CreateHelper_Test_001::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);

    auto tpl = GetTemplate();
    auto result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, E_BUNDLE_NAME_NOT_EXIST);
    LOG_INFO("SlientSwitch_SwitchEnable_CreateHelper_Test_001::End");
}

/**
 * @tc.name: SlientSwitch_SwitchEnable_CreateHelper_Test_002
 * @tc.desc: Test DataShareHelper creation (with provider URI) and AddQueryTemplate when the silent switch is enabled:
 *           verify helper creation succeeds, AddQueryTemplate returns E_BUNDLE_NAME_NOT_EXIST.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. CreateDataShareHelper accepts STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, and DATA_SHARE_URI (provider URI)
       as parameters, returning a non-null DataShareHelper on success.
    2. The GetTemplate() function returns a valid Template object; AddQueryTemplate accepts DATA_SHARE_PROXY_URI,
       SUBSCRIBER_ID, and Template, returning E_BUNDLE_NAME_NOT_EXIST under specific conditions.
    3. Predefined constants (E_OK, E_BUNDLE_NAME_NOT_EXIST, DATA_SHARE_URI) are valid; SetSilentSwitch works with empty
       Uri.
 * @tc.step:
    1. Create an empty Uri object; call DataShareHelper::SetSilentSwitch with this Uri and true to enable the switch,
       then verify the return value is E_OK.
    2. Call CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, and DATA_SHARE_URI (provider URI)
       to create a helper; verify it is non-null.
    3. Call GetTemplate() to obtain a valid Template object.
    4. Call the helper's AddQueryTemplate method with DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, and the Template; check the
       return value.
 * @tc.expect:
    1. The DataShareHelper instance is non-null (creation succeeds).
    2. The AddQueryTemplate method returns E_BUNDLE_NAME_NOT_EXIST.
 */
HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_CreateHelper_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_CreateHelper_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, DATA_SHARE_URI);
    ASSERT_TRUE(helper != nullptr);

    auto tpl = GetTemplate();
    auto result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, E_BUNDLE_NAME_NOT_EXIST);
    LOG_INFO("SlientSwitch_SwitchEnable_CreateHelper_Test_002::End");
}
} // namespace DataShare
} // namespace OHOS