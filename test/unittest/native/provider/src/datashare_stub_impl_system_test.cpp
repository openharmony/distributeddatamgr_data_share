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

#define LOG_TAG "StubImplSystemTest"

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
std::string VERIFIED_CREATE_URI = "datashare:///com.ohos.settingsdata.DataAbility";
std::string VERIFIED_QUERY_URI = "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
std::string TBL_STU_NAME = "name";
std::string TBL_STU_AGE = "age";
std::string TBL_SETTINGS_COL1 = "KEYWORD";
std::string TBL_SETTINGS_COL2 = "VALUE";
const int PERMISSION_ERR_CODE = -2;
std::shared_ptr<DataShare::DataShareHelper> g_exHelper;
std::shared_ptr<DataShare::DataShareHelper> settings_exHelper;

class DataShareStubImplSystemTest : public testing::Test {
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
            .permissionName = "ohos.permission.MANAGE_SECURE_SETTINGS",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 }
        }
    };
    return permissionStateFulls;
}

void DataShareStubImplSystemTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    HapInfoParams info = {
        .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .isSystemApp = true,
        .appIDDesc = "ohos.datashareclienttest.demo"
    };
    auto permStateList = GetPermissionStateFulls();
    HapPolicyParams policy = {
        .apl = APL_SYSTEM_CORE,
        .domain = "test.domain",
        .permList = {
            {
                .permissionName = "ohos.permission.test",
                .bundleName = "ohos.datashareclienttest.demo",
                .grantMode = 1,
                .availableLevel = APL_SYSTEM_CORE,
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

    settings_exHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, VERIFIED_CREATE_URI);
    ASSERT_TRUE(settings_exHelper != nullptr);
    LOG_INFO("SetUpTestCase end");
}

void DataShareStubImplSystemTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId.tokenIDEx);
    g_exHelper = nullptr;
    settings_exHelper = nullptr;
}

void DataShareStubImplSystemTest::SetUp(void) {}
void DataShareStubImplSystemTest::TearDown(void) {}

/**
 * @tc.name: SystemApp_Silent_Insert_Test001
 * @tc.desc: Verify silent access Insert operation behavior when caller is system app and provider not in allowList
 * @tc.type: FUNC
 * @tc.require: gitcode#852
 * @tc.precon: Test process is set to be equivalent to a system app
 * @tc.step:
    1. Create a DataShareHelper instance with silent access configuration
    2. Prepare test data(name and age) in a DataShareValuesBucket
    3. Perform initial Insert operation and verify result
 * @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Insert operation return positive value(success)
 */
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Silent_Insert_Test001, TestSize.Level0)
{
    LOG_INFO("SystemApp_Silent_Insert_Test001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SILENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);

    Uri uri(SILENT_ACCESS_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SystemApp_Silent_Insert_Test001::End");
}

/**
* @tc.name: SystemApp_Silent_Update_Test001
* @tc.desc: Verify silent access Update operation behavior when caller is system app and provider not in allowList
* @tc.type: FUNC
* @tc.require: gitcode#852
* @tc.precon: Test process is set to be equivalent to a system app
* @tc.step:
    1. Create a DataShareHelper instance with silent access configuration
    2. Set self as a system app
    3. Insert a data using DataShareHelper created in step 1
    4. Set self back to system app
    5. Define update predicates to target data Inserted in step 3
    6. Prepare update data in a DataShareValuesBucket and call Update function
* @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Insert operation return greater than 0(success)
    3. Update operation return greater than 0(success)
*/
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Silent_Update_Test001, TestSize.Level1)
{
    LOG_INFO("SystemApp_Silent_Update_Test001::Start");

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

    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'zhangsan'";
    predicates.SetWhereClause(selections);
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put(TBL_STU_AGE, 10);

    int retUpdate = helper->Update(uri, predicates, valuesBucket1);
    EXPECT_EQ((retUpdate > 0), true);
    LOG_INFO("SystemApp_Silent_Update_Test001::End");
}

/**
 * @tc.name: SystemApp_Silent_Query_Test001
 * @tc.desc: Verify silent access Query operation behavior when caller is system app and provider not in allowList
 * @tc.type: FUNC
 * @tc.require: gitcode#852
 * @tc.precon: Test process is set to be equivalent to a system app
 * @tc.step:
    1. Create a DataShareHelper instance with silent access configuration
    2. Set self as a system app
    3. Insert a data using DataShareHelper created in step 1
    4. Set self back to system app
    5. Define query predicates to target data Inserted in step 3
    6. Perform Query operation and verify resultSet
* @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Insert operation return greater than 0(success)
    3. Query operation returns resultSet that is not nullptr and rowcount is greater than 0(success)
 */
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Silent_Query_Test001, TestSize.Level0)
{
    LOG_INFO("SystemApp_Silent_Query_Test001::Start");
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

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "zhangsan");
    vector<string> columns;
    int totalRow = 0;

    auto resultSet = helper->Query(uri, predicates, columns);
    ASSERT_NE(resultSet, nullptr);
    resultSet->GetRowCount(totalRow);
    EXPECT_TRUE(totalRow > 0);
    resultSet->Close();
    LOG_INFO("SystemApp_Silent_Query_Test001::End");
}

/**
* @tc.name: SystemApp_Silent_Delete_Test001
* @tc.desc: Verify silent access Delete operation behavior when caller is system app and provider not in allowList
* @tc.type: FUNC
* @tc.require: gitcode#852
* @tc.precon: None
* @tc.step:
    1. Create a DataShareHelper instance with silent access configuration
    2. Define delete predicates targeting non-existent data
    3. Perform Delete operation and verify result
* @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Delete operation return 0(no data deleted, execute success)
*/
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Silent_Delete_Test001, TestSize.Level1)
{
    LOG_INFO("SystemApp_Silent_Delete_Test001::Start");

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SILENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);
    Uri uri(SILENT_ACCESS_URI);

    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'lisan'";
    deletePredicates.SetWhereClause(selections);

    int retDelete = helper->Delete(uri, deletePredicates);
    EXPECT_EQ(retDelete, 0);
    LOG_INFO("SystemApp_Silent_Delete_Test001::End");
}

/**
 * @tc.name: SystemApp_Silent_InsertEx_Test001
 * @tc.desc: Verify silent access InsertEx operation behavior when caller is system app and provider not in allowList
 * @tc.type: FUNC
 * @tc.require: gitcode#852
 * @tc.precon: Test process is set to be equivalent to a system app
 * @tc.step:
    1. Create a DataShareHelper instance with silent access configuration
    2. Prepare test data(name and age) in a DataShareValuesBucket
    3. Perform initial InsertEx operation and verify result
 * @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. InsertEx operation return <0, positive value>(success)
 */
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Silent_InsertEx_Test001, TestSize.Level0)
{
    LOG_INFO("SystemApp_Silent_InsertEx_Test001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SILENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);

    Uri uri(SILENT_ACCESS_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    auto [errCode, retInsertEx] = helper->InsertEx(uri, valuesBucket);
    EXPECT_EQ(errCode, 0);
    EXPECT_TRUE(retInsertEx > 0);
    LOG_INFO("SystemApp_Silent_InsertEx_Test001::End");
}

/**
* @tc.name: SystemApp_Silent_UpdateEx_Test001
* @tc.desc: Verify silent access UpdateEx operation behavior when caller is system app and provider not in allowList
* @tc.type: FUNC
* @tc.require: gitcode#852
* @tc.precon: Test process is set to be equivalent to a system app
* @tc.step:
    1. Create a DataShareHelper instance with silent access configuration
    2. Set self as a system app
    3. Insert a data using DataShareHelper created in step 1
    4. Set self back to system app
    5. Define update predicates to target data Inserted in step 3
    6. Prepare update data in a DataShareValuesBucket and call UpdateEx function
* @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Insert operation return greater than 0(success)
    3. UpdateEx operation return <0, positive value>(success)
*/
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Silent_UpdateEx_Test001, TestSize.Level1)
{
    LOG_INFO("SystemApp_Silent_UpdateEx_Test001::Start");
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

    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'zhangsan'";
    predicates.SetWhereClause(selections);
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put(TBL_STU_AGE, 10);

    auto [errCode, retUpdateEx] = helper->UpdateEx(uri, predicates, valuesBucket1);
    EXPECT_EQ(errCode, 0);
    EXPECT_TRUE(retUpdateEx > 0);
    LOG_INFO("SystemApp_Silent_UpdateEx_Test001::End");
}

/**
* @tc.name: SystemApp_Silent_DeleteEx_Test001
* @tc.desc: Verify silent access DeleteEx operation behavior when caller is system app and provider not in allowList
* @tc.type: FUNC
* @tc.require: gitcode#852
* @tc.precon: None
* @tc.step:
    1. Create a DataShareHelper instance with silent access configuration
    2. Define delete predicates targeting non-existent data
    3. Perform DeleteEx operation and verify result
* @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. DeleteEx operation return <0, positive value>(success)
*/
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Silent_DeleteEx_Test001, TestSize.Level1)
{
    LOG_INFO("SystemApp_Silent_DeleteEx_Test001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SILENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);
    Uri uri(SILENT_ACCESS_URI);

    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'lisan'";
    deletePredicates.SetWhereClause(selections);

    auto [errCode, retDeleteEx] = helper->DeleteEx(uri, deletePredicates);
    EXPECT_EQ(errCode, 0);
    EXPECT_EQ(retDeleteEx, 0);
    LOG_INFO("SystemApp_Silent_DeleteEx_Test001::End");
}

/**
 * @tc.name: SystemApp_Active_Insert_Test001
 * @tc.desc: Verify Active(non-silent) access Insert operation behavior when caller is system app
    and provider not in allowList
 * @tc.type: FUNC
 * @tc.require: gitcode#852
 * @tc.precon: Test process is set to be equivalent to a system app
 * @tc.step:
    1. Create a DataShareHelper instance with non-silent access configuration
    2. Prepare test data(name and age) in a DataShareValuesBucket
    3. Perform initial Insert operation and verify result
 * @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Insert operation return positive values(success)
 */
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Active_Insert_Test001, TestSize.Level0)
{
    LOG_INFO("SystemApp_Active_Insert_Test001::Start");
    Uri uri(DATA_SHARE_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    int retVal = g_exHelper->Insert(uri, valuesBucket);
    EXPECT_TRUE(retVal > 0);
    LOG_INFO("SystemApp_Active_Insert_Test001::End");
}

/**
* @tc.name: SystemApp_Active_Update_Test001
* @tc.desc: Verify silent Active(non-silent) Update operation behavior when caller is system app and provider
    not in allowList
* @tc.type: FUNC
* @tc.require: gitcode#852
* @tc.precon: Test process is set to be equivalent to a system app
* @tc.step:
    1. Create a DataShareHelper instance with non-silent access configuration
    2. Set self as a system app
    3. Insert a data using DataShareHelper created in step 1
    4. Set self back to system app
    5. Define update predicates to target data Inserted in step 3
    6. Prepare update data in a DataShareValuesBucket and call Update function
* @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Insert operation return greater than 0(success)
    3. Update operation return positive values(success)
*/
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Active_Update_Test001, TestSize.Level1)
{
    LOG_INFO("SystemApp_Active_Update_Test001::Start");
    Uri uri(DATA_SHARE_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "zhangsan";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 18;
    valuesBucket.Put(TBL_STU_AGE, age);

    int retInsert = g_exHelper->Insert(uri, valuesBucket);
    EXPECT_EQ((retInsert > 0), true);

    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'zhangsan'";
    predicates.SetWhereClause(selections);
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put(TBL_STU_AGE, 10);

    int retUpdate = g_exHelper->Update(uri, predicates, valuesBucket1);
    EXPECT_TRUE(retUpdate > 0);
    LOG_INFO("SystemApp_Active_Update_Test001::End");
}

/**
 * @tc.name: SystemApp_Active_Query_Test001
 * @tc.desc: Verify Active(non-silent) access Query operation behavior when caller is system app and
    provider not in allowList
 * @tc.type: FUNC
 * @tc.require: gitcode#852
 * @tc.precon: Test process is set to be equivalent to a system app
 * @tc.step:
    1. Create a DataShareHelper instance with non-silent access configuration
    2. Set self as a system app
    3. Insert a data using DataShareHelper created in step 1
    4. Set self back to system app
    5. Define query predicates to target data Inserted in step 3
    6. Perform Query operation and verify resultSet
* @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Insert operation return greater than 0(success)
    3. Query operation returns resultSet that is not nullptr and rowcount is greater than 0(success)
 */
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Active_Query_Test001, TestSize.Level0)
{
    LOG_INFO("SystemApp_Active_Query_Test001::Start");
    Uri uri(DATA_SHARE_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "zhangsan";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 18;
    valuesBucket.Put(TBL_STU_AGE, age);

    int retInsert = g_exHelper->Insert(uri, valuesBucket);
    EXPECT_EQ((retInsert > 0), true);

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "zhangsan");
    vector<string> columns;
    int totalRow = 0;

    auto resultSet = g_exHelper->Query(uri, predicates, columns);
    ASSERT_NE(resultSet, nullptr);
    resultSet->GetRowCount(totalRow);
    EXPECT_TRUE(totalRow > 0);
    resultSet->Close();
    LOG_INFO("SystemApp_Active_Query_Test001::End");
}

/**
* @tc.name: SystemApp_Active_Delete_Test001
* @tc.desc: Verify Active(non-silent) access Delete operation behavior when caller is system app and
    provider not in allowList
* @tc.type: FUNC
* @tc.require: gitcode#852
* @tc.precon: None
* @tc.step:
    1. Create a DataShareHelper instance with non-silent access configuration
    2. Define delete predicates targeting non-existent data
    3. Perform Delete operation and verify result
* @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Delete operation return 0(no data deleted, execute success)
*/
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Active_Delete_Test001, TestSize.Level1)
{
    LOG_INFO("SystemApp_Active_Delete_Test001::Start");
    Uri uri(DATA_SHARE_URI);

    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'lisan'";
    deletePredicates.SetWhereClause(selections);

    int retDelete = g_exHelper->Delete(uri, deletePredicates);
    EXPECT_EQ(retDelete, 0);
    LOG_INFO("SystemApp_Active_Delete_Test001::End");
}

/**
 * @tc.name: SystemApp_Active_InsertEx_Test001
 * @tc.desc: Verify Active(non-silent) access InsertEx operation behavior when caller is system app and
    provider not in allowList
 * @tc.type: FUNC
 * @tc.require: gitcode#852
 * @tc.precon: Test process is set to be equivalent to a system app
 * @tc.step:
    1. Create a DataShareHelper instance with non-silent access configuration
    2. Prepare test data(name and age) in a DataShareValuesBucket
    3. Perform initial InsertEx operation and verify result
 * @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. InsertEx operation return <0, positive value>(success)
 */
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Active_InsertEx_Test001, TestSize.Level0)
{
    LOG_INFO("SystemApp_Active_InsertEx_Test001::Start");
    Uri uri(SILENT_ACCESS_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    auto [errCode, retInsertEx] = g_exHelper->InsertEx(uri, valuesBucket);
    EXPECT_EQ(errCode, 0);
    EXPECT_TRUE(retInsertEx > 0);
    LOG_INFO("SystemApp_Active_InsertEx_Test001::End");
}

/**
* @tc.name: SystemApp_Active_UpdateEx_Test001
* @tc.desc: Verify Active(non-silent) access UpdateEx operation behavior when caller is system app and
    provider not in allowList
* @tc.type: FUNC
* @tc.require: gitcode#852
* @tc.precon: Test process is set to be equivalent to a system app
* @tc.step:
    1. Create a DataShareHelper instance with silent access configuration
    2. Set self as a system app
    3. Insert a data using DataShareHelper created in step 1
    4. Set self back to system app
    5. Define update predicates to target data Inserted in step 3
    6. Prepare update data in a DataShareValuesBucket and call UpdateEx function
* @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Insert operation return greater than 0(success)
    3. UpdateEx operation return <0, positive value>(success)
*/
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Active_UpdateEx_Test001, TestSize.Level1)
{
    LOG_INFO("SystemApp_Active_UpdateEx_Test001::Start");
    Uri uri(DATA_SHARE_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "zhangsan";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 18;
    valuesBucket.Put(TBL_STU_AGE, age);

    int retInsert = g_exHelper->Insert(uri, valuesBucket);
    EXPECT_EQ((retInsert > 0), true);

    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'zhangsan'";
    predicates.SetWhereClause(selections);
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put(TBL_STU_AGE, 10);

    auto [errCode, retUpdateEx] = g_exHelper->UpdateEx(uri, predicates, valuesBucket1);
    EXPECT_EQ(errCode, 0);
    EXPECT_TRUE(retUpdateEx > 0);
    LOG_INFO("SystemApp_Active_UpdateEx_Test001::End");
}

/**
 * @tc.name: SystemApp_Active_BatchUpdate_Test001
 * @tc.desc: Verify Active(non-silent) access BatchUpdate operation behavior when caller is system app and
    provider not in allowList
 * @tc.type: FUNC
 * @tc.require: gitcode#852
 * @tc.precon: Test process is set to be equivalent to a system app
 * @tc.step:
    1. Create a DataShareHelper instance with non-silent access configuration
    2. Prepare test data in a DataShare::UpdateOperations form
    3. Perform BatchUpdate operation and verify result
 * @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. BatchUpdate operation returns not equalto PERMISSION_ERR_CODE and result size is 2(success)
 */
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Active_BatchUpdate_Test001, TestSize.Level0)
{
    LOG_INFO("SystemApp_Active_BatchUpdate_Test001::Start");
    Uri uri(SILENT_ACCESS_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "batchUpdateTest");
    int ret = g_exHelper->Insert(uri, valuesBucket);
    EXPECT_TRUE(ret > 0);

    DataShare::UpdateOperations operations;
    std::vector<DataShare::UpdateOperation> updateOperations1;
    DataShare::UpdateOperation updateOperation1;
    updateOperation1.valuesBucket.Put("name", "batchUpdateTested");
    updateOperation1.predicates.EqualTo("name", "batchUpdateTest");
    updateOperations1.push_back(updateOperation1);

    std::vector<DataShare::UpdateOperation> updateOperations2;
    DataShare::UpdateOperation updateOperation2;
    updateOperation2.valuesBucket.Put("name", "undefined1");
    updateOperation2.predicates.EqualTo("name", "undefined");
    updateOperations1.push_back(updateOperation2);
    updateOperations2.push_back(updateOperation2);

    operations.emplace(SILENT_ACCESS_URI, updateOperations1);
    operations.emplace(SILENT_ACCESS_URI, updateOperations2);
    std::vector<BatchUpdateResult> results;

    auto retBatchUpdate = g_exHelper->BatchUpdate(operations, results);
    EXPECT_NE(retBatchUpdate, PERMISSION_ERR_CODE);
    EXPECT_TRUE(results.size() > 0);
    LOG_INFO("SystemApp_Active_BatchUpdate_Test001::End");
}

/**
 * @tc.name: SystemApp_Active_VerifiedProvider_Test001
 * @tc.desc: Verify Active(non-silent) access Insert, Update and Delete operation behavior when caller
    is system app and provider in allowList
 * @tc.type: FUNC
 * @tc.require: gitcode#852
 * @tc.precon: Test process is set to be equivalent to a system app, helper created using settingsdata
    non-silent uri.
 * @tc.step:
    1. Create a DataShareHelper instance with non-silent access configuration
    2. Prepare test data(KEYWORD and VALUE) that does not match data type in a DataShareValuesBucket
    3. Perform initial Insert operation and verify result
    4. Try Update operation to update data supposedly inserted in step 3 and verify result
    5. Try Delete operation to delete data supposedly updated in step 4 and verify result
 * @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Insert operation return not equal to PERMISSION_ERR_CODE(-2)(pass permission check)
    3. Update operation return not equal to PERMISSION_ERR_CODE(-2)(pass permission check)
    4. Delete operation return not equal to PERMISSION_ERR_CODE(-2)(pass permission check)
 */
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Active_VerifiedProvider_Test001, TestSize.Level0)
{
    LOG_INFO("SystemApp_Active_VerifiedProvider_Test001::Start");
    Uri uri(VERIFIED_QUERY_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(TBL_SETTINGS_COL1, "SystemApp_Active_VerifiedProvider_Test001");
    valuesBucket.Put(TBL_SETTINGS_COL2, "DataShareStubImplSystemTest");
    int retVal = settings_exHelper->Insert(uri, valuesBucket);
    EXPECT_NE(retVal, PERMISSION_ERR_CODE);

    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_SETTINGS_COL1 + " = 'SystemApp_Active_VerifiedProvider_Test001'";
    predicates.SetWhereClause(selections);
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put(TBL_SETTINGS_COL2, "DataShareStubImplSystemTest");
    int retUpdate = settings_exHelper->Update(uri, predicates, valuesBucket1);
    EXPECT_NE(retUpdate, PERMISSION_ERR_CODE);

    DataShare::DataSharePredicates deletePredicates;
    std::string selectionsDelete = TBL_SETTINGS_COL2 + " = 'DataShareStubImplSystemTest'";
    deletePredicates.SetWhereClause(selectionsDelete);

    int retDelete = settings_exHelper->Delete(uri, deletePredicates);
    EXPECT_NE(retDelete, PERMISSION_ERR_CODE);
    LOG_INFO("SystemApp_Active_VerifiedProvider_Test001::End");
}

/**
 * @tc.name: SystemApp_Active_VerifiedProvider_Test002
 * @tc.desc: Verify Active(non-silent) access InsertEx, UpdateEx and DeleteEx operation behavior when caller
    is system app and provider in allowList
 * @tc.type: FUNC
 * @tc.require: gitcode#852
 * @tc.precon: Test process is set to be equivalent to a system app, helper created using settingsdata
    non-silent uri.
 * @tc.step:
    1. Create a DataShareHelper instance with non-silent access configuration
    2. Prepare test data(KEYWORD and VALUE) that does not match data type in a DataShareValuesBucket
    3. Perform InsertEx operation and verify result
    4. Try UpdateEx operation to update data supposedly inserted in step 3 and verify result
    5. Try DeleteEx operation to delete data supposedly updated in step 4 and verify result
 * @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. InsertEx operation return errCode not equal to PERMISSION_ERR_CODE(-2)(pass permission check)
    3. UpdateEx operation return errCode not equal to PERMISSION_ERR_CODE(-2)(pass permission check)
    4. DeleteEx operation return errCode not equal to PERMISSION_ERR_CODE(-2)(pass permission check)
 */
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Active_VerifiedProvider_Test002, TestSize.Level0)
{
    LOG_INFO("SystemApp_Active_VerifiedProvider_Test002::Start");
    Uri uri(VERIFIED_QUERY_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(TBL_SETTINGS_COL1, "SystemApp_Active_VerifiedProvider_Test002");
    valuesBucket.Put(TBL_SETTINGS_COL2, "DataShareStubImplSystemTest");
    auto [errCode, retVal] = settings_exHelper->InsertEx(uri, valuesBucket);
    EXPECT_NE(errCode, PERMISSION_ERR_CODE);

    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_SETTINGS_COL1 + " = 'SystemApp_Active_VerifiedProvider_Test002'";
    predicates.SetWhereClause(selections);
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put(TBL_SETTINGS_COL2, "DataShareStubImplSystemTest");
    auto [errUpdate, retUpdate] = settings_exHelper->UpdateEx(uri, predicates, valuesBucket1);
    EXPECT_NE(errUpdate, PERMISSION_ERR_CODE);

    DataShare::DataSharePredicates deletePredicates;
    std::string selectionsDelete = TBL_SETTINGS_COL2 + " = 'DataShareStubImplSystemTest'";
    deletePredicates.SetWhereClause(selectionsDelete);
    auto [errDelete, retDelete] = settings_exHelper->DeleteEx(uri, deletePredicates);
    EXPECT_NE(errDelete, PERMISSION_ERR_CODE);
    LOG_INFO("SystemApp_Active_VerifiedProvider_Test002::End");
}

/**
 * @tc.name: SystemApp_Active_VerifiedProvider_Test003
 * @tc.desc: Verify Active(non-silent) access BatchInsert and BatchUpdate operation behavior when
    caller is system app and provider in allowList
 * @tc.type: FUNC
 * @tc.require: gitcode#852
 * @tc.precon: Test process is set to be equivalent to a system app, helper created using settingsdata
    non-silent uri.
 * @tc.step:
    1. Create a DataShareHelper instance with non-silent access configuration
    2. Prepare test data
    3. Perform BatchInsert operation and verify result
    4. Try BatchUpdate operation to update data supposedly inserted in step 3 and verify result
 * @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. BatchInsert operation return not equal to PERMISSION_ERR_CODE(-2)(pass permission check)
    3. BatchUpdate operation return not equal to PERMISSION_ERR_CODE(-2)(pass permission check) and result size is 0
 */
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Active_VerifiedProvider_Test003, TestSize.Level0)
{
    LOG_INFO("SystemApp_Active_VerifiedProvider_Test003::Start");
    Uri uri(VERIFIED_QUERY_URI);

    DataShare::DataShareValuesBucket valuesBucket1;
    DataShare::DataShareValuesBucket valuesBucket2;

    valuesBucket1.Put(TBL_SETTINGS_COL1, "SystemApp_Active_VerifiedProvider_Test003");
    valuesBucket2.Put(TBL_SETTINGS_COL1, "SystemApp_Active_VerifiedProvider_Test003_2");
    std::vector<DataShareValuesBucket> vBuckets = { valuesBucket1, valuesBucket2 };

    auto retBatchInsert = settings_exHelper->BatchInsert(uri, vBuckets);
    EXPECT_NE(retBatchInsert, PERMISSION_ERR_CODE);

    DataShare::UpdateOperations operations;
    std::vector<DataShare::UpdateOperation> updateOperations1;
    DataShare::UpdateOperation updateOperation1;
    updateOperation1.valuesBucket.Put(TBL_SETTINGS_COL1, "batchUpdateTested");
    updateOperation1.predicates.EqualTo(TBL_SETTINGS_COL1, "SystemApp_Active_VerifiedProvider_Test003");
    updateOperations1.push_back(updateOperation1);
 
    std::vector<DataShare::UpdateOperation> updateOperations2;
    DataShare::UpdateOperation updateOperation2;
    updateOperation2.valuesBucket.Put(TBL_SETTINGS_COL2, "undefined1");
    updateOperation2.predicates.EqualTo(TBL_SETTINGS_COL1, "SystemApp_Active_VerifiedProvider_Test003_2");
    updateOperations1.push_back(updateOperation2);
    updateOperations2.push_back(updateOperation2);

    operations.emplace(VERIFIED_QUERY_URI, updateOperations1);
    operations.emplace(VERIFIED_QUERY_URI, updateOperations2);
    std::vector<BatchUpdateResult> results;

    auto retBatchUpdate = settings_exHelper->BatchUpdate(operations, results);
    EXPECT_NE(retBatchUpdate, PERMISSION_ERR_CODE);
    EXPECT_EQ(results.size(), 0);
    LOG_INFO("SystemApp_Active_VerifiedProvider_Test003::End");
}

/**
 * @tc.name: SystemApp_Active_VerifiedProviderQuery_Test001
 * @tc.desc: Verify Active(non-silent) access Query operation behavior when caller is system app and
    provider in allowList
 * @tc.type: FUNC
 * @tc.require: gitcode#852
 * @tc.precon: Test process is set to be equivalent to a system app
 * @tc.step:
    1. Create a DataShareHelper instance with non-silent access configuration
    2. Define query predicates to target data Inserted in step 3
    3. Perform Query operation and verify resultSet
* @tc.expect:
    1. DataShareHelper is created successfully(not nullptr)
    2. Query operation return a non-null resultSet and rowcount is greater than 0(success)
 */
HWTEST_F(DataShareStubImplSystemTest, SystemApp_Active_VerifiedProviderQuery_Test001, TestSize.Level0)
{
    LOG_INFO("SystemApp_Active_VerifiedProviderQuery_Test001::Start");

    Uri uri(VERIFIED_QUERY_URI);
    DataShare::DataSharePredicates predicates;
    vector<string> columns;
    int totalRow = 0;

    auto resultSet = settings_exHelper->Query(uri, predicates, columns);
    ASSERT_NE(resultSet, nullptr);
    resultSet->GetRowCount(totalRow);
    EXPECT_TRUE(totalRow > 0);
    resultSet->Close();
    LOG_INFO("SystemApp_Active_VerifiedProviderQuery_Test001::End");
}
}
}