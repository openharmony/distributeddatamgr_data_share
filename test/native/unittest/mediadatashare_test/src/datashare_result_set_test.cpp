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

#include "basic/result_set.h"
#define LOG_TAG "datashare_result_set_test"

#include <gtest/gtest.h>
#include <unistd.h>
#include <vector>

#include "accesstoken_kit.h"
#include "data_ability_observer_stub.h"
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
using ChangeInfo = DataShareObserver::ChangeInfo;
constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;

std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";
std::string SLIENT_ACCESS_URI = "datashareproxy://com.acts.datasharetest/test?Proxy=true";

std::string TBL_STU_NAME = "name";
std::string TBL_STU_AGE = "age";
std::shared_ptr<DataShare::DataShareHelper> g_datashareResultSetHelper;

class DataShareResultSetTest : public testing::Test {
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
        }
    };
    return permissionStateFulls;
}

void DataShareResultSetTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    auto dataShareHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, DATA_SHARE_URI);
    ASSERT_TRUE(dataShareHelper != nullptr);
    int sleepTime = 3;
    sleep(sleepTime);

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

    g_datashareResultSetHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(g_datashareResultSetHelper != nullptr);
    LOG_INFO("SetUpTestCase end");
}

void DataShareResultSetTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId.tokenIDEx);
    g_datashareResultSetHelper = nullptr;
}

void DataShareResultSetTest::SetUp(void) {}
void DataShareResultSetTest::TearDown(void) {}

/**
 * @tc.name: ResultSet_IsAtLastRow_Test_001
 * @tc.desc: Test traversing the ResultSet using IsAtLastRow as the loop end condition, verifying the normal end of the
 *           database compilation cycle after inserting, querying, and deleting test data.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The global DataShareHelper instance g_datashareResultSetHelper is properly initialized and non-null.
    2. The test environment supports DataShareValuesBucket, DataSharePredicates, and ResultSet operations (IsAtLastRow,
       GoToNextRow, GetString).
    3. The SLIENT_ACCESS_URI is valid and points to an existing data table; TBL_STU_NAME (string type) and TBL_STU_AGE
       (int type) are valid columns of the table.
    4. The InsertEx, DeleteEx, and Query methods of DataShareHelper can be normally called without initialization
       errors.
 * @tc.step:
    1. Create a DataShareValuesBucket, add TBL_STU_NAME ("lisi") and TBL_STU_AGE (25) to it, then call InsertEx with
       SLIENT_ACCESS_URI to insert the test data.
    2. Create a DataSharePredicates instance, call EqualTo to set the condition: TBL_STU_NAME = "lisi".
    3. Call Query with SLIENT_ACCESS_URI, the created predicates, and an empty column vector to obtain a ResultSet,
       then verify the ResultSet is non-null.
    4. Call DeleteEx with SLIENT_ACCESS_URI and the predicates to delete the inserted test data, verifying the deletion
       is successful (errCode = 0, retVal > 0).
    5. Initialize a boolean variable isAtLastRow to false; if the ResultSet is non-null, use IsAtLastRow as the loop
       end condition to traverse the ResultSet (call GoToNextRow in the loop).
 * @tc.expect:
    1. The InsertEx method returns (errCode = 0, retVal > 0) (insertion success).
    2. The Query method returns a non-null ResultSet.
    3. The DeleteEx method returns (errCode = 0, retVal > 0) (deletion success).
    4. The ResultSet traversal using IsAtLastRow completes normally, and the database compilation cycle ends without
       errors.
 */
HWTEST_F(DataShareResultSetTest, ResultSet_IsAtLastRow_Test_001, TestSize.Level0)
{
    LOG_INFO("[ttt]ResultSet_IsAtLastRow_Test_001::Start");
    bool isAtLastRow = false;
    auto helper = g_datashareResultSetHelper;
    Uri uri(SLIENT_ACCESS_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    auto [errCode, retVal] = helper->InsertEx(uri, valuesBucket);
    EXPECT_EQ((errCode == 0), true);
    EXPECT_EQ((retVal > 0), true);

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "lisi");
    vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    EXPECT_EQ((resultSet != nullptr), true);

    auto [errCode2, retVal2] = helper->DeleteEx(uri, predicates);
    EXPECT_EQ((errCode2 == 0), true);
    EXPECT_EQ((retVal2 > 0), true);
    
    if (resultSet != nullptr) {
        resultSet->IsAtLastRow(isAtLastRow);
        while (!isAtLastRow) {
            if (resultSet->GoToNextRow() != 0) {
                break;
            }
            resultSet->IsAtLastRow(isAtLastRow);
        }
    }
    LOG_INFO("[ttt] ResultSet_IsAtLastRow_Test_001::End");
}

/**
 * @tc.name: ResultSet_IsEnded_Test_001
 * @tc.desc: Test traversing the ResultSet using IsEnded as the loop end condition, ensuring the database compilation
 *           cycle ends normally after inserting, querying, and deleting test data.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The global DataShareHelper instance g_datashareResultSetHelper is properly initialized and non-null.
    2. The test environment supports DataShareValuesBucket, DataSharePredicates, and ResultSet operations (IsEnded,
       GoToNextRow).
    3. The SLIENT_ACCESS_URI is valid and maps to an existing table; TBL_STU_NAME (string) and TBL_STU_AGE (int) are
       valid columns of the table.
    4. The InsertEx, DeleteEx, and Query methods of DataShareHelper can be invoked normally.
 * @tc.step:
    1. Create a DataShareValuesBucket, add TBL_STU_NAME ("lisi") and TBL_STU_AGE (25) to it, then call InsertEx with
       SLIENT_ACCESS_URI to insert the test data.
    2. Create a DataSharePredicates instance, set the condition to TBL_STU_NAME = "lisi" via the EqualTo method.
    3. Call Query with SLIENT_ACCESS_URI, the predicates, and an empty column vector to get a ResultSet, then verify
       the ResultSet is non-null.
    4. Call DeleteEx with SLIENT_ACCESS_URI and the predicates to delete the test data, checking that the deletion
       succeeds (errCode = 0, retVal > 0).
    5. Initialize a boolean variable isEnded to false; if the ResultSet is non-null, use IsEnded as the loop end
       condition to traverse the ResultSet (call GoToNextRow in the loop).
 * @tc.expect:
    1. InsertEx returns (errCode = 0, retVal > 0) (successful insertion).
    2. Query returns a non-null ResultSet.
    3. DeleteEx returns (errCode = 0, retVal > 0) (successful deletion).
    4. The ResultSet traversal using IsEnded finishes normally, and the database compilation cycle ends without errors.
 */
HWTEST_F(DataShareResultSetTest, ResultSet_IsEnded_Test_001, TestSize.Level0)
{
    LOG_INFO("[ttt]ResultSet_IsEnded_Test_001::Start");
    auto helper = g_datashareResultSetHelper;
    Uri uri(SLIENT_ACCESS_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    auto [errCode, retVal] = helper->InsertEx(uri, valuesBucket);
    EXPECT_EQ((errCode == 0), true);
    EXPECT_EQ((retVal > 0), true);

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "lisi");
    vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    EXPECT_EQ((resultSet != nullptr), true);

    auto [errCode2, retVal2] = helper->DeleteEx(uri, predicates);
    EXPECT_EQ((errCode2 == 0), true);
    EXPECT_EQ((retVal2 > 0), true);

    bool isEnded = false;
    if (resultSet != nullptr) {
        resultSet->IsEnded(isEnded);
        while (!isEnded) {
            resultSet->GoToNextRow();
            resultSet->IsEnded(isEnded);
        }
    }
    LOG_INFO("[ttt] ResultSet_IsEnded_Test_001::End");
}

/**
 * @tc.name: ResultSet_ShareBlock_Test_001
 * @tc.desc: Test the scenario where 2 MB shared memory is full when reading large data from ResultSet, verifying that
 *           shared memory page turning works correctly and the read data matches the inserted data.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The global DataShareHelper instance g_datashareResultSetHelper is properly initialized and non-null.
    2. The test environment supports creating large strings (200000 'a' characters) and handles shared memory up to 2MB
       with page turning.
    3. The SLIENT_ACCESS_URI is valid; TBL_STU_NAME (supports large strings) and TBL_STU_AGE (int) are valid table
       columns.
    4. InsertEx, Query (handling large result sets), GetString, and DeleteEx methods work normally.
 * @tc.step:
    1. Create a DataShareValuesBucket: add TBL_STU_AGE (25) and a large string (200000 'a' characters) to TBL_STU_NAME.
    2. Loop 20 times: call InsertEx with SLIENT_ACCESS_URI and the bucket each time to insert 20 pieces of test data.
    3. Call Query with SLIENT_ACCESS_URI, an empty DataSharePredicates, and empty column vector to obtain a ResultSet;
       verify the ResultSet is non-null.
    4. Traverse the ResultSet via GoToNextRow: for each row, call GetString to read the value of the first column
       (TBL_STU_NAME), and compare it with the inserted large string.
    5. Call DeleteEx with SLIENT_ACCESS_URI and empty predicates to delete all inserted test data.
 * @tc.expect:
    1. All 20 InsertEx calls return (errCode = 0, retVal > 0) (all insertions succeed).
    2. Query returns a non-null ResultSet, and shared memory page turning occurs normally when reading large data.
    3. Every string read via GetString matches the inserted large string (200000 'a' characters).
    4. DeleteEx returns (errCode = 0, retVal > 0) (all test data is deleted successfully).
 */
HWTEST_F(DataShareResultSetTest, ResultSet_ShareBlock_Test_001, TestSize.Level0)
{
    LOG_INFO("[ttt]ResultSet_ShareBlock_Test_001::Start");
    auto helper = g_datashareResultSetHelper;
    Uri uri(SLIENT_ACCESS_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    std::string value(200000, 'a');
    valuesBucket.Put(TBL_STU_NAME, value);
    for (int i = 0; i < 20; ++i) {
        auto [errCode, retVal] = helper->InsertEx(uri, valuesBucket);
        EXPECT_EQ(errCode, 0);
        EXPECT_GT(retVal, 0);
    }

    DataShare::DataSharePredicates predicates;
    vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    EXPECT_NE(resultSet, nullptr);

    while (resultSet->GoToNextRow() == 0) {
        std::string retValue;
        int ret = resultSet->GetString(1, retValue);
        EXPECT_EQ(ret, 0);
        EXPECT_EQ(retValue, value);
    }

    auto [errCode2, retVal2] = helper->DeleteEx(uri, predicates);
    EXPECT_EQ(errCode2, 0);
    EXPECT_GT(retVal2, 0);
    LOG_INFO("[ttt] ResultSet_ShareBlock_Test_001::End");
}

} // namespace DataShare
} // namespace OHOS