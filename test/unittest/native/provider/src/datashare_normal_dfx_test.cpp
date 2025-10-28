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

#define LOG_TAG "StubImplNormalTest"

#include "datashare_helper.h"

#include <gtest/gtest.h>

#include "datashare_stub_impl.h"
#include "accesstoken_kit.h"
#include "datashare_errno.h"
#include "datashare_log.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "token_setproc.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
const int STORAGE_MANAGER_MANAGER_ID = 5003;
std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetestsetup";
std::string SILENT_ACCESS_URI = "datashareproxy://com.acts.datasharetest/DataShareStubImpl?Proxy=true";
std::string TBL_STU_NAME = "name";
std::string TBL_STU_AGE = "age";
std::shared_ptr<DataShare::DataShareHelper> g_exHelper;

class DataShareNormalDfxTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int32_t systemAbilityId, std::string uri)
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
    return DataShare::DataShareHelper::Creator(remoteObj, uri);
}

void SetSelfSystemAPP()
{
    // set system app
    HapInfoParams info = {
        .userID = 100,
        .bundleName = "ohos.permission.write.demo",
        .instIndex = 0,
        .isSystemApp = true,
        .apiVersion = 8,
        .appIDDesc = "ohos.permission.write.demo"
    };
    HapPolicyParams policy = {
        .apl = APL_SYSTEM_CORE,
        .domain = "test.domain",
        .permStateList = {
            {
                .permissionName = "ohos.permission.GET_BUNDLE_INFO",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 }
            }
        }
    };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenIDEx(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId.tokenIDEx);
}

std::vector<PermissionStateFull> GetPermissionStateFulls()
{
    std::vector<PermissionStateFull> permissionStateFulls = {
        {
            .permissionName = "ohos.permission.WRITE_CONTACTS",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 }
        },
        {
            .permissionName = "ohos.permission.WRITE_CALL_LOG",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 }
        },
        {
            .permissionName = "ohos.permission.GET_BUNDLE_INFO",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 }
        },
        {
            .permissionName = "ohos.permission.MANAGE_SETTINGS",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 }
        }
    };
    return permissionStateFulls;
}

void DataShareNormalDfxTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    HapInfoParams info = {
        .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareclienttest.demo"
    };
    auto permStateList = GetPermissionStateFulls();
    HapPolicyParams policy = {
        .apl = APL_NORMAL,
        .domain = "test.domain",
        .permList = {
            {
                .permissionName = "ohos.permission.test",
                .bundleName = "ohos.datashareclienttest.demo",
                .grantMode = 1,
                .availableLevel = APL_NORMAL,
                .label = "label",
                .labelId = 1,
                .description = "ohos.datashareclienttest.demo",
                .descriptionId = 1
            }
        },
        .permStateList = permStateList
    };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenIDEx(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId.tokenIDEx);

    auto dataShareHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, DATA_SHARE_URI);
    ASSERT_TRUE(dataShareHelper != nullptr);
    int sleepTime = 3;
    sleep(sleepTime);

    g_exHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, DATA_SHARE_URI);
    ASSERT_TRUE(g_exHelper != nullptr);
    LOG_INFO("SetUpTestCase end");
}

void DataShareNormalDfxTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId.tokenIDEx);
    g_exHelper = nullptr;
}

void DataShareNormalDfxTest::SetUp(void) {}
void DataShareNormalDfxTest::TearDown(void) {}

/**
* @tc.name: NormalApp_Silent_Update_Test001
* @tc.desc: Verify silent access Update operation behavior when caller is normal app and provider not in allowList.
    This test is only for printing message.
* @tc.type: FUNC
* @tc.require: gitcode#852
* @tc.precon: Test process is set to be equivalent to a normal app
* @tc.step:
    1. Create a DataShareHelper instance with silent access configuration
    2. Set self as a system app
    3. Insert a data using DataShareHelper created in step 1
    4. Set self back to normal app
    5. Define update predicates to target data Inserted in step 3
    3. Prepare update data in a DataShareValuesBucket and call Update function
* @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Insert operation return greater than 0(success)
    3. Update operation return greater than 0(success)
*/
HWTEST_F(DataShareNormalDfxTest, NormalApp_Silent_Update_Test001, TestSize.Level1)
{
    LOG_INFO("NormalApp_Silent_Update_Test001::Start");
    auto originalToken = GetSelfTokenID();
    // Set system app
    SetSelfSystemAPP();

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SILENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);
    Uri uri(SILENT_ACCESS_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "zhangsan";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 18;
    valuesBucket.Put(TBL_STU_AGE, age);

    int retInsert = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retInsert > 0), true);
    // Set back to normal app
    SetSelfTokenID(originalToken);

    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'zhangsan'";
    predicates.SetWhereClause(selections);
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put(TBL_STU_AGE, 10);

    int retUpdate = helper->Update(uri, predicates, valuesBucket1);
    EXPECT_EQ((retUpdate > 0), true);
    LOG_INFO("NormalApp_Silent_Update_Test001::End");
}

/**
 * @tc.name: NormalApp_Active_Insert_Test001
 * @tc.desc: Verify Active(non-silent) access Insert operation behavior when caller is normal app
    and provider not in allowList. This test is only for printing message.
 * @tc.type: FUNC
 * @tc.require: gitcode#852
 * @tc.precon: Test process is set to be equivalent to a normal app
 * @tc.step:
    1. Create a DataShareHelper instance with non-silent access configuration
    2. Prepare test data(name and age) in a DataShareValuesBucket
    3. Perform initial Insert operation and verify result
 * @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Insert operation return greater than 0(success)
 */
HWTEST_F(DataShareNormalDfxTest, NormalApp_Active_Insert_Test001, TestSize.Level0)
{
    LOG_INFO("NormalApp_Active_Insert_Test001::Start");
    Uri uri(DATA_SHARE_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    int retVal = g_exHelper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("NormalApp_Active_Insert_Test001::End");
}
}
}