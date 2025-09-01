/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#include "datashare_helper.h"
#include "datashare_log.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "rdb_errno.h"
#include "system_ability_definition.h"
#include "token_setproc.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;
std::string DATA_SHARE_URI = "datashare:///com.acts.errorcodetest";
std::string SLIENT_ACCESS_URI = "datashareproxy://com.acts.errorcodetest/test?Proxy=true";
std::string TBL_STU_NAME = "name";
std::string TBL_STU_AGE = "age";
std::shared_ptr<DataShare::DataShareHelper> g_slientAccessHelper;
std::shared_ptr<DataShare::DataShareHelper> dataShareHelper;

class ErrorCodeTest : public testing::Test {
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

HapPolicyParams GetPolicy()
{
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
        .permStateList = {
            {
                .permissionName = "ohos.permission.test",
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
        }
    };
    return policy;
}

void ErrorCodeTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    dataShareHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, DATA_SHARE_URI);
    ASSERT_TRUE(dataShareHelper != nullptr);
    int sleepTime = 3;
    sleep(sleepTime);

    HapInfoParams info = {
        .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareclienttest.demo"
    };
    auto policy = GetPolicy();
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenID(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId);

    g_slientAccessHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(g_slientAccessHelper != nullptr);
    LOG_INFO("SetUpTestCase end");
}

void ErrorCodeTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenID(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId);
    g_slientAccessHelper = nullptr;
    dataShareHelper = nullptr;
}

void ErrorCodeTest::SetUp(void) {}
void ErrorCodeTest::TearDown(void) {}

/**
* @tc.name: ErrorCodeTest_Insert_Test_001
* @tc.desc: Verify successful insertion operation returns positive ID
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Get slient access helper instance
    2. Create test URI and DataShareValuesBucket with student data
    3. Call Insert method with URI and values bucket
    4. Check if returned ID is positive
* @tc.experct: Insert operation succeeds and returns positive ID
*/
HWTEST_F(ErrorCodeTest, ErrorCodeTest_Insert_Test_001, TestSize.Level0)
{
    LOG_INFO("ErrorCodeTest_Insert_Test_001::Start");
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ACCESS_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("ErrorCodeTest_Insert_Test_001::End");
}

/**
* @tc.name: ErrorCodeTest_QUERY_Test_001
* @tc.desc: Verify query operations return correct error codes for valid and invalid URIs
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Query existing data with valid URI and check for success
    2. Verify row count is 1 for existing data
    3. Query with invalid URI and check error code
    4. Verify result set is null for invalid URI
* @tc.experct:
    1. Valid query returns 0 error code and 1 row
    2. Invalid query returns E_DB_NOT_EXIST error and null result set
*/
HWTEST_F(ErrorCodeTest, ErrorCodeTest_QUERY_Test_001, TestSize.Level0)
{
    LOG_INFO("ErrorCodeTest_QUERY_Test_001::Start");
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ACCESS_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "lisi");
    vector<string> columns;
    DatashareBusinessError noError;
    auto resultSet = helper->Query(uri, predicates, columns, &noError);
    EXPECT_EQ(noError.GetCode(), 0);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);

    std::string ERR_SLIENT_ACCESS_URI = "datashare:///com.acts.errorcodetest/entry/DB01/TBL01?Proxy=true";
    Uri uriErr(ERR_SLIENT_ACCESS_URI);
    DatashareBusinessError error;
    resultSet = helper->Query(uriErr, predicates, columns, &error);
    EXPECT_EQ(error.GetCode(), NativeRdb::E_DB_NOT_EXIST);
    EXPECT_EQ(resultSet, nullptr);
    LOG_INFO("ErrorCodeTest_QUERY_Test_001::End");
}

/**
* @tc.name: ErrorCodeTest_QUERY_Test_002
* @tc.desc: Verify unauthorized query returns correct error code
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Insert test data using dataShareHelper
    2. Query inserted data with predicates
    3. Check error code and result set for unauthorized access
    4. Clean up by deleting inserted data
* @tc.experct:
    1. Insert succeeds with positive ID
    2. Query returns 401 error code and null result set
    3. Delete operation succeeds with positive count
*/
HWTEST_F(ErrorCodeTest, ErrorCodeTest_QUERY_Test_002, TestSize.Level0)
{
    LOG_INFO("ErrorCodeTest_QUERY_Test_002::Start");
    ASSERT_TRUE(dataShareHelper != nullptr);
    Uri uri(DATA_SHARE_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "wangwu";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 30;
    valuesBucket.Put(TBL_STU_AGE, age);
    int retVal = dataShareHelper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "wangwu");
    vector<string> columns;
    DatashareBusinessError error;
    auto resultSet = dataShareHelper->Query(uri, predicates, columns, &error);
    EXPECT_EQ(error.GetCode(), 401);
    EXPECT_EQ(resultSet, nullptr);

    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'wangwu'";
    deletePredicates.SetWhereClause(selections);
    retVal = dataShareHelper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("ErrorCodeTest_QUERY_Test_002::End");
}
} // namespace DataShare
} // namespace OHOS