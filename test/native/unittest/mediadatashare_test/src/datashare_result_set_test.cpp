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
* @tc.desc: Test uses IsAtLastRow as the loop end condition.
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: None
* @tc.step:
* 1. Insert test data (name="lisi", age=25) into SLIENT_ACCESS_URI
* 2. Create predicates to select inserted record
* 3. Call Query() to obtain resultSet with SLIENT_ACCESS_URI
* 4. Delete test data. The database is empty.
* 5. uses IsAtLastRow as the loop end condition.
* @tc.expect: The database compilation cycle ends normally.
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
* @tc.desc: Test uses IsEnded as the loop end condition.
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: None
* @tc.step:
* 1. Insert test data (name="lisi", age=25) into SLIENT_ACCESS_URI
* 2. Create predicates to select inserted record
* 3. Call Query() to obtain resultSet with SLIENT_ACCESS_URI
* 4. Delete test data. The database is empty.
* 5. uses IsEnded as the loop end condition.
* @tc.expect: The database compilation cycle ends normally.
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
* @tc.desc: Test the 2 MB shared memory is full.
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: None
* @tc.step:
* 1. Insert test data (name=string value(200000, 'a'), age=25) into SLIENT_ACCESS_URI 20 times
* 2. Call Query() to obtain resultSet with SLIENT_ACCESS_URI
* 3. Read the string in the first column of resultSet.
*    The data exceeds 2 MB, pages are turned in the shared memory.
* 4. Delete test data. The database is empty.
* @tc.expect: The read string is the same as the inserted string.
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